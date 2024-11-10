#include <bits/stdc++.h>
#define _ ios_base::sync_with_stdio(0);cin.tie(0);cout.tie(0);
using namespace std;

typedef long long ll;

const int INF = 0x3f3f3f3f;
const ll LINF = 0x3f3f3f3f3f3f3f3f;

class Graph{
    public:

    int vertices;
    vector<list<int>> adjList;
    Graph(int i){
        vertices = i;
        adjList.resize(vertices);
    }
    void addEdge(int src, int dest){
        
        adjList[src].push_back(dest);

    }

    int Bfs(int startingNode){

        vector<int> dist(vertices, -1);
        vector<bool> visited(vertices, false);
        queue<int> q;
        int counter = 0;
        int maxDist = 0;
        
        dist[startingNode] = 0;
        q.push(startingNode);

        while(!q.empty()){
            int curr = q.front();
            q.pop();
            if(visited[curr]) continue;
            counter++;
            visited[curr] = true;
            for(auto& neighbor : adjList[curr]){
                if(!visited[neighbor]) q.push(neighbor);
                dist[neighbor] = dist[curr] + 1;
                maxDist = max(maxDist, dist[neighbor]);
            }
        }       
        if(counter != vertices) return INF;
        return maxDist;
    }

    int FindCapital(){
        
        int currCapital = 0;
        int MinDist = INF;

        for(int i = 0 ; i < vertices ; i ++){
            int currDist = Bfs(i);
            if(currDist < MinDist){
                currCapital = i;
                MinDist = currDist;
            } 
        }
        return currCapital;
    }

    void Dfs(int startingVertex, vector<bool>& visited, stack<int>& s){
        if(visited[startingVertex]) return;
        visited[startingVertex] = true;
        for(auto& neighbor : adjList[startingVertex]){
            if(!visited[neighbor]) Dfs(neighbor, visited, s);
        }
        s.push(startingVertex);
    }
    
    void RevertGraph(){

        vector<list<int>> aux(vertices);
        for(int i = 0 ; i < vertices ; i++){
            for(auto& neighbor : adjList[i]){
                aux[neighbor].push_back(i);
            }
        }
        adjList = aux;
    }

    int FirstKosaraju(vector<int>& batalion_name){

        vector<bool> visited_1(vertices, false);
        stack<int> s_1;
        for(int i = 0 ; i < vertices ; i ++){
            Dfs(i, visited_1, s_1);
        }
        RevertGraph();
        vector<bool> visited_2(vertices, false);
        stack<int> s_2;
        int counter = 0;
        while(!s_1.empty()){
            int curr = s_1.top();
            s_1.pop();
            if(!visited_2[curr]){
                batalion_name.push_back(curr);
                counter++;
                Dfs(curr, visited_2, s_2);
            }
        }
        RevertGraph();
        return counter;
    }

    void ReachesCapital(int capital, vector<bool>& reaches){

        RevertGraph();
        stack<int> temp;
        Dfs(capital, reaches, temp);
        RevertGraph();
    }

};

int main(){

    int v, e; cin>>v>>e;
    unordered_map<string,int> map;
    unordered_map<int,string> reverse;
    Graph g(v);

    int counter = 0;
    while(e--){

        string src, dest;
        cin>>src>>dest;
        if(map.find(src) == map.end()){
            map[src] = counter;
            reverse[counter] = src;
            counter++;
        }
        if(map.find(dest) == map.end()){

            map[dest] = counter;
            reverse[counter] = dest;
            counter++;
        }
        g.addEdge(map[src],map[dest]);
    }
    //1
    int capital = g.FindCapital();
    cout<<reverse[capital]<<endl;

    //2.1
    vector<int> Batalions;
    int n_batalions = g.FirstKosaraju(Batalions);
    cout<<n_batalions - 1<<endl;

    //2.2
    vector<bool> reaches_capital(v, false);
    g.ReachesCapital(capital, reaches_capital);
    
    for(int i = 0 ; i < n_batalions; i ++){
        if(!reaches_capital[Batalions[i]] && Batalions[i] != capital) cout<<reverse[Batalions[i]]<<endl;
    }
    //3.1
    
//PROBLEMAS ATUAIS: Critério de desempate do batalhão e parte 3.

}