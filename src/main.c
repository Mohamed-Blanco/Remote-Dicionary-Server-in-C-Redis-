
#include "eventloop/eventloop.h"
#include "messages/messages.h"
#include "clients/clients.h"
#include "server/server.h"
#include "conn.h"

int main() {

/*
	// Disable output buffering
	setbuf(stdout, NULL);
	setbuf(stderr, NULL);

	 // You can use print statements as follows for debugging, they'll be visible when running tests.
	printf("Logs from your program will appear here!\n");

	 int server_fd;

	 server_fd = socket(AF_INET, SOCK_STREAM, 0);
	 if (server_fd == -1) {
	 	printf("Socket creation failed: %s...\n", strerror(errno));
	 	return 1;
	 }

	  //Since the tester restarts your program quite often, setting SO_REUSEADDR
	  //ensures that we don't run into 'Address already in use' errors
	 int reuse = 1;
	 if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
	 	printf("SO_REUSEADDR failed: %s \n", strerror(errno));
	 	return 1;
	 }

	 struct sockaddr_in serv_addr = { .sin_family = AF_INET ,
	 								 .sin_port = htons(6379),
	 								 .sin_addr = { htonl(INADDR_ANY) },
	 								};

	 if (bind(server_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) != 0) {
	 	printf("Bind failed: %s \n", strerror(errno));
	 	return 1;
	 }*/
	 int server_fd = intialize_server();

	 int connection_backlog = 5;
	 if (listen(server_fd, connection_backlog) != 0) {
	 	printf("Listen failed: %s \n", strerror(errno));
	 	return 1;
	 }

	printf("Waiting for a client to connect...\n");

	struct Conn clients_cons[MAX_CLIENTS] = {0};
	int number_of_clients = 1 ; //tracking the number of the clients 1-index since first one is taken by the server

	struct pollfd event_loop[MAX_CLIENTS] = {-1};


	for (int i = 0; i < MAX_CLIENTS; ++i) {
		printf("slot %d fd=%d\n", i, clients_cons[i].fd);
	}

	while (true)
	{
		//Event loop
		while (true)
		{
			memset(event_loop,0,sizeof(event_loop));

			//adding the server socket, with ready to read (ACCPET CONNECTIONS) flag , first POLLIN
			struct pollfd pfd = {server_fd,POLLIN , 0};
			event_loop[0] = pfd ;


			for (int i = 1 ; i < MAX_CLIENTS ; i++ )
			{
				if (clients_cons[i].fd == 0)
				{
					continue; // no connection in this slot
				}

				struct pollfd pfd = {clients_cons[i].fd , POLLERR , 0 }; /* tells the system to notify us if there is any error */

				if (clients_cons[i].want_read)
				{
					pfd.events |= POLLIN ;
				}
				if (clients_cons[i].want_write)
				{
					pfd.events |= POLLOUT ;
				}

				event_loop[i] = pfd ;
			}

				/* wait for readiness of the sockets */
			int rv = poll(event_loop , MAX_CLIENTS , -1 ); //never timeout

			if (rv < 0 && errno == EINTR)
			{
				continue;
			}
			if (rv<0)
			{
				perror("error during poll");
			}

			if (event_loop[0].revents ) // handeling the server socket
				// note : for server socket we are just interested in accepting new connections not reading or writing
			{
				struct Conn *conn = handle_accept(server_fd);

				if (conn && number_of_clients+1 < MAX_CLIENTS ) //accpeting the connection
				{
					int slot = -1 ;
					for (int i = 1 ; i < MAX_CLIENTS ; ++i)
					{
						if (clients_cons[i].fd == 0 )
						{
							slot = i ;
							break ;
						}
					}
					assert(!clients_cons[slot].fd);
					clients_cons[slot] =  *conn ;
					number_of_clients+=1 ;
					free(conn);
				}
			}

			//handle connection sockets
			for (size_t i = 1 ; i < MAX_CLIENTS ; ++i)
			{
				uint32_t ready = event_loop[i].revents;
				if (ready == 0)
				{
					continue;
				}

				if (event_loop[i].revents) {
					struct Conn *conn = &clients_cons[i]; // safe because same slot

					if (ready & POLLIN )
					{
						assert(conn->fd);
						handle_read(conn);
					}

					if (ready & POLLOUT)
					{
						assert(conn->fd);
						handle_write(conn);
					}

					if ((ready & POLLERR) || conn->want_close)
					{
						(void)close(conn->fd);
						memset(conn,0,sizeof(*conn));
						--number_of_clients;
						clients_cons[i].fd = 0 ;
						event_loop[i].fd = -1 ;
					}

				}
			}
		}

		close(server_fd);
		return 0 ;
	}
	return 0;
}

