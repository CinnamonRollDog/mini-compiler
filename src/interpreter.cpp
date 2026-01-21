#include "../include/ast.h"
#include "../include/interpreter.h"
#include <iostream>
#include <stdexcept>
int Interpreter::evalExpr(Expr *e)
{
    if (auto p = dynamic_cast<NumberExpr *>(e))
    {
        return p->value;
    }
    else if (auto p = dynamic_cast<VarExpr *>(e))
    {
        if (vars.find(p->name) != vars.end())
        {
            return vars[p->name];
        }
        else
        {
            throw std::runtime_error("undefined variable: " + p->name);
        }
    }
    else if (auto p = dynamic_cast<BinaryExpr *>(e))
    {
        int l = evalExpr(p->lhs.get()), r = evalExpr(p->rhs.get());
        switch (p->op)
        {
        case '+':
            return l + r;
        case '-':
            return l - r;
        case '*':
            return l * r;
        case '/':
            if (r)
            {
                return l / r;
            }
            else
            {
                throw std::runtime_error("divide by zero");
            }
        }
    }
    else
    {
        throw std::runtime_error("unknown expr type");
    }
}
void Interpreter::execStmt(Stmt *s)
{
    if (auto p = dynamic_cast<AssignStmt *>(s))
    {
        vars[p->name] = evalExpr(p->expr.get());
    }
    else if (auto p = dynamic_cast<PrintStmt *>(s))
    {
        std::cout << evalExpr(p->expr.get()) << std::endl;
    }
    else if (auto p = dynamic_cast<ExprStmt *>(s))
    {
        std::cout << evalExpr(p->expr.get()) << std::endl;
    }
    else
    {
        throw std::runtime_error("unknown stmt type");
    }
}
void Interpreter::run(const std::vector<StmtPtr> &stmts)
{
    for (auto &s : stmts)
    {
        execStmt(s.get());
    }
}