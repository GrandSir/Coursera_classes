// 101100
//     normal    -fast
// 1:  1.630s    0.577s
// 2:  3.391s    1.251s
// 3:  7.014s    2.887s
// 4: 11.187s    4.163s
// 5: 14:482s    5.698s
// 6: 18.019s    7.402s

#include <iostream>
#include <fstream>
#include <queue>
#include <boost/algorithm/string.hpp>
#include <boost/unordered_map.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;
int times = 0;
int source = 0;

struct node {
    int idx;
    int finish;
    int leader;
    bool visited;
    node(int i = 0, int f = 0, int s = 0, bool v = false) : idx(i), finish(f), leader(s), visited(v) {}
};

inline int get_index(int, int);
void dfs_loop(vector<node> &nodes, vector<node *> &nodes_p, unordered_map<int, vector<int> > &graph);
void dfs(vector<node> &nodes, unordered_map<int, vector<int> > &graph, int s);
void compile_result(vector<node> &nodes);
bool satisfy(vector<node> &nodes, int spacing);

bool compare_node_p(node * a, node *b) {
    return a->finish < b->finish;
}

int main(int argc, const char *argv[]) {
    const char *filename = argv[1];
    
    ifstream input(filename);
    string line;
    vector<string> strs;
    if (!input.good()) {
        cout << "Can't open file" << endl;
        exit(1);
    }

    getline(input, line);
    int spacing = atoi(line.c_str());
    int num_nodes = spacing * 2;
    vector<node> nodes(num_nodes + 1);
    vector<node *> nodes_p(num_nodes + 1);
    for (int i = 1; i < nodes.size(); i++) {
        nodes[i].idx = i;
        nodes_p[i] = &nodes[i];
    }

    unordered_map<int, vector<int> > graph;
    unordered_map<int, vector<int> > r_graph;

    while (!input.eof()) {
        getline(input, line);
        split(strs, line, is_any_of(" "), token_compress_on);
        int x = atoi(strs[0].c_str());
        int y = atoi(strs[1].c_str());
        int n_x = get_index(-x, spacing);
        int n_y = get_index(-y, spacing);
        x = get_index(x, spacing);
        y = get_index(y, spacing);
        graph[n_x].push_back(y);
        r_graph[y].push_back(n_x);
        graph[n_y].push_back(x);
        r_graph[x].push_back(n_y);
    }

    dfs_loop(nodes, nodes_p, r_graph);

    for (int i = 1; i < nodes.size(); i++) {
        nodes[i].visited = false;
        nodes[i].leader = 0;
    }
    cout << "1st dfs done" << endl;
    sort(nodes_p.begin() + 1, nodes_p.end(), compare_node_p);

    dfs_loop(nodes, nodes_p, graph);
    cout << "2nd dfs done" << endl;
    bool s = satisfy(nodes, spacing);
    cout << "result: " << s << endl;
    
    compile_result(nodes);

    nodes.clear();
    nodes_p.clear();
    graph.clear();
    r_graph.clear();
    return 0;
}

int get_index(int i, int spacing) {
    if (i > 0) return i;
    return -i + spacing;
}

void dfs_loop(vector<node> &nodes, vector<node *> &nodes_p, unordered_map<int, vector<int> > &graph) {
    times = 0;
    source = 0;
    for (int j = nodes_p.size() - 1; j > 0; j--) {
        int c = nodes_p[j]->idx;
        if (!nodes[c].visited) {
            source = c;
            dfs(nodes, graph, c);
        }
    }
}

void dfs(vector<node> &nodes, unordered_map<int, vector<int> > &graph, int c) {
    nodes[c].visited = true;
    nodes[c].leader = source;
    
    vector<int> &neighbors = graph[c];
    for (int i = 0; i < neighbors.size(); i++) {
        int a = neighbors[i];
        if (!nodes[a].visited) 
            dfs(nodes, graph, a);
    }

    nodes[c].finish = ++times;
}

void compile_result(vector<node> &nodes) {
    unordered_map<int, int> result;
    for (int i = 1; i < nodes.size(); i++) {
        int s = nodes[i].leader;
        if (result.find(s) == result.end())
           result[s] = 1;
        else
            result[s]++;
    }
    
    priority_queue<int, vector<int>, greater<int> > queue;
    unordered_map<int, int>::iterator it;
    for (it = result.begin(); it != result.end(); it++) {
        int total = it->second;
        if (queue.size() < 5)
            queue.push(total);
        else {
            if (queue.top() < total) {
                queue.pop();
                queue.push(total);
            }
        }
    }
    
    string r = "";
    while (!queue.empty()) {
        r = lexical_cast<string>(queue.top()) + "," + r;
        queue.pop();
    }
    cout << "Top 5 biggest SCC: ";
    cout << r.substr(0, r.size() - 1) << endl;
}

bool satisfy(vector<node> &nodes, int spacing) {
    for (int i = 1; i < nodes.size() / 2 + 1; i++) {
        if (nodes[i].leader == nodes[i + spacing].leader)
            return false;
    }
    return true;
}