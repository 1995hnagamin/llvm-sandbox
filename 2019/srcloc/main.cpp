#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"
#include <memory>
#include <string>

static llvm::cl::OptionCategory SrcLocOptionCategory("SrcLoc options");

void outputSourceRange(clang::SourceRange const &range,
                       clang::SourceManager const &SM, std::string const &msg) {
  llvm::outs() << msg << ": ";
  range.print(llvm::outs(), SM);
  llvm::outs() << "\n";
}

class SrcLocVisitor : public clang::RecursiveASTVisitor<SrcLocVisitor> {
public:
  explicit SrcLocVisitor(clang::CompilerInstance *C) : Compiler(C) {}

  bool VisitDecl(clang::Decl *D) {
    auto const range = D->getSourceRange();
    outputSourceRange(range, Compiler->getSourceManager(), "Decl");
    return true;
  }

private:
  clang::CompilerInstance *Compiler;
};

class SrcLocASTConsumer : public clang::ASTConsumer {
public:
  explicit SrcLocASTConsumer(clang::CompilerInstance *C) : Compiler(C) {}

  virtual void HandleTranslationUnit(clang::ASTContext &Context) override {
    SrcLocVisitor Visitor(Compiler);
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }

private:
  clang::CompilerInstance *Compiler;
};

class SrcLocFrontendAction : public clang::ASTFrontendAction {
public:
  virtual std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &Compiler, llvm::StringRef File) {
    return llvm::make_unique<SrcLocASTConsumer>(&Compiler);
  }
};

int main(int argc, char const **argv) {
  clang::tooling::CommonOptionsParser OptionsParser(argc, argv,
                                                    SrcLocOptionCategory);
  clang::tooling::ClangTool Tool(OptionsParser.getCompilations(),
                                 OptionsParser.getSourcePathList());
  return Tool.run(
      clang::tooling::newFrontendActionFactory<SrcLocFrontendAction>().get());
}
