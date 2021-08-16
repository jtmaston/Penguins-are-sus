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
    { c:[command]; s:[sender], h:[hash]; d:[data]; }

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

void Packet::load(const char* data){
    std::string converted = data;
    this->load(converted);
}

void Packet::load(std::string raw){
    std::cout << raw << '\n';
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

bool Packet::send_packet(SOCKET sock, bool wait_for_ack){
    int iResult;
    std::string transport = this -> dump();

    bool ackd = !wait_for_ack;                  // if the other party has acknowledged that the message is ok
    
    do{
        iResult = send( sock, transport.c_str(), transport.length(), 0 );

        int recv_buffer_length = 25;
        char recv_buffer[25] = "";

        if ( this->command != "ACK" ){
            Packet ACK;
            ACK.recv_into_packet(sock);
            if (ACK.command == "ACK"){
                ackd = true;
                std::cout << "ACKED \n";
            }
        }else
            break;

    }while (! ackd );

    
    if (iResult == SOCKET_ERROR) {
        std::cerr << "Send failed with error:" <<  WSAGetLastError() << '\n';
        //closesocket(sock);
        //WSACleanup();
        return 1;
    }
    return 0;
}

bool Packet::recv_into_packet(SOCKET sock){

    /*if (this->should_block){
         u_long mode = 0;                                        
         ioctlsocket(sock, FIONBIO, &mode);
    }else{
         u_long mode = 1;                                        
         ioctlsocket(sock, FIONBIO, &mode);
    }*/

    int recv_buffer_length = 1024;
    char recv_buffer[1024] = "";

    int size = recv(sock, recv_buffer, recv_buffer_length, 0);
    this->load(recv_buffer);

    if (this->command == "ACK")
        return 0; // no need ACK the ACK

    if(true){                                   // TODO: checksum
        Packet ACK(this->sender, "ACK", "");
        ACK.send_packet(sock, false);
        //ok = true;
    }

    return 0;
}