#include "include/grammar.h"
#include "include/Token.h"

// 统一规则：每个函数判断前，it已经指向需要判断的当前symbol

Grammar::Grammar()
{
    token.clear();
}

Grammar::Grammar(const token_list &list)
{
    token = list;
    it = token.begin();
}

void Grammar::set_token(const token_list &list)
{
    token = list;
    it = token.begin();
}
/*
enum Symbol{
    CONST = 0, VOID = 1, IF = 2, ELSE = 3, WHILE = 4, MAIN = 5,
    RETURN = 6, PRINTF = 7, SCANF = 8,
    INTEGER = 9, FLOAT = 10,
    CONSTANT = 11,
    IDENTIFIER = 12,
    ADD = 13, SUB = 14, MULT = 15, DIV = 16, ASSIGN = 17,
    LESS = 18, MORE = 19, LESSE = 20, MOREE = 21,
    EQU = 22, NEQU = 23,
    LPAR = 24, RPAR = 25, LBRA = 26{,
    RBRA = 27}, COMMA = 28, SEMICO = 29;,
    ERROR = 31
};
*/

bool Grammar::nxtsym(st_token &u) //获取当前字符到u中，并移向下一指针
{
    if(it == token.end())
        return false;
    u = *it;
    it++;
    return true;
}

bool Grammar::checknxt(st_token &u, Symbol x, const string& err)
{ //获取当前字符到u中，it移向下一指针，判断是否为x
    if(!nxtsym(u))
    {
        Error_message += "";
        return false;
    }
    if(u.symbol != x)
    {
        Error_message += err;
        return false;
    }
    return true;
}

bool Grammar::checkcur(st_token &u, Symbol x, const string& err)
{ //获取当前字符到u中，it不动，判断是否为x
    if(it == token.end())
        return false;
    u = *it;
    if(u.symbol != x)
    {
        Error_message += err;
        return false;
    }
    return true;
}

bool Grammar::const_define() // ok
{
    st_token u;
    if(!checknxt(u, IDENTIFIER, ""))
        return false;
    if(!checknxt(u, EQU, ""))
        return false;
    if(!checknxt(u, INTEGER, ""))
        return false;
    return true;
}

bool Grammar::const_statement() // ok
{
    st_token u;
    if(!checknxt(u, CONST, ""))
        return false;
    if(!const_define())
        return false;
    while(checkcur(u, COMMA, ""))
    {
        checknxt(u, COMMA, "");
        if(!const_define())
            return false;
    }
    if(!checknxt(u, SEMICO, "应包括分号"))
        return false;
    return true;
}

bool Grammar::Interger(string &err)
{
    st_token u;
    bool negative = false;
    if(checkcur(u, ADD, ""))
    {
        checknxt(u, ADD, "");
    }
    if(checkcur(u, SUB, ""))
    {
        checknxt(u, SUB, "");
        negative = true;
    }
    /*
    if(!checkcur(u, CONST, ""))
    {
        err = err + "不是整数";
        return false;
    }*/
    // calc u's integer number
    return true;
}

bool Grammar::var_statement() // ok
{
    st_token u;
    if(!checknxt(u, INTEGER, ""))
        return false;
    if(!checknxt(u, IDENTIFIER, ""))
        return false;
    while(checkcur(u, COMMA, ""))
    {
        checknxt(u, COMMA, "");
        if(!checknxt(u, IDENTIFIER, "逗号后应为标识符"))
            return false;
    }
    if(!checknxt(u, SEMICO, "应包括分号"))
        return false;
    return true;
}

bool Grammar::func_define() // ok
{
    st_token u;
    if(checkcur(u, VOID, "") || checkcur(u, INTEGER, ""))
    {
        ++it;
    }
    if(!checknxt(u, IDENTIFIER, ""))// func_name
        return false;
    if(!parameters()) // parameter can be ()
        return false;
    if(!statement_block())
        return false;
}

bool Grammar::func_call() // ok
{ //func_call ::=  ＜标识符＞‘（’＜值参数表＞‘）’
    st_token u;
    if(!checknxt(u, IDENTIFIER, ""))
        return false;
    if(!checknxt(u, LPAR, ""))
        return false;
    if(!val_parameter())
        return false;
    if(!checknxt(u, RPAR, ""))
        return false;
    return true;
}

bool Grammar::main_func()
{
    st_token u;
}

