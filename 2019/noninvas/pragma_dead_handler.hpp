#ifndef PRAGMA_DEAD_HANDLER_HPP
#define PRAGMA_DEAD_HANDLER_HPP

#include "clang/Basic/SourceLocation.h"
#include "clang/Lex/Pragma.h"
#include <queue>

struct PragmaDeadHandler : public clang::PragmaHandler {
  PragmaDeadHandler();
  void HandlePragma(clang::Preprocessor &PP, clang::PragmaIntroducerKind Introducer,
                    clang::Token &FirstToken) override;
};

struct Directive {
  std::string Message;
  clang::SourceLocation SrcLoc;
};

extern std::queue<Directive> DirectiveQueue;

#endif /* !PRAGMA_DEAD_HANDLER_HPP */
