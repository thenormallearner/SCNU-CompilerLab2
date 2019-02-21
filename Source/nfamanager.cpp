#include "nfamanager.h"
#include "graph.h"
#include <QDir>
#include <QFileInfo>
#include<QCoreApplication>
#include <QDebug>
#include <map>

nfaManager::nfaManager()
{
    int NumOfChar=0;
}

//正则表达式插入连接符“.”
string nfaManager::insert_concat(string regexp){
    string ret="";
    char c,c2;
    for(unsigned int i=0; i<regexp.size(); i++){
        c=regexp[i];
        if(i+1<regexp.size()){
            c2=regexp[i+1];
            ret+=c;
            if(c!='('&&c2!=')'&&c!='|'&&c2!='|'&&c2!='*'){
                ret+='.';
            }
        }
    }
    ret+=regexp[regexp.size()-1];//处理原正则表达式最后一个字符
    return ret;
}

int nfaManager::priority(char c){
    if(c=='*')
        return 3;
    else if(c=='.')
        return 2;
    else if(c=='|')
        return 1;
    else return 0;
}

string nfaManager::regexp_to_postfix(string regexp)
{
    string postfix="";
    stack<char> op;
    char c;
    for(unsigned int i=0; i<regexp.size(); i++)
    {
        if((regexp[i]>=48 && regexp[i]<=57)||(regexp[i]>=65 && regexp[i]<=90)
                ||(regexp[i]>=97 && regexp[i]<=122))
            postfix+=regexp[i];
        else
             switch(regexp[i])
            {
            case '(':
                op.push(regexp[i]); break;
            case ')':
                while(op.top()!='('){
                    postfix+=op.top();
                    op.pop();
                }
                op.pop();
                break;
            default:
                while(!op.empty()){
                    c=op.top();
                    if(priority(c)>=priority(regexp[i])){
                        postfix+=op.top();
                        op.pop();
                    }
                    else break;
                    }
                op.push(regexp[i]);
            }
    }
    while(!op.empty())
    {
        postfix += op.top();
        op.pop();
    }
    return postfix;
}

void nfaManager::character(char c)
{
    st.push(nfa.numVertices);
    nfa.insertVertex();
    st.push(nfa.numVertices);
    nfa.insertVertex();
    nfa.insertEdge(nfa.numVertices-2,nfa.numVertices-1,c);
}

void nfaManager::union_()//处理选择
{
    nfa.insertVertex();
    nfa.insertVertex();
    int d = st.top(); st.pop();
    int c = st.top(); st.pop();
    int b = st.top(); st.pop();
    int a = st.top(); st.pop();
    nfa.insertEdge(nfa.numVertices-2,a,'e');
    nfa.insertEdge(nfa.numVertices-2,c,'e');
    nfa.insertEdge(b,nfa.numVertices-1,'e');
    nfa.insertEdge(d,nfa.numVertices-1,'e');
    st.push(nfa.numVertices-2);
    st.push(nfa.numVertices-1);

}

void nfaManager::concatenation()//处理连接
{
    int d = st.top(); st.pop();
    int c = st.top(); st.pop();
    int b = st.top(); st.pop();
    int a = st.top(); st.pop();
    nfa.insertEdge(b,c,'e');
    st.push(a);
    st.push(d);
}

void nfaManager::kleene_star()//闭包
{
    nfa.insertVertex();
    nfa.insertVertex();
    int b = st.top();
    st.pop();
    int a = st.top();
    st.pop();
    nfa.insertEdge(nfa.numVertices-2,nfa.numVertices-1,'e');
    nfa.insertEdge(b,nfa.numVertices-1,'e');
    nfa.insertEdge(nfa.numVertices-2,a,'e');
    nfa.insertEdge(b,a,'e');
    st.push(nfa.numVertices-2);
    st.push(nfa.numVertices-1);
}

