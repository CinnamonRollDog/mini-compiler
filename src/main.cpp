#include <iostream>
#include <fstream>
#include <string>
#include "../include/interpreter.h"
#include "../include/lexer.h"
#include "../include/parser.h"

using namespace std;
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