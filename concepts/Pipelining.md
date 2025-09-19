
so this concept of pipelining is allowing us to have less RTT (round-trip time) delays
Why ? 

Because our current TCP server , parses only one request per loop and Responds exactly to one 
so this means if the client sends Three simultanious requests : 

he needs to wait for three boucles to be treated until he gets the full message : 
example : 

    - Without Pipelining : 
        Req1 → wait 50ms → Resp1  
        Req2 → wait 50ms → Resp2  
        Req3 → wait 50ms → Resp3  
        
        total time= 150ms 

    - With Pipelining : 
        Req1 Req2 Req3 → wait 50ms → Resp1 Resp2 Resp3  
    
    they are both sent once from the client . 
        
So in summary Pipelining Makes the user wait less !