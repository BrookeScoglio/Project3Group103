#include <iostream>
#include <sstream>
#include <fstream>
#include <random>
#include <vector>
#include <set>
#include <chrono>
using namespace std;

typedef chrono::high_resolution_clock Clock;

struct Edge {
    long int src, dest;
    Edge(long int _src, long int _dest) {src = _src; dest = _dest;}
};

class Graph {
public:
    long int numVertices;
    vector<vector<int>> adjList;
    vector<Edge> edges_;

    Graph(const vector<Edge>& edges, long int vertices) {
        numVertices = vertices;
        adjList.resize(vertices);
        for (auto &edge : edges) {
            adjList[edge.src].push_back(edge.dest);
        }

        for (unsigned int i = 0; i < edges.size(); i++)
        {
            edges_.push_back(edges[i]);
        }
    }
};

vector<Edge> GetTwitterData(string& edgesCSV) {
    ifstream inFile;
    inFile.open(edgesCSV);
    vector<Edge> edges;
    if (inFile.is_open()) {
        string lineFromFile;
        getline(inFile,lineFromFile);
        do {
            istringstream stream(lineFromFile);
            string src;
            string dest;
            getline(stream, src, ',');
            getline(stream, dest);
            Edge edge(stoi(src), stoi(dest));
            edges.push_back(edge);
        }while (getline(inFile, lineFromFile));
    }
    inFile.close();
    return edges;
}

Graph MakeGraph(int numVert, string edgesCSV) {
    vector<Edge> edges = GetTwitterData(edgesCSV);
    long int vertices = numVert;
    Graph graph = Graph(edges, vertices);
    return graph;
}

long int recursiveFindAccNum(long int accNum, long int accFollowing) {
    if (accNum == accFollowing) {
        random_device rd;
        mt19937 mt(rd());
        uniform_int_distribution<long int> dist(1, 119999);
        accFollowing = dist(mt);
        recursiveFindAccNum(accNum, accFollowing);
    }
    return accFollowing;
}

void WritingInFunc() {
    ofstream myFile;
    myFile.open("twitteredges.csv");
    //random amount per account
    //~7.5 edges to accounts
    random_device rd;
    mt19937 mt(rd());

    //for 120,000 accounts
    if(myFile.is_open()) {
        for (int i = 1; i < 120000; i++) {
            //make random number 1-15
            uniform_int_distribution<int> dist(1, 15);
            int numFollowers = dist(mt);
            for (int j = 0; j < numFollowers; j++) {
                uniform_int_distribution<long int> dist(1, 119999);
                long int accFollowing = dist(mt);
                accFollowing = recursiveFindAccNum(i, accFollowing);
                myFile << to_string(i) + "," + to_string(accFollowing) + "\n";
            }
            //make for loop for random number
            //within loop, generate random number, check that it isnt equal to account number
            //then write it to the file
        }
    }
    myFile.close();
}

int dijkstra(const Graph& g, int src, int dest)
{
    // distances start as the largest possible, "infinite", parent vertices start as -1
    vector<int> d(g.numVertices, INT_MAX);
    vector<int> p(g.numVertices, -1);

    // insert all unvisited vertices other than the src (since that is where we start)
    set<int> unvisited;
    for (int i = 0; i < g.numVertices; i++)
        if (i != src)
            unvisited.insert(i);

    d.at(src) = 0;
    int curNode = src;

    while (!unvisited.empty())
    {
        // for every adjacent node to the current node
        vector<int> adjacent = g.adjList.at(curNode);
        for (int i = 0; i < adjacent.size(); i++)
        {
            // if the neighbor is has not yet been visited
            if (unvisited.count(adjacent.at(i)) != 0)
            {
                // if the recorded distance to a neighbor is greater than curNode's distance plus 1
                if ((d.at(adjacent.at(i)) > (d.at(curNode) + 1)) && (d.at(curNode)) != INT_MAX)
                {
                    // the shorter path is the one from the src to curNode to the neighbor
                    d.at(adjacent.at(i)) = d.at(curNode) + 1;
                    p.at(adjacent.at(i)) = curNode;
                }
            }
        }

        unvisited.erase(curNode);

        // pick the neighbor that's closest to the current node as the next node
        int curMin = INT_MAX;
        for (auto element : unvisited)
        {
            if (curMin >= d.at(element))
            {
                curMin = element;
                curNode = element;
            }
        }
    }

    return d.at(dest);
}

