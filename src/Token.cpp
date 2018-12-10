#include "include/Token.h"
#include<algorithm>
// #include "include/Logger.h"

using std::string;
using std::vector;
/**
 * 0~10: 关键字
 * 11: const
 * 12: 标识符
 * 13-23: 运算符
 * 24-29: 界符
 **/

char token[1024], ch;
int len;
int line, column;
const string SYMBOL[32] = {
    "CONST",
    "VOID",
    "IF",
    "ELSE",
    "WHILE",
    "main",
    "RETURN",
    "PRINTF",
    "SCANF",
    "INT",
    "FLOAT",
    "CONST_VALUE",
    "IDENTIFIER",
    "ADD",
    "MINUS",
    "MULTIPLY",
    "DIVISION",
    "ASSIGN",
    "LESS",
    "MORE",
    "LESS_EQUAL",
    "MORE_EQUAL",
    "EQUAL",
    "NOT_EQUA",
    "LPAR: (",
    "RPAR: )",
    "LBRA: {",
    "RBRA: }",
    "COMMA: ,",
    "SEMICOLON: ;",
    "",
    "Token error!"
};

void nextChar(FILE *f) {
    ch = fgetc(f);
    column++;
    // putchar(ch);
}

void Token::token_error(string str = "") {
    printf("Token error! %s\n", str.c_str());
    save(31);
}
void deci2bin(const vector<char>& src, vector<char> &ret) {
    int mid = -1;
    for(size_t i = 0; i < src.size(); ++i) {
        if(src[i] == '.') {
            mid = i;
            break;
        }
    }
    if(mid >= 0) { // real->store in double  IEEE
        char s[src.size() + 1];
        memset(s, 0, sizeof s);
        for(size_t i = 0; i < src.size(); ++i) {
            s[i] = src[i];
        }
        double number;
        sscanf(s, "%lf", &number);
        double *p = &number;
        printf("  %.3lf\n", *p);
        unsigned char *ptr = (unsigned char *)p;
        for(int i = 0; i < 8; ++i) {
            int x = *ptr;
            for(int i = 0; i < 8; ++i) {
                ret.push_back(x >> i & 1);
            }
            ptr++;
        }
    } else {
        long long a = 0;
        for(size_t i = 0; i < src.size(); ++i) {
            a = a * 10 + src[i] - '0';
        }
        if(a == 0) {
            ret.push_back(0);
        } else {
            while(a) {
                ret.push_back(a & 1);
                a >>= 1;
            }
        }
        std::reverse(ret.begin(), ret.end());
    }
}
void Token::save(int k) {
    st_token tmp;
    tmp.symbol = (Symbol)k;
    tmp.value.resize(len);
    for(int i = 0; i < len; ++i) {
        tmp.value[i] = token[i];
        tmp.str_value += token[i];
    }
    if(k == CONSTANT) {
        if(token[0] == '.' || (token[0] >= '0' && token[0] <= '9')) {
            deci2bin(tmp.value, tmp.binary);
            for(auto &c : tmp.binary) tmp.str_bin += c != '.' ? c + '0' : c;
            printf("binary : %s\n", tmp.str_bin.c_str());
        } else {
            tmp.str_bin = tmp.str_value;
        }
    }
    tmp.type = SYMBOL[k];
    this->result.push_back(tmp);
}
void retract(FILE *f) {
    if(!feof(f)) fseek(f, -1, SEEK_CUR);
}
bool isEmpty() {
    return ch == '\t' || ch == '\n' || ch == '\r' 
        || ch == ' ' || ch == '\a' || ch == '\b';
}
bool isLetter() {
    return (ch >= 'a' && ch <= 'z') ||
        (ch >= 'A' && ch <= 'Z');
}
bool isDigit() {
    return ch >= '0' && ch <= '9';
}
bool isStar() {
    return ch == '*';
}
void catToken() {
    token[len++] = ch;
}
void clearToken() {
    for(int i = 0; i < len; ++i) token[i] = 0;
    len = 0;
}
bool cmp(char *src, char *token) {
    if(strlen(src) != strlen(token)) return 0;
    for(int i = 0; src[i]; ++i)
        if(src[i] != token[i]) return 0;
    return 1;
}
int reserver() {
    static char a[11][10] = {
        "const", 
        "void", 
        "if", 
        "else", 
        "while", 
        "main", 
        "return", 
        "printf", 
        "scanf", 
        "int", 
        "float"
    };
    for(int i = 0; i < 11; ++i) if(cmp(token, a[i])) return i;
    return -1;
}
Token::Token() {
    result.clear();
}

Token::Token(const string &file_path) {
    path = file_path;
    result.clear();
}

void Token::setpath(const string &file_path) {
    path = file_path;
}

int Token::getsymbol(FILE *f) {
    clearToken();
    while(isEmpty()) {
        nextChar(f);
        if(ch == EOF) return EOF;
    }
    if(ch == '"') {
        catToken();
        nextChar(f);
        while(ch != '"' && ch != EOF) {
            catToken();
            nextChar(f);
        }
        if(ch == EOF) {
            token_error(string("Only a '\"'"));
            return 0;
        }
        catToken();
        save(11);
    } else if(isLetter()) {
        while(isLetter() || isDigit() || ch == '_') {
            catToken();
            nextChar(f);
        }
        retract(f);
        int type = reserver();
        save(type < 0 ? 12 : type);
    } else if(isDigit()) {
        int cnt = 0;
        while(isDigit() || ch == '.') {
            cnt += ch == '.';
            catToken();
            nextChar(f);
        }
        retract(f);
        if(cnt > 1) {
            token_error(string("Too much '.' in a float"));
            return 0;
        } else {
            save(11);
        }
    } else if(ch == '=') {
        catToken();
        nextChar(f);
        if(ch == '=') {
            catToken();
            save(22);
        } else {
            retract(f);
            save(17);
        }
    } else if(ch == '!') {
        catToken();
        nextChar(f);
        catToken();
        if(ch == '=') {
            save(23);
        } else {
            token_error(string("Wrong char after '!': ") + ch);
            return 0;
        }
    } else if(ch == '<') {
        catToken();
        nextChar(f);
        if(ch == '=') {
            catToken();
            save(20);
        } else {
            retract(f);
            save(18);
        }
    } else if(ch == '>') {
        catToken();
        nextChar(f);
        if(ch == '=') {
            catToken();
            save(21);
        } else {
            retract(f);
            save(19);
        }
    } else if(ch == '/') {
        nextChar(f);
        catToken();
        if(isStar()) {
            do {
                while(nextChar(f), !isStar());
                do {
                    nextChar(f);
                    if(ch == '/') return 0;
                } while(isStar());
            } while(!isStar());
        } else {
            retract(f);
            save(16);
        }
    } else {
        catToken();
        switch(ch) {
            case '+':
                save(13); break;
            case '-':
                save(14); break;
            case '*':
                save(15); break;
            case '(':
                save(24); break;
            case ')':
                save(25); break;
            case '{':
                save(26); break;
            case '}':
                save(27); break;
            case ',':
                save(28); break;
            case ';':
                save(29); break;
            default:
                token_error(string("Unknown char: ") + ch);
                return 0;
        }
    }
    return 1;
}

void Token::analyse() {
    f = fopen(path.c_str(), "r");
    if(f == NULL) {
        // Logger.error(string("file not found!"));
        puts("file not found!");
        return;
    }
    while(!feof(f)) {
        nextChar(f);
        if(feof(f)) break;
        getsymbol(f);
    }
    fclose(f);
}
/*
int main() {
    Token tk(string("3.c"));
    tk.analyse();
    tk.test();
    return 0;
}
*/
