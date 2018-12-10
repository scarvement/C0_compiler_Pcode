#ifndef TOKEN_H
#define TOKEN_H

#include<fstream>
#include<cstring>
#include<vector>
#include<string>

using std::string;
using std::vector;

enum Symbol{ 
    CONST = 0, VOID = 1, IF = 2, ELSE = 3, WHILE = 4, MAIN = 5,
    RETURN = 6, PRINTF = 7, SCANF = 8,
    INTEGER = 9, FLOAT = 10,
    CONSTANT = 11,
    IDENTIFIER = 12,
    ADD = 13, SUB = 14, MULT = 15, DIV = 16, ASSIGN = 17,
    LESS = 18, MORE = 19, LESSE = 20/*<=*/, MOREE = 21/*>=*/,
    EQU = 22 /*==*/, NEQU = 23 /*!=*/,
    LPAR = 24, RPAR = 25, LBRA = 26/*{*/,
    RBRA = 27/*}*/, COMMA = 28/*,*/, SEMICO = 29/*;*/,
    ERROR = 31
};
struct st_token{
    Symbol symbol;
    vector<char> binary, value;
    string str_value, str_bin, type;
    int line, column;
};

class Token
{
public:
    Token();
    Token(const string&);
    void setpath(const string&);
    void analyse();
    void token_error(string);
    vector<st_token> result;
    void test() {
        for(auto x : result) {
        	for(auto &c : x.value) putchar(c);
            printf("\t%d\n", (int)x.symbol);
        }
    }
private:
    string path;
    FILE *f;
    void save(int);
    int getsymbol(FILE *f);
};

#endif // TOKEN_H
