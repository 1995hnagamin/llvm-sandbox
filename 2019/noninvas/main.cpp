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
  explicit ListTypesVisitor() = default;

  bool VisitDecl(clang::Decl *D) {
    auto const Range = D->getSourceRange();
    if (!DirectiveQueue.empty() &&
        DirectiveQueue.front().SrcLoc < Range.getBegin()) {
      auto const Dir = DirectiveQueue.front();
      DirectiveQueue.pop();
      llvm::outs() << Dir.Message << ": " << D->getDeclKindName() << "\n";
    }
    return true;
  }
};

class LTASTConsumer : public clang::ASTConsumer {
public:
  explicit LTASTConsumer() = default;

  virtual void HandleTranslationUnit(clang::ASTContext &Context) override {
    ListTypesVisitor Visitor;
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }
};

class LTFrontendAction : public clang::ASTFrontendAction {
public:
  virtual std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &Compiler, llvm::StringRef File) {
    auto &PP = Compiler.getPreprocessor();
    auto const pHandler = new PragmaDeadHandler;
    PP.AddPragmaHandler(pHandler);
    return llvm::make_unique<LTASTConsumer>();
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
