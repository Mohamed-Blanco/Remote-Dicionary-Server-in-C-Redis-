#include "messages.h"
#include "../conn.h"
#include "../KVstorage/KVstorage.h"
#define OK "+OK\r\n"

void buf_append(uint8_t *outgoing_buf ,size_t *outgoing_len, const uint8_t *data , size_t lenof_data )
{
    memcpy(outgoing_buf + *outgoing_len , data , lenof_data);
    *outgoing_len += lenof_data ;
}

// and remove it from the front of the incomming request we cann also use it in handle_write
void buf_consume(uint8_t *buf ,size_t *total_len , size_t nbytes_todelte)
{
    if (nbytes_todelte >= *total_len)
    {
        *total_len = 0 ;
    }else
    {
        memmove(buf , buf+nbytes_todelte , *total_len-nbytes_todelte);
        *total_len -= nbytes_todelte;
    }
}

char* byte_to_string(uint8_t *message_inbytes, size_t message_len )
{
    char *outmessage = malloc(message_len+1) ;
    if (!outmessage) return  NULL ;
    memcpy(outmessage,message_inbytes,message_len);
    outmessage[message_len] = '\0';
    return outmessage ;
}

int extract_number(char *p , char* end)
{

    char tmp = *end ;
    *end = '\0';
    int number_elements = atoi(p); //it stops first non-digit char
    *end = tmp ;

    return number_elements ;
}

void handle_PING(struct Conn *client_conn )
{
    buf_append(client_conn->outgoing , &client_conn->outgoing_len , PONG , strlen(PONG) );
    buf_consume(client_conn->incomming , &client_conn->incomming_len ,14);
}

bool handle_ECHO(char  *msg_ptr , struct Conn *client_conn, char* message)
{
    char *newline = strchr(message,'\n'); // extract the length of the message
    if (!newline) return false; // if we did not find the end of that message

    int echo_message_length = extract_number(message+1, newline);
    printf("the echo message length is : %d \n", echo_message_length);

    if (echo_message_length >= K_MAX_MSG) {
        client_conn->want_close = true;
        perror("ECHO Message is too long");
        perror("Connection Closed");
        return false;
    }

    // Move to the actual message content
    message = newline + 1;

    // See if the message is received completely
    newline = strchr(message,'\n');
    if (!newline) return false;

    // Extracting the message (FIX: +1 for null terminator)
    char echo_msg[echo_message_length + 1];
    memcpy(echo_msg, message, echo_message_length);
    echo_msg[echo_message_length] = '\0';  // FIX: Correct index

    printf("Here is the client ECHO Message: %s\n", echo_msg);  // FIX: Print echo_msg

    // Building the Response Message with proper length handling
    char length_str[16];
    int length_str_len = sprintf(length_str, "%d", echo_message_length);

    // Calculate proper buffer size: $ + length_digits + \r\n + message + \r\n
    int response_size = 1 + length_str_len + 2 + echo_message_length + 2;
    char echo_response[response_size + 1]; // +1 for null terminator

    int pos = 0;
    echo_response[pos++] = '$';

    // FIX: Use the length_str, not the wrong cast!
    memcpy(echo_response + pos, length_str, length_str_len);
    pos += length_str_len;

    echo_response[pos++] = '\r';
    echo_response[pos++] = '\n';

    memcpy(echo_response + pos, echo_msg, echo_message_length);
    pos += echo_message_length;

    echo_response[pos++] = '\r';
    echo_response[pos++] = '\n';
    echo_response[pos] = '\0';

    // Use exact size instead of strlen
    buf_append(client_conn->outgoing, &client_conn->outgoing_len,
               (uint8_t*)echo_response, response_size);

    // FIX: Just use total_msg_len - it's already calculated correctly
    buf_consume(client_conn->incomming, &client_conn->incomming_len, ((newline)-msg_ptr)+1);

    return true;
}

