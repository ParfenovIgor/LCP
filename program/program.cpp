/// Author: Igor Parfenov

#include <bits/stdc++.h>

using namespace std;

bool Warning_Free_Variable=false;
bool Show_Reductions=false;
bool Show_Variables_Number=false;
enum Reduction_Strategy{
    OUTERMOST,
    INNERMOST
};
Reduction_Strategy Current_Reduction_Strategy=INNERMOST;

bool is_var_char(char c){
    if((c>='A' && c<='Z') ||
       (c>='a' && c<='z') ||
       (c>='0' && c<='9'))
        return true;
    else
        return false;
}

bool equal_case_insensitive(string s,string t){
    if(s.size()!=t.size())
        return false;
    for(int i=0;i<(int)s.size();i++){
        if(s[i]==t[i]) continue;
        if(s[i]>='A' && s[i]<='Z' && t[i]==s[i]-'A'+'a')
            continue;
        if(s[i]>='a' && s[i]<='z' && t[i]==s[i]-'a'+'A')
            continue;
        return false;
    }
    return true;
}

bool is_lambda_term(string s){
    int n=(int)s.size();
    vector < vector <bool> > V(n,vector <bool> (n));
    vector < vector <bool> > L(n,vector <bool> (n));

    for(int len=1;len<=n;len++){
        for(int first=0;first+len-1<n;first++){
            int last=len+first-1;
            if(len==1){
                if(is_var_char(s[first])){
                    V[first][last]=true;
                    L[first][last]=true;
                }
            }
            else{
                if(V[first][last-1] && is_var_char(s[last]))
                    V[first][last]=true;
                if(V[first][last])
                    L[first][last]=true;
                if(s[first]=='\\'){
                    int pos=-1;
                    for(int i=first+1;i<last;i++){
                        if(s[i]=='.'){
                            pos=i;
                            break;
                        }
                    }
                    if(pos!=-1 && V[first+1][pos-1] && L[pos+1][last])
                        L[first][last]=true;
                }
                if(s[first]=='('){
                    int level=0;
                    int pos=-1;
                    for(int i=first;i<last;i++){
                        if(s[i]=='(')
                            level++;
                        if(s[i]==')')
                            level--;
                        if(level==0){
                            pos=i;
                            break;
                        }
                    }
                    if(pos!=-1 && L[first+1][pos-1] && L[pos+1][last])
                        L[first][last]=true;
                }
            }
        }
    }

    return L[0][n-1];
}

void remove_name_collisions(string &s,int &cnt){
    int n=(int)s.size();

    function <string(int)> itostr=[](int n){
        string res;

        if(!n)
            res.push_back('0');

        while(n){
            res.push_back((char)(n%10+'0'));
            n/=10;
        }

        reverse(res.begin(),res.end());
        return res;
    };

    string t;

    int Level=0;
    vector < map <string,string> > name;
    name.push_back({});
    for(int i=0;i<n;i++){
        if(s[i]=='('){
            Level++;
            name.push_back({});
            t.push_back('(');
        }
        else if(s[i]==')'){
            Level--;
            name.pop_back();
            t.push_back(')');
        }
        else if(is_var_char(s[i])){
            string cur;
            cur.push_back(s[i]);
            while(i+1<n && is_var_char(s[i+1])){
                i++;
                cur.push_back(s[i]);
            }
            int pos=-1;
            for(int j=(int)name.size()-1;j>=0;j--){
                if(name[j].count(cur)){
                    pos=j;
                    break;
                }
            }
            if(equal_case_insensitive(cur,"read") || equal_case_insensitive(cur,"write")){
                t+=cur;
            }
            else if(pos==-1){
                Warning_Free_Variable=true;
                t+=itostr(cnt);
                cnt++;
            }
            else{
                t+=name[pos][cur];
            }
        }
        else if(s[i]=='\\'){
            t+="\\";
            i++;
            string cur;
            cur.push_back(s[i]);
            while(i+1<n && is_var_char(s[i+1])){
                i++;
                cur.push_back(s[i]);
            }
            if(name.back().count(cur)){
                throw "Error: several declarations of same argument";
            }
            else{
                name.back()[cur]=itostr(cnt);
                cnt++;
                t+=name.back()[cur];
            }
        }
        else{
            t.push_back(s[i]);
        }
    }

    s=t;
}

