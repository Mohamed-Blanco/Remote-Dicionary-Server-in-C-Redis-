

RESP Serialization Protocole  : 
 - Redis uses this protocole to receive and send messages , here is an example : 
 - Client sends : *1\r\n$4\r\nPING\r\n
 - so How the server is going to treat this message ?? 

 this request contains alot off informations other than the message .
 first Byte represents the status : 
    - + = Simple String 
    - - = Simple Error 
    - : = Integers 
    - * = Arrays 
 to see all the signinfications : https://redis.io/docs/latest/develop/reference/protocol-spec/

 RESP uses \r\n as there deliminator it separates the message sections 
 Note : \r \n are considered single bytes elements 
 
 to read the full message we need to know first the length of it : 
 so the length is defined by the Byte "$4"
 
 this means that we going to receive a message of length = 4 
 then we can simple check for the \n\r if this section is completed or not 

 after that we can extract the msg , in our case the message is PING : 
 Note : PING is often used to test if the server is started or not 
 The Server should respond with PONG , if everything is fine : 
 server response : "+PONG\n\r" this is a Simple String 
