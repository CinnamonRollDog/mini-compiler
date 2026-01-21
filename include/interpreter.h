#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "ast.h"
#include <string>
#include <unordered_map>
#include <vector>
class Interpreter
{
private:
    std::unordered_map<std::string, int> vars;
    int evalExpr(Expr *e);
    void execStmt(Stmt *s);

public:
    void run(const std::vector<StmtPtr> &stmts);
};
#endif