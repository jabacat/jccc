/**
 * This file defines lexer tokens and their structure. The lexer will produce
 * these tokens after scanning a file.
*/

#pragma once

// The type of a token.
typedef enum {
    TT_LITERAL, // a quote, integer, floating-point thing, etc.
    TT_IDENTIFIER, // an identifier, including user-defined types!
    TT_OPAREN, // (
    TT_CPAREN, // )
    TT_OBRACE, // {
    TT_CBRACE, // }
    TT_OBRACKET, // [
    TT_CBRACKET, // ]
    TT_SEMI, // ;
    TT_NO_TOKEN, // Not a token
	
	TT_PERIOD, // .
	TT_COMMA, // ,
	TT_MINUS, // -
	TT_PLUS, // +
	TT_STAR, // *
	TT_SLASH, // /
	TT_ASSIGN, // =
	TT_COLON, // :
	TT_MOD, // %
	TT_BAND, // &
	TT_LAND, // &&
	TT_BOR, // |
	TT_LOR, // ||
	TT_DEC, // -=
	TT_INC, // +=
	TT_PLUSPLUS, // ++
	TT_MINUSMINUS, // --
	TT_DIVEQ, // /=
	TT_MULEQ, // *=
	TT_MODEQ, // %=
	TT_BANDEQ, // &=
	TT_BOREQ, // |=
	TT_LANDEQ, // &&=
	TT_LOREQ, // ||=
	TT_GREATER, // >
	TT_LESS, // <
	TT_LESSEQ, // <=
	TT_GREATEREQ, // >=
	TT_LEFTSHIFT, // <<
	TT_RIGHTSHIFT, // >>
	TT_LNOT, // !
	TT_LTILDE, // ~
	TT_EQUALS, // ==
	
	TT_AUTO, // auto
	TT_BREAK, // break
	TT_CHAR, // char
	TT_CONST, // const
	TT_CASE, // case
	TT_CONTINUE, // continue
	TT_DOUBLE, // double
	TT_DO, // do
	TT_DEFAULT, // default
	TT_ENUM, // enum
	TT_ELSE, // else
	TT_EXTERN, // extern
	TT_FLOAT, // float
	TT_FOR, // for
	TT_GOTO, // goto
	TT_IF, // if
	TT_INT, // int
	TT_LONG, // long
	TT_RETURN, // return
	TT_REGISTER, // register
	TT_STATIC, // static
	TT_SWITCH, // switch
	TT_SHORT, // short
	TT_SIGNED, // signed
	TT_STRUCT, // struct
	TT_SIZEOF, // sizeof
	TT_typedef, // typedef
	TT_UNSIGNED, // unsigned
	TT_UNION, // union
	TT_VOID, // void
	TT_VOLATILE, // volitile
	TT_WHILE, // while
} TokenType;

// The maximum possible length of a token.
#define TOKEN_LENGTH 256

typedef struct {
    TokenType type; // What type of token this is.
    char contents[TOKEN_LENGTH]; // The actual contents of the token.
    unsigned length; // How long the token is.
} Token;
