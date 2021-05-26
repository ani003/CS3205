// Name: Anirudh S
// Roll No: CS17B003

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketException;
import java.util.ArrayList;

public class ReceiverSR {
    public static void main(String[] args) {
        int port_num = 12345;
        int max_packets = 100;
        int seq_len = 5;
        int window_size = 4;
        int buffer_size = 25;
        double error_rate = 0.1;
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
            else if(args[i].equals("-N")) {
                if(++i < args.length) {
                    max_packets = Integer.parseInt(args[i]);
                }
                else {
                    System.out.println("Incorrect input format");
                }
            }
            else if(args[i].equals("-n")) {
                if(++i < args.length) {
                    seq_len = Integer.parseInt(args[i]);
                }
                else {
                    System.out.println("Incorrect input format");
                }
            }
            else if(args[i].equals("-W")) {
                if(++i < args.length) {
                    window_size = Integer.parseInt(args[i]);
                }
                else {
                    System.out.println("Incorrect input format");
                }
            }
            else if(args[i].equals("-B")) {
                if(++i < args.length) {
                    buffer_size = Integer.parseInt(args[i]);
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

        ReceiverSRUtil recv = new ReceiverSRUtil(port_num, max_packets, seq_len, window_size, buffer_size, error_rate, debug);
        recv.run();
    }
}

class ReceiverSRUtil {
    int port_num, max_packets, seq_len, window_size, buffer_size;
    double error_rate;
    boolean debug;
    DatagramSocket socket;
    int num_ack_packets, max_seq_num, window_start;
    ArrayList<BufferElement> buffer;
    long abs_start_time;

    public ReceiverSRUtil(int port_num, int max_packets, int seq_len, int window_size, int buffer_size, 
                            double error_rate, boolean debug) {
        
        this.port_num = port_num;
        this.max_packets = max_packets;
        this.seq_len = seq_len;
        this.window_size = window_size;
        this.buffer_size = buffer_size;
        this.error_rate = error_rate;
        this.debug = debug;
        num_ack_packets = 0;
        max_seq_num = (int) Math.pow(2, seq_len);

        try {
            socket = new DatagramSocket(port_num);
        } catch (SocketException e) {
            e.printStackTrace();
            System.exit(0);
        }

        buffer = new ArrayList<>();

        for(int i = 0; i < buffer_size; i++) {
            buffer.add(new BufferElement(i % max_seq_num, false));
        }

        window_start = 0;
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

            int len = buffer.size();
            int window_end = Math.min(window_start + window_size, len);
            
            for(int i = window_start; i < window_end; i++) {
                if(buffer.get(i).seq_no == sender_seq_no) {
                    double prob = Math.random();
                    if(prob > error_rate) {
                        buffer.get(i).ack_time = System.nanoTime() - abs_start_time;
                        num_ack_packets++;
                        buffer.get(i).ack = true;
                        String ack_msg = "ACK " + sender_seq_no;

                        byte[] send_buf = ack_msg.getBytes();
                        DatagramPacket sendPacket = new DatagramPacket(send_buf, send_buf.length, recvPacket.getAddress(), recvPacket.getPort());
                        try {
                            socket.send(sendPacket);
                        } catch (IOException e) {
                            e.printStackTrace();
                        }
                    }

                    break;
                }
            }
            
            while(window_start < window_end) {
                BufferElement ele = buffer.get(window_start);
                if(ele.ack) {
                    if(debug) {
                        TimeFormat tf = new TimeFormat(ele.ack_time);
                        String print_msg = "";
                        print_msg += "Seq " + ele.seq_no + ": ";
                        print_msg += "Time received: " + tf.milliseconds + ":" + tf.microseconds;
                        System.out.println(print_msg);
                    }
                    window_start++;
                }
                else
                    break;
            }

            for(int j = buffer.size(); j < Math.min(window_start + buffer_size, max_packets); j++) {
                buffer.add(new BufferElement(j % max_seq_num, false));
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

    class BufferElement {
        int seq_no;
        boolean ack;
        long ack_time;

        public BufferElement(int seq_no, boolean ack) {
            this.seq_no= seq_no;
            this.ack = ack;
            this.ack_time = 0;
        }
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