void unwrap_recursive_definition(map <string,string> functions,string &fun){
    const int MAX_DEPTH=1000;
    const int MAX_LENGTH=1000000;
    int depth=0;
    while(true){
        string s=fun;
        string t;
        int n=(int)s.size();
        depth++;
        if(depth>MAX_DEPTH || n>MAX_LENGTH){
            throw "Error: could not unwrap recursive definition";
        }
        for(int i=0;i<n;i++){
            if(is_var_char(s[i])){
                string cur;
                cur.push_back(s[i]);
                while(i+1<n && is_var_char(s[i+1])){
                    i++;
                    cur.push_back(s[i]);
                }
                if(functions.count(cur)){
                    t+=functions[cur];
                }
                else{
                    t+=cur;
                }
            }
            else{
                t.push_back(s[i]);
            }
        }
        if(fun==t)
            break;
        fun=t;
    }
}

bool equivalent_lambda_expressions(string a,string b){
    int cnt=0;
    remove_name_collisions(a,cnt);
    cnt=0;
    remove_name_collisions(b,cnt);
    return (a==b);
}

bool apply_reduction(string &s,map <string,string> &functions,int &cnt){
    int n=(int)s.size();

    vector < pair <int,int> > redexes;
    stack <int> open;
    for(int i=0;i<n;i++){
        if(s[i]=='('){
            open.push(i);
        }
        if(s[i]==')'){
            assert(!open.empty());
            if(s[open.top()+1]=='\\')
                redexes.push_back({open.top(),i});
            open.pop();
        }
    }

    if(redexes.empty())
        return false;

    int Best=0;

    if(Current_Reduction_Strategy==INNERMOST){
        for(int i=1;i<(int)redexes.size();i++){
            if(redexes[i].second<redexes[Best].second){
                Best=i;
            }
        }
    }
    if(Current_Reduction_Strategy==OUTERMOST){
        for(int i=1;i<(int)redexes.size();i++){
            if(redexes[i].first<redexes[Best].first){
                Best=i;
            }
        }
    }

    int first=redexes[Best].first;
    int last=redexes[Best].second;

    string t=s.substr(0,first);
    int dot=first+1;
    while(s[dot]!='.' && dot!=last)
        dot++;
    assert(dot<last);

    string arg=s.substr(first+2,dot-first-2);

    int last_application=n-1;
    int level=1;
    for(int i=last+1;i<n;i++){
        if(s[i]=='(')
            level++;
        if(s[i]==')')
            level--;
        if(level==0){
            last_application=i-1;
            break;
        }
    }

    string application=s.substr(last+1,last_application-last);
    if(equal_case_insensitive(application,"read")){
        getline(cin,application);
        unwrap_recursive_definition(functions,application);
        remove_name_collisions(application,cnt);
    }
    else if(equal_case_insensitive(application,"write")){
        string output=s.substr(dot+1,last-dot-1);
        for(auto i : functions){
            if(equivalent_lambda_expressions(i.second,output)){
                output=i.first;
                break;
            }
        }
        cout<<output<<endl;
    }

    for(int i=dot+1;i<last;i++){
        if(is_var_char(s[i])){
            string cur;
            cur.push_back(s[i]);
            while(i+1<last && is_var_char(s[i+1])){
                i++;
                cur.push_back(s[i]);
            }
            if(cur==arg){
                t+=application;
            }
            else{
                t+=cur;
            }
        }
        else{
            t.push_back(s[i]);
        }
    }

    t+=s.substr(last_application+1);

    s=t;

    return true;
}

void crop_string(string &s){
    string t;
    for(int i=0;i<(int)s.size();i++){
        if(s[i]!=' ' && s[i]!='\t'){
            t=s.substr(i);
            break;
        }
    }
    while(t.back()==' ' || t.back()=='\t')
        t.pop_back();
    s=t;
}

