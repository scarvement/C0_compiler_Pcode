#include "include/opg.h"
#include<cstdio>
#include<cstring>
#include<string>
#include<vector>
#include<stack>

// #define GRH_DEBUG
#ifdef GRH_DEBUG

    #include<iostream>
    #include<iomanip>
    #define endl '\n'
    using std::cout;
    using std::setw;

#endif

using std::string;
using std::vector;
using std::stack;
#define pb emplace_back

bool F[512][512]; // Oper for last/first_Vt

bool isBlank(char ch) {
    return ch == '\t' || ch == ' ' || ch == '\a' || ch == '\b';
}
bool Uppercase(char ch) { // Vn
    return ch >= 'A' && ch <= 'Z';
}
bool isEmpty(char ch) {
    return ch == '\t' || ch == '\n' || ch == '\r'
        || ch == ' ' || ch == '\a' || ch == '\b';
}

void OPG::printStatute(
    const vector<char>& res,
    const vector<char>& input,
    int cnt,
    char L,
    char priority,
    char R,
    const string& info
) {
    string Result, str;
    for(auto ch : res) Result += ch;
    for(auto it = input.rbegin(); it != input.rend(); ++it)
        str += *it;
    st_OPG_result tmp;
    tmp.a[0] = std::to_string(cnt);
    tmp.a[1] = Result;
    tmp.a[2] = string(" ") + L + " " + priority + " " + R;
    tmp.a[3] = info;
    tmp.a[4] = str;
    result.push_back(tmp);
#ifdef GRH_DEBUG
    cout << "| " << setw(4) << std::left << cnt
         << "|" << setw(10) << std::left << Result
         << "|" << setw(8) << std::left
         << (string(" ") + L + " " + priority + " " + R)
         << "|" << setw(12) << " " + info
         << "|" << setw(10) << std::right << str
         << "|" << endl;
#endif
}

struct st_Vb {
    char V, b;
} Stack[2048];
int top = 0;

OPG::OPG() {
    hasGrammar = false;
}

bool check_OPG(const string &str) { // is OPG grammar, VnVn is unacceptable.
    if(str.length() < 4) return false;
    if(!Uppercase(str[0])) return false;
    if(str[1] != '-' || str[2] != '>') return false;
    for(size_t i = 3; i < str.length() - 1; ++i)
        if(Uppercase(str[i]) && Uppercase(str[i + 1]))
            return false;
    return true;
}

bool OPG::check_sentence(string &str) { // deal with '\n', ' '; and add '#';
    int len = str.length();
    if(len <= 0) return false;
    if(str[len - 1] != '#') {
        str += "#";
        ++len;
    }
    string tmp;
    for(int i = 0; i < len - 1; ++i) {
        if(str[i] == '#')
            return false;
        if(isBlank(str[i])) continue;
        tmp += str[i];
        if(!inVn(str[i]) && !inVt(str[i]))
            return false;
    }
    str = tmp + "#";
#ifdef GRH_DEBUG
    cout << "sentence: " << str << endl;
#endif
    return true;
}

bool OPG::setGrammar(const string &GRAMMAR) { // Grammar initialize.
    // clear
    Vn.clear();
    Vt.clear();
    grammar.clear();
    memset(mapVn, 0, sizeof mapVn);
    memset(mapVt, 0, sizeof mapVt);
    src = GRAMMAR + '\n';
    // data init & Generate Vn
    int len = src.length();
    string line = "";
    for(int i = 0; i < len; ++i) {
        if(isBlank(src[i])) continue;
        if(src[i] == '\r' || src[i] == '\n') {
            // printf("line: %s\n", line.c_str());
            if(line.length() != 0) {
                if(!check_OPG(line))
                    return false;
                grammar.push_back(line);
                if(!inVn(line[0]))
                    Vn.push_back(line[0]);
            }
            line = "";
            continue;
        }
        line += src[i];
    }

    // split '|'
    for(size_t i = 0; i < grammar.size(); ++i) {
        string &str = grammar[i];
        string left = str.substr(0, 1) + "->";
        // cout << left << ' ' << str << '\n';
        for(size_t j = 3; j < str.length(); ++j) {
            if(str[j] == '|') {
                // cout << "init: " << str << '\n';
                string new_str = left + str.substr(j + 1, str.length() - j - 1);
                grammar.push_back(new_str);
                grammar[i] = grammar[i].substr(0, j);
                // cout << "\tcut1: " << grammar[i] << '\n';
                // cout << "\tcut2: " << new_str << '\n';
                break;
            }
        }
    }
#ifdef GRH_DEBUG
    for(string &str : grammar) {
        cout << "grammar: " << str << '\n';
    }
#endif
    // Generate Vt
    for(const string &str : grammar) {
        for(size_t j = 3; j < str.length(); ++j) {
            if(Uppercase(str[j])) {
                if(!inVn(str[j])) return false;
            } else {
                if(!inVt(str[j])) Vt.push_back(str[j]);
            }
        }
    }

    // mapVn mapVt
    for(size_t i = 0; i < Vn.size(); ++i)
        mapVn[(int)Vn[i]] = i;
    for(size_t i = 0; i < Vt.size(); ++i)
        mapVt[(int)Vt[i]] = i;
    // mapVn['#'] = Vn.size();
    mapVt[(int)'#'] = Vt.size();
    return true;
}