bool handle_SET(char *msg_ptr , struct Conn *client_conn , char* message)
{
    char *newline = strchr(message,'\n'); // extract the length of the message
    if (!newline) return false; // if we did not find the end of that message

    int set_key_length = extract_number(message+1, newline);
    printf("SET KEY MSG LENGTH IS : %d \n", set_key_length);

    if (set_key_length >= K_MAX_MSG) {
        client_conn->want_close = true;
        perror("SET KEY LENGTH IS TO LONG ! \n ");
        perror("CONNECTION CLOSED \n BYE ;) ");
        return false;
    }

    message = newline + 1;

    // See if the message is received completely
    newline = strchr(message,'\n');
    if (!newline) return false;

    // Extracting the message (FIX: +1 for null terminator)
    char set_key[set_key_length + 1];
    memcpy(set_key, message, set_key_length);
    set_key[set_key_length] = '\0';  // FIX: Correct index

    printf("SET KEY IS : %s\n", set_key);  // FIX: Print echo_msg

    // Move to the actual message content
    message = newline + 1;

    //Extracting the Value
    newline = strchr(message,'\n');
    if (!newline) return false;

    int set_value_length = extract_number(message+1, newline);
    printf("SET VALUE MSG LENGTH IS : %d \n", set_value_length);

    if (set_value_length >= K_MAX_MSG) {
        client_conn->want_close = true;
        perror("SET VALUE LENGTH IS TO LONG ! \n ");
        perror("CONNECTION CLOSED \n BYE ;) ");
        return false;
    }

    message = newline + 1;

    // See if the message is received completely
    newline = strchr(message,'\n');
    if (!newline) return false;

    char set_value[set_value_length + 1];
    memcpy(set_value, message, set_value_length);
    set_value[set_value_length] = '\0';

    printf("SET VALUE IS : %s\n", set_value);

    store_kv(set_key ,set_key_length , set_value, set_value_length);

    buf_append(client_conn->outgoing , &client_conn->outgoing_len , OK , strlen(OK) );
    buf_consume(client_conn->incomming, &client_conn->incomming_len, ((newline)-msg_ptr)+1);

    return true ;
}

bool handle_GET(char *msg_ptr , struct Conn *client_conn , char* message)
{
    char *newline = strchr(message,'\n'); // extract the length of the message
    if (!newline) return false; // if we did not find the end of that message

    int get_key_length = extract_number(message+1, newline);
    printf("GET KEY MSG LENGTH IS : %d \n", get_key_length);

    if (get_key_length >= K_MAX_MSG) {
        client_conn->want_close = true;
        perror("GET KEY LENGTH IS TO LONG ! \n ");
        perror("CONNECTION CLOSED \n BYE ;) ");
        return false;
    }

    message = newline + 1;

    // See if the message is received completely
    newline = strchr(message,'\n');
    if (!newline) return false;

    // Extracting the message (FIX: +1 for null terminator)
    char get_key[get_key_length + 1];
    memcpy(get_key, message, get_key_length);
    get_key[get_key_length] = '\0';  // FIX: Correct index

    printf("GET KEY IS : %s\n", get_key);  // FIX: Print echo_msg
    char *value = retreive_value(get_key , get_key_length);

    buf_append(client_conn->outgoing , &client_conn->outgoing_len , "$3\r\nbar\r\n" , strlen("$3\r\nbar\r\n") );
    buf_consume(client_conn->incomming, &client_conn->incomming_len, ((newline)-msg_ptr)+1);

    return true ;
}

bool parse_command(char *msg_ptr,char *command_start, int command_length , struct  Conn *conn , int elements_number )
{
    char command[command_length+1] ;
    memcpy(command,command_start,command_length);
    command[command_length] = '\0';

    if (strcmp(command,"PING") == 0)
    {
        printf("This is an PING command \n");
        handle_PING(conn);
        return true ;
    }
    else if (strcmp(command,"ECHO") == 0 )
    {
        printf("This is an ECHO command \n");
        if (elements_number != 2 ) return false;
        return handle_ECHO(msg_ptr , conn,command_start+command_length+2); //starting from the end of \n , "ECHO\r\n <-- from here"
    } else if (strcmp(command,"SET") == 0)
    {
        printf("this is a SET Command \n");
        return  handle_SET(msg_ptr, conn , command_start+command_length+2 );
    }  else if (strcmp(command,"GET") == 0)
    {
        printf("PARSED INTO A GET COMMAND ! \n");
        return  handle_GET(msg_ptr , conn,command_start+command_length+2);
    }

    return false;
}

bool try_one_request(struct Conn *client_con)
{

    if (client_con->incomming_len < 1 ) return  false ;
    char *message = byte_to_string(client_con->incomming , client_con->incomming_len);
    char *msg_ptr = message ;

    if (message[0] != '*') {
        free(msg_ptr);
        return false ;
    }
    char *newline = strchr(message,'\n');
    if (!newline) {
        free(msg_ptr);
        return false ;
    }
    int elements_number = extract_number(message +1,newline);
    printf("number of elements in the array :  %d \n", elements_number);

    message = newline +1;//skip the first section, and getting to "$"
    newline = strchr(message,'\n');
    if (!newline)
    {
        free(msg_ptr);
        return false ;
    }
    int command_length = extract_number(message+1 , newline);
    printf("the Command Length : %d \n", command_length);
    if (command_length >= K_MAX_MSG )
    {
        client_con->want_close = true ;
        perror("Command is too long");
        perror("Connection Closed ");
        free(msg_ptr);
        return  false ;
    }

    message = newline +1 ;
    newline = strchr(message,'\n');
    if (!newline)
    {
        free(msg_ptr);
        return false ;
    }
    if (!parse_command(msg_ptr,message,command_length,client_con,elements_number))
    {
        free(msg_ptr);
        return false ;
    }

    free(msg_ptr);
    return true ;
}

