DDOS Attack Protection

    - Misbehaving clients might send endless or oversized messages.
    - If a message is larger than our allowed size, we immediately close the connection (conn->want_close = true).

    Why? :
        - Don’t waste server time processing invalid requests.
        - Prevent clients from flooding and slowing down the server.
        - Avoid hanging forever waiting for a message that never ends