void nfaManager::postfix_to_nfa(string postfix)
{
    for(unsigned int i=0; i<postfix.size(); i++)
    {

        if((postfix[i]>=48 && postfix[i]<=57)||(postfix[i]>=65 && postfix[i]<=90)
                ||(postfix[i]>=97 && postfix[i]<=122))
        {
            character(postfix[i]);
            NumOfChar++;
            chars.push_back(postfix[i]);
        }
        else
            switch(postfix[i])
            {
            case '*': kleene_star(); break;
            case '.': concatenation(); break;
            case '|': union_();
        }
    }

   int w = st.top();
   nfa.NodeTable[w].final=1;//标识最后一个结点

   int m=st.top();
   st.pop();
   int n=st.top();
   st.pop();
   st.push(n);
   st.push(m);
   start_state = n;//记录起始结点的位置

}

void nfaManager::show_nfa(string zzz)
{
    string s = zzz;
    s =s+ "\\graph\\nfa.txt";
    cout<<s;
    ofstream out(s);
    out<<"digraph abc{"<<endl;

    for(int i=0;i<nfa.numVertices; i++)
    {
        //不显示结点的名称
        out<<nfa.NodeTable[i].data<<"[fontcolor=white][shape=circle];"<<endl;
    }

    //开始结点显示“begin”，这里发现画图命令后面的会覆盖前面的
    out<<nfa.NodeTable[start_state].data<<"[fontcolor=black][label=begin];"<<endl;

    for(int i=0;i<nfa.numVertices; i++)
    {
        //接受结点显示“acc”
        if(nfa.NodeTable[i].final==1)
            out<<nfa.NodeTable[i].data<<"[fontcolor=black][label=acc];"<<endl;

        Edge *p=nfa.NodeTable[i].adj;
        while(p!=NULL)
        {         
            out<<nfa.NodeTable[i].data<<"->"<<p->nextVertex<<"[label="<<p->dest<<"];"<<endl;
            p=p->link;
        }
    }

    out<<"}";
    out.close();
}

void nfaManager::getNeighbor(int v, char c,set<int>&di)
{
    Edge *p=nfa.NodeTable[v].adj;
    while(p!=NULL)
    {
        if(p->dest==c)
            di.insert(p->nextVertex);
        p=p->link;
    }
}

void nfaManager::epsilon_closure(int v,set<int>&si)
{
    Edge *p=nfa.NodeTable[v].adj;
    while(p!=NULL)
    {
        if(p->dest=='e')
        {
             si.insert(p->nextVertex);
             epsilon_closure(p->nextVertex,si);
        }
        p=p->link;
    }
}

void nfaManager::nfa_to_dfa(set<int>&si)
{
    map<set<int>, int> mp;
    mp[si]=-1;
    queue<set<int> > que;

    si.clear();
    si.insert(start_state);
    int ct=0;

    queue<int> s;

    epsilon_closure(start_state,si);
    if(mp.count(si)==0){
        mp[si]=ct++;
        que.push(si);
        dfa.insertVertex();
        s.push(dfa.numVertices-1);
    }

    for (set<int>::iterator it=si.begin(); it!=si.end(); ++it)
    {
        if(nfa.NodeTable[*it].final==1)
            dfa.NodeTable[0].final=1;
    }

    int h;
    while(que.size()!=0)
    {
        h=s.front();
        s.pop();

        si.empty();
        si=que.front();
        que.pop();

        for(int j=0;j<chars.size();j++)
        {
            set<int> di;
            //先走一步
            for (set<int>::iterator it=si.begin(); it!=si.end(); ++it)
                getNeighbor(*it,chars[j],di);


            set<int> gi;//传递：si -"字母"-> di -"e"-> gi
            //再走一步
            for (set<int>::iterator ite=di.begin(); ite!=di.end(); ++ite)
                 {
                    epsilon_closure(*ite,gi);
                    gi.insert(*ite);
                 }

                if(mp.count(gi)==0)
                {
                    mp[gi]=ct++;
                    que.push(gi);
                    dfa.insertVertex();
                    s.push(dfa.numVertices-1);
                    dfa.insertEdge(h,ct-1,chars[j]);
                    int f1=0;
                    for (set<int>::iterator it=gi.begin(); it!=gi.end(); ++it)
                    {
                        if(nfa.NodeTable[*it].final==1)
                            f1=1;
                    }
                    dfa.NodeTable[dfa.numVertices-1].final=f1;
                }
                else
                {
                    dfa.insertEdge(h,mp[gi],chars[j]);
                }
                di.empty();
                gi.empty();

        }
    }
}

