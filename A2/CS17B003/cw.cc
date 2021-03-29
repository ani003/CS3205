#include <bits/stdc++.h>
using namespace std;

int main(int argc, char* argv[]) {
    double ki, km, kn, kf, ps;
    int t;
    string out;

    // Given default values
    ki = 1.0;
    km = 1.0;
    kn = 1.0;

    // Parse command line arguments
    for(int i = 1; i < argc; i++) {
        if(strcmp(argv[i], "-i") == 0) {
            if(++i < argc) {
                ki = stod(argv[i]);
            }
            else {
                cout << "Incorrect input format\n";
                exit(0);
            }
        }
        else if(strcmp(argv[i], "-m") == 0) {
            if(++i < argc) {
                km = stod(argv[i]);
            }
            else {
                cout << "Incorrect input format\n";
                exit(0);
            }
        }
        else if(strcmp(argv[i], "-n") == 0) {
            if(++i < argc) {
                kn = stod(argv[i]);
            }
            else {
                cout << "Incorrect input format\n";
                exit(0);
            }
        }
        else if(strcmp(argv[i], "-f") == 0) {
            if(++i < argc) {
                kf = stod(argv[i]);
            }
            else {
                cout << "Incorrect input format\n";
                exit(0);
            }
        }
        else if(strcmp(argv[i], "-s") == 0) {
            if(++i < argc) {
                ps = stod(argv[i]);
            }
            else {
                cout << "Incorrect input format\n";
                exit(0);
            }
        }
        else if(strcmp(argv[i], "-T") == 0) {
            if(++i < argc) {
                t = stoi(argv[i]);
            }
            else {
                cout << "Incorrect input format\n";
                exit(0);
            }
        }
        else if(strcmp(argv[i], "-o") == 0) {
            if(++i < argc) {
                out = argv[i];
            }
            else {
                cout << "Incorrect input format\n";
                exit(0);
            }
        }
    }
    
    cout << "ki = " << ki << "\n";
    cout << "km = " << km << "\n";
    cout << "kn = " << kn << "\n";
    cout << "kf = " << kf << "\n";
    cout << "ps = " << ps << "\n";
    cout << "T = " << t << "\n";
    cout << "outfile = " << out << "\n";

    return 0;
}