void check_declaration(string &s){
    crop_string(s);

    for(int i=0;i<(int)s.size();i++){
        if(s[i]==' '){
            throw "Error: several words in declaration";
        }
    }
}

void remove_spaces_in_lambda_expression(string &s){
    int n=(int)s.size();
    string t;

    for(int i=0;i<n;i++){
        if(i==0){
            while(s[i]==' ')
                i++;
        }
        t.push_back(s[i]);
        char c=s[i];
        while(i+1<n && s[i+1]==' '){
            i++;
        }
        if(s[i]==' ' && i+1<n && is_var_char(c) && is_var_char(s[i+1])){
            cout<<c<<' '<<i<<endl;
            throw "Error: several variables in one brace";
        }
    }

    s=t;
}

void solve_lambda_expression(map <string,string> functions,string &program){
    const int MAX_STEPS=1000000;
    const int MAX_LENGTH=1000000;

    if(!functions.count("MAIN")){
        throw "Error: could not find function MAIN";
    }

    for(auto &fun : functions){
        unwrap_recursive_definition(functions,fun.second);
    }

    program=functions["MAIN"];

    int cnt=0,steps=0;
    remove_name_collisions(program,cnt);
    if(Show_Reductions)
        cout<<"* "<<program<<endl;
    if(Show_Variables_Number)
        cout<<"* "<<cnt<<endl;
    while(apply_reduction(program,functions,cnt)){
        steps++;
        if(steps>MAX_STEPS || (int)program.size()>MAX_LENGTH){
            throw "Error: the expression seems to diverge";
        }

        cnt=0;
        remove_name_collisions(program,cnt);
        if(Show_Reductions)
            cout<<"* "<<program<<endl;
        if(Show_Variables_Number)
            cout<<"* "<<cnt<<endl;
    }

    cnt=0;
    remove_name_collisions(program,cnt);

    string alt;
    for(auto i : functions){
        if(equivalent_lambda_expressions(i.second,program)){
            alt+=i.first+" ";
        }
    }

    if(!alt.empty())
        program=alt;

    if(Show_Reductions)
        cout<<"* "<<program<<endl;

    if(Warning_Free_Variable)
        cerr<<"Warning: free variable detected"<<endl;
    Warning_Free_Variable=false;
}

void load_files(string path,map <string,string> &functions){
    set <string> read_files;
    queue <string> q;

    read_files.insert(path);
    q.push(path);

    while(!q.empty()){
        string filename=q.front();
        q.pop();

        ifstream fin(filename);
        if(!fin){
            throw "Error: could not open file";
        }

        string cur;
        while(getline(fin,cur)){
            if(cur.empty() || cur[0]=='#') continue;
            int n=(int)cur.size();

            bool load=false;

            for(int i=0;i<n;i++){
                if(cur[i]==' '){
                    continue;
                }
                else if(cur[i]=='!'){
                    load=true;
                    cur=cur.substr(i+1);
                    break;
                }
                else{
                    break;
                }
            }

            if(load){
                n=(int)cur.size();
                int first=-1,last=-1;
                for(int i=0;i<n;i++){
                    if(cur[i]!=' '){
                        first=i;
                        break;
                    }
                }
                for(int i=n-1;i>=0;i--){
                    if(cur[i]!=' '){
                        last=i;
                        break;
                    }
                }
                string filename;
                if(cur[first]=='"' && cur[last]=='"')
                    filename=cur.substr(first+1,last-first-1);
                else
                    filename=cur.substr(first,last-first+1);
                if(read_files.find(filename)==read_files.end()){
                    q.push(filename);
                    read_files.insert(filename);
                }
                continue;
            }

            int pos=-1;
            for(int i=0;i+2<n;i++){
                if(cur.substr(i,3)=="::="){
                    pos=i;
                }
            }
            if(pos==-1){
                throw "Error: incorrect declaration";
            }
            string decl=cur.substr(0,pos);
            check_declaration(decl);
            string lamb_exp=cur.substr(pos+3);
            remove_spaces_in_lambda_expression(lamb_exp);
            if(!is_lambda_term(lamb_exp)){
                throw "Error: incorrect lambda expression";
            }
            if(functions.count(decl)){
                cerr<<"Warning: function "+decl+" redeclaration"<<endl;
            }
            functions[decl]=lamb_exp;
        }
    }
}

