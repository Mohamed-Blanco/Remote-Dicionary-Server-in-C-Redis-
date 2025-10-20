#include "eventloop/eventloop.h"
#include "messages/messages.h"
#include "clients/clients.h"
#include "server/server.h"
#include "conn.h"
#include "globals/globals.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <poll.h>
#include <assert.h>
#include <time.h>      // for clock_gettime() and CLOCK_MONOTONIC
#include <stdint.h>    // optional, if you need uint64_t for conversions
#include "DLinkedList/list.h"
#include "dynamicArrays/DynamicArrays.h"

#define K_MAX_TTL_WORK  10
const uint64_t k_idle_timeout_ms = 20 * 1000;

static uint64_t get_monotonic_msec() ;
static int32_t next_timer_ms();
static void process_timers();
static void conn_destroy(struct Conn *conn,const size_t i );

struct Conn clients_cons[MAX_CLIENTS] = {0};
int number_of_clients = 1 ; //tracking the number of the clients 1-index since first one is taken by the server
struct pollfd event_loop[MAX_CLIENTS] = {-1};


int main() {

	int server_fd = intialize_server();

	while (true)
	{
		memset(event_loop,0,sizeof(event_loop));

		//adding the server socket, with ready to read (ACCPET CONNECTIONS) flag , first POLLIN
		struct pollfd pfd_server = {server_fd,POLLIN , 0};
		event_loop[0] = pfd_server ;

		for (int i = 1 ; i < MAX_CLIENTS ; i++ )
		{
			if (clients_cons[i].fd == 0) continue; // no connection in this slot

			struct pollfd pfd = {clients_cons[i].fd , POLLERR , 0 }; /* tells the system to notify us if there is any error */

			if (clients_cons[i].want_read)	pfd.events |= POLLIN ;
			if (clients_cons[i].want_write) pfd.events |= POLLOUT ;

			event_loop[i] = pfd ;
		}

		/* wait for readiness of the sockets */
		//poll syscall
		int32_t timeout_ms = next_timer_ms(); // get the min-heap element
		int rv = poll(event_loop , MAX_CLIENTS , timeout_ms ); //never timeout

		if (rv < 0 && errno == EINTR) continue; //if there was any problem with server socket

		if (rv<0)
		{
			perror("error during poll");
		}

		if (event_loop[0].revents ) // handeling the server socket
			// note : for server socket we are just interested in accepting new connections not reading or writing
		{
			struct Conn *conn = handle_accept(server_fd);


			if (conn && number_of_clients+1 < MAX_CLIENTS)
			{
				int slot = -1;
				for (int i = 1; i < MAX_CLIENTS; ++i)
				{
					if (clients_cons[i].fd == 0)
					{
						slot = i;
						break;
					}
				}

				assert(slot != -1);
				assert(!clients_cons[slot].fd);

				// Copy the entire struct FIRST
				clients_cons[slot] = *conn;
				// Then RESET the idle_node to a clean state
				clients_cons[slot].idle_node.next = NULL;
				clients_cons[slot].idle_node.prev = NULL;

				clients_cons[slot].last_active_ms = get_monotonic_msec();
				dlist_insert_before(&glob_db.idle_list, &clients_cons[slot].idle_node);

				number_of_clients += 1;
				free(conn);
			}
		}

		//handle connection sockets
		for (size_t i = 1 ; i < MAX_CLIENTS ; ++i)
		{
			// Check if connection exists FIRST
			if (clients_cons[i].fd == 0) {
				continue;
			}

			uint32_t ready = event_loop[i].revents;
			if (ready == 0)
			{
				continue;
			}

			if (event_loop[i].revents) {
				struct Conn *conn = &clients_cons[i]; // safe because same slot
				//whenever the element is ready or has event means that is still alive
				// we need to update the last used time , and also move it into the back of the heap
				conn->last_active_ms = get_monotonic_msec();
				dlist_detach(&conn->idle_node);
				dlist_insert_before(&glob_db.idle_list, &conn->idle_node);

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
					conn_destroy(conn, i );
				}
			}
		}

		process_timers();
	}
	return 0 ;
}

static void conn_destroy(struct Conn *conn,const size_t i )
{
	dlist_detach(&conn->idle_node);
	(void)close(conn->fd);
	memset(conn,0,sizeof(*conn));
	--number_of_clients;
	clients_cons[i].fd = 0 ;
	event_loop[i].fd = -1 ;
}

static uint64_t get_monotonic_msec() {
    struct timespec tv = {0, 0};
    clock_gettime(CLOCK_MONOTONIC, &tv);
    return (uint64_t)tv.tv_sec * 1000 + tv.tv_nsec / 1000 / 1000;
}

static void process_timers()
{
	uint64_t now_ms = get_monotonic_msec();
	DList *node = glob_db.idle_list.next;

	const Array *heap = &glob_db.heap ;
	size_t nworks = 0 ;



	while ( heap != NULL && heap->used > 0 && heap->array[0].val < now_ms && nworks < K_MAX_TTL_WORK )
	{
		HeapItem *top = &heap->array[0];
		if (!top->ref) break;
		Entry *ent = container_of(top->ref, Entry, heap_idx);

		//deleting from the hashtable
		HNode *hnode = hm_delete(&glob_db.db , &ent->node , &hnode_same ) ;

		//assert(hnode == &ent->node);
		fprintf(stderr, "key expired: %s\n", ent->key );

		//deleting the intrusive data structure
		entry_del_ttl(ent);
		++nworks ;
	}


	//TTL FOR THE Clients
	while (node != &glob_db.idle_list) {//since its cercular doubly linked list
		struct Conn *conn = container_of(node, struct Conn, idle_node);
		uint64_t expire_time = conn->last_active_ms + k_idle_timeout_ms;

		if (expire_time > now_ms) break; //since this node has more time to live , means that the newer ones have more
		//so just stop here , because the remaining are all still not expired

		node = node->next; // move before detach
		dlist_detach(&conn->idle_node);

		int fd_to_close = conn->fd;  // Save fd BEFORE clearing
		fprintf(stderr, "removing idle connection: fd=%d\n", fd_to_close);

		if (fd_to_close > 0) {
			if (close(fd_to_close) == -1) {
				fprintf(stderr, "ERROR closing fd %d: %s\n", fd_to_close, strerror(errno));
			}
		} else {
			fprintf(stderr, "ERROR: tried to close invalid fd: %d\n", fd_to_close);
		}

		// find the slot and deletee it
		for (int i = 1; i < MAX_CLIENTS; i++) {
			if (&clients_cons[i] == conn) {
				memset(&clients_cons[i], 0, sizeof(struct Conn));
				--number_of_clients;
				break;
			}
		}
	}
}

static int32_t next_timer_ms()
{
	if (dlist_empty(&glob_db.idle_list)) // if its null
	{
		return -1 ;  //means there is no timers -> no timeouts .
	}

	uint64_t now_ms = get_monotonic_msec();
	struct Conn *conn = container_of(glob_db.idle_list.next , struct Conn , idle_node ) ;
	uint64_t next_ms = conn->last_active_ms + k_idle_timeout_ms ;

	//TTL timers using a heap :

	if (glob_db.heap.used > 0 && glob_db.heap.array[0].val < next_ms)
	{
		next_ms = glob_db.heap.array[0].val ;
	}




	if (next_ms == (uint64_t)-1)
	{
		return -1 ; // no timers , no timeouts
	}

	if (next_ms <= now_ms)
	{
		return 0 ; // means that this is a non idle connection
	}

	return (int32_t)(next_ms-now_ms);
}

