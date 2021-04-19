#include <bits/stdc++.h>
using namespace std;

int id, hi, lsai, spfi;
string infile, outfile;

int num_nodes, num_edges;
struct node_info {
    int id;
    int wt;
    int min_wt;
    int max_wt;
};
vector<node_info> neighbours;

vector<vector<pair<int, int>>> adj_list;

void parse_args(int argc, char* argv[]);
void read_file();

void hello_send() {
    while(true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(hi));
        cout << "Hello " << id << "\n";
    }
}

void lsai_send() {
    while(true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(lsai));
        cout << "LSAI send\n";
    }
}

void receive() {
    while(true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        cout << "Receive\n"; 
    }
}

void dijkstra_algo() {
    while(true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(spfi));
        cout << "Dijkstra\n";
    }
}

void test_dijkstra() {
    cout << "In dijkstra\n";
    int source = id;
    typedef pair<int, pair<int,int>> heap_ele;
    priority_queue<heap_ele, vector<heap_ele>, greater<heap_ele>> pq;
    unordered_map<int, bool> done;
    vector<string> paths;
    paths.resize(num_nodes, "");
    paths[source] = to_string(source);

    vector<int> cost;
    cost.resize(num_nodes, INT_MAX);
    cost[source] = 0;

    for(int i = 0; i < num_nodes; i++) {
        pq.push(make_pair(INT_MAX, make_pair(i, -1)));
        done[i] = false;
    }

    pq.push(make_pair(0, make_pair(source, -1)));

    while (!pq.empty()) {
        pair<int, pair<int, int>> info = pq.top();
        pq.pop();

        int wt = info.first;
        int node = info.second.first;
        int parent = info.second.second;

        if(!done[node]) {
            done[node] = true;
            // Update neighbours
            vector<pair<int,int>> nbrs_list = adj_list[node];
            for(int i = 0; i < nbrs_list.size(); i++) {
                int nbr = nbrs_list[i].first;
                int edge_wt = nbrs_list[i].second;

                pq.push(make_pair(wt + edge_wt, make_pair(nbr, node)));
            }

            // Update path
            if(parent != -1) {
                paths[node] = paths[parent] + to_string(node);
                cost[node] = wt;
            }
        }
    }

    for(int i = 0; i < paths.size(); i++) {
        if (i == source)
            continue;

        cout << i << " - " << paths[i] << " " << cost[i] << "\n";
    }
    
}

void run() {
    thread hello(hello_send);
    thread lsai(lsai_send);
    thread recv(receive);
    thread dijkstra(dijkstra_algo);

    //this_thread::sleep_for(1000);

    hello.join();
    lsai.join();
    recv.join();
    dijkstra.join();

    cout << "Joined\n";
}




int main(int argc, char* argv[]) {
    // Parse arguments
    parse_args(argc, argv);

    cout << id << " " << infile << " " << outfile << " ";
    cout << hi << " " << lsai << " " << spfi << "\n";

    // Read info from the file
    read_file();
    test_dijkstra();

    // Run ospf
    //run();

    return 0;
}

void parse_args(int argc, char* argv[]) {
    // Given default values
    hi = 1.0;
    lsai = 5.0;
    spfi = 20.0;

    // Parse command line arguments
    for(int i = 1; i < argc; i++) {
        if(strcmp(argv[i], "-i") == 0) {
            if(++i < argc) {
                id = stoi(argv[i]);
            }
            else {
                cout << "Incorrect input format\n";
                exit(0);
            }
        }
        else if(strcmp(argv[i], "-f") == 0) {
            if(++i < argc) {
                infile = argv[i];
            }
            else {
                cout << "Incorrect input format\n";
                exit(0);
            }
        }
        else if(strcmp(argv[i], "-o") == 0) {
            if(++i < argc) {
                outfile = argv[i];
            }
            else {
                cout << "Incorrect input format\n";
                exit(0);
            }
        }
        else if(strcmp(argv[i], "-h") == 0) {
            if(++i < argc) {
                hi = (int)(1000.0 * stod(argv[i]));
            }
            else {
                cout << "Incorrect input format\n";
                exit(0);
            }
        }
        else if(strcmp(argv[i], "-a") == 0) {
            if(++i < argc) {
                lsai = (int)(1000.0 * stod(argv[i]));
            }
            else {
                cout << "Incorrect input format\n";
                exit(0);
            }
        }
        else if(strcmp(argv[i], "-s") == 0) {
            if(++i < argc) {
                spfi = (int)(1000.0 * stod(argv[i]));
            }
            else {
                cout << "Incorrect input format\n";
                exit(0);
            }
        }
    }
}

void read_file() {
    ifstream file(infile);
    string line;
    string word;
    stringstream ss;

    // First line
    getline(file, line);
    ss << line;
    ss >> word;
    num_nodes = stoi(word);
    ss >> word;
    num_edges = stoi(word);
    ss.clear();

    for(int j = 0; j < num_nodes; j++) {
        vector<pair<int, int>> v;
        adj_list.push_back(v);
    }

    // Edges
    for(int i = 0; i < num_edges; i++) {
        getline(file, line);
        ss << line;
        ss >> word;
        int node1 = stoi(word);
        ss >> word;
        int node2 = stoi(word);
        ss >> word;
        int min_wt = stoi(word);
        ss >> word;
        int max_wt = stoi(word);
        ss.clear();

        if(node1 == id) {
            node_info nbr = {node2, INT_MAX, min_wt, max_wt};
            neighbours.push_back(nbr);
        }
        else if(node2 == id) {
            node_info nbr = {node1, INT_MAX, min_wt, max_wt};
            neighbours.push_back(nbr);
        }

        //TO BE DELETED
        adj_list[node1].push_back(make_pair(node2, min_wt));
        adj_list[node2].push_back(make_pair(node1, min_wt));
    }
}