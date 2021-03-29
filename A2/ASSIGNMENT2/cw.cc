/**
 * Name: Anirudh S
 * Roll no: CS17B003
*/

#include <bits/stdc++.h>
#define MSS 1.0             // MSS size in KB
#define RWS 1024.0          // RWS size in KB
#define THRESH_MULT 0.5     // Threshold multiplier for every timeout
using namespace std;

/**
 * AIMD simulator
 * Arguments:
 *      ki - initial CW size (in MSS)
 *      km - exponential growth multiplier
 *      kn - linear growth multiplier
 *      kf - timeout multiplier
 *      ps - probability of timeout
 *      T - total number of updates
 *      outfile - output file name
*/
void simulate_aimd(double ki, double km, double kn, double kf, double ps, int T, string outfile) {
    double cw, threshold;       // Congestion window size and congestion threshold
    cw = ki * MSS;              // Initial command window size
    threshold = DBL_MAX;        // Set to infinity

    ofstream output;
    output.open(outfile, ios::out | ios::trunc);
    output << cw << "\n";       // Print initial CW size

    int num_upd = 0;
    bool slow_start = true;     // Indicates whether protocol is in slow start or congestion avoidance

    // Seed random number generator
    srand((unsigned)time(NULL));

    while(num_upd < T) {
        // Send segments in the command window
        for(int i = 0; i < ceil(cw / MSS); i++) {
            // Generate random probability
            double prob = (double)rand()/ RAND_MAX;
            if(prob < ps) {
                // Timeout; congestion detection phase
                threshold = THRESH_MULT * cw;   // Update threshold
                cw = max(1.0, kf * cw);         // Update CW size
                slow_start = true;              // Restart from slow start phase

                output << cw << "\n";
                num_upd++;
                break;
            }
            
            // Packet acknowledged successfully; update window size accordingly
            if(slow_start) {
                // In slow start phase
                cw = min(cw + km * MSS, RWS);

                // Check if CW size has reached threshold
                if(cw >= threshold) {
                    slow_start = false;
                }
            }
            else {
                // In congestion avoidance phase
                cw = min(cw + (kn * MSS * MSS / cw), RWS);
            }

            output << cw << "\n";
            if(++num_upd >= T)
                break;
        }
    }

    
}

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
    
    // Invoke simulator
    simulate_aimd(ki, km, kn, kf, ps, t, out);

    return 0;
}