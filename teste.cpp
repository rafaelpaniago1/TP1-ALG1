#include <bits/stdc++.h>
using namespace std;

class Graph{
    public:

    int vertices;
    vector<vector<int>> adjList;
    vector<int> colors;
    unordered_map<int, vector<int>> SCC_Components;
    vector<pair<int,int>> batalions;
    int patrols = 0;
    unordered_map<string,int> map;
    unordered_map<int, int> node_index;
    int capital; // Adicionar a capital como membro da classe

    Graph(int i){
        vertices = i;
        adjList.resize(vertices);
        colors.resize(vertices);
    }

    void addEdge(int src, int dest){
        adjList[src].push_back(dest);
    }

    Graph* transpose(){
        Graph* Gt = new Graph(vertices);

        for(int u = 0; u < vertices; u++)
            for(int v : adjList[u]){
                Gt->adjList[v].push_back(u);
            }
        Gt->colors = colors;

        return Gt;
    }

    void Bfs(int startingNode, vector<int>& distances, vector<vector<int>>& adj){
        vector<bool> visited(vertices, false);
        queue<int> q;

        distances[startingNode] = 0;
        q.push(startingNode);

        while(!q.empty()){
            int curr = q.front();
            q.pop();

            if(visited[curr]) continue;
            visited[curr] = true;

            for(int neighbor : adj[curr]){
                if(distances[neighbor] > distances[curr] + 1){
                    distances[neighbor] = distances[curr] + 1;
                    q.push(neighbor);
                } 
            }
        }
    }

    int FindCapital(){
        int currCapital = 0;
        int MinDist = INT_MAX;

        for(int i = 0 ; i < vertices ; i++){
            vector<int> distances(vertices, INT_MAX);
            Bfs(i, distances, adjList);

            int currDist = *max_element(distances.begin(), distances.end());

            if(currDist < MinDist){
                currCapital = i;
                MinDist = currDist;
            } 
        }

        return currCapital;
    }

    void fillOrder(int v, vector<bool>& visited, stack<int>& Stack){
        visited[v] = true;
        for(int neighbor : adjList[v])
            if(!visited[neighbor])
                fillOrder(neighbor, visited, Stack);
        Stack.push(v);
    }

    void DFSUtil(int v, vector<bool>& visited, int color){
        visited[v] = true;
        colors[v] = color;
        SCC_Components[color].push_back(v);
        for(int neighbor : adjList[v])
            if(!visited[neighbor])
                DFSUtil(neighbor, visited, color);
    }

    int Kosaraju() {
    stack<int> Stack;

    vector<bool> visited(vertices, false);

    // Passo 1: Ordem topológica no grafo original
    for (int i = 0; i < vertices; i++)
        if (!visited[i])
            fillOrder(i, visited, Stack);

    // Passo 2: Grafo transposto
    Graph* gr = transpose();

    fill(visited.begin(), visited.end(), false);

    int color = 0;

    // Passo 3: DFS no grafo transposto para identificar SCCs
    while (!Stack.empty()) {
        int v = Stack.top();
        Stack.pop();

        if (!visited[v]) {
            gr->DFSUtil(v, visited, color);

            // Determinar o nó mais próximo da capital dentro do SCC
            int closestToCapital = -1;
            int minDistance = INT_MAX;
            vector<int> distances(vertices, INT_MAX);

            // Calcular distâncias da capital até cada nó do SCC
            Bfs(capital, distances, adjList);
            for (int node : gr->SCC_Components[color]) {
                if (distances[node] < minDistance) {
                    closestToCapital = node;
                    minDistance = distances[node];
                }
            }

            batalions.push_back({closestToCapital, (int)gr->SCC_Components[color].size()});
            if (gr->SCC_Components[color].size() > 1)
                patrols++;
            color++;
        }
    }

    colors = gr->colors;
    SCC_Components = gr->SCC_Components;

    delete gr;
    return color;
}


    void buildCondensationGraph(unordered_map<int, vector<int>>& condensationGraph, unordered_map<int, vector<int>>& condensationGraphReversed) {
        unordered_map<int, unordered_set<int>> cond_adj;
        unordered_map<int, unordered_set<int>> cond_adj_rev;
        for(int u = 0; u < vertices; u++) {
            int cu = colors[u];
            for(int v : adjList[u]) {
                int cv = colors[v];
                if(cu != cv) {
                    cond_adj[cu].insert(cv);
                    cond_adj_rev[cv].insert(cu); // Construir grafo condensado reverso
                }
            }
        }
        for(auto& p : cond_adj) {
            int u = p.first;
            for(int v : p.second) {
                condensationGraph[u].push_back(v);
            }
        }
        for(auto& p : cond_adj_rev) {
            int u = p.first;
            for(int v : p.second) {
                condensationGraphReversed[u].push_back(v);
            }
        }
    }

