#include "graph.h"
#include <iostream>
using namespace std;

bool Graph::insertVertex(){
    if(numVertices==maxVertices)
           return false;
       NodeTable[numVertices].data=numVertices;
       numVertices++;   //注意这里自增了，numVertices是结点添加顺序的编号
       return true;
}

bool Graph::insertEdge(int v1,int v2,char c){
    if(v1>-1 && v1<numVertices && v2>-1 && v2<numVertices)
    {
        Edge *p=NodeTable[v1].adj;
        while(p!=NULL && !((p->nextVertex==v2)&&(p->dest==c)))//此时允许图中两结点间存在多条边，但边的值不允许重复
            p=p->link;
        if(p!=NULL) return false;//说明此时v1 v2之间的边已存在

        //头插入，注意
        p=new Edge(c,NodeTable[v1].adj,v2);
        NodeTable[v1].adj=p;

        numEdges++;  return true;
    }
    return 0;
}

int Graph::getFirstNeighbor(int v, char c)
{
   Edge *p =NodeTable[v].adj;
   while(p!=NULL && p->dest!=c)
       p=p->link;

   if(p!=NULL)
       return p->nextVertex;
   else
       return -1;
}

int Graph::getNextNeighbor(int v, int w,char c){
    if(v>-1&&v< numVertices)
        {
            Edge *p=NodeTable[v].adj;
            while(p!=NULL&&p->nextVertex!=w)
                p=p->link;
            p=p->link;
            while(p!=NULL && p->dest !=c)
                 p=p->link;

            if(p == NULL)
                return -1;
            else
                return p->nextVertex;
        }
        return -1;
}

//根据结点序号获取结点的值
int Graph::getValue(int i){
   return i>=0&&i<=numVertices? NodeTable[i].data:-1;
}

//根据结点的值获取结点的序号
int Graph::getVertexPos(int Vertex){
    for(int i=0; i< numVertices; i++)
            {
                if(NodeTable[i].data ==Vertex)
                    return i;
            }
            return -1;
}

int Graph::NumberOfVertices(){
 return numVertices;
}

int Graph::NumberOfEdges(){
 return numEdges;
}


//深度优先搜索
void Graph::DFS(int v,char c,bool visited[])
{
    cout<<this->getValue(v)<<" ";
    visited[v]=true;

    int w=this->getFirstNeighbor(v,c);
    while(w != -1)
    {
        if(visited[w]==false)
            DFS(w,c,visited);

        w=this->getNextNeighbor(v,w,c);
    }
}


void Graph::DFS(int v,char c)
{
    int i,loc,n=this->NumberOfVertices();
    bool *visited= new bool[n];
    for(i=0;i<n;i++)
        visited[i]= false;
    loc= this->getVertexPos(v);

    DFS(loc,c,visited);

    delete []visited;

}

