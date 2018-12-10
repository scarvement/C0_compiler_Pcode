#ifndef OPG_H
#define OPG_H

#include<cstdio>
#include<cstring>
#include<string>
#include<vector>

using std::string;
using std::vector;

struct st_OPG_result {
    string a[5];
};

class OPG {
public:
    OPG();
    bool setGrammar(const string&); // 1
    void firstVT();
    void lastVT();
    bool op_init(); //2
    bool analyse(string);
    bool GrammarExist() {
        return hasGrammar;
    }

    vector<st_OPG_result> result;
    vector<vector<char>> fVT, lVT;
    vector<string> grammar;
    vector<char> Vn, Vt;

    // vector<>;
    char op[1024][1024];

private:
    string src;
    int mapVn[256], mapVt[256];
    bool hasGrammar;

    bool inVt(char);
    bool inVn(char);
    void insert(char, char);
    char Statute(const string&);
    bool check_sentence(string &);
    void printStatute(const vector<char>&, const vector<char>&, int, char, char, char, const string&);
};

#endif // OPG_H
