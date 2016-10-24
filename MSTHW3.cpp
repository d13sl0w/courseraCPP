#include <random>
#include <unordered_map>
#include <set>
#include <iomanip>
#include <iostream>
#include <fstream>

/*
 *  Built on modern MacOS, using CMake. To reproduce, please create a CMakeLists.txt file with the following content!
 *   Keep in mind though, you will likely not be working in directory "Coursera", so you will have to make that change.
 *
 *  cmake_minimum_required(VERSION 3.5)
 *  project(Coursera)
 *
 *  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
 *
 *  set(SOURCE_FILES MSTHW3.cpp)
 *  add_executable(Coursera ${SOURCE_FILES})
 *
 */

using namespace std;


class Graph {
    double INF = numeric_limits<double>::infinity();
    int QNAN = numeric_limits<int>::quiet_NaN();
    using min_edge = pair<pair<int, int>, double>;

    struct Vertex {
        int id;
        unordered_map<int, double> edges;
    };

    private:
        vector<Vertex> graph;
        int size;
        string filename;

    public:
        // Default constructor for ad hoc building (here unused)
        Graph() : graph() {};

        // Creates graph, but prepopulates with vertices since size is known.
        Graph(int size) : size(size), graph() {
            for (int i = 0; i < size; ++i) { graph.push_back(Vertex{i}); }
        };

        // Creates graph via filename pointing to file in the format:
        // line 1: size_of_graph
        // line 2: vertex_1 vertex_2 edge_weight
        //  ...
        Graph(string filename) : filename(filename), graph() {
            ifstream infile(filename);
            infile >> size;
            for (int i = 0; i < size; ++i) { graph.push_back(Vertex{i}); }
            int vertex_a, vertex_b, edge_weight;
            while (infile >> vertex_a >> vertex_b >> edge_weight) {
                graph[vertex_a].edges[vertex_b] = edge_weight;
                graph[vertex_b].edges[vertex_a] = edge_weight;
            }
        }

    friend auto find_minimum_spanning_tree(const Graph &graph);

    friend auto run_mc_prims_mst(const Graph &graph);

    friend ostream &operator<<(ostream &os, const Graph &graph);
};


// This implementation uses Prim's algorithm to find a minimum spanning tree. As per the instructions, we are assuming
//   a connected UAG, with non-negative weights
auto find_minimum_spanning_tree(const Graph &graph) {
    // An MST of a UAG must be of the same size, it's edge count would be known as well (V-1), but that is of no use here.
    Graph mst{graph.size};

    // Fairly common graph algorithm paradigm:
    //   - V is the set of all vertices in the graph
    //   - X is the set of vertices in some subset of concern, in this case nodes already in our growing MST
    //   - V-X (VminX) is the set of nodes yet to be dealt with
    //   - the frontier is the set of edges from X and into V-X, or in the case of a UAG, edges between the sets
    set<int> VminX, X, V;
    vector<double> weights(graph.size, graph.INF);
    for (int i = 0; i < graph.size; ++i) {
        V.insert(i);
        VminX.insert(i);
    }

    // Initializing the algorithm with the origin node, 0 if no other preconditions.
    VminX.erase(0);
    X.insert(0);
    weights[0] = 0;

    // Primary loop: loops through each node already in the tree, which has been initialized with the origin, and
    //  chooses the vertex from V-X, i.e. the vertex corrsponding to the frontier edge, with the lowest weight. It then
    //  adds this to the MST and move it from V-X to X and proceeds. Termination is on emptiness of V-X.
    while (VminX.size() != 0) {
        Graph::min_edge mini_edge{{graph.QNAN, graph.QNAN}, graph.INF};
        for (auto x : X) {
            for (auto edge : graph.graph[x].edges) {
                if (VminX.count(edge.first) and edge.second < mini_edge.second) {
                    mini_edge.first.first = x;
                    mini_edge.first.second = edge.first;
                    mini_edge.second = edge.second;
                }
            }
        }
        VminX.erase(mini_edge.first.second);
        X.insert(mini_edge.first.second);
        weights[mini_edge.first.second] = mini_edge.second;

        // Incrementally builds the MST.
        mst.graph[mini_edge.first.first].edges[mini_edge.first.second] = mini_edge.second;
        mst.graph[mini_edge.first.second].edges[mini_edge.first.first] = mini_edge.second;
    }

    // Find the total traversal cost of the MST.
    double cost = accumulate(begin(weights) + 1, end(weights), 0);

    return pair<double, Graph> {cost, mst};
};


// Runs MST on algorithm and prints useful information.
auto run_mc_prims_mst(const Graph &graph) {
    auto results = find_minimum_spanning_tree(graph);
    cout << "MST for input graph:" << endl;
    cout << results.second << endl;
    cout << "MST total cost: " << results.first <<  endl;
};


// Implementation of printer for Graph datatype.
ostream &operator<<(ostream &os, const Graph &g) {
    for (int i{0}; i < g.graph.size(); ++i) {
        os << i << " -> ";
        for (auto &edge : g.graph[i].edges) {
            os << "{" << edge.first << ": " << edge.second << "}, ";
        }
        os << endl;
    }
    return os;
}


int main() {
    cout << "Please input the absolute path of the file containing your graph" << endl;
    string filename;
    getline (cin, filename);
    Graph graph(filename);

    cout << endl << "Here is your graph!" << endl;
    cout << setprecision(5) << graph << endl;

    cout << "Now let's get you your MST!" << endl << endl;
    run_mc_prims_mst(graph);

    return 0;
}