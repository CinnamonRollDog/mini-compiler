#ifndef TOKEN_H
#define TOKEN_H

#include <string>
enum class TokenKind
{
    Ident,
    Number,
    Assign,
    LParen,
    RParen,
    Plus,
    Minus,
    Mul,
    Div,
    Print,
    Semicolon,
    End,
    Unknown,
};

struct Token
{
    TokenKind kind;
    std::string text;
    int pos;
    int val = 0;
};
#endif