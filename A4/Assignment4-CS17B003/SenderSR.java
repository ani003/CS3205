// Name: Anirudh S
// Roll No: CS17B003

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.util.*;
import java.util.concurrent.ConcurrentHashMap;

public class SenderSR {
    public static void main(String[] args) {
        String ip = "127.0.0.1";
        int port_num = 12345;
        int seq_len = 5;
        int max_len = 256;
        int gen_rate = 20;
        int max_packets = 100;
        int window_size = 4;
        int buffer_size = 20;
        boolean debug = false;

        for(int i = 0; i < args.length; i++) {
            if(args[i].equals("-d")) {
                debug = true;
            }
            else if(args[i].equals("-s")) {
                if(++i < args.length) {
                    ip = args[i];
                }
                else {
                    System.out.println("Incorrect input format");
                }
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
                    seq_len = Integer.parseInt(args[i]);
                }
                else {
                    System.out.println("Incorrect input format");
                }
            }
            else if(args[i].equals("-L")) {
                if(++i < args.length) {
                    max_len = Integer.parseInt(args[i]);
                }
                else {
                    System.out.println("Incorrect input format");
                }
            }
            else if(args[i].equals("-R")) {
                if(++i < args.length) {
                    gen_rate = Integer.parseInt(args[i]);
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
        }

        SenderSRUtil sender = new SenderSRUtil(ip, port_num, seq_len, max_len, gen_rate, max_packets, window_size, buffer_size, debug);
        sender.run();
        System.exit(0);
    }
}


class SenderSRUtil {
    String ip;
    InetAddress recv_ip;
    int port_num, seq_len, max_len, gen_rate, max_packets, window_size, buffer_size;
    boolean debug;
    ConcurrentHashMap<Integer, Packet> buffer;
    int max_seq_num, num_ack_packets;
    int timer_delay, window_start, sent_index;
    DatagramSocket socket;
    boolean stop;
    long abs_start_time, total_msg_len;
    int total_num_tries;
    double rtt_sum;

    private enum Status {
        WAITING,
        SENT,
        ACKNOWLEDGED
    }

    public SenderSRUtil(String ip, int port_num, int seq_len, int max_len, int gen_rate, int max_packets,
                        int window_size, int buffer_size, boolean debug) {

        this.ip = ip;
        this.port_num = port_num;
        this.seq_len = seq_len;
        this.max_len = max_len;
        this.gen_rate = gen_rate;
        this.max_packets = max_packets;
        this.window_size = window_size;
        this.buffer_size = buffer_size;
        this.debug = debug;

        try {
            recv_ip = InetAddress.getByName(ip);
        } catch (UnknownHostException e) {
            e.printStackTrace();
            System.exit(0);
        }

        try {
            if(port_num == 10000)
                socket = new DatagramSocket(10001);
            else
                socket = new DatagramSocket(10000);

        } catch (SocketException e) {
            e.printStackTrace();
            System.exit(0);
        }
        num_ack_packets = 0;
        window_start = 0;
        sent_index = -1;
        max_seq_num = (int) Math.pow(2, seq_len);
        timer_delay = 300;
        buffer = new ConcurrentHashMap<>();
        stop = false;
        total_num_tries = 0;
        total_msg_len = 0;
        rtt_sum = 0.0;
    }

