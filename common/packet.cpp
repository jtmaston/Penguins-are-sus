#include "packet.h"
/*
Packet strucutre is
    Packet {
        command: ,
        sender: , 
        hash: ,
        data: (optional),
    }

    On the wire, it would look like
    { c:[command]; s:[sender], h:[hash]; d:[data] }

*/
Packet::Packet(){}

Packet::Packet(std::string sender, std::string command, std::string data){
    this->sender = sender;
    this->command = command;
    this->data = data;
    hash(this);

}

void Packet::hash(Packet* packet){
    packet->checksum = std::to_string(std::hash<std::string>{}(packet->data));
}

void Packet::print(){
    std::cout << "sender: " << this->sender << "\ncommand: " << this->command << "\ndata: " << this->data << "\nchecksum: " << this->checksum << '\n';
}

std::string Packet::dump(){
    std::string result = "{ c:";
    result += this->command;
    result += "; s:";
    result += this->sender;
    result += "; h:";
    result += this->checksum;
    result += "; d:";
    result += this->data;
    result += "; }";
    return result;
}

void Packet::load(std::string raw){
    int delimiter_left = 0;
    delimiter_left = raw.find_first_of(':');
    do{
        int delimiter_right = raw.find(';', delimiter_left + 1);

        switch(raw[delimiter_left - 1]){
            case 'c':
                this->command = raw.substr(delimiter_left + 1, delimiter_right - delimiter_left - 1);
                break;
            case 's':
                this->sender = raw.substr(delimiter_left + 1, delimiter_right - delimiter_left - 1);
                break;
            case 'h':
                this->checksum = raw.substr(delimiter_left + 1, delimiter_right - delimiter_left - 1);
                break;
            case 'd':
                this->data = raw.substr(delimiter_left + 1, delimiter_right - delimiter_left - 1);
                break;
            default:
                std::cout << "Unrecognized argument " << raw[delimiter_left - 1] << '\n';
        }

        delimiter_left = raw.find(':', delimiter_left + 2);

    }while(delimiter_left != std::string::npos);

}