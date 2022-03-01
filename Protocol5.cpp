

#include <iostream>
#include <cstdio>
#include <ctime>
#include <queue>
#include <vector>
#include <fstream>
#include <string>
using namespace std;



#define MAX_SEQ 7

#define MAX_PKT 1                                  /* determines packet size in bytes */



typedef enum { frame_arrival, cksum_err, timeout, network_layer_ready } event_type;
typedef enum { False, True } boolean;                    /* boolean type */
typedef unsigned int seq_nr;                           /* sequence or ack numbers */
typedef struct { unsigned char data[MAX_PKT]; } packet;  /* packet definition */
typedef enum { data, ack, nak } frame_kind;              /* frame_kind definition */
typedef struct {                                       /* frames are transported in this layer */
	frame_kind kind;                                   /* what kind of frame is it? */
	seq_nr seq;                                        /* sequence number */
	seq_nr ack;                                        /* acknowledgement number */
	packet info;                                       /* the network layer packet */
} frame;

struct timer {
	time_t starttime;
	seq_nr frame_nr;
};

string Data;
queue<timer> sendertimer;
queue<timer> acktimer;
queue<event_type> eventseq;
queue<int > wrongpackets;

static int acktime = 3;
static int timeouttime = 20;
static int lastchar = 0;
static seq_nr nbuffered = 0;
static seq_nr layers = 0;
static seq_nr endofmessage = 0;
seq_nr endm = 10;
packet buffer[MAX_SEQ + 1];
string physicalLayerPath = "physicalLayer.txt";
string outnetworkLayerPath = "networkLayer.txt";
string innetworkLayerPath = "message.txt";
string senderPath = "sender.txt";
string receiverPath = "receiver.txt";

void send_frame_to_physical_layer_info(string x, seq_nr a)
{
	ofstream send("sender.txt", fstream::app);
	send << " send frame to physical layer : Data->  " << x << "   seq_num ->  " << a << endl;
	send.close();

}
void end_message_info(string v)
{
	ofstream send(v, fstream::app);
	send << " Message is sent successfully " << endl;
	send.close();

}


void received_frame_from_physical_layer_info(string x, seq_nr a, bool z)
{
	ofstream receive("receiver.txt", fstream::app);
	receive << " received frame from physical layer : Data->   " << x << "  seq_num ->   " << a << "     ";
	if (z)
	{
		receive << "(accepted)" << endl;
	}
	else
	{
		receive << "(discarded)" << endl;
	}
	receive.close();

}

string print_packet(packet c)
{

	string x = "";
	for (int q = 0; q < MAX_PKT; q++)
	{

		x += c.data[q];

	}
	string s = x;

	return s;
}

string print_frame(frame c)
{

	string x = "";
	for (int q = 0; q < MAX_PKT; q++)
	{

		x += c.info.data[q];

	}
	string s = to_string(c.seq) + ";" + to_string(c.ack) + ";" + x;

	return s;
}

frame get_frame(string c)
{

	frame u;
	int l = c.length();
	int indcator = 0;
	int sublength = 0;
	for (int i = 0; (i < l) && indcator != 2; i++)
	{
		if (c[i] == ';')
		{
			switch (indcator)
			{
			case 0:

				u.seq = stoi(c.substr(sublength, i - sublength));
				sublength = i + 1;
				indcator++;
				break;

			case 1:
				u.ack = stoi(c.substr(sublength, i - sublength));
				sublength = i + 1;
				indcator++;
				break;

			}
		}
	}

	for (int d = 0; d + sublength < l&&d < MAX_PKT; d++)
	{

		u.info.data[d] = c[d + sublength];
	}

	return u;
}

void get_packet(seq_nr c)
{
	for (int q = 0; (q < MAX_PKT) && (lastchar <= Data.length()); q++)
	{
		char z = Data[lastchar + q];
		ofstream send("sender.txt", fstream::app);
		send << " get packet  -> " << z << "  from network layer " << endl;
		send.close();
		buffer[c].data[q] = z;
		lastchar += 1;

	}

}





// flush sender timer
void flush_sendertimer()
{

	while (!sendertimer.empty())
	{
		sendertimer.pop();
	}

}



/* Wait for an event to happen; return its type in event. */
void wait_for_event(event_type * event)
{

	if (!eventseq.empty())
	{

		*event = eventseq.front();

		if (*event == timeout)
		{
			while (!eventseq.empty())
			{
				eventseq.pop();
			}
			eventseq.push(cksum_err);
			flush_sendertimer();
			while (!acktimer.empty())
			{
				acktimer.pop();

			}
			remove("physicalLayer.txt");
			ifstream infile(physicalLayerPath);
			infile.close();
		}
		else
		{

			eventseq.pop();
		}


	}
	else
		*event = cksum_err;



}




