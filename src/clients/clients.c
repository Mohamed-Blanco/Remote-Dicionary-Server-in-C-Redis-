
#include "clients.h"
#include "../messages/messages.h"

struct Conn *handle_accept(int fd)
{
    //accept connection
    struct sockaddr_in client_addr = {};
    socklen_t addrlen = sizeof(client_addr);
    int connfd = accept(fd,(struct sockaddr *)&client_addr , &addrlen );
    if (connfd < 0)
    {
        perror("accept connection error ");
        return NULL ;
    }
    uint32_t ip = client_addr.sin_addr.s_addr ;
    fprintf(stderr, "new client from %u.%u.%u.%u:%u\n",
        ip & 255, (ip >> 8) & 255, (ip >> 16) & 255, ip >> 24,
        ntohs(client_addr.sin_port)
    );

    //set socket into non blocking mode
    fb_set_nb(connfd);

    //create a struct conn.c
    struct Conn *conn  = malloc(sizeof(struct Conn));
    memset(conn , 0 , sizeof(*conn));  /* cleaning the allocated memory from garbage */
    conn->fd = connfd ;

    conn->want_read = true ;
    return conn ;
}

void handle_read(struct Conn *conn )
{
    uint8_t buf[K_MAX_MSG]; //preparing the buffer
    ssize_t rv = read(conn->fd,buf , K_MAX_MSG);
    if (rv + conn->incomming_len > K_MAX_MSG )
    {
        conn->want_close = true ;
        perror("Message exceeds the memory limit ");
        return;
    }
    if (rv < 0 && errno == EAGAIN ) // EAGAIN means there is no data available right now, try again later". Only when using non blocking sockets I/O
    {
        return ;
    }
    if (rv < 0 ) //there is any error with file descriptor
    {
        conn->want_close  = true ;
        perror("error want close ");
        return ;
    }
    if (rv == 0 ) // connection closed
    {
        if (conn->incomming_len == 0)
        {
            perror("client closed");
        }else
        {
            perror("unexpected EOF");
        }
        conn->want_close = true ;
        return ;
    }

    memcpy(
        conn->incomming + conn->incomming_len //means that we will start from the incomming_len + postion
        , buf,
        (size_t)rv
    );

    conn->incomming_len += rv;

    //here we are waiting for the readiness of the msg , as soon there is an full msg we will respond
    while (try_one_request(conn)){}

    //if responses are ready , switch to write mode imediatly
    if (conn->outgoing_len > 0 )
    {
        conn->want_read = false ;
        conn->want_write = true ;
    }else
    {
        conn->want_read = true ;
    }
}

void handle_write(struct Conn *conn )
{
    assert(conn->outgoing_len > 0);
    ssize_t rv = write(conn->fd, conn->outgoing, conn->outgoing_len);
    if (rv < 0 && errno == EAGAIN)
    {
        return ;
    }

    if (rv < 0)
    {
        perror("write() error ");
        conn->want_close = true ;
        return ;
    }

    /*
         *we cannot assume that the whole buffer is written, due to multiple conditions :
        * Socket send buffer is nearly full
        System is under load
        Network congestion
    */
    buf_consume(conn->outgoing , &conn->outgoing_len , rv); //erease the readen data

    if (conn->outgoing_len  == 0 )
    {
        conn->want_write = false ;
        conn->want_read = true ;
    }else
    {
        conn->want_write = true ;
    }
}

//setting the socket into non blocking mode
void fb_set_nb(int fd)
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




