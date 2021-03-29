# Name: Anirudh S
# Roll no: CS17B003

import os
import argparse
import matplotlib.pyplot as plt

def simulate(ki, km, kn, kf, ps, T, out, plot_dir):
    if not os.path.exists("./cw"):
        os.system("make")

    cmd = "./cw"
    cmd += " -i " + str(ki)
    cmd += " -m " + str(km)
    cmd += " -n " + str(kn)
    cmd += " -f " + str(kf)
    cmd += " -s " + str(ps)
    cmd += " -T " + str(T)
    cmd += " -o " + out
    os.system(cmd)

    plot_title = "K_i = " + str(ki)
    plot_title += "; K_m =" + str(km)
    plot_title += "; K_n = " + str(kn)
    plot_title += "; K_f = " + str(kf)
    plot_title += "; P_s = " + str(ps)

    plot_name = plot_dir + out.split('/')[-1] 

    with open(out, 'r') as f:
        val = [float(v) for v in f.readlines()]
        plt.figure(figsize=(10,6))
        plt.plot(val)
        plt.title(plot_title)
        plt.xlabel("Number of updates")
        plt.ylabel("CW size (in KB)")
        plt.xlim(0, len(val))
        plt.ylim(0, max(val) + 10)
        plt.savefig(plot_name + ".png")
        plt.close()

def main():
    parser = argparse.ArgumentParser()

    # Define the command line arguments
    parser.add_argument("-a", "--all", help="Run all parameter combinations", action="store_true")
    parser.add_argument("-i", "--init", help="Initial value of CW (in MSS)", type=float, default=1.0)
    parser.add_argument("-m", "--exp", help="Exponential growth muliplier", type=float, default=1.0)
    parser.add_argument("-n", "--lin", help="Linear growth multiplier", type=float, default=1.0)
    parser.add_argument("-f", "--to_mul", help="Timeout multiplier", type=float)
    parser.add_argument("-s", "--prob", help="Probability of timeout", type=float)
    parser.add_argument("-T", "--num_upd", help="Total number of updates", type=int)
    parser.add_argument("-o", "--out", help="Output file name")
    
    # Parse the arguments
    args = parser.parse_args()

    if args.all:
        if not os.path.isdir("./graphs"):
            os.system("mkdir graphs")
        
        if not os.path.isdir("./outputs"):
            os.system("mkdir outputs")

        for ki in [1.0, 4.0]:
            for km in [1.0, 1.5]:
                for kn in [0.5, 1.0]:
                    for kf in [0.1, 0.3]:
                        for (ps, T) in [(0.01, 1000), (0.0001, 20000)]:
                            # Output file name
                            out = "outputs/ki_" + str(ki)
                            out += "km_" + str(km)
                            out += "kn_" + str(kn)
                            out += "kf_" + str(kf)
                            out += "ps_" + str(ps)

                            simulate(ki, km ,kn, kf, ps, T, out, "graphs/")

    else:
        ki = args.init
        km = args.exp
        kn = args.lin
        kf = args.to_mul
        ps = args.prob
        T = args.num_upd
        out = args.out

        if(kf == None):
            print('Required argument -f')
            exit(0)
        if(ps == None):
            print('Required argument -s')
            exit(0)
        if(T == None):
            print('Required argument -T')
            exit(0)
        if(out == None):
            print('Required argument -o')
            exit(0)

        simulate(ki, km ,kn, kf, ps, T, out, "./")

if __name__ == "__main__":
    main()