void process_code(string path){
    map <string,string> functions;

    load_files(path,functions);

    string program;

    solve_lambda_expression(functions,program);
}

bool update_parameters(string cur,bool show_output){
    crop_string(cur);
    if(cur=="+"){
        Show_Reductions=true;
        Show_Variables_Number=false;
        if(show_output)
            cout<<"Show reductions enabled"<<endl;
        return true;
    }
    if(cur=="^"){
        Show_Reductions=false;
        Show_Variables_Number=true;
        if(show_output)
            cout<<"Show variables number enabled"<<endl;
        return true;
    }
    if(cur=="-"){
        Show_Reductions=false;
        Show_Variables_Number=false;
        if(show_output)
            cout<<"Show reductions and variables number disabled"<<endl;
        return true;
    }
    if(cur=="<"){
        Current_Reduction_Strategy=INNERMOST;
        if(show_output)
            cout<<"Innermost reduction strategy enabled"<<endl;
        return true;
    }
    if(cur==">"){
        Current_Reduction_Strategy=OUTERMOST;
        if(show_output)
            cout<<"Outermost reduction strategy enabled"<<endl;
        return true;
    }
    return false;
}

void process_interaction(){
    cout<<"LCP Version 2.0"<<endl;
    cout<<"Lambda Calculus Programming"<<endl;
    cout<<"By Igor Parfenov"<<endl;

    map <string,string> functions;
    string cur;

    while(cin){
        cout<<">> ";
        getline(cin,cur);

        if(cur.empty() || cur[0]=='#') continue;
        int n=(int)cur.size();

        try{
            bool query=false;
            bool load=false;

            if(update_parameters(cur,true))
                continue;

            for(int i=0;i<n;i++){
                if(cur[i]==' '){
                    continue;
                }
                else if(cur[i]=='?'){
                    query=true;
                    cur=cur.substr(i+1);
                    break;
                }
                else if(cur[i]=='!'){
                    load=true;
                    cur=cur.substr(i+1);
                    break;
                }
                else{
                    break;
                }
            }

            if(query){
                remove_spaces_in_lambda_expression(cur);
                functions["MAIN"]=cur;
                string program;
                solve_lambda_expression(functions,program);
                cout<<">> "<<program<<endl;
                functions.erase("MAIN");
                continue;
            }

            if(load){
                crop_string(cur);
                string filename=cur;
                if((int)cur.size()>=2 && cur[0]=='"' && cur.back()=='"'){
                    filename=cur.substr(1,(int)cur.size()-2);
                }
                load_files(filename,functions);
                if(functions.count("MAIN")){
                    cerr<<"Warning: it is not allowed to use MAIN function"<<endl;
                }
                cout<<"File successfully read"<<endl;
                continue;
            }


            int pos=-1;
            for(int i=0;i+2<n;i++){
                if(cur.substr(i,3)=="::="){
                    pos=i;
                }
            }
            if(pos==-1){
                throw "Error: incorrect command";
            }
            string decl=cur.substr(0,pos);
            check_declaration(decl);
            if(decl=="MAIN"){
                cerr<<"Warning: it is not allowed to use MAIN function"<<endl;
            }
            string lamb_exp=cur.substr(pos+3);
            remove_spaces_in_lambda_expression(lamb_exp);
            if(!is_lambda_term(lamb_exp)){
                throw "Error: incorrect lambda expression";
            }
            functions[decl]=lamb_exp;
        }
        catch(const char *error){
            cerr<<error<<endl;
        }
    }
}

int main(int argc,char *argv[]){
    try{
        if(argc==1){
            process_interaction();
        }
        for(int i=1;i<argc;i++){
            string cur(argv[i]);
            update_parameters(cur,false);
        }
        if(argc>=2){
            string path(argv[1]);
            process_code(path);
        }
    }
    catch(const char *error){
        cerr<<error<<endl;
    }

    return 0;
}