    void path_patrol(int s, unordered_map<int, string>& reverse_map) {
        int color = colors[s];

        unordered_map<int, vector<int>> scc_adjList;

        for(int u : SCC_Components[color]){
            // Ordenar as arestas para tentar seguir a ordem esperada
            sort(adjList[u].begin(), adjList[u].end(), [&](int a, int b) {
                return reverse_map[a] < reverse_map[b];
            });
            for(int v : adjList[u]){
                if(colors[v] == color){
                    scc_adjList[u].push_back(v);
                }
            }
        }

        // Algoritmo de Hierholzer
        stack<int> curr_path;
        vector<int> circuit;
        unordered_map<int, vector<int>::iterator> edge_iter;

        for(auto& p : scc_adjList){
            edge_iter[p.first] = p.second.begin();
        }

        curr_path.push(s);
        while(!curr_path.empty()){
            int u = curr_path.top();
            if(edge_iter[u] != scc_adjList[u].end()){
                int v = *edge_iter[u];
                edge_iter[u]++;
                curr_path.push(v);
            } else {
                circuit.push_back(u);
                curr_path.pop();
            }
        }

        reverse(circuit.begin(), circuit.end());

        // Remover o último vértice se for igual ao primeiro
        if(circuit.size() > 1 && circuit.front() == circuit.back())
            circuit.pop_back();

        // Imprime o circuito
        for(size_t i = 0; i < circuit.size(); ++i){
            cout << reverse_map[circuit[i]];
            if(i != circuit.size() - 1)
                cout << " ";
        }
        cout << endl;
    }

    void determine_patrols(unordered_map<int,string>& reverse_map){
        cout << patrols << endl;
        for(size_t i = 0; i < batalions.size(); i++)
            if(batalions[i].second > 1){
                int s = batalions[i].first;
                // Se for a capital, usar a capital como início
                if(s != capital)
                    path_patrol(s, reverse_map);
                else
                    path_patrol(capital, reverse_map);
            }
    }
};

int main(){

    int v, e; cin >> v >> e;
    unordered_map<int,string> reverse;
    Graph g(v);

    int counter = 0;
    for(int i = 0; i < e; i++){

        string src, dest;
        cin >> src >> dest;
        if(g.map.find(src) == g.map.end()){
            g.map[src] = counter;
            reverse[counter] = src;
            g.node_index[counter] = i;
            counter++;
        }
        if(g.map.find(dest) == g.map.end()){
            g.map[dest] = counter;
            reverse[counter] = dest;
            g.node_index[counter] = i;
            counter++;
        }
        g.addEdge(g.map[src], g.map[dest]);
    }

    // 1. Encontra a capital
    g.capital = g.FindCapital();
    int capital = g.capital;
    cout << reverse[capital] << endl;

    // 2.1 Encontra SCCs e batalhões
    int n_batalions = g.Kosaraju();
    cout << n_batalions - 1 << endl; // Número de batalhões além da capital

    // 2.2 Imprime batalhões que não podem alcançar a capital
    unordered_map<int, vector<int>> condensationGraph, condensationGraphReversed;
    g.buildCondensationGraph(condensationGraph, condensationGraphReversed);

    int capitalColor = g.colors[capital];
    unordered_set<int> sccsThatReachCapital;
    stack<int> s;
    unordered_set<int> visitedSCCs;
    s.push(capitalColor);
    while(!s.empty()) {
        int curr = s.top(); s.pop();
        if(visitedSCCs.count(curr)) continue;
        visitedSCCs.insert(curr);
        sccsThatReachCapital.insert(curr);
        for(int neighbor : condensationGraphReversed[curr]) {
            if(!visitedSCCs.count(neighbor)) {
                s.push(neighbor);
            }
        }
    }

    // Coletar os batalhões que não podem alcançar a capital
    vector<string> unreachable_battalions;
    for(int i = 0 ; i < n_batalions; i++){
        int batalion = g.batalions[i].first;
        int batalionColor = g.colors[batalion];
        if(sccsThatReachCapital.count(batalionColor) == 0 && batalion != capital)
            unreachable_battalions.push_back(reverse[batalion]);
    }

    // Ordenar os batalhões em ordem lexicográfica
    sort(unreachable_battalions.begin(), unreachable_battalions.end());

    // Imprimir os batalhões ordenados
    for(const auto& name : unreachable_battalions){
        cout << name << endl;
    }

    // 3.1 Determina as patrulhas
    g.determine_patrols(reverse);

    return 0;
}