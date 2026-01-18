#include <bits/stdc++.h>
using namespace std;

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
    string text;
    int pos;
    int val = 0;
};

class Lexer
{
private:
    string src;
    int i = 0;

public:
    Lexer(string s) : src(s) {}
    char read()
    {
        return i < src.size() ? src[i] : '\0';
    }
    Token lexicalAnalysis()
    {
        int start = i; // 记录 token 在 src 的开始下表
        char c = read();
        while (c == ' ')
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
            string str = "";
            while (isalpha(c))
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
            return {TokenKind::Number, to_string(v), start, v};
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
                return {TokenKind::Unknown, string(1, c), start};
            }
        }
    }
};

struct Expr
{
    virtual ~Expr() = default; // 确保正确析构且使类成为多态
};
using ExprPtr = unique_ptr<Expr>; // 定义了一个类型别名，表示“指向 Expr 的独占智能指针”
struct VarExpr : Expr
{
    string name;
    VarExpr(string n) : name(n) {}
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
using StmtPtr = unique_ptr<Stmt>;
struct ExprStmt : Stmt
{
    ExprPtr expr;
    ExprStmt(ExprPtr e) : expr(move(e)) {}
};
struct AssignStmt : Stmt
{
    string name;
    ExprPtr expr;
    AssignStmt(string n, ExprPtr e) : name(n), expr(move(e)) {}
};
struct PrintStmt : Stmt
{
    ExprPtr expr;
    PrintStmt(ExprPtr e) : expr(move(e)) {}
};
class Parser
{
public:
    vector<Token> tokens;
    int i = 0;
    Parser(const vector<Token> &t) : tokens(t) {}
    Token read()
    {
        return i < tokens.size() ? tokens[i] : Token{TokenKind::End, "", 0};
    }
    // program -> stmt
    vector<StmtPtr> parseProgram()
    {
        vector<StmtPtr> stmts;
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
    StmtPtr parseStmt()
    {
        auto s = read();
        if (s.kind == TokenKind::Print)
        {
            i++;
            auto e = parseExpr();
            if (read().kind == TokenKind::Semicolon)
            {
                i++;
                return make_unique<PrintStmt>(move(e));
            }
            else
            {
                throw runtime_error("expect ';' after print");
            }
        }
        else if (s.kind == TokenKind::Ident && tokens.size() > i + 1 && tokens[i + 1].kind == TokenKind::Assign)
        {
            i += 2;
            auto e = parseExpr();
            if (read().kind == TokenKind::Semicolon)
            {
                i++;
                return make_unique<AssignStmt>(s.text, move(e));
            }
            else
            {
                throw runtime_error("expect ';' after assignment");
            }
        }
        else
        {
            auto e = parseExpr();
            if (read().kind == TokenKind::Semicolon)
            {
                i++;
                return make_unique<ExprStmt>(move(e));
            }
            else
            {
                throw runtime_error("expect ';' after expression");
            }
        }
    }
    // expr -> addsub
    ExprPtr parseExpr()
    {
        return parseAddSub();
    }
    // addsub -> muldiv ('+'|'-' muldiv)
    ExprPtr parseAddSub()
    {
        auto node = parseMulDiv();
        auto t = read();
        while (t.kind == TokenKind::Plus || t.kind == TokenKind::Minus)
        {
            char op = t.kind == TokenKind::Plus ? '+' : '-';
            i++;
            auto rhs = parseMulDiv();
            node = make_unique<BinaryExpr>(op, move(node), move(rhs));
            t = read();
        }
        return node;
    }
    // muldiv -> factor ('*'|'/' factor)
    ExprPtr parseMulDiv()
    {
        auto node = parseFactor();
        auto t = read();
        while (t.kind == TokenKind::Mul || t.kind == TokenKind::Div)
        {
            char op = t.kind == TokenKind::Mul ? '*' : '/';
            i++;
            auto rhs = parseFactor();
            node = make_unique<BinaryExpr>(op, move(node), move(rhs));
            t = read();
        }
        return node;
    }
    // factor -> NUMBER | IDENT | '(' expr ')'
    ExprPtr parseFactor()
    {
        auto t = read();
        if (t.kind == TokenKind::Number)
        {
            i++;
            return make_unique<NumberExpr>(t.val);
        }
        else if (t.kind == TokenKind::Ident)
        {
            i++;
            return make_unique<VarExpr>(t.text);
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
                throw runtime_error("expect ')'");
            }
        }
        else
        {
            throw runtime_error("unexpected token in factor");
        }
    }
};

class Interpreter
{
public:
    unordered_map<string, int> vars;
    int evalExpr(Expr *e)
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
                throw runtime_error("undefined variable: " + p->name);
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
                    throw runtime_error("divide by zero");
                }
            }
        }
        else
        {
            throw runtime_error("unknown expr type");
        }
    }
    void execStmt(Stmt *s)
    {
        if (auto p = dynamic_cast<AssignStmt *>(s))
        {
            vars[p->name] = evalExpr(p->expr.get());
        }
        else if (auto p = dynamic_cast<PrintStmt *>(s))
        {
            cout << evalExpr(p->expr.get()) << endl;
        }
        else if (auto p = dynamic_cast<ExprStmt *>(s))
        {
            cout << evalExpr(p->expr.get()) << endl;
        }
        else
        {
            throw runtime_error("unknown stmt type");
        }
    }
    void run(const vector<StmtPtr> &stmts)
    {
        for (auto &s : stmts)
        {
            execStmt(s.get());
        }
    }
};
vector<Token> tokenizeAll(const string &src)
{
    Lexer lex(src);
    vector<Token> tokens;
    Token t;
    do
    {
        t = lex.lexicalAnalysis();
        tokens.emplace_back(t);
    } while (t.kind != TokenKind::End);
    return tokens;
}
int main(int argc, char **argv)
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    if (argc >= 2)
    { // 从文件读取
        ifstream ifs(argv[1]);
        if (ifs)
        {
            istreambuf_iterator<char> begin(ifs);
            istreambuf_iterator<char> end;
            string input(begin, end);
            try
            {
                vector<Token> tokens = tokenizeAll(input);
                Parser p(tokens);
                vector<StmtPtr> stmts = p.parseProgram();
                Interpreter i;
                i.run(stmts);
            }
            catch (const exception &e)
            {
                cerr << "Error: " << e.what() << endl;
                return 1;
            }
        }
        else
        {
            cerr << "cannot open " << argv[1] << endl;
            return 1;
        }
    }
    else
    { // REPL
        string input, buffer;
        Interpreter i;
        cout << "mini REPL. Type 'exit' to quit. End statements with ';'" << endl;
        while (true)
        {
            cout << ">>" << flush; // flush:即时打印
            if (!getline(cin, input))
            {
                break;
            }
            else if (input == "exit")
            {
                break;
            }
            else
            {
                buffer += input;
                if (input.find(';') != string::npos)
                {
                    try
                    {
                        auto tokens = tokenizeAll(buffer);
                        Parser p(tokens);
                        auto stmts = p.parseProgram();
                        i.run(stmts);
                    }
                    catch (const exception &e)
                    {
                        cerr << "Error: " << e.what() << endl;
                        return 1;
                    }
                    buffer.clear();
                }
            }
        }
    }
}