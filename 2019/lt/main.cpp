#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"
#include <iostream>
#include <memory>

static llvm::cl::OptionCategory LtOptionCategory("LT options");

class ListTypesVisitor : public clang::RecursiveASTVisitor<ListTypesVisitor> {
public:
  explicit ListTypesVisitor() {}

  bool VisitType(clang::Type *T) {
    T->dump();
    return true;
  }
};

class LtASTConsumer : public clang::ASTConsumer {
public:
  explicit LtASTConsumer() = default;

  virtual void HandleTranslationUnit(clang::ASTContext &Context) override {
    ListTypesVisitor Visitor;
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }
};

class LtFrontendAction : public clang::ASTFrontendAction {
public:
  virtual std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &Compiler, llvm::StringRef File) {
    return llvm::make_unique<LtASTConsumer>();
  }
};

int main(int argc, char const **argv) {
  clang::tooling::CommonOptionsParser OptionsParser(argc, argv,
                                                    LtOptionCategory);
  auto const v = OptionsParser.getSourcePathList();
  for (auto &&x : v) {
    std::cout << x << "\n";
  }
}
