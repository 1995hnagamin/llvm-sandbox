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

llvm::StringRef getToken(clang::SourceLocation Loc,
                         clang::CompilerInstance const &Compiler) {
  return clang::Lexer::getSourceText(clang::CharSourceRange::getTokenRange(Loc),
                                     Compiler.getSourceManager(),
                                     Compiler.getLangOpts());
}

void outputSourceRange(clang::SourceRange const &range,
                       clang::CompilerInstance const &Compiler,
                       std::string const &msg) {
  llvm::outs() << msg << ": ";
  range.print(llvm::outs(), Compiler.getSourceManager());
  auto const first = getToken(range.getBegin(), Compiler);
  if (range.getBegin() == range.getEnd()) {
    llvm::outs() << "\n" << first << "\n\n";
    return;
  }
  auto const last = getToken(range.getEnd(), Compiler);
  llvm::outs() << "\n" << first << " ... " << last << "\n\n";
}

class SrcLocVisitor : public clang::RecursiveASTVisitor<SrcLocVisitor> {
public:
  explicit SrcLocVisitor(clang::CompilerInstance *C) : Compiler(C) {}

  bool VisitAttr(clang::Attr *A) {
    auto const Loc = A->getLocation();
    llvm::outs() << "Attr: ";
    Loc.print(llvm::outs(), Compiler->getSourceManager());
    llvm::outs() << "\n" << A->getSpelling() << "\n\n";
    return true;
  }

  bool VisitDecl(clang::Decl *D) {
    auto const range = D->getSourceRange();
    outputSourceRange(range, *Compiler,
                      std::string("Decl(") + D->getDeclKindName() + ")");
    return true;
  }

  bool VisitQualifiedTypeLoc(clang::QualifiedTypeLoc TL) {
    auto const range = TL.getLocalSourceRange();
    outputSourceRange(range, *Compiler, "QualifiedTypeLoc");
    return true;
  }

  bool VisitStmt(clang::Stmt *S) {
    auto const range = S->getSourceRange();
    outputSourceRange(range, *Compiler, "Stmt");
    return true;
  }

  bool VisitTypeLoc(clang::TypeLoc TL) {
    auto const range = TL.getSourceRange();
    outputSourceRange(range, *Compiler, "TypeLoc");
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