/* Fetch a packet from the network layer for transmission on the channel. */
//void from_network_layer(packet * p);
/* Deliver information from an inbound frame to the network layer. */
void to_network_layer(packet * p) {

	ofstream outfile(outnetworkLayerPath, fstream::app);

	for (int q = 0; q < MAX_PKT; q++)
	{

		outfile << p->data[q];
		ofstream receive("receiver.txt", fstream::app);
		receive << " send packet \" " << p->data[q] << " \" to network layer" << endl;
		receive.close();
	}

	outfile.close();
}
/* Go get an inbound frame from the physical layer and copy it to r. */
void from_physical_layer(frame * r)
{
	ifstream infile(physicalLayerPath, fstream::app);
	string t;
	string line;
	if (!infile.bad()) {

		getline(infile, t);

		*r = get_frame(t);


		ofstream outfile("t.txt", fstream::app);
		while (getline(infile, line))
		{
			outfile << line << endl;


		}
		infile.close();
		outfile.close();
		remove("physicalLayer.txt");
		rename("t.txt", "physicalLayer.txt");

	}



}
/* Pass the frame to the physical layer for transmission. */
void to_physical_layer(frame * s)
{


	ofstream outfile(physicalLayerPath, fstream::app);

	for (int q = 0; q < MAX_PKT; q++)
	{

		outfile << print_frame(*s) << endl;
		//cout << " to_physcial_layer  :   " << print_frame(*s) << endl;
	}

	outfile.close();

}
/* Start the clock running and enable the timeout event. */
void start_timer(seq_nr k)
{
	timer q;
	q.frame_nr = k;
	q.starttime = clock();
	sendertimer.push(q);

	if (buffer[k].data[0] == 'u')
	{

		int v = 0;
		v++;

	}
}
/* Stop the clock and disable the timeout event. */
void stop_timer(seq_nr k)
{

	timer v = sendertimer.front();
	if (v.frame_nr == k)
	{
		sendertimer.pop();
	}

}







//run time out event

void check_sender_timer(void)
{
	if (!sendertimer.empty())
	{
		timer v = sendertimer.front();

		double time = (clock() - v.starttime) /  CLOCKS_PER_SEC;

		if (time >= timeouttime)
		{

			eventseq.push(timeout);
			layers = 0;




		}

	}
}
/* Start an auxiliary timer and enable the ack timeout event. */
void start_ack_timer(void)
{
	timer q;
	q.starttime = clock();
	acktimer.push(q);

}
/* Stop the auxiliary timer and disable the ack timeout event. */
void check_ack_timer(void)
{
	if (!acktimer.empty())
	{
		timer v = acktimer.front();
		double time = (clock() - v.starttime) / CLOCKS_PER_SEC;

		if (time >= acktime)
		{

			eventseq.push(frame_arrival);

			acktimer.pop();

		}

	}


}
/* Allow the network layer to cause a network layer ready event. */
void enable_network_layer(void)
{
	if ((layers + nbuffered) < 7)
	{
		eventseq.push(network_layer_ready);
		layers++;
	}


}
/* Forbid the network layer from causing a network layer ready event. */
void disable_network_layer(void)
{

}
/* Macro inc is expanded in-line: increment k circularly. */
#define inc(k) if(k < MAX_SEQ) k = k + 1; else k = 0;
/* Protocol 5 (Go-back-n) allows multiple outstanding frames. The sender may transmit up
to MAX_SEQ frames without waiting for an ack. In addition, unlike in the previous
protocols, the network layer is not assumed to have a new packet all the time. Instead,
the network layer causes a network_layer_ready event when there is a packet to send. */

