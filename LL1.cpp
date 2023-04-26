#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <queue>
using namespace std;
const int N=1e3+7;

string beg;
map<string,bool>is_end,no_end;

string stk[N];
int top=0;

class Select{
public:
    string left;// 左部
    static const int Size=20;//最大容量
    vector<string>right[Size];//右部
    set<string>select[Size];//select集
    set<string>follow;//follow集
    set<string>first[Size];//first集
    int top=0;//大小

    Select(){}
    void init(string x){
        top=0;
        stringstream sin(x);
        sin>>left;//存下左部
        string b;
        //存下右部
        while(sin>>b) {
            if(b=="|"||sin.eof()) {
                if(b!="|") right[top].push_back(b);
                top++;
            }
            if(b!="|")
                right[top].push_back(b);
        }
    }
    bool calc_select();
    Select* operator[](int size)
    {
        return new Select[size];
    }

    bool updata_first(int x);
    bool calc_first();

    bool updata_follow(int x);
    bool calc_follow();
};

Select se[20];int senum=0;

Select& find(string t)
{
    
    for(int i=0;i<senum;i++) if(se[i].left==t) return se[i];
    return se[senum];
}

bool cpy(const set<string>& a,set<string>& b,string at)
{
    bool flag=0;
    for(auto t:a){
        if(t!=at&&b.find(t)==b.end()){
            flag=1;
            b.insert(t);
        }
    }
    return flag;
}
bool has(set<string>& a,string str)
{
    for(auto t:a){
        if(t==str) return true;
    }
    return false;
}


void pr(const set<string>& a,string x)
{
    cout<<x<<"_________";
    for(auto t:a ) cout<<t<<' ';
    cout<<endl;
}

bool Select::calc_first(){
    int flag=0;
    for(int i=0;i<top;i++){
        if(updata_first(i)) flag=1;
    } 
    for(int i=0;i<top;i++){
        cpy(first[i],first[top],"");
        //合并函数，将左边的合并到右边，第三个参数表示不参与合并的字符串
    }
    return flag;
}

bool Select::updata_first(int x)
{
    string t;int flag=0;
    for(int i=0;i<right[x].size();i++){
        t=right[x][i];

        if(is_end[t]) {//终结符直接加入first
            if(!has(first[x],t)) {//判断当前first是否有t，有就不用更新直接返回
                first[x].insert(t); return true;
            }
            return flag;
        }
        else {//非终结符
            Select& p=find(t);//通过find找到左部是t的Select，然后将t的first集添加到x的first集
            if(cpy(p.first[p.top],first[x],"@"))  flag=true;//注意要排除"@"
            if(has(p.first[p.top],"@")) continue;//如果当前非终结符可以推导出空串，那么就看下一个符号
            else return flag;//否则first集就计算完毕
        }
    }
    if(!has(first[x],"@")){//能走到这一步说明x能推导出空，把"@"加入first
        first[x].insert("@");
        return true;
    }
    return flag;
}

bool Select::calc_follow(){
    int flag=0;
    for(int i=0;i<top;i++){
        if(updata_follow(i)) flag=1;
    } 
    return flag;
}

bool Select::updata_follow(int x)
{
    string t;bool flag=0,isnull=1;
    int i=right[x].size()-1;
    t=right[x][i];
    
    if(no_end[t]){//直接判断最后一位，如果是非终结符，把follow加到t的follow里面
        Select& p=find(t);
        if(cpy(follow,p.follow,"")){
            flag=1;
        }
        if(!has(p.first[p.top],"@")) isnull=0; //不能推出@
        //isnull从位置i开始之后的字符是否可以推导出'@'
    }else isnull=0;//最后一位是终结符，不能推出@

    //从后往前分析，最后一位已经分析过了，从倒数第二位开始分析
    for(int i=right[x].size()-2;i>=0;i--){
        t=right[x][i];
        if(no_end[t]){//当前是非终结符，更新t的follow
            Select& p=find(t);            //获取t的Select
            string tt=right[x][i+1];
            if(is_end[tt]&&!has(p.follow,tt)){//如果下一位是终结符，直接更新: S->Ab
                    flag=1;
                    p.follow.insert(tt);
            }
            else if(no_end[tt]){ //如果下一位是非终结符，S->AB
                Select& pp=find(tt);

                if(has(pp.first[pp.top],"@")&&isnull) {//如果B->@, 并且A之后都能推出@, 那么S.follow合并给A.follow
                    if(cpy(follow,p.follow,"")) flag=1;
                } else{
                    isnull=0;//不能推出@,isnull=0
                }

                //S->ABCDE中,A.follow可能包含B.first C.first D.first...
                for(int j=i+1;j<right[x].size();j++){
                    t=right[x][j];
                    if(is_end[t]){//终结符，直接加入follow集并退出
                        if(!has(p.follow,t)) p.follow.insert(t),flag=1;
                        break;
                    }
                    else {
                    //遇到非终结符,保证前面可以推出空，first合并给A.follow
                        pp = find(t);
                        if(cpy(pp.first[pp.top],p.follow,"@")) flag=1;

                        if(has(pp.first[pp.top],"@")) {
                            continue;//当前非终结符仍然可以推出空，继续看下一个
                        }else break;
                    }
                }
            }
        }
    }

    return flag;
}

