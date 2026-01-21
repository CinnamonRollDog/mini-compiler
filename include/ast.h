#ifndef AST_H
#define AST_H

#include <memory>
#include <string>
struct Expr
{
    virtual ~Expr() = default; // 确保正确析构且使类成为多态
};
using ExprPtr = std::unique_ptr<Expr>; // 定义了一个类型别名，表示“指向 Expr 的独占智能指针”
struct VarExpr : Expr
{
    std::string name;
    VarExpr(std::string n) : name(n) {}
};
struct NumberExpr : Expr
{
    int value;
    NumberExpr(int v) : value(v) {}
};
struct BinaryExpr : Expr
{
    char op;
    ExprPtr lhs, rhs;
    BinaryExpr(char o, ExprPtr l, ExprPtr r) : op(o), lhs(move(l)), rhs(move(r)) {}
};

struct Stmt
{
    virtual ~Stmt() = default;
};
using StmtPtr = std::unique_ptr<Stmt>;
struct ExprStmt : Stmt
{
    ExprPtr expr;
    ExprStmt(ExprPtr e) : expr(move(e)) {}
};
struct AssignStmt : Stmt
{
    std::string name;
    ExprPtr expr;
    AssignStmt(std::string n, ExprPtr e) : name(n), expr(move(e)) {}
};
struct PrintStmt : Stmt
{
    ExprPtr expr;
    PrintStmt(ExprPtr e) : expr(move(e)) {}
};
#endif