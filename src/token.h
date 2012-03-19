#ifndef _TOKEN_H_
#define _TOKEN_H_

#define TOKEN_MAX_LENGTH 128

typedef enum {
  TOKEN_INTEGER = 0,
  TOKEN_STRING  = 1,
  TOKEN_IF      = 2,
  TOKEN_ELSE    = 3,
  TOKEN_WHILE   = 4,
  TOKEN_PRINT   = 5,
  TOKEN_END     = 6,
  TOKEN_ID      = 7,
  TOKEN_NUMERO  = 8,
  TOKEN_CADENA  = 9,
  TOKEN_ASIGN   = 10,
  TOKEN_SUM     = 11,
  TOKEN_MUL     = 12,
  TOKEN_REL     = 13,
  TOKEN_NEG     = 14,
  TOKEN_OR      = 15,
  TOKEN_AND     = 16,
  TOKEN_IPAR    = 17,
  TOKEN_DPAR    = 18,
  TOKEN_EOF     = 19
} Tipo;

typedef struct {
  Tipo tipo;
  char nombre[TOKEN_MAX_LENGTH + 1];
} Token;

Token *token_copy(Token *destino, Token *origen);

#endif // _TOKEN_H_

