#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h> 
#include <string.h>
#include <assert.h>


#define K_MAX_MSG 4096

static void do_somthing(int connfd); 
static int32_t read_full(int fd , char *buf , size_t n ); 
static int32_t write_full(int fd ,const char *buf , size_t n ); 
static int32_t one_request(int connfd); 


int main(int argc,char *argv[])
{
    printf("Ready to accept clients \n");

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
        struct sockaddr_in client_addr = {} ;
        socklen_t addrlen = sizeof(client_addr);
        int connfd = accept(fd, (struct sockaddr *)&client_addr, &addrlen);
        if (connfd <= 0 ) {
            continue ; /* error */
        }

        while (1)
        {
            int32_t err = one_request(connfd);
            if (err)
            {
                break;
            }
        }
        close(connfd);
    }



    return 0 ;
}

//reading one request and replying on one request
static int32_t one_request(int connfd)
{
    char rbuf[4+K_MAX_MSG];
    errno = 0 ; 
    int32_t err = read_full(connfd, rbuf , 4);
    if (err)
    {
        perror(errno == 0 ? "EOF" : "read() error"); 
        return err; 
    }
    uint32_t len =  0 ; /* intialize the msg length */     
    memcpy(&len , rbuf , 4);  /* memcpy is */
    if (len > K_MAX_MSG)
    {
        perror("msg too long try another "); 
        return -1; 
    }

    err = read_full(connfd, &rbuf[4], len);
    if(err)
    {
        perror("read error()"); 
        return err ; 
    } 

    //do somthing 
    printf("client says:  %.*s\n",  len, rbuf + 4);
    //reply using the same protocol 
    /* protocol is [4bytes for len message , msg itself ]*/
    const char reply[] = "sahbi kon ragel ";
    char wbuf[4+sizeof(reply)] ; 
    len = (uint32_t)strlen(reply); 

   
    memcpy(wbuf ,&len , 4);  /* copy the len of message to the begening 
    , of memcpy(destination , source , number_of_bytes)*/
    memcpy(&wbuf[4], reply , len); /* copy the actual message right after the first 4 bytes */
    
    return write_full(connfd , wbuf , 4+len);
} 

static void do_somthing(int connfd )    {
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


static int32_t read_full(int fd , char *buf , size_t n )
{
    while(n>0)
    {
        ssize_t rv = read(fd,buf,n);
        if (rv <= 0 )
        {
            return -1 ; /* Error or unexpected EOF*/
        } 
        assert((size_t)rv<=n); 
        n-= (size_t)rv ; 
        buf+= rv ; 
    }
    return 0 ; 
}

static int32_t write_full(int fd ,const char *buf , size_t n )
{
    while(n>0)
    {
        ssize_t rv = write(fd,buf,n);
        if(rv <= 0)
        {
            return -1; 
        } 
        assert((size_t)rv<= n ); /* we use this for the condidtions that should never fail 
        if its called this means that the code or the OS are malfunctioning 
        (so this should never fail )
        */
        n-= (size_t)rv ; 
        buf+= rv ; 
    }
    return 0 ; 
}




