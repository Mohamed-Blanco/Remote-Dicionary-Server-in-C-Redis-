#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <errno.h> 
#include <assert.h>

#define K_MAX_MSG 4096

static int32_t query(int fd , const char *text); 
static int32_t read_full(int fd , char *buf , size_t n ); 
static int32_t write_full(int fd ,const char *buf , size_t n ); 

int main(int argc,char *argv[])
{
    int fd = socket(AF_INET , SOCK_STREAM, 0);
    if (fd < 0 )
    {
        perror("socket");
        return 1;
    }
    
    struct sockaddr_in addr = {}; 
    addr.sin_family = AF_INET ; 
    addr.sin_port = ntohs(1234); 
    addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK); //127.0.0.1 , is defined by 0x7f000001 localhost basicly 

    
    int rv = connect(fd,(const struct sockaddr *)&addr, sizeof(addr)) ; 

    if(rv < 0 )
    {
        perror("connection problem client");
        close(fd); 
        return -1 ;  // non zero exit means error 
    }

    int32_t err = query(fd , "hello1"); 
    if(err)
    {
        goto L_DONE;    
    }

    err = query(fd , "hello2"); 
    if(err)
    {
        goto L_DONE ;
    }

    L_DONE : 
        close(fd); 
        return 0 ; 
}

static int32_t query(int fd , const char *text)
{
    uint32_t len = (uint32_t) strlen(text); 
    if (len > K_MAX_MSG)
    {
        return -1 ; 
    }

    //send request
    char wbuf[4+K_MAX_MSG];
    memcpy(wbuf , &len , 4 ); //assume little endian  
    memcpy(&wbuf[4],text , len);

    int32_t err = write_full(fd , wbuf , len +4);
    if(err)
    {
        return err ; 
    } 

    // receiving the 4byte header
    char rbuf[4+K_MAX_MSG];
    uint32_t rlen= 0 ;

    err = read_full(fd , rbuf , 4);
    if (err)
    {
        perror("read length");
        return err ;
    }

    // step 2 extract the length
    memcpy(&rlen, rbuf , 4);

    if(rlen > K_MAX_MSG) {
        perror("message too long");
        return -1;
    }

    // step 3 reading the actual message
    err = read_full(fd , rbuf +4 , rlen );
    if (err)
    {
        perror("reading the full message error ");
        return err ;
    }

    printf("server says %.*s\n" , rlen , rbuf+4);
    return 0 ;
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



