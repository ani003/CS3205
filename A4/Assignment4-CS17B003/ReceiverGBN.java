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

// Name: Anirudh S
// Roll No: CS17B003

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketException;

public class ReceiverGBN {
    public static void main(String[] args) {
        int port_num = 12345;
        int max_packets = 100;
        double error_rate = 0.05;
        boolean debug = false;

        for(int i = 0; i < args.length; i++) {
            if(args[i].equals("-d")) {
                debug = true;
            }
            else if(args[i].equals("-p")) {
                if(++i < args.length) {
                    port_num = Integer.parseInt(args[i]);
                }
                else {
                    System.out.println("Incorrect input format");
                }
            }
            else if(args[i].equals("-n")) {
                if(++i < args.length) {
                    max_packets = Integer.parseInt(args[i]);
                }
                else {
                    System.out.println("Incorrect input format");
                }
            }
            else if(args[i].equals("-e")) {
                if(++i < args.length) {
                    error_rate = Double.parseDouble(args[i]);
                }
                else {
                    System.out.println("Incorrect input format");
                }
            }
        }

        ReceiverGBNUtil recv = new ReceiverGBNUtil(port_num, max_packets, error_rate, debug);
        recv.run();
    }
}

class ReceiverGBNUtil {
    int port_num, max_packets;
    double error_rate;
    boolean debug;
    DatagramSocket socket;
    int num_ack_packets, next_packet;
    long abs_start_time;

    public ReceiverGBNUtil(int port_num, int max_packets, double error_rate, boolean debug) {
        this.port_num = port_num;
        this.max_packets = max_packets;
        this.error_rate = error_rate;
        this.debug = debug;

        num_ack_packets = 0;
        next_packet = 0;

        try {
            socket = new DatagramSocket(port_num);
        } catch (SocketException e) {
            e.printStackTrace();
            System.exit(0);
        }
    }

    public void run() {
        System.out.println("Receiver started...");
        abs_start_time = System.nanoTime();
        byte[] recv_buf;

        while(num_ack_packets < max_packets) {
            recv_buf = new byte[65535];
            DatagramPacket recvPacket = new DatagramPacket(recv_buf, recv_buf.length);
            try {
                socket.receive(recvPacket);
            } catch (IOException e) {
                e.printStackTrace();
            }

            String msg = data(recv_buf).toString();
            if(msg.equals("exit"))
                break;
            
            int sender_seq_no = Integer.parseInt(msg.split(" ")[0]);

            if(next_packet == sender_seq_no) {
                double prob = Math.random();
                if(prob > error_rate) {
                    long ack_time = System.nanoTime() - abs_start_time;
                    num_ack_packets++;
                    String ack_msg = "ACK " + sender_seq_no;

                    byte[] send_buf = ack_msg.getBytes();
                    DatagramPacket sendPacket = new DatagramPacket(send_buf, send_buf.length, recvPacket.getAddress(), recvPacket.getPort());
                    try {
                        socket.send(sendPacket);
                    } catch (IOException e) {
                        e.printStackTrace();
                    }

                    if(debug) {
                        TimeFormat tf = new TimeFormat(ack_time);
                        String print_msg = "";
                        print_msg += "Seq " + sender_seq_no + ": ";
                        print_msg += "Time received: " + tf.milliseconds + ":" + tf.microseconds;
                        System.out.println(print_msg);
                    }

                    next_packet++;
                }
            }
        }
    }

    private StringBuilder data(byte[] a) {
        if (a == null)
            return null;
        StringBuilder ret = new StringBuilder();
        int i = 0;
        while (a[i] != 0) {
            ret.append((char) a[i]);
            i++;
        }
        return ret;
    }

    class TimeFormat {
        int milliseconds;
        int microseconds;

        public TimeFormat(long time) {
            time = time / 1000;
            microseconds = (int)(time % 1000);
            milliseconds = (int)(time / 1000);
        }
    }
}
