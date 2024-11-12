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

    // Visita todos os vértices em relação a startingNode e registra a distância deles em distances,
    // para caso seja necessário a adoção de critérios de desempate na decisão do vértice do batalhão.
    int Bfs(int startingNode, vector<vector<int>>& distances){

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
                if(!visited[neighbor]){
                    q.push(neighbor);
                    distances[startingNode][neighbor] = distances[startingNode][curr] + 1;
                    maxDist = max(maxDist, distances[startingNode][neighbor]);
                } 
            }
        }
        if(counter != vertices) return INF;
        return maxDist;
    }

    //Encontra a capital, calcula todas as distancias quaisquer 2 vertices e registra em matrix.
    int FindCapital(vector<vector<int>>& matrix){
        
        int currCapital = 0;
        int MinDist = INF;
        for(int i = 0 ; i < vertices ; i ++){
            int currDist = Bfs(i, matrix);
            if(currDist < MinDist){
                currCapital = i;
                MinDist = currDist;
            } 
        }
        return currCapital;
    }
    //Dfs auxiliar, para quando for necessário uma versão mais simples.
     void SimpleDfs(int startingVertex, vector<bool>& visited){
    
        if(visited[startingVertex]) return;
        visited[startingVertex] = true;

        for(auto& neighbor : adjList[startingVertex]){
            if(!visited[neighbor]) SimpleDfs(neighbor, visited);
        }
    }
    //Segunda Dfs usada no Kosaraju, calcula os componenentes conexos e descobre o vértice do batalhão
    //do atual SCC percorrido.
    void Dfs(int startingVertex, vector<bool>& visited, stack<int>& s, 
    int& batalion, int& min_dist_to_capital, vector<vector<int>>& matrix, int capital){
    
        if(visited[startingVertex]) return;
        visited[startingVertex] = true;

        if(matrix[capital][startingVertex] < min_dist_to_capital){
            batalion = startingVertex;
            min_dist_to_capital = matrix[capital][startingVertex];
        }
        for(auto& neighbor : adjList[startingVertex]){
            if(!visited[neighbor]) Dfs(neighbor, visited, s, batalion, min_dist_to_capital, matrix, capital);
        }
        s.push(startingVertex);
    }
    
    //Função usada no kosaraju para reverter o grafo
    void RevertGraph(){

        vector<list<int>> aux(vertices);
        for(int i = 0 ; i < vertices ; i++){
            for(auto& neighbor : adjList[i]){
                aux[neighbor].push_back(i);
            }
        }
        adjList = aux;
    }

    //Função que calcula os SCCS e descobre o vértice que cada batalhão estará.
    //Caso seja necessário vou implementar o negócio de 
    int FirstKosaraju(vector<int>& batalion_name, vector<vector<int>>& matrix, int capital){

        vector<bool> visited_1(vertices, false);
        stack<int> s_1;
        int temp1 = 0, temp2 = 0;
        for(int i = 0 ; i < vertices ; i ++){
            Dfs(i, visited_1, s_1, temp1, temp2, matrix, capital);
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
                int batalion_dist_to_capital = matrix[capital][curr];
                counter++;
                Dfs(curr, visited_2, s_2, batalion, batalion_dist_to_capital, matrix, capital);
                batalion_name.push_back(batalion);
            }
        }
        RevertGraph();
        return counter;
    }
    //PARTE 3


    void WalkMinimalPath(int target, int startingVertex, 
                         vector<vector<int>>& previous, 
                         unordered_map<int,string>& reverse, 
                         set<pair<int,int>>& walked){
                        
        if(startingVertex == target) return;
        WalkMinimalPath(target, previous[target][startingVertex], previous, reverse, walked);
        cout << reverse[startingVertex]<<" ";
        //MARCAR O WALKED DEPOIS
    }

    void PatrolDFS(int target, int startingVertex, 
                   int ancestor, vector<vector<int>>& previous,
                   vector<bool>& visited){
        
        if(visited[startingVertex]) return;
        visited[startingVertex] = true;
        previous[target][startingVertex] = ancestor;

        for(auto& neighbor : adjList[startingVertex]){

            if(!visited[neighbor]) PatrolDFS(target, neighbor, startingVertex, previous, visited);
        }
    }

    void RecordAllMinimalPaths(vector<vector<int>>& previous){

        for(int i = 0 ; i < vertices; i++){
            vector<bool> visited(vertices, false);
            PatrolDFS(i, i, i, previous, visited);
        }
    }

    //Fazer Segundo Kosaraju, que calcula o SCC, adiciona as arestas a serem percorridas e segue a lógica
    //do Gustavo.  

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

    int capital = g.FindCapital(matrix);
    cout<<reverse[capital]<<endl;

    //2.1
    vector<int> Batalions;
    int n_batalions = g.FirstKosaraju(Batalions, matrix, capital);
    cout<<n_batalions - 1<<endl;

    //2.2
    for(int i = 0 ; i < n_batalions; i ++){
        if(matrix[Batalions[i]][capital] == INF && Batalions[i] != capital) cout<<reverse[Batalions[i]]<<endl;
    }
    //3.1
    vector<vector<int>> previous(v, vector<int>(v,INF));
    g.RecordAllMinimalPaths(previous);
    set<pair<int,int>> walked;
    g.WalkMinimalPath(1 , 2, previous, reverse, walked);
}