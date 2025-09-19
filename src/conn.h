#ifndef CONN_H   // If CONN_H is not defined
#define CONN_H   // Define it

#include <stdio.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#define K_MAX_MSG 4096
#define PONG "+PONG\r\n"
#define MAX_CLIENTS 10

    struct Conn
    {
        int fd ;
        bool want_read ;
        bool want_write ;
        bool want_close ;

        /*For network protocols or low-level TCP communication, you want exact 1-byte chunks, which is why uint8_t is used.*/
        uint8_t incomming[K_MAX_MSG]; /* an array of bytes each byte is exacltly 8 bits in any computer */
        uint8_t	outgoing[K_MAX_MSG];

        size_t incomming_len ; /* size_t is basicly an unsigned integer that is able to hold the largest possible size of peice of data
        or index for the current system */
        size_t outgoing_len ; /* and also it cannot be negative */
    };


#endif // CONN_H