int bellmanFord(const Graph& g, int src, int dest) {
    int V = g.numVertices;               //total number of vertices
    vector<int> d(V, INT_MAX);           //array to hold distances
    vector<int> p(V, -1);          //array to hold predecessors
    int w = 1;                          //constant weight of 1

    //Mark the source vertex with distance 0
    d.at(src) = 0;

    //Step 2: Relax edges |V| - 1 times
    //for each vertex v in graph
    for (int i = 1; i < V; i++) {
        //for each edge in graph
        for (int j = 0; j < g.edges_.size(); j++) {
            //If the distance at the source + 1 is less than the distance at the destination
            if (d.at(g.edges_[j].src) != INT_MAX && d.at(g.edges_[j].src) + w < d.at(g.edges_[j].dest))
            {
                //The distance at the destination is equal to the distance at the source + 1
                d.at(g.edges_[j].dest) = d.at(g.edges_[j].src) + w;
                //The predecessor of destination is the source
                p.at(g.edges_[j].dest) = g.edges_[j].src;
            }
        }
    }

    //Step 3: Detect negative cycle
    //This is never true and never prints because we chose a constant weight of 1 per edge
    /*for (int i = 0; i < g.edges_.size(); i++)
    {
        if (d.at(g.edges_[i].src) + w < d.at(g.edges_[i].dest))
            cout << "Negative cycle exists" << endl;
    }*/

    //Else, No negative weight cycle found! Print number of connections between src and dest
    //cout << "The number of connections between source vertex " << src << " and destination vertex " << dest << " is:" << d.at(dest) << endl;
    return d.at(dest);
}

