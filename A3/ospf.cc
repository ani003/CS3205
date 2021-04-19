#include <bits/stdc++.h>
using namespace std;

int id, hi, lsai, spfi;
string infile, outfile;

void parse_args(int argc, char* argv[]);

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

    // Run ospf
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