#include <iostream>
#include <fstream>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <iomanip>
#include <ctime>
#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include <string>
#include <random>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include "read_data.cpp"
#include "tools.cpp"

using namespace std;

/*
INPUT  
        the name of dataset (necessary)
        --graph_type can be Real / HyperCL / HyperPA / HyperFF / HyperLAP / HyperLAP+.
        --binsize is optional, default size of bin is the number of hyperedges + 1.
OUTOUT
        Degree distribution of pairs of nodes.
        In the output file "./results/" + dataname + "/pair_degree_" + graphType + ".txt",
        store '# overlapping hyperedges'    :   '# node pairs' table.
        Each row    x                       :           y               means there exists y node pairs that share x number of hyperedges.
*/

int main (int argc, char *argv[]){
    clock_t start;

    string dataname = argv[1];
    int bin_size = 0;
    string graphType = "";

    for(int i=2; i<argc ; i++){
        string input = argv[i];
        if(input.compare("--graph_type") == 0) graphType = argv[++i];
        else if(input.compare("--binsize") == 0) bin_size = atoi(argv[++i]);
    }

    string dirpath = "../dataset/";

    string graphFile = dirpath + dataname + "_" + graphType + ".txt";
    cout << "Load " << graphFile << endl;

    // Read data ----------------------------------------------------------------------------------------
	start = clock();
    vector< vector <int> > hyperedge2node;
    vector< vector <int> > node2hyperedge;

	read_data(graphFile, node2hyperedge, hyperedge2node);

    int V = node2hyperedge.size();
    int E = hyperedge2node.size();

    if(bin_size == 0) bin_size = E+1;

    cout << "# of nodes: " << V << '\n';
	cout << "# of hyperedges: " << E << '\n';
	cout << "Reading data done: "
		<< (double)(clock() - start) / CLOCKS_PER_SEC << " sec" << endl;
	cout << "-----------------------------------------------------------" << endl << endl;

    // Get adjacency list -------------------------------------------------------------------------------
    start = clock();
    vector< vector< pair<int, int> > > node_adj; // store < neighbor node, how many hyperedges share each other >
    vector< unordered_map<int, int> > node_inter;
    
    project(hyperedge2node, node2hyperedge, node_adj, node_inter);

	cout << "Find node's neighbors: "
		<< (double)(clock() - start) / CLOCKS_PER_SEC << " sec" << endl;
	cout << "-----------------------------------------------------------" << endl << endl;

    // Fill bins according to degree of node pair -------------------------------------------------------
    start = clock();
    int max_inter = 0;
    vector< int > bin(bin_size, 0);
    for(int v=0; v<V ; v++){
        int deg = (int)node_adj[v].size();
        for(int j=0 ; j<deg ; j++){
            int nv = node_adj[v][j].first;
            if(v >= nv) continue;
            int inter = node_adj[v][j].second;
            int idx = min(bin_size - 1, inter);
            bin[idx]++;
        }
    }
    cout << "Find degree of node pair: "
		<< (double)(clock() - start) / CLOCKS_PER_SEC << " sec" << endl;
	cout << "-----------------------------------------------------------" << endl << endl;
    
    // Write File ----------------------------------------------------------------------------
    start = clock();
    string outputdir = "./results/" + dataname;
    mkdir(outputdir.c_str(), 0776);
    string writeFile = outputdir + "/pair_degree_" + graphType + ".txt";
    
    ofstream resultFile(writeFile.c_str());
    resultFile << "# overlapping hyperedges,# node pairs" << endl;
    for(int i=1 ; i<bin_size ; i++){
        resultFile << i << "," << bin[i] << endl;
    }
    resultFile.close();

    cout << "Write File: "
		<< (double)(clock() - start) / CLOCKS_PER_SEC << " sec" << endl;
	cout << "-----------------------------------------------------------" << endl << endl;

    return 0;
}    
