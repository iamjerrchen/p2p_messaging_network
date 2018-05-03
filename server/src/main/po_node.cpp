#include "po_node.h"

#include "global_data.h"
#include "connection.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
// system logging
#include <string.h>
#include <syslog.h>
#include <errno.h>
#include <iostream>

/* 
 * @purpose: server <- client
 * 		Main function for a neighbor to server connection thread.
 *		This is used when this node is acting a server to a neighbor client.
 * @param:
 *
 * @return:	
 *		
 */
void start_neighbor_to_server_conn(std::string ip, int sock_fd, global_data* data)
{
	// data->num_connections++;
	Connection *stream = new Connection(sock_fd, ip, data);


	// TODO: Spawn receive message and send message thread
	// listen for messages and send messages simultaneously
	// while(1) // change to 1
	// {
		if(stream->receive_message())
		{
			stream->send_message();
		}
	// }


	return;
}

/* 
 * @purpose: client -> server
 * 		Main function for a server to neighbor connection thread.
 *		This is used when this node is acting as a client to a neighbor server
 * @param:
 *
 * @return:	
 *		
 */
 bool start_server_to_neighbor_conn(std::string ip, global_data* data)
 {
	// data->num_connections++;

 	int sock_fd;
 	Socket client_sock;
 	if((sock_fd = client_sock.setup_client_socket(SERVER_PORT, ip.c_str())) < 0)
 	{	// unable to create client
 		syslog(LOG_NOTICE, "[po_node] Unable to connect to the neighbor ip (%s)", ip.c_str());
 		return false;
 	}

 	Connection* stream = new Connection(sock_fd, ip, data);

 	// send initialization message
 	stream->greet_neighbor();
 	stream->receive_message(); // waits till message is received
 	
 	std::cout << "Message received" << std::endl;
 	while(0) // change to 1
 	{
 		// listen for messages and send messages simultaneously
 	}

 	// close(sock_fd);
 	return true;
}

void start_client_conn(global_data * data)
{
	int sock_fd;
	char buf[2048];

	client_connection connection = client_connection(data);
	Socket client_listener;
	client_listener.setup_server_socket(CLIENT_PORT);
	while (1) {
		if ((sock_fd = client_listener.accept_conn()) < 0) {
			continue;
		}

		int request_length = read(sock_fd, buf, 2048);
		if (request_length == 0) {
			printf("Nothing has been read\n");
			write(sock_fd, "FAIL", 4);
		} else {
			std::string request = std::string(buf).substr(0, request_length);
			connection.process_request(sock_fd, request);
		}

		close(sock_fd);
	}
}