static boolean between(seq_nr a, seq_nr b, seq_nr c)
{
	/* Return true if a <= b < c circularly; false otherwise. */
	if (((a <= b) && (b < c)) || ((c < a) && (a <= b)) || ((b < c) && (c < a)))
		return(True);
	else
		return(False);
}
static void send_data(seq_nr frame_nr, seq_nr frame_expected, packet buffer[])
{
	/* Construct and send a data frame. */
	frame s;                                                                           /* scratch variable */
	s.info = buffer[frame_nr];                                                         /* insert packet into frame */
	s.seq = frame_nr;                                                                  /* insert sequence number into frame */
	s.ack = s.seq;
	to_physical_layer(&s);                                                             /* transmit the frame */
	start_timer(frame_nr);
	start_ack_timer();
}
void protocol5(void)
{
	seq_nr next_frame_to_send;                                                         /* MAX_SEQ > 1; used for outbound stream */
	seq_nr ack_expected;                                                               /* oldest frame as yet unacknowledged */
	seq_nr frame_expected;                                                             /* next frame_expected on inbound stream */
	frame r;                                                                           /* scratch variable */

																					   /* number of output buffers currently in use */
	seq_nr i;                                                                          /* used to index into the buffer array */
	event_type event;



	cout << endl;
	enable_network_layer();                                                            /* allow network_layer_ready events */
	ack_expected = 0;                                                                  /* next ack_expected inbound */
	next_frame_to_send = 0;                                                            /* next frame going out */
	frame_expected = 0;                                                                /* number of frame_expected inbound */
	nbuffered = 0;                                                                     /* initially no packets are buffered */
	while (true) {
		//    delay 1 second
		time_t x = clock() - 1;

		while (((clock() - x) / CLOCKS_PER_SEC) < 2)
		{
			int q = 0;
			q++;
		};

		wait_for_event(&event);                                                        /* four possibilities: see event_type above */

		switch (event) {





		case network_layer_ready:                                                  /* the network layer has a packet to send */
		/* Accept, save, and transmit a new frame. */
			//from_network_layer(&buffer[next_frame_to_send]);                       /* fetch new packet */
			get_packet(next_frame_to_send);
			if (endofmessage)
			{

				break;
			}
			if (lastchar == Data.length() && (buffer[next_frame_to_send].data[MAX_PKT - 1] == Data[Data.length() - 1]) && next_frame_to_send == (Data.length() - 1) % (MAX_SEQ + 1))
			{
				endofmessage++;
				endm = next_frame_to_send;
			}
			layers--;
			send_frame_to_physical_layer_info(print_packet(buffer[next_frame_to_send]), next_frame_to_send);

			if (!wrongpackets.empty() && (((wrongpackets.front() - 1) % (MAX_SEQ + 1)) == next_frame_to_send) && lastchar >= wrongpackets.front())
			{
				wrongpackets.pop();
				inc(next_frame_to_send);
				nbuffered = nbuffered + 1;

			}
			else
			{
				nbuffered = nbuffered + 1;                                             /* expand the sender’s window */
				//the function must run sender timer
				send_data(next_frame_to_send, frame_expected, buffer);                 /* transmit the frame */
				inc(next_frame_to_send);                                               /* advance sender’s upper window edge */
			}
			if ((nbuffered < MAX_SEQ))
			{
				enable_network_layer();

			}
			break;
		case frame_arrival:                                                        /* a data or control frame has arrived */

			from_physical_layer(&r);                                               /* get incoming frame from_physical_layer */

			received_frame_from_physical_layer_info(print_packet(r.info), r.seq, (r.seq == frame_expected));
			if (r.seq == frame_expected)
			{
				/* Frames are accepted only in order. */

				to_network_layer(&r.info);                                         /* pass packet to_network_layer */
				inc(frame_expected);                                               /* advance lower edge of receiver’s window */
				nbuffered = nbuffered - 1;                                         /* one frame fewer buffered */


				stop_timer(ack_expected);                                          /* frame_arrived intact; stop_timer */


				ofstream send("sender.txt", fstream::app);
				send << " received : ack " << ack_expected << endl;
				send.close();
				if (endm == ack_expected)
				{
					end_message_info(senderPath);
				}

				inc(ack_expected);
				if (lastchar >= Data.length() && (r.info.data[MAX_PKT - 1] == Data[Data.length() - 1]) && r.seq == (Data.length() - 1) % (MAX_SEQ + 1))
				{
					end_message_info(receiverPath);
					return;
				}
				if ((nbuffered < MAX_SEQ))
				{
					enable_network_layer();

				}

			}
			else
			{
				//cout << "                                                   neglected  :   " << r.info.data[0] << endl << endl;
			}



			break;

		case cksum_err:


			break;                                                     /* just ignore bad frames */
		case timeout:                                                              /* trouble; retransmit all outstanding frames */

			ofstream send("sender.txt", fstream::app);
			send << " Timeout event   number of packets buffered -> " << nbuffered << endl;
			send.close();
			next_frame_to_send = ack_expected;                                     /* start retransmitting here */
			for (i = 1; i <= nbuffered; i++) {
				send_data(next_frame_to_send, frame_expected, buffer);             /* resend frame */
				send_frame_to_physical_layer_info(print_packet(buffer[next_frame_to_send]), next_frame_to_send);
				inc(next_frame_to_send);                                           /* prepare to send the next one */
			}
		}







		check_ack_timer();
		check_sender_timer();

	}
}

int main()
{

	remove("physicalLayer.txt");
	remove("networkLayer.txt");
	remove("receiver.txt");
	remove("sender.txt");
	ofstream out("receiver.txt");
	out << endl;
	ofstream out1("sender.txt");
	out1 << endl;
	cout << endl<<endl << "\t\t\t\t\t\tGo Back N Protocol simulator" << endl << endl;
	cout << "\t\t\t\t\t\t     packet size=1" << endl;
	cout << "\t\t\t\t\t\t     window size=7" << endl<<endl;
	cout << "please enter the message you want to send:";
	cin >> Data;
	int i;
    cout << "please enter sequence of numbers(in order) represent wrong packet's number  (Enter -1 to finish)"<<endl;
	while (1)
	{
		cin >> i;
		if (i == -1) break;
		wrongpackets.push(i);
	}
	system("start Sender.exe");
	system("start Receiver.exe");
	system("start physical_layer.exe");
	protocol5();
	cout << endl << "\t\t\t\t\t\tEnd of simulation" << endl;
	while (1);
	return 0;



}
