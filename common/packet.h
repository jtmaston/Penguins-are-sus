#pragma once
#include <string>
#include <iostream>
#include <winsock2.h>

// NOTE: by default, any socket associated with a packet is NONBLOCKING. Use should_block to override this!
// each recv_into RESETS TO !! NONBLOCKING !!

class Packet{
    public:
        std::string sender;         // the username of the sender
        std::string command;        // command to be used
        std::string data;           // optional data parameter
        std::string checksum;       // auto-generated checksum from sender, command and data
        bool should_block = false;  // determines whether or not an operation should block a socket.

        Packet(std::string sender, std::string command, std::string content);
        Packet();                   // blank packets are accepted ( for example, if reading into them )

        bool recv_into_packet(SOCKET);  // recieve data into the packet. Returns 0 if completed successfully ( and will return 1 if failed, NOT IMPLEMENTED )
        bool send_packet(SOCKET, bool = true); //sends data to socket. second parameter specifies if it should wait for an ACK. returns 0 on success, 1 on failure
        
        void print();               // print a human-readable representation of the fields

    private:
        void hash(Packet*);         // used internally to hash the packet
        
        std::string dump();         // returns a string with the packet structure

        bool load(std::string);     // load the fields from a dumped packet. Returns 0 on checksum match or 1 on checksum error
        bool load(const char* );    // ditto above, but with c-strings ( as sock.recv uses c-strings ). converts to std::string and calls above
};

