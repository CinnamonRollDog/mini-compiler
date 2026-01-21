#include "../include/parser.h"
#include <stdexcept>

Parser::Parser(const std::vector<Token> &t) : tokens(t) {}
Token Parser::read()
{
    return i < tokens.size() ? tokens[i] : Token{TokenKind::End, "", 0};
}
// program -> stmt
std::vector<StmtPtr> Parser::parseProgram()
{
    std::vector<StmtPtr> stmts;
    while (read().kind != TokenKind::End)
    {
        auto s = parseStmt();
        if (s)
        {
            stmts.emplace_back(move(s));
        }
        else
        {
            break;
        }
    }
    return stmts;
}
// stmt -> 'print' expr ';' | IDENT '=' expr ';' | expr ';'
StmtPtr Parser::parseStmt()
{
    auto s = read();
    if (s.kind == TokenKind::Print)
    {
        i++;
        auto e = parseExpr();
        if (read().kind == TokenKind::Semicolon)
        {
            i++;
            return std::make_unique<PrintStmt>(move(e));
        }
        else
        {
            throw std::runtime_error("expect ';' after print");
        }
    }
    else if (s.kind == TokenKind::Ident && tokens.size() > i + 1 && tokens[i + 1].kind == TokenKind::Assign)
    {
        i += 2;
        auto e = parseExpr();
        if (read().kind == TokenKind::Semicolon)
        {
            i++;
            return std::make_unique<AssignStmt>(s.text, move(e));
        }
        else
        {
            throw std::runtime_error("expect ';' after assignment");
        }
    }
    else
    {
        auto e = parseExpr();
        if (read().kind == TokenKind::Semicolon)
        {
            i++;
            return std::make_unique<ExprStmt>(move(e));
        }
        else
        {
            throw std::runtime_error("expect ';' after expression");
        }
    }
}
// expr -> addsub
ExprPtr Parser::parseExpr()
{
    return parseAddSub();
}
// addsub -> muldiv ('+'|'-' muldiv)
ExprPtr Parser::parseAddSub()
{
    auto node = parseMulDiv();
    auto t = read();
    while (t.kind == TokenKind::Plus || t.kind == TokenKind::Minus)
    {
        char op = t.kind == TokenKind::Plus ? '+' : '-';
        i++;
        auto rhs = parseMulDiv();
        node = std::make_unique<BinaryExpr>(op, move(node), move(rhs));
        t = read();
    }
    return node;
}
// muldiv -> factor ('*'|'/' factor)
ExprPtr Parser::parseMulDiv()
{
    auto node = parseFactor();
    auto t = read();
    while (t.kind == TokenKind::Mul || t.kind == TokenKind::Div)
    {
        char op = t.kind == TokenKind::Mul ? '*' : '/';
        i++;
        auto rhs = parseFactor();
        node = std::make_unique<BinaryExpr>(op, move(node), move(rhs));
        t = read();
    }
    return node;
}
// factor -> NUMBER | IDENT | '(' expr ')'
ExprPtr Parser::parseFactor()
{
    auto t = read();
    if (t.kind == TokenKind::Number)
    {
        i++;
        return std::make_unique<NumberExpr>(t.val);
    }
    else if (t.kind == TokenKind::Ident)
    {
        i++;
        return std::make_unique<VarExpr>(t.text);
    }
    else if (t.kind == TokenKind::LParen)
    {
        i++;
        auto e = parseExpr();
        if (read().kind == TokenKind::RParen)
        {
            i++;
            return e;
        }
        else
        {
            throw std::runtime_error("expect ')'");
        }
    }
    else
    {
        throw std::runtime_error("unexpected token in factor");
    }
}