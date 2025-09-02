



#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

static void do_somthing(int connfd); 


int main(int argc,char *argv[])
{
    
    /*step 1 : obtaining the socket using socket()*/
    int fd = socket(
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

    rv = listen(
        fd,// the socket 
        SOMAXCONN /* sizeof the queue */
    ); 

    while(1)
    {
        //accpting connection
        struct sockaddr_in client_addr = {} ;  //the client socket 
        socklen_t addrlen = sizeof(client_addr); 
        int connfd = accept(fd,(struct sockaddr *)&client_addr, &addrlen);  /* returns the client address */
        if (connfd < 0 )
        {
            perror("Accept error");
            continue; /* could not establish a connection with the client */
        }

        printf("Hey Mohamed you will become a cracked dev oneday , and you will be very popular and wealthy !! "); 
        do_somthing(connfd); 
        close(connfd); //closing the socket connection 
    }

    

    return 0 ; 
}


static void do_somthing(int connfd ){
    char rbuf[64] = {}; 
    ssize_t n = read(
        connfd, //peer address 
        rbuf, // received buffer 
        sizeof(rbuf)-1 // size of the allowed buffer to write, -1 since there is /0 end char 
    ); 

    if (n < 0 )
    {
        perror("read() error "); 
        return; 
    }

    printf("client says : %s\n", rbuf ); 
    
    char wbuf[] = "Tebi"; 
    
    write(
        connfd ,
        wbuf, 
        strlen(wbuf)
    );
    
    /* we can also use recv instead of read , and send instead of write , but they use flags that we dont want now */
    
} 

