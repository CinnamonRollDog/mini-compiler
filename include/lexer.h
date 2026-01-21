#ifndef LEXER_H
#define LEXER_H

#include "token.h"
#include <string>
#include <vector>
class Lexer
{
private:
    std::string src;
    int i = 0;
    char read();

public:
    explicit Lexer(std::string s);
    Token lexicalAnalysis();
};

std::vector<Token> tokenizeAll(const std::string &src);
#endif