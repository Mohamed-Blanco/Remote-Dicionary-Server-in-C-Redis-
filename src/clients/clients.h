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
#include <poll.h>
#include <assert.h>
#include <fcntl.h>
#include <poll.h>
#include "../conn.h"

struct Conn *handle_accept(int fd);
void handle_write(struct Conn *conn );
void handle_read(struct Conn *conn );
bool try_one_request(struct Conn *conn);

void fb_set_nb(int fd);
