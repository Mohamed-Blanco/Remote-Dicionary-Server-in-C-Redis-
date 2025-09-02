

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

static void do_somthing(int); 


int main(int argc,char *argv[])
{   

    int fd = socket(AF_INET , SOCK_STREAM, 0);
    if (fd < 0 )
    {
        close(fd); 
    }
    
    struct sockaddr_in addr = {}; 
    addr.sin_family = AF_INET ; 
    addr.sin_port = ntohs(1234); 
    addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK); //127.0.0.1

    
    int rv = connect(fd,(const struct sockaddr *)&addr, sizeof(addr)) ; 

    if(rv < 0 )
    {
        perror("connection problem client "); 
    }

    char msg[] = "hello"; 
    write(fd,msg,strlen(msg)); 

    char rbuf[64] = {}; 
    ssize_t n = read(fd,rbuf, sizeof(rbuf)-1);

    printf("the server say's %s ", rbuf); 
    close(fd); 

    return 0 ; 
}
