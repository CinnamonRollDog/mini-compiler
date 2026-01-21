#include "../include/lexer.h"
#include <cctype>

Lexer::Lexer(std::string s) : src(s) {}
char Lexer::read()
{
    return i < src.size() ? src[i] : '\0';
}
Token Lexer::lexicalAnalysis()
{
    int start = i; // 记录 token 在 src 的开始下表
    char c = read();
    while (isspace(c))
    { // 跳过空格
        i++;
        c = read();
    }
    if (c == '\0')
    {
        return {TokenKind::End, "", start};
    }
    else if (isalpha(c) || c == '_')
    {
        std::string str = "";
        while (isalpha(c) || c == '_')
        {
            str.push_back(c);
            i++;
            c = read();
        }
        if (str == "print")
        {
            return {TokenKind::Print, str, start};
        }
        return {TokenKind::Ident, str, start};
    }
    else if (isdigit(c))
    {
        int v = 0;
        while (isdigit(c))
        {
            v = v * 10 + c - '0';
            i++;
            c = read();
        }
        return {TokenKind::Number, std::to_string(v), start, v};
    }
    else
    {
        i++;
        switch (c)
        {
        case '=':
            return {TokenKind::Assign, "=", start};
        case '(':
            return {TokenKind::LParen, "(", start};
        case ')':
            return {TokenKind::RParen, ")", start};
        case '+':
            return {TokenKind::Plus, "+", start};
        case '-':
            return {TokenKind::Minus, "-", start};
        case '*':
            return {TokenKind::Mul, "*", start};
        case '/':
            return {TokenKind::Div, "/", start};
        case ';':
            return {TokenKind::Semicolon, ";", start};
        default:
            return {TokenKind::Unknown, std::string(1, c), start};
        }
    }
}
std::vector<Token> tokenizeAll(const std::string &src)
{
    Lexer lex(src);
    std::vector<Token> tokens;
    Token t;
    do
    {
        t = lex.lexicalAnalysis();
        tokens.emplace_back(t);
    } while (t.kind != TokenKind::End);
    return tokens;
}