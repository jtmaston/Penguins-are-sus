#ifndef WIN32_LEAN_AND_MEAN         // this prevents conflicts with Windows.h, which is imported
#define WIN32_LEAN_AND_MEAN         // internally in the winsock libraries.
#endif

#include <windows.h>
#include <winsock2.h>               // winsock lib
#include <ws2tcpip.h>               // winsock TCP/IP stack library
#include <iphlpapi.h>               // IP helper tools
#include <iostream>
#include <vector>
#include <unordered_map>
#include <thread>
#include <queue>

#pragma comment(lib, "Ws2_32.lib")  // This will link the required wisock lib
#define OPEN_PORT "8123"

bool running = true;
bool waiting = false;

std::unordered_map <std::string, SOCKET> clients;
//std::vector <client> client_sockets;
//std::vector <std::thread> workers;


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
            
            if (iResult > 0){
                recv_buffer[iResult] = '\0';    // add NULL terminator
                for (auto const& destination : clients){    // iterate through the clients again, but ignore the sender, he doesn't need to get the message
                    std::cout << client.first << ": " << destination.first; // twice
                    if (destination.first != client.first){
                        std::string message = client.first + ": " + std::string(recv_buffer); // formatting. Could be rpettier
                        sender(destination.second, (char*) message.c_str());
                    }
                }
                memset(recv_buffer, 0, sizeof(recv_buffer));    // finally, clear the buffer
            }
                
            else if (iResult == 0)
                std::cout << "Connection closing.\n";  // fringe case ( again )
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
    WSADATA wsaData;        // this will store information about the winsock implementation

    if (WSAStartup(MAKEWORD(1,1), &wsaData) != 0) { // initialzie winsock and error out if it fails
        std::cerr << "Failed initializing winsock. Exiting...\n";
        WSACleanup();
        exit(1);
    } else
        std::cout << "Initialized winsock.\n";


    addrinfo socket_info;   // used to configure the socket
    addrinfo* result = NULL;

    ZeroMemory(&socket_info, sizeof(socket_info)); // initialize the socket info with zeros

    socket_info.ai_family = AF_INET;       // set the type of protocol to be used. AF_INET is IPv4, AF_INET6 is 
                                           // IPv6, etc
    socket_info.ai_socktype = SOCK_STREAM; // sets how data is transmitted. for TCP we use stream. for UDP we use dgram
    socket_info.ai_protocol = IPPROTO_TCP; // set protocol to TCP
    socket_info.ai_flags = AI_PASSIVE;     // passive socket is not connected, accepts connections. upon a connection being
                                           // made, an active socket is spawned.
    
    if(getaddrinfo(NULL, OPEN_PORT, &socket_info, &result)){    // resolve localhost port and address, to be used by the 
        std::cerr << "Addrinfo failed! Exiting...\n";           // sockets.
        WSACleanup();
        exit(1);
    }else
        std::cout << "Resolved local address and port. \n";

    SOCKET ListenerSocket = INVALID_SOCKET; 
    ListenerSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if(ListenerSocket == INVALID_SOCKET){
        std::cerr << "Socket is invalid! Error was " << WSAGetLastError() << '\n';
        freeaddrinfo(result); 
        WSACleanup();
        exit(1);
    }

    int bind_result = bind(ListenerSocket, result->ai_addr, (int)result->ai_addrlen);
    if(bind_result == SOCKET_ERROR){
        std::cerr << "Bind failed! Error was " << WSAGetLastError() << '\n';
        freeaddrinfo(result);
        WSACleanup();
        exit(1);
    }else
        std::cout << "Socket is now bound. \n";

    freeaddrinfo(result);
    listen(ListenerSocket, 100);        // start listening on the passive socket, for up to 100 users


    std::thread listner_thread(listner); // this handles getting and redirecting messages to the other users

    while (1){
        // <----- This section handles auth with the client -----> 
        SOCKET ClientSocket = INVALID_SOCKET;
        ClientSocket = accept(ListenerSocket, NULL, NULL);      // accept a connection
        waiting = true;

        u_long mode = 0;                                        // set the socket to block. This is done because the sockets are used as flow control
        ioctlsocket(ClientSocket, FIONBIO, &mode);

        char client_name[1024] = "";                            // get the client's name. This socket blocks till a name is recieved, then continues
        int iResult;
        iResult = recv(ClientSocket, client_name, 1024, 0);

        if (! (iResult > 0)){                                   // handle edge case where the client closes connection
            std::cout << "Client closed connection unexpectedly.\n";
            continue;
        }            

        client_name[iResult] = '\0';                            // null terminator needs to be added, it doesn't seem to get sent

        bool done = false;
        while(!done){
            if(clients.find(std::string(client_name)) == clients.end()){  // check: is there already a client with this name?   
                // << this section just identifies the client >>      
                SOCKADDR_IN client;
                int lensin = sizeof(client);
                getpeername(ClientSocket, (sockaddr *)&client, &lensin);
                std::cout << client.sin_port <<" is " << client_name << '\n';
                // << --------------------------------------- >> 
                done = true;

                sender(ClientSocket, "OK");                               // send OK to the client
                u_long mode = 1;                                          // then set it to be non-blocking, since AUTH is done
                ioctlsocket(ClientSocket, FIONBIO, &mode);

                clients[std::string(client_name)] = std::move(ClientSocket); // finally, add it to the connections list
                waiting = false;
            }else{
                //if(strlen(client_name) > 0){              TODO: fix multiple users with same name
                //    send(ClientSocket, "UNAME_TAKEN");
                //    memset(&client_name[0], 0, sizeof(client_name));
                //    recv(ClientSocket, client_name, 1024, 0);
                //    std::cout << client_name;
                break;
            }
        // <--------------------------------------> 
        }
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