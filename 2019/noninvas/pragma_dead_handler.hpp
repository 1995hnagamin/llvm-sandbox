#ifndef PRAGMA_DEAD_HANDLER_HPP
#define PRAGMA_DEAD_HANDLER_HPP

struct PragmaDeadHandler : public clang::PragmaHandler {
  PragmaDeadHandler();
  void HandlePragma(clang::Preprocessor &PP, clang::PragmaIntroducerKind Introducer,
                    clang::Token &FirstToken) override;
};

#endif /* !PRAGMA_DEAD_HANDLER_HPP */