int main()
{
	cout << "--------------------------------------------------" << endl;
	cout << "---------Degree of Separation Calculator----------" << endl;
	cout << "Comparing Dijkstra's and Bellman Ford's Algorithms" << endl;
	cout << "By: Allen Sun, Brooke Scoglio, David Szczesniak" << endl;
	cout << "--------------------------------------------------" << endl;

	bool running = true;
	while (running)
	{
		cout << "MENU" << endl;
		cout << "1: Compare times for 120k Vertices" << endl;
		cout << "2: Compare times for 60k Vertices" << endl;
		cout << "3: Enter a file to open" << endl;
		cout << "4: Compare times for a 5 Vertice graph test case" << endl;
		cout << "5: Exit" << endl;
		cout << "--------------------------------------------------" << endl;

		char option;
		cin >> option;

        if (option == '1')
        {
            Graph graph = MakeGraph(120000, "twitteredges.csv");

            int src, dest, numConnections;
            cout << "Enter source node" << endl;
            cin >> src;
            cout << "Enter destination node" << endl;
            cin >> dest;

            auto t1 = Clock::now();
            numConnections = dijkstra(graph, src, dest);
            auto t2 = Clock::now();
            cout << "Times for 120k Vertices" << endl;
            cout << "Dijkstra's (Number of Connections): " << numConnections << endl;
            cout << "Dijkstra's (Time in Seconds): " << chrono::duration_cast<chrono::seconds>(t2 - t1).count() << endl;

            t1 = Clock::now();
            numConnections = dijkstra(graph, src, dest);
            t2 = Clock::now();
            cout << "Bellman Ford's (Number of Connections): " << numConnections << endl;
            cout << "Bellman Ford's (Time in nanoseconds): " << chrono::duration_cast<chrono::seconds>(t2 - t1).count() << endl;
        }
        else if (option == '2')
        {
            Graph graph = MakeGraph(60000, "edges60k.csv");

            int src, dest, numConnections;
            cout << "Enter source node" << endl;
            cin >> src;
            cout << "Enter destination node" << endl;
            cin >> dest;

            auto t1 = Clock::now();
            numConnections = dijkstra(graph, src, dest);
            auto t2 = Clock::now();
            cout << "Times for 60k Vertices" << endl;
            cout << "Dijkstra's (Number of Connections): " << numConnections << endl;
            cout << "Dijkstra's (Time in Seconds): " << chrono::duration_cast<chrono::seconds>(t2 - t1).count() << endl;

            t1 = Clock::now();
            numConnections = bellmanFord(graph, src, dest);
            t2 = Clock::now();
            cout << "Bellman Ford's (Number of Connections): " << numConnections << endl;
            cout << "Bellman Ford's (Time in Nanoseconds): " << chrono::duration_cast<chrono::seconds>(t2 - t1).count() << endl;
        }
		else if (option == '3')
		{
		    string userInput;
		    int numVertices, src, dest, option, numConnections;
			cout << "Enter name of file" << endl;
			cin >> userInput;
			cout << "Enter number of vertices" << endl;
			cin >> numVertices;
			Graph graph = MakeGraph(numVertices, userInput);

			cout << "Enter source node" << endl;
			cin >> src;
			cout << "Enter destination node" << endl;
			cin >> dest;

			cout << "1: Dijkstra's Algorithm" << endl;
			cout << "2: Bellman Ford's Algorithm" << endl;
			cin >> option;
			if (option == 1)
            {
                auto t1 = Clock::now();
                numConnections = dijkstra(graph, src, dest);
                auto t2 = Clock::now();
                cout << "Dijkstra's (Number of Connections): " << numConnections << endl;
                cout << "Dijkstra's (Time in Seconds): " << chrono::duration_cast<chrono::seconds>(t2 - t1).count() << endl;
            }
			if (option == 2)
            {
                auto t1 = Clock::now();
                numConnections = bellmanFord(graph, src, dest);
                auto t2 = Clock::now();
                cout << "Bellman Ford's (Number of Connections): " << numConnections << endl;
                cout << "Bellman Ford's (Time in Nanoseconds): " << chrono::duration_cast<chrono::seconds>(t2 - t1).count() << endl;
            }
		}
		else if (option == '4')
		{
			cout << "Times for a 5 Vertice graph test case" << endl;

			int numEdges = 8;
			int numVertices = 5;
			vector<Edge> edges;
			vector<vector<int>> input = { {0, 1}, {0, 2}, {1, 0}, {1, 2}, {2, 0}, {2, 1}, {3, 4}, {4, 3} };
			for (int i = 0; i < numEdges; i++)
			{
				Edge newEdge = Edge(input.at(i).at(0), input.at(i).at(1));
				edges.push_back(newEdge);
			}
			Graph test = Graph(edges, numVertices);

			auto t1 = Clock::now();
			int numConnections = dijkstra(test, 0, 2);
			auto t2 = Clock::now();
			cout << "Connected Vertices (0,2)" << endl;
			cout << "Dijkstra's (Number of Connections): " << numConnections << endl;
			cout << "Dijkstra's (Time in Nanoseconds): " << chrono::duration_cast<chrono::nanoseconds>(t2 - t1).count() << endl;

			t1 = Clock::now();
			numConnections = bellmanFord(test, 0, 2);
			t2 = Clock::now();
            cout << "Bellman Ford's (Number of Connections): " << numConnections << endl;
			cout << "Bellman Ford's (Time in Nanoseconds): " << chrono::duration_cast<chrono::nanoseconds>(t2 - t1).count() << endl;

			cout << "\nUnconnected Vertices (0,4)" << endl;

			t1 = Clock::now();
			numConnections = dijkstra(test, 0, 4);
			t2 = Clock::now();
            cout << "Dijkstra's (Number of Connections): " << numConnections << endl;
			cout << "Dijkstra's (Time in Nanoseconds): " << chrono::duration_cast<chrono::nanoseconds>(t2 - t1).count() << endl;

			t1 = Clock::now();
			numConnections = bellmanFord(test, 0, 4);
			t2 = Clock::now();
			cout << "Bellman Ford's (Number of Connections): " << numConnections << endl;
			cout << "Bellman Ford's (Time in Nanoseconds): " << chrono::duration_cast<chrono::nanoseconds>(t2 - t1).count() << endl;

			cout << "--------------------------------------------------" << endl;
		}
		else if (option == '5')
		{
			cout << "Exiting program..." << endl;
			running = false;
		}
		else
		{
			cout << "Improper input, try again" << endl;
		}
	}
}