void OPG::firstVT() {
    memset(F, 0, sizeof F);
    for(size_t i = 0; i < grammar.size(); ++i) {
        char U = grammar[i][0];
        for(char b : grammar[i]) {
            if(b == U || b == '-' || b == '>') // || isEmpty(b)
                continue;
            if(inVt(b)) {
                insert(U, b);
                break;
            }
        }
    }
    while(top) {
        char V = Stack[top].V, b = Stack[top].b;
        top--;
        for(auto &list : grammar) {
            char U = list[0];
            if(U != V && list[3] == V) {
                insert(U, b);
            }
        }
    }
    for(size_t i = 0; i < Vn.size(); ++i) {
        fVT.emplace_back(vector<char>(0));
        auto &list = fVT[i];
        for(size_t j = 0; j < Vt.size(); ++j)
            if(F[i][j])
               list.push_back(Vt[j]);
    }
}

void OPG::lastVT() {
    memset(F, 0, sizeof F);
    for(size_t i = 0; i < grammar.size(); ++i) {
        char U = grammar[i][0];
        for(auto it = grammar[i].rbegin(); it != grammar[i].rend(); ++it) {
            char b = *it;
            if(b == U || b == '-' || b == '>') //  || isEmpty(b)
                continue;
            if(inVt(b)) {
                insert(U, b);
                break;
            }
        }
    }
    while(top) {
        char V = Stack[top].V, b = Stack[top].b;
        top--;
        for(auto &list : grammar) {
            char U = list[0];
            if(U != V && V == *list.rbegin()) {
                insert(U, b);
            }
        }
    }
    for(size_t i = 0; i < Vn.size(); ++i) {
        lVT.emplace_back(vector<char>(0));
        auto &list = lVT[i];
        for(size_t j = 0; j < Vt.size(); ++j) {
            if(F[i][j])
                list.emplace_back(Vt[j]);
        }
    }
}

