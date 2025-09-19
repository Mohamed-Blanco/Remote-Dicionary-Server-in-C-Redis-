DDOS Attack Protection : 
- How do we secure the server from misbehaving clients ? 
- if a client sent an large amount of messages or sent an message that is large than our SIZE .
- we drop imedialty the connection with this user (conn->want_close = true) 
- Why ? : 
  - to avoid the attack we should not take time to treat this request if we did not 
  - either we are waiting endless time for a request 
  - or we allow the user to drain our server with similar messages 
