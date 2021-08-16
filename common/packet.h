#pragma once
#include <string>
#include <iostream>
#include <winsock2.h>

class Packet{
    public:
        std::string sender;
        std::string command;
        std::string data;
        std::string checksum;
        bool should_block = false;

        Packet(std::string sender, std::string command, std::string content);
        Packet();

        bool recv_into_packet(SOCKET);
        bool send_packet(SOCKET, bool = true);
        

        void print();

    private:
        void hash(Packet*);
        std::string dump();

        void load(std::string);
        void load(const char* );
};

