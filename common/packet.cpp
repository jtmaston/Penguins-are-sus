#include "packet.h"
Packet::Packet(){}

Packet::Packet(const char sender[], int command, const char data[]){
    strcpy_s(this->sender, sender);
    this->command = command;
    strcpy_s(this->data, data);
    //hash(this);                                                  TODO
}

void Packet::hash(Packet* packet){
    char transport[1024] = "";
    char command[2] =  { (char)(packet->command + '0') };
    strcat_s(transport, packet->sender);
    strcat_s(transport, command);
    strcat_s(transport, packet->data);
    packet->checksum = std::hash<char*>{}(transport);
}

bool hash_compare(const Packet* packet){
    char transport[1024] = "";
    char command[2] =  { (char)(packet->command + '0') };
    strcat_s(transport, packet->sender);
    strcat_s(transport, command);
    strcat_s(transport, packet->data);

    return ( packet->checksum == std::hash<char*>{}(transport) );
}

void Packet::print(){
    std::cout << "sender: " << this->sender << "\ncommand: " << this->command << "\ndata: " << this->data << "\nchecksum: " << this->checksum << '\n';
}

bool Packet::send_packet(SOCKET sock, bool wait_for_ack){
    int iResult;
    //std::string transport = this -> dump();
    const char* transport = (const char*)this;

    bool ackd = !wait_for_ack;                  // if the other party has acknowledged that the message is ok


    do{
        iResult = send( sock, transport, sizeof(*this), 0 );

        int recv_buffer_length = 25;
        char recv_buffer[25] = "";

        if ( this->command != 0 && this->command != 1){
            Packet ACK;
            ACK.recv_into_packet(sock);
            if (ACK.command == 0){
                ackd = true;
                std::cout << "ACKED \n";
            }else if (ACK.command == 1){
                std::cout << "Checksum error! Re-sending!";
            }
        }else
            break;
        break;

    }while (! ackd );

    
    if (iResult == SOCKET_ERROR) {
        std::cerr << "Send failed with error:" <<  WSAGetLastError() << '\n';
        closesocket(sock);
        WSACleanup();
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

    Packet* packet = (Packet*) recv_buffer;
    
    strcpy_s(sender, packet->sender);
    strcpy_s(data, packet->data);
    command = packet->command;
    checksum = packet->checksum;


    int checksum_err = hash_compare(this);

    if (this->command == ACK || this->command == BAD)
        return 0; // no need ACK the ACK

    if(! checksum_err){                                   // TODO: checksum
        Packet acknowledge( this->sender, ACK, "");
        acknowledge.send_packet(sock, false);
    }else{
        Packet resend( this->sender, BAD, "" );
        resend.send_packet(sock, false);
    }

    return 0;
}
