#include <bits/stdc++.h>
#include "udp.h"
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
UDP_server* server;
unordered_map<int, int> seq;

void parse_args(int argc, char* argv[]);
void read_file();

void server_setup() {
    int port_num = 10000 + id;
    server = new UDP_server(port_num);
}

void hello_send() {
    UDP_client client;
    string msg = "HELLO " + to_string(id);

    while(true) {
        this_thread::sleep_for(std::chrono::milliseconds(hi));
        for(int i = 0; i < neighbours.size(); i++) {
            int port_num = 10000 + neighbours[i].id;
            client.send(port_num, msg);
        }
    }
}

void lsai_send() {
    UDP_client client;
    int seq_no = 0;

    while(true) {
        this_thread::sleep_for(std::chrono::milliseconds(lsai));

        string msg = "LSA " + to_string(id) + " " + to_string(seq_no) + " " + to_string(neighbours.size());
        for(int i = 0; i < neighbours.size(); i++) {
            msg += (" " + to_string(neighbours[i].id) + " " + to_string(neighbours[i].wt));
        }

        for(int i = 0; i < neighbours.size(); i++) {
            int port_num = 10000 + neighbours[i].id;
            client.send(port_num, msg);
        }
        seq_no++;
    }
}

void receive() {
    stringstream ss;
    string word;

    while(true) {
        ss.clear();
        string msg = server->recieve();
        ss << msg;

        ss >> word;

        if(word == "HELLO") {
            ss >> word;
            int src = stoi(word);

            for(int i = 0; i < neighbours.size(); i++) {
                if(neighbours[i].id == src) {
                    int new_wt;
                    if(neighbours[i].wt == INT_MAX) {
                        int min_lim = neighbours[i].min_wt;
                        int max_lim = neighbours[i].max_wt;
                        srand((unsigned)time(NULL));
                        new_wt = min_lim + (rand() % (max_lim - min_lim + 1));
                        neighbours[i].wt = new_wt;
                        adj_list[id].push_back(make_pair(src, new_wt));
                    }
                    string new_msg = "HELLOREPLY " + to_string(id) + " " + to_string(src) + " "
                                    + to_string(neighbours[i].wt);
                    UDP_client client;
                    client.send(10000 + src, new_msg);
                    client.~UDP_client();
                    break;
                }
            }
        }
        else if(word == "LSA") {
            ss >> word;
            int src = stoi(word);
            ss >> word;
            int seq_no = stoi(word);

            if(seq_no > seq[src]) {
                seq[src] = seq_no;
                ss >> word;
                int num_entries = stoi(word);

                for(int j = 0; j < num_entries; j++) {
                    ss >> word;
                    int nbr_node = stoi(word);
                    ss >> word;
                    int nbr_wt = stoi(word);

                    for(int l = 0; l < adj_list[src].size(); l++) {
                        if(adj_list[src][l].first == nbr_node) {
                            adj_list[src][l].second = nbr_wt;
                        }
                    }
                }

                UDP_client client;
                for(int i = 0; i < neighbours.size(); i++) {
                    if(neighbours[i].id != src) {
                        int port_num = 10000 + neighbours[i].id;
                        client.send(port_num, msg);
                    }
                }
            }
            else {
                while(ss >> word);
            }
        }
        else if(word == "HELLOREPLY") {
            ss >> word;
            int src = stoi(word);
            ss >> word;
            int dest = stoi(word);
            ss >> word;
            int edge_wt = stoi(word);

            if(dest == id) {
                for(int i = 0; i < neighbours.size(); i++) {
                    if(neighbours[i].id == src) {
                        if(neighbours[i].wt == INT_MAX) {
                            neighbours[i].wt = edge_wt;
                            adj_list[id].push_back(make_pair(src, edge_wt));
                        }
                    }
                }
            }
            else {
                cout << "Destination mismatch:" << msg << "\n";
            }
        }
        else {
            while(ss >> word);
            cout << "Invalid packet:" << msg << "\n";
        }
    }
}

void dijkstra_algo() {
    int time_step = 0;
    while(true) {
        this_thread::sleep_for(std::chrono::milliseconds(spfi));
        time_step += spfi / 1000;

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
                    paths[node] = paths[parent] + "," + to_string(node);
                    cost[node] = wt;
                }
            }
        }

        ofstream output;
        output.open(outfile, ios::out | ios::app);
        output << "Routing table for node " + to_string(id) + " at time " << time_step << "\n";
        output << "Destination - Path - Cost\n";

        for(int i = 0; i < paths.size(); i++) {
            if (i == source)
                continue;

            output << i << " - " << paths[i] << " - " << cost[i] << "\n";
        }
        output << "\n";
        output.close();
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
}




int main(int argc, char* argv[]) {
    // Parse arguments
    parse_args(argc, argv);

    // Read info from the file
    read_file();

    // Run ospf
    server_setup();
    run();

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
            seq[node2] = -1;
        }
        else if(node2 == id) {
            node_info nbr = {node1, INT_MAX, min_wt, max_wt};
            neighbours.push_back(nbr);
            seq[node1] = -1;
        }
    }

    ofstream output(outfile);
    output.open(outfile, ios::out | ios::trunc);
    output.close();
}