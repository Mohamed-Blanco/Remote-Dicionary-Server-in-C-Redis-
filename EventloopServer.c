

// basicly each client connection needs to keep those states so when we loop on them we can know
// each one what does it wants to do
// and also the incomming and outgoing data from them .

#define K_MAX_MSG 4096
#define K_MAX_FDS 10

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <stdbool.h>

    struct Conn {
            int fd ;
            bool want_read ;
            bool want_write ;
            bool want_close ;

            uint8_t incoming[K_MAX_MSG];
            uint8_t outgoing[K_MAX_MSG];
    };
    //this function is setting the new socket connection to non blocking mode using fcntl unix function
    static void fb_set_nb(int fd)
    {
        errno = 0 ;
        int flags = fcntl(fd,F_GETFL,0);
        if (errno)
        {
            perror("fcntl error ");
        }

        flags |= O_NONBLOCK;

        errno = 0 ;
        (void)(fcntl(fd,F_SETFL,flags));
        if (errno)
        {
            perror("fcntl error");
            exit(0);
        }
    }

    static struct Conn *handle_accept(int fd) /* why fd ? fd is an int that is auto incremented by the operating system, it signs the number of the socket like 0,1,2,3*/
    {
        struct sockaddr_in client_add = {};
        socklen_t addrlen = sizeof(client_add);
        int connfd = accept(fd,(struct sockaddr *)&client_add , &addrlen);
        if (connfd < 0)
        {
            perror(" Cannot accept Client connection ");
            return NULL ;
        }
        uint32_t client_ip = client_add.sin_addr.s_addr ; /* assigning an ip addr */
        fprintf(stderr, "new client from : %u.%u.%u.%u:%u\n", client_ip & 255 , (client_ip >> 8 ) , (client_ip >> 16) & 255 , client_ip >> 24 , ntohs(client_add.sin_port));

        fb_set_nb(fd); /* setting the new socket to non blocking */

        struct Conn conn =  {fd,1,0,0,NULL,NULL};

        return &conn ;
    }

    int main(int argc , char *argv[]) {

        int fd = socket
        (
       AF_INET, /* for ipv4 addresses */
       SOCK_STREAM, /* for TCP , SOCK_DGRAM for UDP */
       0
        );

        /* step 2 : configuring the socket options adding REUSEADDR ... */
        int val = 1 ;
        setsockopt(
            fd, /*the socker */
            SOL_SOCKET,
            SO_REUSEADDR, /* this option allows the program to use the same TCP/IP address without having
            to wait , the TIME_STOP wich is around 30s to 120s */
            &val, /* option value in our case its 1 so they are all enabled */
            sizeof(val)
        );

        /* step 3 : binding into an address */
        struct sockaddr_in addr = {} ;

        addr.sin_family = AF_INET;
        addr.sin_port = htons(1234); // specefying the port
        addr.sin_addr.s_addr = htonl(0); // wildcard IP 0.0.0.0

        int rv = bind(
            fd,
            (const struct sockaddr *)&addr ,
            sizeof(addr)
        );

        if(rv) /*if it returns 0 */
        {
            perror("Error While Binding the socket with the ipv4:PORT ");
            close(rv);
        }

        fb_set_nb(fd);

        //step 4 : listening on the IP/PORT
        rv = listen(
            fd,// the socket
            SOMAXCONN /* sizeof the queue */
        );

        printf("Server started successfully listening ::) ");

        struct Conn *fd2conn[K_MAX_FDS]; //holding all the connnections and their states
        struct pollfd poll_args[K_MAX_FDS]; // a watch list informing us on the state of each socket
        int poll_count = 0 ; /* number of clients sockets */

        /* while the server is running */
        while (true)
        {
            poll_count = 0 ; //clearing the poll count
            const struct pollfd pfd = /* server socket syscall , tell me when there is a new client trying to connect ? */
            {
                fd,
                POLLIN,
                0
            };

            ++poll_count; /* incrementing the number of sockets in the watch list */
            poll_args[poll_count-1] = pfd ; // adding the  server socket first

            for (int i = 0 ; i < K_MAX_FDS; i++ )
            {
                if (!fd2conn[i]) continue; /*for non sockets */
                struct pollfd pfd_client =
                {
                    fd2conn[i]->fd, /* socket */
                    POLLERR, /*error poll()*/
                    0
                };

                if (fd2conn[i]->want_read)
                {
                    pfd_client.events |= POLLIN ;
                }
                if (fd2conn[i]->want_write)
                {
                    pfd_client.events |= POLLOUT ; /* this is a bitwise operation, it means that we are updating only the PLLOUT state
                    without affecting the other flags */
                }

                ++poll_count; /* since we are not using dynamique allocation of the array */
                poll_args[poll_count-1] = pfd;
            }

            /* wait for readiness */
            int rv_client = poll(poll_args , poll_count , -1 );
            if (rv_client < 0 && errno == EINTR) continue ;

            if (rv_client < 0 )
            {
                perror("error with poll  ");
                exit(1);
            }

            /* handle the server socket */
            /*listens if there is any client that wants to connect to us */
            if (poll_args[0].revents)
            {
                int fd2_count = 0 ;
                /* counting the existing clients */
                for (int i= 0 ; i <= K_MAX_FDS ; i++ )
                {
                    if (fd2conn[i]) ++fd2_count;
                }
                /* accepting the client connection */
                if (struct Conn *conn = handle_accept())
                {
                    if (fd2_count < K_MAX_MSG)
                    {
                        assert(!fd2conn[conn->fd]); /* checks if there is already an connection in that fd2conn slot */
                        fd2conn[conn->fd] = conn ;
                    }else
                    {
                        perror("Size not enough to accept this client ");
                    }
                }
            }

        }
    }


