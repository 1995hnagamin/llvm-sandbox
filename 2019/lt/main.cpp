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

  bool VisitType(clang::Type *T) {
    T->dump();
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
    return llvm::make_unique<LTASTConsumer>();
  }
};

int main(int argc, char const **argv) {
  clang::tooling::CommonOptionsParser OptionsParser(argc, argv,
                                                    LTOptionCategory);
  auto const v = OptionsParser.getSourcePathList();
  for (auto &&x : v) {
    llvm::errs() << x << "\n";
  }
  clang::tooling::ClangTool Tool(OptionsParser.getCompilations(),
                                 OptionsParser.getSourcePathList());
  return Tool.run(
      clang::tooling::newFrontendActionFactory<LTFrontendAction>().get());
}