void nfaManager::show_dfa(string zzz)
{
    string s = zzz;
    s =s+ "\\graph\\dfa.txt";
    ofstream out(s);
    out<<"digraph abc{";

    for(int i=0;i<dfa.numVertices; i++)
    {
        //不显示结点的名称
        out<<dfa.NodeTable[i].data<<"[fontcolor=white][shape=circle];"<<endl;//
    }

    //开始结点显示“begin”，这里发现画图命令后面的会覆盖前面的
    out<<dfa.NodeTable[0].data<<"[fontcolor=black][label=begin];"<<endl;

    for(int i=0;i<dfa.numVertices; i++)
    {
        //接受结点显示“acc”
        if(dfa.NodeTable[i].final==1)
            out<<dfa.NodeTable[i].data<<"[fontcolor=black][label=acc];"<<endl;

        Edge *p=dfa.NodeTable[i].adj;
        while(p!=NULL)
        {
            out<<dfa.NodeTable[i].data<<"->"<<p->nextVertex<<"[label="<<p->dest<<"];"<<endl;
            p=p->link;
        }
    }
    out<<"}";
    out.close();
}

int nfaManager::dfa_transform(int v, char c, map<int,vector<int>> mp_1)
{
    Edge *p=dfa.NodeTable[v].adj;
    int node;
    //遍历查看该结点是否有该边
     while(p!=NULL)
    {
        if(p->dest==c)
        {
            node=p->nextVertex;
            break;
        }
        p=p->link;
    }
    //假如该结点没有该边，这里我是这样处理的：该结点在该符号下指向的自己的集合
    //因此让node为本身，这样在接下里的处理，让它返回自身集合
    if(p==NULL)
        node= v;
    //经过以上工作，确定该结点，接下里返回该结点所在的集合（即指向某个vector<int>的指针）
    int q; //所在集合在临时表中的序号
    for(int i=0;i<mp_1.size();i++)
    {
        for(int j=0;j<mp_1[i].size();j++)
        {
            if(mp_1[i][j]==node)
                q = i;
        }
    }
    return q;
}

bool nfaManager::if_equal(int v1, int v2, vector<char> chars, map<int,vector<int>> mp_1)
{
    for(int i=0;i<chars.size();i++)
    {
        if(dfa_transform(v1,chars[i],mp_1)!=dfa_transform(v2,chars[i],mp_1))
            return false;
    }
    return true;
}

