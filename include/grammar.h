#ifndef GRAMMAR_H
#define GRAMMAR_H

#include "include/Token.h"

typedef vector<st_token> token_list;
/*
 struct st_token{
    Symbol symbol;
    vector<char> binary, value;
    string str_value, str_bin, type;
    int line, column;
};
*/

class Grammar {
public:
    Grammar();
    Grammar(const token_list&);
    void set_token(const token_list&);
    bool program();
    string Error_message;
private:
    token_list token;
    token_list::iterator it;
    bool nxtsym(st_token &);
    bool checknxt(st_token &, Symbol, const string &);
    bool checkcur(st_token &, Symbol, const string &);


    bool const_statement();
    bool const_define();
    bool Interger();
    bool var_statement();
    bool func_define();
    bool func_call();
    bool main_func();
    bool parameters();
    bool val_parameter();

    bool statement();
    bool statement_sequence();
    bool statement_block();


    bool expression(); // 表达式
    bool term(); // 项
    bool factor(); // 因子

    bool if_statement(); // 条件语句
    bool condition();
    bool assign_statement(); // 赋值语句
    bool while_statement();
    bool read_statement();
    bool write_statement();
    bool return_statement();

};

#endif // GRAMMAR_H