    public void run() {
        System.out.println("Sender running...");
        abs_start_time = System.nanoTime();
        
        Generator generator = new Generator();
        Receiver receiver = new Receiver();

        receiver.start();
        generator.start();

        while(num_ack_packets < max_packets) {
            Packet packet = null;
            int packet_index = -1;
            int len = buffer.size();
            int window_end = Math.min(window_start + window_size, len);
            for(int i = window_start; i < window_end; i++) {
                packet = buffer.get(i);
                if(packet.getStatus() == Status.WAITING && i > sent_index) {
                    packet_index = i;
                    break;
                }
            }

            if(packet != null && sent_index < packet_index) {
                sent_index = packet_index;
                packet.send();
            }

            packet = null;
        }
        
        try {
            receiver.join();
            generator.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        double avg_len = (double)total_msg_len / num_ack_packets;
        double rtt_avg = rtt_sum / num_ack_packets;
        double rt_ratio = (double)total_num_tries / num_ack_packets;

        System.out.println("Packet gen rate = " + gen_rate);
        System.out.println("Average packet length = " + avg_len);
        System.out.println("Retransmission ratio = " + rt_ratio);
        System.out.println("Average RTT = " + rtt_avg);
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

    class Packet {
        private int seq_no;
        private Status st;
        private Timer timer;
        private String msg;
        private int num_tr;
        private PacketSend task;
        private long gen_time;
        private long start_time;
        private int msg_len;
        private int pkt_delay;
        
        public Packet(int seq_no, int pkt_delay) {
            gen_time = System.nanoTime()- abs_start_time;
            this.seq_no = seq_no;
            this.pkt_delay = pkt_delay;
            st = Status.WAITING;
            msg = "" + seq_no + " ";
            Random random = new Random();
            msg_len = 40 + random.nextInt(max_len - 40);

            for(int i = 0; i < msg_len; i++) {
                msg += ".";
            }

            num_tr = 0;
            timer = new Timer();
            task = new PacketSend(this);
        }

        public void send() {
            start_time = System.nanoTime();
            timer.schedule(task, 0, pkt_delay);
        }

        public void setStatus(Status st) {
            synchronized(this) {
                this.st = st;
            }
        }

        public Status getStatus() {
            synchronized(this) {
                return this.st;
            }
        }

        public void cancel() {
            synchronized(this) {
                task.cancel();
                timer.cancel();
            }
        }

        public void incrTries() {
            synchronized(this) {
                num_tr++;
            }
        }

        public int getTries() {
            synchronized(this) {
                return num_tr;
            }
        }

        public int getSeqNo() {
            synchronized(this) {
                return seq_no;
            }
        }

        public String getMsg() {
            synchronized(this) {
                return msg;
            }
        }

        public long getGenTime() {
            synchronized(this) {
                return gen_time;
            }
        }

        public long getStartTime() {
            synchronized(this) {
                return start_time;
            }
        }

        public int getLength() {
            synchronized(this) {
                return msg_len;
            }
        }
    }

    class PacketSend extends TimerTask {
        Packet packet;
        DatagramPacket msg_packet;

        public PacketSend(Packet packet) {
            this.packet = packet;
            byte[] buf = packet.getMsg().getBytes();
            msg_packet = new DatagramPacket(buf, buf.length, recv_ip, port_num);
        }
        
        @Override
        public void run() {
            switch(packet.getStatus()) {
                case WAITING:
                    packet.setStatus(Status.SENT);
                    packet.incrTries();
                    try {
                        socket.send(msg_packet);
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                    break;

                case SENT:
                    if(packet.getTries() >= 10) {
                        stop = true;
                        System.out.println("Max retransmissions exceeded for packet " + packet.getSeqNo());
                        byte[] exit_buf = "exit".getBytes();
                        DatagramPacket exitPacket = new DatagramPacket(exit_buf, exit_buf.length, recv_ip, port_num);
                        try {
                            socket.send(exitPacket);
                        } catch (IOException e) {
                            e.printStackTrace();
                        }

                        double avg_len = (double)total_msg_len / num_ack_packets;
                        double rtt_avg = rtt_sum / num_ack_packets;
                        double rt_ratio = (double)total_num_tries / num_ack_packets;

                        System.out.println("Packet gen rate = " + gen_rate);
                        System.out.println("Average packet length = " + avg_len);
                        System.out.println("Retransmission ratio = " + rt_ratio);
                        System.out.println("Average RTT = " + rtt_avg);

                        System.exit(0);
                    }

                    packet.incrTries();
                    try {
                        socket.send(msg_packet);
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                    break;

                default:
                    break;
            }
        }
    }

    class Generator extends Thread {
        @Override
        public void run() {
            int num_gen_packets = 0;
            while(!stop) {
                int len = buffer.size();
                int curr_buff_size = len - window_start;
                if(curr_buff_size < buffer_size) {
                    int seq_no = num_gen_packets % max_seq_num;
                    Packet packet = new Packet(seq_no, timer_delay);
                    buffer.put(num_gen_packets, packet);
                    num_gen_packets++;
                }

                try {
                    Thread.sleep((long)(1000 / gen_rate));
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    class Receiver extends Thread {
        @Override
        public void run() {
            Packet packet;
            byte[] buf;

            while(num_ack_packets < max_packets) {
                buf = new byte[65535];
                DatagramPacket recvPacket = new DatagramPacket(buf, buf.length);
                try {
                    socket.receive(recvPacket);
                } catch (IOException e) {
                    e.printStackTrace();
                }

                long recv_time = System.nanoTime();
                String msg = data(buf).toString();
                if(msg != null) {
                    String[] words = msg.split(" ");
                    if(words.length != 2) {
                        System.out.println("Error in packet message: " + msg);
                        continue;
                    }

                    int ack_seq_num = Integer.parseInt(words[1]);
                    int len = buffer.size();
                    int window_end = Math.min(window_start + window_size, len);
                    for(int i = window_start; i < window_end; i++) {
                        packet = buffer.get(i);

                        if(packet.getSeqNo() == ack_seq_num && packet.getStatus() == Status.SENT) {
                            packet.setStatus(Status.ACKNOWLEDGED);
                            packet.cancel();

                            TimeFormat tf_rtt = new TimeFormat(recv_time - packet.getStartTime());
                            double rtt = (double)tf_rtt.milliseconds + (double)tf_rtt.microseconds / 1000;

                            if(debug) {
                                TimeFormat tf_gen = new TimeFormat(packet.getGenTime());
                                String print_msg = "";
                                print_msg += "Seq " + packet.getSeqNo() + ": ";
                                print_msg += "Time generated: " + tf_gen.milliseconds + ":" + tf_gen.microseconds + " ";
                                print_msg += "RTT: " + rtt + " ";
                                print_msg += "Number of attempts: " + packet.getTries();
                                System.out.println(print_msg);
                            }

                            rtt_sum += rtt;
                            total_num_tries += packet.getTries();
                            total_msg_len += packet.getLength();
                            num_ack_packets++;

                            if(num_ack_packets >= 10) {
                                double rtt_avg = rtt_sum / num_ack_packets;
                                timer_delay = (int)(20 * rtt_avg);
                            }

                            break;
                        }
                    }

                    while(window_start < window_end) {
                        if(buffer.get(window_start).getStatus() == Status.ACKNOWLEDGED) {
                            window_start++;
                            //buffer.remove(window_start - 1);
                        }
                        else
                            break;
                    }                    
                }
            }

            //Execution done. Cancel all subsequent packets
            stop = true;
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
    }
}