void nfaManager::minimize_dfa()
{
    vector<int> s1,s2;//先把所有的dfa结点划分为两个子集：终态（s1）和非终态（s2）
    for(int i=0;i<dfa.numVertices; i++)
    {
        if(dfa.NodeTable[i].final==1)
            s1.push_back(dfa.NodeTable[i].data);
        else
            s2.push_back(dfa.NodeTable[i].data);
    }

    queue<vector<int> > states;  // 等待划分的状态集合队列
    states.push(s1);             //先处理终态集
    states.push(s2);

    //这张映射表是用来在临时划分的结点和dfa结点集建立映射关系的，临时的
    map<int,vector<int>> mp_1;
    int ct_1=0;
    mp_1[ct_1++]=s1;
    mp_1[ct_1++]=s2;

    //这张映射表是用来在mini_dfa结点和dfa结点集建立映射关系的，最终结果的
    map<int,vector<int>> mp;
    int ct=0;

    int pop_num=-1;    //这里是为了记录此时处理的集合在临时表中的序号，一旦该集合需要划分，则要在临时表里删除掉该集合

    //开始划分状态集合
    while(!states.empty())
    {

        vector<int> s= states.front();
        states.pop();
        pop_num++;

        if(s.empty()) continue;
        int sz= s.size();
        if(sz==1)
        {
            mp[ct++]= s;
            continue;
        }

        //用来收集dfa结点序号
        vector<vector<int>> sub_states;

        int node= s[0];
        bool ok = true;

        for(int i=1;i<sz;i++)
        {
            //如果两个结点的转换结果不一样，则需要划分
            if(if_equal(s[i],node,chars,mp_1)==false)
            {
                int cur_node =s[i];
                vector<int> cur_states;
                for(vector<int>::iterator it = s.begin()+1; it != s.end();)
                {
                    if(if_equal(*it,cur_node,chars,mp_1))
                    {
                        cur_states.push_back(*it);  // 加入新集合
                        it =s.erase(it);            // 在s中删除该结点, 更新it为原it的下一个位置
                    }
                    else
                        ++it;
                }
                sub_states.push_back(cur_states);
                // 因为有删除s中的元素，要更新一下s的大小
                sz = s.size();
                ok = false;
            }
        }
        if(ok)
        {
            mp[ct++]=s;
            //输出日志
            cout<<"This is a test:"<<endl;
            cout<<ct-1;
            for(int r=0;r<mp[ct-1].size();r++)
            {
                cout<<mp[ct-1][r]<<" ";
            }
            cout<<endl;
        }
        else
        {
            sub_states.push_back(s);
            for(int l=0;l<sub_states.size();l++)
            {
                states.push(sub_states[l]);
                mp_1[ct_1++]=sub_states[l];
            }
            vector<int> n;
            mp_1[pop_num]=n;
        }
    }

    //获得最终结果的mini_dfa结点和dfa结点集的映射表mp后，下一步生成最小化dfa

    //首先在mini_dfa中插入结点
    for(int i=0;i<mp.size();i++)
    {
        mini_dfa.insertVertex();
    }

    //接着在mini_dfa中插入边
    for(int i=0;i<mp.size();i++)
    {
        //遍历每个mini_dfa结点所对应的dfa结点集中的结点
        for(int j=0;j<mp[i].size();j++)
        {
            //遍历每个dfa结点的边
            Edge *p=dfa.NodeTable[mp[i][j]].adj;
            while(p!=NULL)
            {
                //然后又要遍历每个mini_dfa结点所对应的dfa结点集中的结点，以找出该结点所连接的结点所在的mini_dfa
                for(int k=0;k<mp.size();k++)
                {
                    vector<int>::iterator it;
                    it=std::find(mp[k].begin(),mp[k].end(),p->nextVertex);
                    if(it!=mp[k].end())
                    {
                        mini_dfa.insertEdge(i,k,p->dest);
                        break;
                    }
                }
                p=p->link;
            }
        }
    }

    //再次遍历mini_dfa的结点，这次是为了确定开始结点和接受结点
    for(int i=0;i<mp.size();i++)
    {
        for(int j=0;j<mp[i].size();j++)
        {
            if(dfa.NodeTable[mp[i][j]].data==0)
                start_state_dfa = i;
            if(dfa.NodeTable[mp[i][j]].final==1)
                mini_dfa.NodeTable[i].final = 1;
        }
    }

}

