#include <bits/stdc++.h>
using namespace std;

int main(int argc, char* argv[]) {
    int id;
    string infile, outfile;
    double hi, lsai, spfi;

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
                hi = stod(argv[i]);
            }
            else {
                cout << "Incorrect input format\n";
                exit(0);
            }
        }
        else if(strcmp(argv[i], "-a") == 0) {
            if(++i < argc) {
                lsai = stod(argv[i]);
            }
            else {
                cout << "Incorrect input format\n";
                exit(0);
            }
        }
        else if(strcmp(argv[i], "-s") == 0) {
            if(++i < argc) {
                spfi = stod(argv[i]);
            }
            else {
                cout << "Incorrect input format\n";
                exit(0);
            }
        }
    }

    cout << id << " " << infile << " " << outfile << " ";
    cout << hi << " " << lsai << " " << spfi << "\n";

    return 0;
}