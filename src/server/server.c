
#include "server.h"


int intialize_server()
{
    // Disable output buffering
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

     // You can use print statements as follows for debugging, they'll be visible when running tests.
    printf("Logs from your program will appear here!\n");

     int server_fd;

     server_fd = socket(AF_INET, SOCK_STREAM, 0);
     if (server_fd == -1) {
        printf("Socket creation failed: %s...\n", strerror(errno));
        return -1;
     }

      //Since the tester restarts your program quite often, setting SO_REUSEADDR
      //ensures that we don't run into 'Address already in use' errors
     int reuse = 1;

     if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        printf("SO_REUSEADDR failed: %s \n", strerror(errno));
        return -1;
     }

     struct sockaddr_in serv_addr = { .sin_family = AF_INET ,
                                     .sin_port = htons(6379),
                                     .sin_addr = { htonl(INADDR_ANY) },
                                    };

     if (bind(server_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) != 0) {
        printf("Bind failed: %s \n", strerror(errno));
        return -1;
     }

	return server_fd ;
}

