
#include "../conn.h"



void handle_PING(struct Conn *client_conn ) ;
bool handle_GET(char *msg_ptr , struct Conn *client_conn , char* message);
bool handle_SET(char *msg_ptr , struct Conn *client_conn , char* message);
void buf_append(uint8_t *outgoing_buf ,size_t *outgoing_len, const uint8_t *data , size_t lenof_data );
void buf_consume(uint8_t *buf ,size_t *total_len , size_t nbytes_todelte);
char* byte_to_string(uint8_t *message_inbytes, size_t message_len );
int extract_number(char *p , char* end);
bool handle_ECHO(char  *msg_ptr , struct Conn *client_conn, char* message);
bool parse_command(char *msg_ptr,char *command_start, int command_length , struct  Conn *conn , int elements_number );
bool try_one_request(struct Conn *conn);

