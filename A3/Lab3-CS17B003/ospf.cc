/*
Copyright (c) 2021 Anirudh S <anirudh6626@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/**
 * Name: Anirudh S
 * Roll no: CS17B003
*/

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

                        for(auto it = adj_list[id].begin(); it != adj_list[id].end(); ++it) {
                            if(it->first == src) {
                                it->second = new_wt;
                            }
                        }
                        for(auto it = adj_list[src].begin(); it != adj_list[src].end(); ++it) {
                            if(it->first == id) {
                                it->second = new_wt;
                            }
                        }
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

                    for(auto it = adj_list[src].begin(); it != adj_list[src].end(); ++it) {
                        if(it->first == nbr_node) {
                            it->second = nbr_wt;
                        }
                    }

                    for(auto it = adj_list[nbr_node].begin(); it != adj_list[nbr_node].end(); ++it) {
                        if(it->first == src) {
                            it->second = nbr_wt;
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
                        if(edge_wt < neighbours[i].wt) {
                            neighbours[i].wt = edge_wt;
                            
                            for(auto it = adj_list[id].begin(); it != adj_list[id].end(); ++it) {
                                if(it->first == src) {
                                    it->second = edge_wt;
                                }
                            }
                            
                            for(auto it = adj_list[src].begin(); it != adj_list[src].end(); ++it) {
                                if(it->first == id) {
                                    it->second = edge_wt;
                                }
                            }
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

            if(!done[node] && wt != INT_MAX) {
                done[node] = true;
                // Update neighbours
                for(auto it = adj_list[node].begin(); it != adj_list[node].end(); ++it) {
                    int nbr = it->first;
                    int edge_wt = it->second;
                    if(!done[nbr] && edge_wt != INT_MAX) {
                        pq.push(make_pair(wt + edge_wt, make_pair(nbr, node)));
                    }
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
    hi = 1000.0;
    lsai = 5000.0;
    spfi = 20000.0;

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

        adj_list[node1].push_back(make_pair(node2, INT_MAX));
        adj_list[node2].push_back(make_pair(node1, INT_MAX));
    }

    ofstream output(outfile);
    output.open(outfile, ios::out | ios::trunc);
    output.close();
}