#include "clang/AST/ASTContext.h"
#include "clang/Basic/PragmaKinds.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Parse/LoopHint.h"
#include "clang/Parse/ParseDiagnostic.h"
#include "clang/Parse/Parser.h"
#include "clang/Parse/RAIIObjectsForParser.h"
#include "clang/Sema/Scope.h"
#include "llvm/ADT/StringSwitch.h"
#include "pragma_dead_handler.hpp"
#include <sstream>

using namespace clang;

PragmaDeadHandler::PragmaDeadHandler() : PragmaHandler("dead") {}

void PragmaDeadHandler::HandlePragma(clang::Preprocessor &PP,
                                     PragmaIntroducerKind Introducer,
                                     Token &FirstToken) {
  Token Tok;
  std::stringstream DeadDirective;

  if (Tok.is(tok::eod)) {
    return;
  }
  PP.Lex(Tok);
  do {
    DeadDirective << PP.getSpelling(Tok);
    PP.Lex(Tok);
  } while (Tok.isNot(tok::eod));

  llvm::outs() << DeadDirective.str() << "\n";

  abort();
}
