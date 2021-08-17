
#include "networking.h"
#include "packet.h"

#include <vector>
#include <unordered_map>
#include <thread>
#include <queue>

#pragma comment(lib, "Ws2_32.lib")  // This will link the required wisock lib


bool running = true;
bool waiting = false;

std::unordered_map <std::string, SOCKET> clients;



void sender(SOCKET ClientSocket, char data[]){  // this is generally a wrapper for send() from winsock, because it's desireable to block
    u_long mode = 0;
    ioctlsocket(ClientSocket, FIONBIO, &mode);  // when sending, to ensure the data doesn't get lost

    send(ClientSocket, data, strlen(data), 0);
    
    mode = 1;
    ioctlsocket(ClientSocket, FIONBIO, &mode);
}


void listner(){                     
    char recv_buffer[1024] = "";                // this is the receiver thread. Gets 1024 bytes at a time ( maximum ) More size allows 
    int iResult;                                // higher transmission speeds, but adds overhead. 1024 is a happy medium for text
    int recv_buffer_length = 1024;

    while(running){
        //while(waiting){}; 
        for (auto const& client : clients){     // iterate through the clients, and see if they have any messages to send
            iResult = recv(client.second, recv_buffer, recv_buffer_length, 0); // if any bytes were received, pass them on to the whole chat room
            std::cout << recv_buffer << '\n';
            /*if (iResult > 0){
                recv_buffer[iResult] = '\0';    // add NULL terminator
                for (auto const& destination : clients){    // iterate through the clients again, but ignore the sender, he doesn't need to get the message
                    std::cout << client.first << ": " << destination.first; // twice
                    if (destination.first != client.first){
                        std::string message = client.first + ": " + std::string(recv_buffer); // formatting. Could be rpettier
                        sender(destination.second, (char*) message.c_str());
                    }
                }
                memset(recv_buffer, 0, sizeof(recv_buffer));    // finally, clear the buffer
            }*/
                
            if (iResult == 0){
                std::cout << client.first << " logged out. \n"; 
                return;
            }
    }
    }
    // TODO: ↓
    /*while(running){
        std::cout.flush();
        do{
            
        }while(iResult > 0);


    }*/
    //iResult = shutdown(ClientSocket, SD_SEND);
    //if(iResult == SOCKET_ERROR){
    //    std::cerr << "Cannot shutdown! Error was " << WSAGetLastError() << '\n';
    //    closesocket(ClientSocket);
    //    WSACleanup();
    //    exit(1);
    //}
    
}


int main(){                 // server uses two threads. One handles incoming connections, accepting them
    
    SOCKET ListenerSocket = std::move(init_connection());
    //std::thread listner_thread(listner); // this handles getting and redirecting messages to the other users

    while (1){
        // <----- This section handles auth with the client -----> 
        SOCKET ClientSocket = std::move(accept_connection(ListenerSocket));

        bool ok = false;
        Packet LoginPacket;
        LoginPacket.should_block = true;
        LoginPacket.recv_into_packet(ClientSocket);
        LoginPacket.print();
        //LoginPacket.print();

        bool client_logged_in = false;
        while(!client_logged_in){
            if(clients.find(LoginPacket.sender) == clients.end()){  // check: is there already a client with this name?   
                
                // << this section just identifies the client >>      

                SOCKADDR_IN client;
                int lensin = sizeof(client);
                getpeername(ClientSocket, (sockaddr *)&client, &lensin);
                std::cout << client.sin_port <<" is " << LoginPacket.sender << '\n';

                // << --------------------------------------- >> 
                
                clients[LoginPacket.sender] = std::move(ClientSocket); // finally, add it to the connections list

                LoginPacket = Packet("SERVER", Commands::OK, "");
                LoginPacket.send_packet(ClientSocket);
                client_logged_in = true;

            }else{
                LoginPacket = Packet("SERVER", Commands::UNAME_TAKEN, "");
                LoginPacket.send_packet(ClientSocket);
            }
        }
        // <--------------------------------------> 
        
    }

    std::cout << "Exiting...\n";
    for (auto const& client : clients){
        closesocket(client.second);
    }

    //for (std::thread& elem : workers){
        //elem.join();
    //}

    WSACleanup();
    return 0;
}