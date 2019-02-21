#ifndef NFAMANAGER_H
#define NFAMANAGER_H
#include "graph.h"
#include <iostream>
#include<bits/stdc++.h>
using namespace std;

class nfaManager
{
private:

    stack<int> st;
    Graph nfa;
    Graph dfa;
    Graph mini_dfa;//最小化dfa
    vector<char> chars;
    int NumOfChar;
    int start_state;//这是nfa的开始结点
    int start_state_dfa;//这是mini_dfa的开始结点

public:

    nfaManager();

    //regexp->nfa

    //正则表达式插入连接符“.”
    string insert_concat(string regexp);
    int priority(char c);
    string regexp_to_postfix(string regexp);
    void character(char c);
    void union_();//处理选择
    void concatenation();//处理连接
    void kleene_star();//闭包
    void postfix_to_nfa(string postfix);
    void show_nfa(string zzz);


   //nfa -> dfa
    void getNeighbor(int v, char c,set<int>&di);
    void epsilon_closure(int state,set<int>&si);
    void nfa_to_dfa(set<int>&si);
    void show_dfa(string zzz);

    //最小化dfa
    int dfa_transform(int v,char c,map<int,vector<int>> mp_1);
    bool if_equal(int v1,int v2,vector<char> chars,map<int,vector<int>> mp_1); //此函数用来判断两个dfa结点的状态转换表是否相同
    void minimize_dfa();
    void show_mini_dfa(string zzz);

    //由最小化dfa生成对应的词法分析程序（C语言描述）
    void getCcode(int v,vector<string> &lines);
    void show_code(string zzz);


};

#endif // NFAMANAGER_H
