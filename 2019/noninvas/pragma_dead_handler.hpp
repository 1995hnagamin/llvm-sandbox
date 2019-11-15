#ifndef PRAGMA_DEAD_HANDLER_HPP
#define PRAGMA_DEAD_HANDLER_HPP

#include "clang/Basic/SourceLocation.h"
#include "clang/Lex/Pragma.h"
#include <queue>

struct Directive;

struct PragmaDeadHandler : public clang::PragmaHandler {
  PragmaDeadHandler(std::queue<Directive> *);
  void HandlePragma(clang::Preprocessor &PP,
                    clang::PragmaIntroducerKind Introducer,
                    clang::Token &FirstToken) override;
  std::queue<Directive> *queue;
};

struct Directive {
  std::string Message;
  clang::SourceLocation SrcLoc;
};

#endif /* !PRAGMA_DEAD_HANDLER_HPP */