bool OPG::op_init() { // Generate Operator matrix.
    firstVT();
    lastVT();
    memset(op, 0, sizeof op);
    for(const string& str : grammar) {
        int len = str.length();
        for(int j = 3; j < len - 1; ++j) {
            char a = str[j], b = str[j + 1];
            if(inVt(a) && inVt(b)) {
                // int x = mapVt[(int)a], y = mapVt[(int)b];
                auto &Oper = op[mapVt[(int)a]][mapVt[(int)b]];
                if(Oper && Oper != '=') return false;
                Oper = '=';
                // continue;
            }
            if(j < len - 2) {
                char c = str[j + 2];
                if(inVt(a) && inVt(c) && inVn(b)) {
                    auto &Oper = op[mapVt[(int)a]][mapVt[(int)c]];
                    // int x = mapVt[(int)a], y = mapVt[(int)c];
                    if(Oper && Oper != '=') return false;
                    Oper = '=';
                }
            }
            if(inVt(a) && inVn(b)) {
                int tmp = mapVn[(int)b];
                for(char ch : fVT[tmp]) {
                    // int x = mapVt[(int)a], y = mapVt[(int)ch];
                    auto &Oper = op[mapVt[(int)a]][mapVt[(int)ch]];
                    if(Oper && Oper != '<') return false;
                    Oper = '<';
                }
            }
            if(inVn(a) && inVt(b)) {
                int tmp = mapVn[(int)a];
                for(char ch : lVT[tmp]) {
                    // int x = mapVt[(int)b], y = mapVt[(int)ch];
                    auto &Oper = op[mapVt[(int)ch]][mapVt[(int)b]];
                    if(Oper && Oper != '>') return false;
                    Oper = '>';
                }
            }
        }
    }
    int v = mapVt[(int)'#']; // Vt.size()
    for(auto &vt : fVT) {
        for(auto &u : vt) {
            auto &Oper = op[v][mapVt[(int)u]];
            if(Oper && Oper != '<')
                return false;
            Oper = '<';
        }
    }
    for(auto &vt : lVT) {
        for(auto &u : vt) {
            auto &Oper = op[mapVt[(int)u]][v];
            if(Oper && Oper != '>')
                return false;
            Oper = '>';
        }
    }
#ifdef GRH_DEBUG
    cout << "FIRSTVT: \n";
    for(size_t i = 0; i < fVT.size(); ++i) {
        cout << Vn[i] << ":   ";
        for(auto &c : fVT[i]) cout << c << "   ";
        cout << endl;
    }
    cout << endl;
    cout << "LASTVT: \n";
    for(size_t i = 0; i < lVT.size(); ++i) {
        cout << Vn[i] << ":   ";
        for(auto &c : lVT[i]) cout << c << "   ";
        cout << endl;
    }
    cout << endl;
    cout << "     ";
    for(auto c : Vt) {
        cout << ' ' << c << ' ';
    }
    cout << " " << "#" << "  \n";
    for (unsigned int i = 0; i <= Vt.size(); ++i) {
        cout << endl;
        cout << "  ";
        if (i == Vt.size()) cout << '#';
        else cout << Vt[i];
        cout << "  ";
        for (unsigned int j = 0; j <= Vt.size(); ++j)
        {
            // cout << " " << (op[i][j] ? op[i][j] : '/') << " ";
            cout << " " << op[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;
#endif
    hasGrammar = true;
    return true;
}

bool OPG::analyse(std::string sentence) {
    if(!hasGrammar) return false;
    result.clear();
    /*
     * change all Vn into E
    **/
    for(size_t i = 0; i < grammar.size(); ++i) {
        for(size_t j = 0; j < grammar[i].length(); ++j) {
            if(Uppercase(grammar[i][j]))
                grammar[i][j] = 'E';
        }
    }
    vector<char> res;
    res.pb('#');
    vector<char> Tinput(sentence.length());// Carefully. Reversed input string.
    for(size_t i = 0; i < sentence.length(); ++i) {
        Tinput[i] = sentence[sentence.length() - i - 1];
    }

    if(!check_sentence(sentence)) {
        printStatute(res, Tinput, 0, ' ', ' ', ' ', "Sentence failed.");
        return false;
    }
    vector<char> input(sentence.length());// Carefully. Reversed input string.
    for(size_t i = 0; i < sentence.length(); ++i) {
        input[i] = sentence[sentence.length() - i - 1];
    }

#ifdef GRH_DEBUG
    cout << "input string is : " << endl;
    for(auto it = input.rbegin(); it != input.rend(); ++it)
        cout << *it;
    cout << endl;
    cout << " ** All Vn was changed into 'E' **" << endl;
    cout << " ** New grammar is : **" << endl;
    int counter = 0;
    for(string &str : grammar) {
        cout << " **  " << ++counter << ": " << str << '\n';
    }
    cout << "------------------------------------\n\n";
    // 步骤 析栈 关系标志 当前输入符 剩余输入串
#endif
    char left = '#', right = '\0';
    char priority = 0;
    int cnt = 0;
    while(!input.empty() || res.size() >= 3) {
        for(auto it = res.rbegin(); it != res.rend(); ++it) {
            if(!inVn(*it)) {
                left = *it;
                break;
            }
        }
        if(right == '\0') {
            right = input.back();
            input.pop_back();
        }
        int x = mapVt[(int)left], y = mapVt[(int)right];
        priority = op[x][y];
        if(priority == 0) {
            printStatute(res, input, cnt, left, ' ', right, "Statute failed.");
            return false;
        }
        if(priority == '>') { // Statute
            bool success = false;
            // find handle
            for(int i = res.size() - 1; i; --i) { // Try string length to be statuted.
                string handle;
                for(size_t j = i; j < res.size(); ++j) {
                    handle += res[j];
                }
                char statute = Statute(handle);
                if(statute == -1) continue;
                success = true;
                printStatute(res, input, cnt, left, priority, right,
                    "Statute " + handle);
                for(size_t j = 0; j < handle.length(); ++j)
                    res.pop_back();
                res.pb(statute);
                break;
            }
            if(!success) {
                printStatute(res, input, cnt, left, priority, right,
                    "Statute failed.");
               return false;
            }
        } else { // move in
            printStatute(res, input, cnt, left, priority, right,
                string("Move in ") + right);
            res.pb(right);
            right = '\0';
        }
        cnt++;
    }
    printStatute(res, input, cnt, ' ', ' ', ' ',
        "Successful!");
    return true;
}

char OPG::Statute(const string &handle) { // Ignore E->E. All Vn has been changed into E.
    for(size_t i = 0; i < grammar.size(); ++i) {
        auto &list = grammar[i];
        if(list.length() == 4 && list[3] == 'E') continue;
        if(handle.length() != list.size() - 3) continue;
        bool success = true;
        for(size_t j = 0; j < handle.length(); ++j) {
            if(Uppercase(handle[j]) && Uppercase(list[j + 3]))
                continue;
            if(handle[j] != list[j + 3]) {
                success = false;
                break;
            }
        }
        if(success)
            return grammar[i][0];
    }
    return -1;
}


bool OPG::inVt(char ch) {
    for(auto c : Vt)
        if(ch == c) return 1;
    return 0;
}

bool OPG::inVn(char ch) {
    for(auto c : Vn)
        if(ch == c) return 1;
    return 0;
}

void OPG::insert(char U, char b) {
    int x = mapVn[(int)U], y = mapVt[(int)b];
    if(!F[x][y]) {
        F[x][y] = true;
        Stack[++top] = {U, b};
    }
}
/*
#ifdef GRH_DEBUG
int main() { // Testing code without GUI.
    OPG A;
    if(!A.setGrammar("E->E+T|T \n T->T*F|F \n F->(E)|i")) {
        puts("Grammar error!");
        return 0;
    }

    if(!A.op_init()) {
        puts("Grammar error on generating op table!");
        return 0;
    }
    puts("Set grammar & Generating Op_table successfully!");

    puts(A.analyse("i+i*i+i") ? "Successful!" : "Failed!");
    return 0;
}
#endif

*/
