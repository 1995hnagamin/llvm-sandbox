#include "pragma_dead_handler.hpp"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"
#include <memory>

static llvm::cl::OptionCategory LTOptionCategory("LT options");

class ListTypesVisitor : public clang::RecursiveASTVisitor<ListTypesVisitor> {
public:
  explicit ListTypesVisitor(clang::CompilerInstance *C,
                            std::queue<Directive> *Q)
      : Compiler(C), Queue(Q) {}

  bool VisitDecl(clang::Decl *D) {
    auto const Range = D->getSourceRange();
    if (!Queue->empty() && Queue->front().SrcLoc < Range.getBegin()) {
      auto const Dir = Queue->front();
      Queue->pop();
      llvm::outs() << Dir.Message << ": " << D->getDeclKindName() << "\n";
      Dir.SrcLoc.print(llvm::outs(), Compiler->getSourceManager());
      llvm::outs() << "\n";
    }
    return true;
  }
  bool VisitStmt(clang::Stmt *S) {
    auto const Range = S->getSourceRange();
    if (!Queue->empty() && Queue->front().SrcLoc < Range.getBegin()) {
      auto const Dir = Queue->front();
      Queue->pop();
      llvm::outs() << Dir.Message << ": " << S->getStmtClassName() << "\n";
      Dir.SrcLoc.print(llvm::outs(), Compiler->getSourceManager());
      llvm::outs() << "\n";
    }
    return true;
  }

private:
  clang::CompilerInstance *Compiler;
  std::queue<Directive> *Queue;
};

class LTASTConsumer : public clang::ASTConsumer {
public:
  explicit LTASTConsumer(clang::CompilerInstance *C, std::queue<Directive> *Q)
      : Compiler(C), Queue(Q) {}

  virtual void HandleTranslationUnit(clang::ASTContext &Context) override {
    ListTypesVisitor Visitor(Compiler, Queue);
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }

private:
  clang::CompilerInstance *Compiler;
  std::queue<Directive> *Queue;
};

class LTFrontendAction : public clang::ASTFrontendAction {
public:
  virtual std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &Compiler, llvm::StringRef File) {
    auto &PP = Compiler.getPreprocessor();
    auto const pQue = new std::queue<Directive>;
    auto const pHandler = new PragmaDeadHandler(pQue);
    PP.AddPragmaHandler(pHandler);
    return llvm::make_unique<LTASTConsumer>(&Compiler, pQue);
  }
};

int main(int argc, char const **argv) {
  clang::tooling::CommonOptionsParser OptionsParser(argc, argv,
                                                    LTOptionCategory);
  auto const v = OptionsParser.getSourcePathList();
  clang::tooling::ClangTool Tool(OptionsParser.getCompilations(),
                                 OptionsParser.getSourcePathList());
  return Tool.run(
      clang::tooling::newFrontendActionFactory<LTFrontendAction>().get());
}
