#include <iostream>
#include <algorithm>
#include <cstring>
#include <vector>
#include <fstream>
#include <unordered_map>

using namespace std;
#define ll long long
const int N = 1e3+7;


int cnt;

class DFA{
private:
	int dfa[N][200];
	unordered_map<string,int>id;
	int beg;
	vector<int> end;

	void init(int x);

	void add(const char* a,const char* b,char c){
		dfa[getId(a)][(int)c]=getId(b);
	}
	int getId(const char* x){
		string s(x);
		if(id[s]) return id[s];
		return id[s]=++cnt;
	}
	bool determine(const char* s);
public: 
	DFA(int x){
		init(x);
	}
	bool solve(const char* s){
		return determine(s);
	}
};

void DFA::init(int x)
{
	id.clear();beg=0;
	end.clear();
	ifstream ifs;
	ifs.open("./DFA.txt",ios::in);
	char *a=(char*)malloc(20),*b=(char*)malloc(20); char c;

	ifs>>a>>b;
	beg=getId(a);//开始符号
	while(ifs>>a) end.push_back(getId(a));//结束符号

	while(!ifs.eof()&&strcmp(a,"#")!=0){
		add(a,b,c);
		ifs>>a>>b>>c;
	}
	cout<<"自动机构造完毕"<<endl;
}

bool DFA::determine(const char* s)
{
	int len=strlen(s);
	int now=beg;
	for(int i=0;i<len;i++){
		if(dfa[now][(int)s[i]]==0) return false;
		now = dfa[now][(int)s[i]];
	}
	for(auto t:end){
		if(now==t) return true;
	}
	return false;
}


int main()
{
	DFA dfa(2);
	char *s=(char* )malloc(50);
	while(true){
		cout<<"请输入字符串：";cin>>s;
		if(strcmp(s,"-1")==0) break;

		if(dfa.solve(s)){
			cout<<s<<" 是该语言"<<endl;
		}else cout<<s<<" 不属于该语言"<<endl;
	}
	return 0;
}