bool Select::calc_select()
{
    for(int i=0;i<top;i++){
        if(right[i][0]=="@"||has(first[i],"@")) {// first集包含空串
            cpy(follow,select[i],"");   //select = (first-@) ^ follow
            cpy(first[i],select[i],"@");
        }
        else {
            cpy(first[i],select[i],"@"); //select = follow
        }
    }
    //以上是各个相同左部的产生式的select，以下是合并select
    for(int i=0;i<top;i++){
        for(auto t: select[i]){
            if(select[top].find(t)==select[top].end()) {//如果没有重复就加入，有重复就返回false
                select[top].insert(t);
            }else return false;
        }
    }
    return true;
}



void print(string s,string str)
{
	for(int i=0;i<=top;i++)
		cout<<stk[i];

	cout<<setw(15)<<right<<s;
	cout<<setw(15)<<right<<str<<endl;

}

vector<string> Map(string a,string b)
{
    Select& p=find(a);
    for(int i=0;i<p.top;i++) {
        if(p.select[i].count(b)!=0) return p.right[i];
    }
    cout<<"非终结符"+a+"的select集不包含"+b<<endl;
    throw "error";
}

bool LL1(string s)
{
    top=0; stk[0]='#';
    stk[++top]=beg;
    int len=s.length();
	printf("分析栈    剩余串    产生式\n");
    string t="";
    vector<string>vec;

    int i;
    for(i=0;i<len&&top;i++){
        t=s[i];
        if(!is_end[t]&&!no_end[t]){
			cout<<"非法字符"<<endl;
			return false;
        }
        
        //不断处理非终结符，直到匹配s[i]
        while(top&&!is_end[stk[top]]) {//当前栈顶不是 终结符
            if(!no_end[stk[top]]){//也不是 非终结符
				cout<<"非法字符"<<endl;
				return false;
			}
            try {
                vec=Map(stk[top],s.substr(i,1));//查表
            }
            catch (...){//有异常就没查到
                return false;
            }

            t=""; for(int i=0;i<vec.size();i++) t+=vec[i];//取出查表结果
			print(s.substr(i),string(stk[top]+"-->"+t));//打印过程
            top--;
            if(t=="@") continue;
            for(int i=vec.size()-1;i>=0;i--){//将查表得到的字符串插入栈
                stk[++top]=vec[i];
            }
        }

        if(stk[top][0]!=s[i]) {//栈顶和当前字符不匹配
            cout<<stk[top]<<' '<<s[i]<<"不匹配"<<endl;
            return false;
        }
        print(s.substr(i),s.substr(i,1)+" 匹配");
        top--;
    }
    return true;
}

int main()
{ 
    ifstream ifs;
	ifs.open("./LL1.txt",ios::in);
    string s;
    getline(ifs,s);
    stringstream sin(s);
    sin>>beg;
    no_end[beg]=true;//开始符号
    string a;
    while(sin>>a) no_end[a]=true;//读入非终结符

    sin.clear();getline(ifs,s);sin.str(s);
    while(sin>>a) is_end[a]=true;//读入终结符

    while(getline(ifs,s)){
        if(s[0]=='#') break;
        se[senum++].init(s);//接下来的每一行产生式都交给一个Select
    }


    cout<<"---------FIRST----------"<<endl;    
    
    bool flag=1;
    while(flag) {
        flag=0;
        for(int i=0;i<senum;i++) 
            if(se[i].calc_first()) flag=1;
    }

    for(int i=0;i<senum;i++){
        cout<<"First["<<se[i].left<<"] ";
        for(auto k:se[i].first[se[i].top]) {
            cout<<k<<' ';
        }
        cout<<endl;
    }

    cout<<"---------FOLLOW----------"<<endl;

    flag=1;
    se[0].follow.insert("#");//将#添加到开始符号的follow集
    while(flag) {
        flag=0;
        for(int i=0;i<senum;i++) 
            if(se[i].calc_follow()) flag=1;
    }
    for(int i=0;i<senum;i++){
        cout<<"Follow["<<se[i].left<<"] ";
        for(auto k:se[i].follow) {
            cout<<k<<' ';
        }
        cout<<endl;
    }

    cout<<"---------SELECT----------"<<endl;
    flag=1;
    for(int i=0;i<senum;i++){
        if(!se[i].calc_select()) flag=0;
    }
    if(!flag){
        cout<<"不是LL1文法"<<endl;
        return 0;
    }
    for(int i=0;i<senum;i++){
        for(int j=0;j<se[i].top;j++){
                cout<<"Select["<<se[i].left<<"--->";
                for(auto k:se[i].right[j]){
                cout<<k;
            }
            cout<<"] ";
            for(auto k: se[i].select[j]){
                cout<<k<<' ';
            }
            cout<<endl;
        }
    }
    

    cout<<"----------LL1-----------"<<endl;

    string str;
    while(true){
        cout<<"请输入字符串： ";
        cin>>str;
        if(LL1(str+"#") ){
            cout<<str<<" 是该语言"<<endl;
        }
        else cout<<str<<" 不属于该语言"<<endl;
    }
    
    return 0;
}
