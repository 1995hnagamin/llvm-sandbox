#include "pragma_dead_handler.hpp"
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
#include <sstream>

using namespace clang;

PragmaDeadHandler::PragmaDeadHandler(std::queue<Directive> *que)
    : PragmaHandler("dead"), queue(que) {}

void PragmaDeadHandler::HandlePragma(clang::Preprocessor &PP,
                                     PragmaIntroducerKind Introducer,
                                     Token &FirstToken) {
  Token Tok;
  std::stringstream DeadDirective;

  PP.Lex(Tok);
  auto const Loc = Tok.getLocation();
  do {
    DeadDirective << PP.getSpelling(Tok);
    PP.Lex(Tok);
  } while (Tok.isNot(tok::eod));

  queue->push({DeadDirective.str(), Loc});
}
