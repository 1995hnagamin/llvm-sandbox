#include "pragma_dead_handler.hpp"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"
#include <memory>

static llvm::cl::OptionCategory TipsOptionCategory("Tips options");

class ShowTipsVisitor : public clang::RecursiveASTVisitor<ShowTipsVisitor> {
public:
  explicit ShowTipsVisitor(clang::CompilerInstance *C) : Compiler(C) {}

  bool VisitDecl(clang::Decl *D) {
    auto const Range = D->getSourceRange();
    if (!DirectiveQueue.empty() &&
        DirectiveQueue.front().SrcLoc < Range.getBegin()) {
      auto const Dir = DirectiveQueue.front();
      DirectiveQueue.pop();
      llvm::outs() << Dir.Message << ": " << D->getDeclKindName() << "\n";
      Dir.SrcLoc.print(llvm::outs(), Compiler->getSourceManager());
      llvm::outs() << "\n";
    }
    return true;
  }
  bool VisitStmt(clang::Stmt *S) {
    auto const Range = S->getSourceRange();
    if (!DirectiveQueue.empty() &&
        DirectiveQueue.front().SrcLoc < Range.getBegin()) {
      auto const Dir = DirectiveQueue.front();
      DirectiveQueue.pop();
      llvm::outs() << Dir.Message << ": " << S->getStmtClassName() << "\n";
      Dir.SrcLoc.print(llvm::outs(), Compiler->getSourceManager());
      llvm::outs() << "\n";
    }
    return true;
  }

private:
  clang::CompilerInstance *Compiler;
};

class TipsASTConsumer : public clang::ASTConsumer {
public:
  explicit TipsASTConsumer(clang::CompilerInstance *C) : Compiler(C) {}

  virtual void HandleTranslationUnit(clang::ASTContext &Context) override {
    ShowTipsVisitor Visitor(Compiler);
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }

private:
  clang::CompilerInstance *Compiler;
};

class TipsFrontendAction : public clang::ASTFrontendAction {
public:
  virtual std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &Compiler, llvm::StringRef File) {
    auto &PP = Compiler.getPreprocessor();
    auto const pHandler = new PragmaDeadHandler;
    PP.AddPragmaHandler(pHandler);
    return llvm::make_unique<TipsASTConsumer>(&Compiler);
  }
};

int main(int argc, char const **argv) {
  clang::tooling::CommonOptionsParser OptionsParser(argc, argv,
                                                    TipsOptionCategory);
  auto const v = OptionsParser.getSourcePathList();
  clang::tooling::ClangTool Tool(OptionsParser.getCompilations(),
                                 OptionsParser.getSourcePathList());
  return Tool.run(
      clang::tooling::newFrontendActionFactory<TipsFrontendAction>().get());
}
