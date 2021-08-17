#pragma once
#include <string>
#include <iostream>
#include <winsock2.h>

// NOTE: by default, any socket associated with a packet is NONBLOCKING. Use should_block to override this!
// each recv_into RESETS TO !! NONBLOCKING !!

enum Commands{
    ACK         = 0,
    BAD         = 1,
    LOGIN       = 2,
    MESSAGE     = 3,
    OK          = 4,
    UNAME_TAKEN = 5,
    BYE         = 6
};

class Packet{
    public:
        char sender[100];           // the username of the sender
        int command;                // command to be used 0 = ACK, 1 = BAD, 2 = LOGIN, 3 = SEND, 4 = OK, 5 = UNAME_TAKEN, 6 = BYE
        char data[100];             // optional data parameter
        int checksum;               // auto-generated checksum from sender, command and data
        bool should_block = false;  // determines whether or not an operation should block a socket.

        Packet(const char[], int, const char[]);
        Packet();                   // blank packets are accepted ( for example, if reading into them )

        bool recv_into_packet(SOCKET);  // recieve data into the packet. Returns 0 if completed successfully ( and will return 1 if failed, NOT IMPLEMENTED )
        bool send_packet(SOCKET, bool = true); //sends data to socket. second parameter specifies if it should wait for an ACK. returns 0 on success, 1 on failure
        
        void print();               // print a human-readable representation of the fields

    private:
        void hash(Packet*);         // used internally to hash the packet
        
};

