
#pragma once
#include <stdbool.h>
#include "../conn.h"


struct Conn *handle_accept(int fd);
void handle_write(struct Conn *conn );
void handle_read(struct Conn *conn );
bool try_one_request(struct Conn *conn);

void fb_set_nb(int fd);
