#include <bits/stdc++.h>
#define _ ios_base::sync_with_stdio(0);cin.tie(0);cout.tie(0);
using namespace std;

typedef long long ll;

const int INF = 0x3f3f3f3f;
const ll LINF = 0x3f3f3f3f3f3f3f3f;

class Graph{
    public:

    int vertices;
    vector<list<pair<int,int>>> adjList;
    vector<int> colors;
    unordered_map<int, list<int>> SCC_Components;
    vector<pair<int,int>> edges_list;
    vector<pair<int,int>> batalions;
    int E = 0;
    int patrols = 0;

    Graph(int i){
        vertices = i;
        adjList.resize(vertices);
        colors.resize(vertices);
    }

    void addEdge(int src, int dest){
        E++;
        adjList[src].push_back({dest,E});
        edges_list.push_back({src,dest});
        
    }

    Graph* transpose()
{
    Graph* Gt=new Graph(vertices);

    for(int u=0;u<vertices;u++)
        for(auto& edge:adjList[u])
            {
                Gt->adjList[edge.first].emplace_back(u,edge.second);
            }
    Gt->colors=colors;

    for(const auto&edge:edges_list)
        Gt->edges_list.emplace_back(edge.second,edge.first);

    return(Gt);
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
                if(!visited[neighbor.first]){
                    q.push(neighbor.first);
                    distances[startingNode][neighbor.first] = distances[startingNode][curr] + 1;
                    maxDist = max(maxDist, distances[startingNode][neighbor.first]);
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

    //Segunda Dfs usada no Kosaraju, calcula os componenentes conexos e descobre o vértice do batalhão
    //do atual SCC percorrido.
    void Dfs(int startingVertex, vector<bool>& visited, stack<int>& s, 
    int& batalion, int& min_dist_to_capital, vector<vector<int>>& matrix, int capital,
    int& color_control, bool color_bool, int& number_of_components){
    
        if(visited[startingVertex]) return;
        visited[startingVertex] = true;

        number_of_components++;
        if(matrix[capital][startingVertex] < min_dist_to_capital){
            batalion = startingVertex;
            min_dist_to_capital = matrix[capital][startingVertex];
        }
        for(auto& neighbor : adjList[startingVertex]){
            if(!visited[neighbor.first]) Dfs(neighbor.first, visited, s, batalion, min_dist_to_capital, matrix, capital, color_control, color_bool, number_of_components);
        }
        s.push(startingVertex);
        if(color_bool){
            colors[startingVertex] = color_control;
            SCC_Components[color_control].push_back(startingVertex);
        } 
    }

    void reverseGraph() {
    vector<list<pair<int, int>>> reversedAdjList(vertices);

    for (int src = 0; src < vertices; src++) {
        for (auto& edge : adjList[src]) {
            int dest = edge.first;
            reversedAdjList[dest].push_back({src, edge.second});
        }
    }
    adjList = reversedAdjList;
}


    //Função que calcula os SCCS e descobre o vértice que cada batalhão estará.
    //Caso seja necessário vou implementar o negócio de 
    int Kosaraju(vector<vector<int>>& matrix, int capital){
        
        int control_colors = 0;
        vector<bool> visited_1(vertices, false);
        stack<int> s_1;
        int temp1 = 0;
        for(int i = 0 ; i < vertices ; i ++){
            Dfs(i, visited_1, s_1, temp1, temp1, matrix, capital, temp1, false, temp1);
        }
        reverseGraph();
        vector<bool> visited_2(vertices, false);
        stack<int> s_2;
        int counter = 0;
        int color_control = 0;
        while(!s_1.empty()){
            int curr = s_1.top();
            s_1.pop();
            if(!visited_2[curr]){
                int batalion = curr;
                int batalion_dist_to_capital = matrix[capital][curr];
                counter++;
                int number_of_components = 0;
                Dfs(curr, visited_2, s_2, batalion, batalion_dist_to_capital, matrix, capital, color_control, true, number_of_components);
                if(number_of_components != 1)patrols++;
                batalions.push_back({batalion,number_of_components});
            }
            color_control++;
        }
        reverseGraph();
        return counter;
    }
    //PARTE 3
    
    void bfs_parentage(int s, vector<pair<int,int>>&P)
    {
        vector<bool>visit(vertices,0);
        queue<int>q;

        q.push(s);
        visit[s]=1;
        P[s]={-1,-1};

        while(!q.empty())
        {
            int u=q.front();
            q.pop();
            for(const auto&edge:adjList[u])
            {
                int v=edge.first;
                int edge_id=edge.second;
                if(colors[v]==colors[s] and !visit[v])
                {
                    visit[v]=1;

                    P[v] = {u,edge_id};

                    q.push(v);
                }
            }
        }
    }

    // metodo   : path_patrol
    // descricao    : para uma SCC a partir de um batalhao descreve um circuito visitando todas as aretas da SCC
    void path_patrol(int s, unordered_map<int, string> reverse_map) {
    vector<bool> visit(E, false);  // Mark edges as visited
    vector<int> patrol;
    vector<pair<int, int>> from_battalion(vertices), to_battalion(vertices);

    Graph* Gt = transpose();

    bfs_parentage(s, from_battalion);
    Gt->bfs_parentage(s, to_battalion);

    for (int u : SCC_Components[colors[s]]) {
        for (const auto& edge : adjList[u]) {
            if (colors[edge.first] == colors[s] && !visit[edge.second]) {
                int w = u;
                vector<int> path;
                
                while (w != s) {
                    w = from_battalion[w].first;
                    path.push_back(w);
                }
                reverse(path.begin(), path.end());
                path.push_back(u);
                path.push_back(edge.first);
                
                // Mark edges as visited
                visit[edge.second] = true;

                for (int p : path) {
                    patrol.push_back(p);
                }
            }
        }
    }

    // Print patrol path, avoiding duplicates
    for (int i = 0; i < patrol.size() - 1; i++) {
        if (i > 0 && patrol[i] == patrol[i - 1]) continue;  // Avoid duplicates
        cout << reverse_map[patrol[i]] << " ";
    }
    cout << endl;
}


    // metodo   : determine_patrols
    // descricao    : simplesmente faz a chamada de path_patrol para os batalhoes onde se aplica
    void determine_patrols(unordered_map<int,string> reverse_map)
    {
        cout<<patrols<<endl;
        for(size_t i=0; i<batalions.size(); i++)
            if(batalions[i].second>1)
                path_patrol(batalions[i].first, reverse_map);
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

    int capital = g.FindCapital(matrix);
    cout<<reverse[capital]<<endl;

    //2.1
    int n_batalions = g.Kosaraju(matrix, capital);
    cout<<n_batalions - 1<<endl;

    //2.2
    for(int i = 0 ; i < n_batalions; i ++){
        if(matrix[g.batalions[i].first][capital] == INF && g.batalions[i].first != capital) cout<<reverse[g.batalions[i].first]<<endl;
    }
    //3.1
    g.determine_patrols(reverse);
}