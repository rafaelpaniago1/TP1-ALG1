#include <bits/stdc++.h>
#define _ ios_base::sync_with_stdio(0); cin.tie(0); cout.tie(0);
using namespace std;

typedef long long ll;
const int INF = 0x3f3f3f3f;

class Graph {
public:
    int vertices;
    vector<list<pair<int, int>>> adjList;
    vector<int> colors;
    unordered_map<int, list<int>> SCC_Components;
    vector<pair<int, int>> edges_list;
    vector<pair<int, int>> batalions;
    int E = 0;
    int patrols = 0;

    Graph(int i) {
        vertices = i;
        adjList.resize(vertices);
        colors.resize(vertices, -1);
    }

    void addEdge(int src, int dest) {
        E++;
        adjList[src].emplace_back(dest, E);
        edges_list.emplace_back(src, dest);
    }

    Graph* transpose() {
        Graph* Gt = new Graph(vertices);
        for (int u = 0; u < vertices; ++u) {
            for (auto& edge : adjList[u]) {
                int v = edge.first;
                Gt->addEdge(v, u);
            }
        }
        return Gt;
    }

    int Bfs(int startingNode, vector<vector<int>>& distances) {
        vector<bool> visited(vertices, false);
        queue<int> q;
        int counter = 0, maxDist = 0;

        distances[startingNode][startingNode] = 0;
        q.push(startingNode);

        while (!q.empty()) {
            int curr = q.front();
            q.pop();

            if (visited[curr]) continue;
            visited[curr] = true;
            counter++;

            for (auto& neighbor : adjList[curr]) {
                int next = neighbor.first;
                if (!visited[next]) {
                    q.push(next);
                    distances[startingNode][next] = distances[startingNode][curr] + 1;
                    maxDist = max(maxDist, distances[startingNode][next]);
                }
            }
        }

        if (counter != vertices) return INF;
        return maxDist;
    }

    int FindCapital(vector<vector<int>>& matrix) {
        int currCapital = 0, MinDist = INF;
        for (int i = 0; i < vertices; ++i) {
            int currDist = Bfs(i, matrix);
            if (currDist < MinDist) {
                currCapital = i;
                MinDist = currDist;
            }
        }
        return currCapital;
    }

    void Dfs(int startingVertex, vector<bool>& visited, stack<int>& s, 
             int& batalion, int& min_dist_to_capital, vector<vector<int>>& matrix, int capital, 
             int& color_control, bool color_bool, int& number_of_components) {
        if (visited[startingVertex]) return;
        visited[startingVertex] = true;
        number_of_components++;

        if (matrix[capital][startingVertex] < min_dist_to_capital) {
            batalion = startingVertex;
            min_dist_to_capital = matrix[capital][startingVertex];
        }

        for (auto& neighbor : adjList[startingVertex]) {
            if (!visited[neighbor.first]) {
                Dfs(neighbor.first, visited, s, batalion, min_dist_to_capital, matrix, capital, color_control, color_bool, number_of_components);
            }
        }
        s.push(startingVertex);
        if (color_bool) {
            colors[startingVertex] = color_control;
            SCC_Components[color_control].push_back(startingVertex);
        }
    }

    int Kosaraju(vector<vector<int>>& matrix, int capital) {
        vector<bool> visited(vertices, false);
        stack<int> s;

        for (int i = 0; i < vertices; ++i) {
            if (!visited[i]) {
                int dummy = 0;
                Dfs(i, visited, s, dummy, dummy, matrix, capital, dummy, false, dummy);
            }
        }

        Graph* Gt = transpose();
        fill(visited.begin(), visited.end(), false);
        int color_control = 0;

        while (!s.empty()) {
            int curr = s.top();
            s.pop();

            if (!visited[curr]) {
                int batalion = curr, min_dist = matrix[capital][curr];
                int num_components = 0;
                Gt->Dfs(curr, visited, s, batalion, min_dist, matrix, capital, color_control, true, num_components);

                if (num_components > 1) patrols++;
                batalions.emplace_back(batalion, num_components);
                color_control++;
            }
        }
        return color_control;
    }

    void bfs_parentage(int s, vector<pair<int, int>>& P) {
        vector<bool> visited(vertices, false);
        queue<int> q;

        q.push(s);
        visited[s] = true;
        P[s] = {-1, -1};

        while (!q.empty()) {
            int u = q.front();
            q.pop();

            for (auto& edge : adjList[u]) {
                int v = edge.first, edge_id = edge.second;
                if (colors[v] == colors[s] && !visited[v]) {
                    visited[v] = true;
                    P[v] = {u, edge_id};
                    q.push(v);
                }
            }
        }
    }

    void path_patrol(int s, unordered_map<int, string>& reverse_map) {
        unordered_set<int> visitedNodes;
        unordered_set<int> visitedEdges;
        vector<int> patrolPath;
        stack<int> stack;

        stack.push(s);

        while (!stack.empty()) {
            int curr = stack.top();
            stack.pop();
    
            visitedNodes.insert(curr);
            patrolPath.push_back(curr);

            for (auto& edge : adjList[curr]) {
                int neighbor = edge.first;
                int edgeId = edge.second;

                if (!visitedEdges.count(edgeId) && colors[neighbor] == colors[s]) {
                    visitedEdges.insert(edgeId);
                    stack.push(neighbor);
                }
            }
        }

        for (int node : patrolPath) {
            cout << reverse_map[node] << " ";
        }
        cout << endl;
    }

    void determine_patrols(unordered_map<int, string>& reverse_map) {
        cout << patrols << endl;
        for (size_t i = 0; i < batalions.size(); i++) {
            if (batalions[i].second > 1) {
                path_patrol(batalions[i].first, reverse_map);
            }
        }
    }
};

int main() {
    int v, e;
    cin >> v >> e;
    unordered_map<string, int> map;
    unordered_map<int, string> reverse;
    Graph g(v);

    int counter = 0;
    while (e--) {
        string src, dest;
        cin >> src >> dest;
        if (map.find(src) == map.end()) {
            map[src] = counter;
            reverse[counter] = src;
            counter++;
        }
        if (map.find(dest) == map.end()) {
            map[dest] = counter;
            reverse[counter] = dest;
            counter++;
        }
        g.addEdge(map[src], map[dest]);
    }

    vector<vector<int>> matrix(v, vector<int>(v, INF));
    int capital = g.FindCapital(matrix);
    cout << reverse[capital] << endl;

    int n_batalions = g.Kosaraju(matrix, capital);
    cout << n_batalions - 1 << endl;

    for (auto& batalion : g.batalions) {
        if (matrix[batalion.first][capital] == INF && batalion.first != capital) {
            cout << reverse[batalion.first] << endl;
        }
    }

    g.determine_patrols(reverse);
    return 0;
}