void nfaManager::show_mini_dfa(string zzz)
{
    string s = zzz;
    s =s+ "\\graph\\mini_dfa.txt";
    ofstream out(s);
    out<<"digraph abc{";

    for(int i=0;i<mini_dfa.numVertices; i++)
    {
        //不显示结点的名称
        out<<mini_dfa.NodeTable[i].data<<"[fontcolor=white][shape=circle];"<<endl;
    }

    //开始结点显示“begin”，这里发现画图命令后面的会覆盖前面的
    out<<dfa.NodeTable[start_state_dfa].data<<"[fontcolor=black][label=begin];"<<endl;

    for(int i=0;i<mini_dfa.numVertices; i++)
    {
        //接受结点显示“acc”
        if(mini_dfa.NodeTable[i].final==1)
            out<<mini_dfa.NodeTable[i].data<<"[fontcolor=black][label=acc];"<<endl;

        Edge *p=mini_dfa.NodeTable[i].adj;
        while(p!=NULL)
        {
            out<<mini_dfa.NodeTable[i].data<<"->"<<p->nextVertex<<"[label="<<p->dest<<"];"<<endl;
            p=p->link;
        }
    }
    out<<"}";
    out.close();
}

//由最小化dfa生成对应的词法分析程序（C语言描述）
//从最小化DFA图的起点开始遍历即可，DFA的初态（即起点）一定是唯一的，而接收态（即终点结点）可能有多个（考虑正则表达式”a|b”)。
//进行遍历，如果边指向自己则是while语句，先翻译while语句，然后对于每条前进边都是if-else语句
//此函数通过一步一步分析将生成C语言语句的每一行用一个字符串存储，并将这些字符串存储在vector<string>类型的lines中，方便后续处理，将这些语句写到txt文件
void nfaManager::getCcode(int v, vector<string> &lines)
{
    if(mini_dfa.NodeTable[v].final==1)
        lines.push_back("Done();");

    //参数v为现在处理的结点在mini_dfa图中的序号
    //首先收集结点v有多少条指向自己的边，收集该边的dest
    vector<char> while_char,if_char;
    Edge *p=mini_dfa.NodeTable[v].adj;
    while(p!=NULL)
    {
        if(p->nextVertex==v)
            while_char.push_back(p->dest);
        else
            if_char.push_back(p->dest);
        p=p->link;
    }

    if(!while_char.empty()) //如果不为空，说明存在指向自身的边，这个时候就要生成while语句
    {
        //生成的对应C语言分析程序，首句为“char ch=getChar()”
        lines.push_back("char ch = getChar();");
        string line="while(";
        int i=0;
        for(;i<while_char.size()-1;)
        {
            line +="ch ==";
            string str;
            str+=while_char[i];
            line+=str+"||";
            i++;
        }
        string str;
        str+=while_char[i];
        line+="ch =="+str+")";
        lines.push_back(line);
        lines.push_back("{");
        if(mini_dfa.NodeTable[v].final==1)
            lines.push_back("Done();");
        lines.push_back("ch = getChar();");
        lines.push_back("}");
        if(if_char.empty())
        {
            lines.push_back("error;");
        }
    }

    //处理完指向自身结点的边后，接下里处理指向别的结点的边
    if(!(if_char.empty()))
    {
    if(while_char.empty())
        lines.push_back("char ch = getChar();");
    Edge *q= mini_dfa.NodeTable[v].adj;
    if(q!=NULL)
    {
        while(q!=NULL)
        {
            if(q->nextVertex!=v)
            {
                string line_1="if( ch ==";
                string str_2;
                str_2+=q->dest;
                line_1+=str_2+")";
                lines.push_back(line_1);
                lines.push_back("{");
                getCcode(q->nextVertex,lines);
                lines.push_back("}");
                lines.push_back("else");
            }
            q=q->link;
        }
        string str_3;
        str_3+=48+v;
        string line_2;
        line_2+="error("+str_3+");";
        lines.push_back(line_2);
    }
    }
}


void nfaManager::show_code(string zzz)
{
    vector<string> lines;
    getCcode(mini_dfa.NodeTable[start_state_dfa].data,lines);

    string s = zzz;
   // s =s+ "\\Ccode.txt";
    ofstream out("Ccode.txt");
    for(int i=0;i<lines.size();i++)
    {
       out<<lines[i]<<endl;
    }
    out.close();

}
