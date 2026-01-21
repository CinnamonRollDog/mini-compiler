#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "token.h"
#include <vector>
class Parser
{
private:
    std::vector<Token> tokens;
    int i = 0;
    Token read();
    // stmt -> 'print' expr ';' | IDENT '=' expr ';' | expr ';'
    StmtPtr parseStmt();
    // expr -> addsub
    ExprPtr parseExpr();
    // addsub -> muldiv ('+'|'-' muldiv)
    ExprPtr parseAddSub();
    // muldiv -> factor ('*'|'/' factor)
    ExprPtr parseMulDiv();
    // factor -> NUMBER | IDENT | '(' expr ')'
    ExprPtr parseFactor();

public:
    explicit Parser(const std::vector<Token> &t);
    // program -> stmt
    std::vector<StmtPtr> parseProgram();
};
#endif