bool Grammar::parameters() // ok
{
    st_token u;
    if(!checknxt(u, LPAR, ""))
        return false;
    if(checkcur(u, INTEGER, ""))
    {
        checknxt(i, INTEGER, "");
        if(!checknxt(u, IDENTIFIER, ""))
            return false;
        while(checkcur(u, COMMA, ""))
        {
            checknxt(u, COMMA, "");
            if(!checknxt(u, INTEGER, ""))
                return false;
            if(!checknxt(u, IDENTIFIER, ""))
                return false;
        }
    }
    if(!checknxt(u, RPAR, ""))
        return false;
    return true;
}

bool Grammar::val_parameter()
{

}


bool Grammar::statement() // ok but becareful of statement_sequence
{
// ｜＜空＞
//if_state ｜ while_statement ｜‘{’<语句序列>‘}’ ｜＜空＞
//func_call; | assign_state;| return_state;｜read_state;｜write_state;
    st_token u;
    if(checkcur(u, IF, "")) // if
        return if_statement();
    if(checkcur(u, WHILE, "")) // while
        return while_statement();
    if(checkcur(u, IDENTIFIER, "")) // func_call
    {
        // check in func
        // check func_call
        // check ; SEMICO
    }
    if(checkcur(u, LBRA, "")) //{sequence}
    {
        checknxt(u, LBRA, "");
        if(!statement_sequence())
            return false;
    }
    if(checkcur(u, RETURN, "")) // return
        return return_statement();
    if(checkcur(u, SCANF, "")) // read
        return read_statement();
    if(checkcur(u, PRINTF, "")) // write
        return write_statement();
    /////////////////////////////////////////////////////////////becareful of bug!!!
    auto tmp = it;
    if(checknxt(u, IDENTIFIER, "") && checkcur(u, EQU, "")) // assign
    {
        it = tmp;
        return assign_statement();
    }
    return true;
}

bool Grammar::statement_sequence() // ok but be careful of ending
{
    int cnt = 0;
    auto tmp = it;
    while(statement())
    {
        if(tmp == it)
            break;
        cnt++;
    }
    return statement();
}

bool Grammar::statement_block()
{

}

bool Grammar::expression()
{
    st_token u;
    if(!nxtsym(u))
        return 0;
    bool negative = 0;
    if(u.symbol == ADD || u.symbol == SUB)
        negative = u.symbol == SUB ? 1 : 0;
    else // retract
        --it;
    if(term())
    {

    }
    return false;
}

bool Grammar::term()
{
    if(!factor())
    {

    }
    // { */ factor
}

bool Grammar::factor()
{
    st_token u;

    if(checkcur(u, IDENTIFIER, ""))
    {
        it++;
        return true;
    }
}

bool Grammar::assign_statement() // ok
{
    st_token u;
    if(!checknxt(u, IDENTIFIER, ""))
        return false;
    if(!checknxt(u, EQU, ""))
        return false;
    return expression();
}

bool Grammar::if_statement() // ok
{
    st_token u;
    if(!checknxt(u, IF, ""))
        return false;
    if(!checknxt(u, LPAR, ""))
        return false;
    if(!condition())
        return false;
    if(!checknxt(u, RPAR, ""))
        return false;
    if(!statement())
        return false;
    if(!checkcur(u, ELSE, ""))
        return true;
    else
        return statement();
}

bool Grammar::condition() // ok
{
    if(!expression())
        return false;
    auto tmp = it;
    if(relation_operator())
        return expression();
    else
        it = tmp;
    return true;
}

bool Grammar::while_statement() // ok
{
    st_token u;
    if(!checknxt(u, WHILE, ""))
        return false;
    if(!checknxt(u, LPAR, ""))
        return false;
    if(!condition())
        return false;
    if(!checknxt(u, RPAR, ""))
        return false;
    return statement();
}

bool Grammar::read_statement() // ok
{
    st_token u;
    if(!checknxt(u, SCANF, ""))
        return false;
    if(!checknxt(u, LPAR, ""))
        return false;
    if(!checknxt(u, IDENTIFIER, ""))
        return false;
    if(!checknxt(u, RPAR, ""))
        return false;
}

bool Grammar::write_statement()
{
    st_token u;
    if(!checknxt(u, PRINTF, ""))
        return false;
    if(!checknxt(u, LPAR, ""))
        return false;
    // if(!);[<字符串>,][＜expression ＞]
    if(!checknxt(u, RPAR, ""))
        return false;
}

bool Grammar::return_statement()
{
    st_token u;
    if(!checknxt(u, RETURN, ""))
        return false;
    // [ ‘(’＜表达式＞’)’ ]
}
