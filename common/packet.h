#pragma once
#include <string>
#include <iostream>

class Packet{
    public:
        std::string sender;
        std::string command;
        std::string data;
        std::string checksum;

        Packet(std::string sender, std::string command, std::string content);
        Packet();

        std::string dump();

        void load(std::string);
        void print();

    private:
        void hash(Packet*);
};

