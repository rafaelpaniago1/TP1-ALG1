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

    int Bfs(int startingNode, vector<vector<int>>& distances){

        vector<int> dist(vertices, -1);
        vector<bool> visited(vertices, false);
        queue<int> q;
        int counter = 0;
        int maxDist = 0;
        
        distances[startingNode][startingNode] = 0;
        q.push(startingNode);

        while(!q.empty()){
            int curr = q.front();
            q.pop();
            if(visited[curr]) continue;
            counter++;
            visited[curr] = true;
            for(auto& neighbor : adjList[curr]){
                if(!visited[neighbor]) q.push(neighbor);
                distances[startingNode][neighbor] = distances[startingNode][curr] + 1;
                maxDist = max(maxDist, distances[startingNode][neighbor]);
            }
        }
        if(counter != vertices) return INF;
        return maxDist;
    }

    int FindCapital(vector<int>& dist_to_capital, vector<vector<int>>& matrix){
        
        int currCapital = 0;
        int MinDist = INF;
        for(int i = 0 ; i < vertices ; i ++){
            int currDist = Bfs(i, matrix);
            dist_to_capital[i] = currDist;
            if(currDist < MinDist){
                currCapital = i;
                MinDist = currDist;
            } 
        }
        return currCapital;
    }

     void SimpleDfs(int startingVertex, vector<bool>& visited){
    
        if(visited[startingVertex]) return;
        visited[startingVertex] = true;

        for(auto& neighbor : adjList[startingVertex]){
            if(!visited[neighbor]) SimpleDfs(neighbor, visited);
        }
    }

    void Dfs(int startingVertex, vector<bool>& visited, stack<int>& s, 
    int& batalion, int& min_dist_to_capital, vector<int>& dist_to_capital){
    
        if(visited[startingVertex]) return;
        visited[startingVertex] = true;

        if(dist_to_capital[startingVertex] < min_dist_to_capital){
            batalion = startingVertex;
            min_dist_to_capital = dist_to_capital[startingVertex];
        }
        for(auto& neighbor : adjList[startingVertex]){
            if(!visited[neighbor]) Dfs(neighbor, visited, s, batalion, min_dist_to_capital, dist_to_capital);
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

    int FirstKosaraju(vector<int>& batalion_name, vector<int>& distances_to_capital){

        vector<bool> visited_1(vertices, false);
        stack<int> s_1;
        int temp1 = 0, temp2 = 0;
        for(int i = 0 ; i < vertices ; i ++){
            Dfs(i, visited_1, s_1, temp1, temp2, distances_to_capital);
        }
        RevertGraph();
        vector<bool> visited_2(vertices, false);
        stack<int> s_2;
        int counter = 0;
        while(!s_1.empty()){
            int curr = s_1.top();
            s_1.pop();
            if(!visited_2[curr]){
                int batalion = curr;
                int batalion_dist_to_capital = distances_to_capital[curr];
                counter++;
                Dfs(curr, visited_2, s_2, batalion, batalion_dist_to_capital, distances_to_capital);
                batalion_name.push_back(batalion);
            }
        }
        RevertGraph();
        return counter;
    }

    void ReachesCapital(int capital, vector<bool>& reaches){

        RevertGraph();
        SimpleDfs(capital, reaches);
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

    vector<vector<int>> matrix(v, vector<int>(v,INF));
    vector<int> dist_from_capital(v, INF);

    int capital = g.FindCapital(dist_from_capital, matrix);
    cout<<reverse[capital]<<endl;

    //2.1
    vector<int> Batalions;
    int n_batalions = g.FirstKosaraju(Batalions, dist_from_capital);
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