#ifndef WIN32_LEAN_AND_MEAN         // this prevents conflicts with Windows.h, which is imported
#define WIN32_LEAN_AND_MEAN         // internally in the winsock libraries.
#endif

#include <windows.h>
#include <winsock2.h>               // winsock lib
#include <ws2tcpip.h>               // winsock TCP/IP stack library
#include <iphlpapi.h>               // IP helper tools
#include <iostream>
#include <queue>
#include <thread>
#pragma comment(lib, "Ws2_32.lib")  // This will link the required wisock lib
#define OPEN_PORT "8123"
#include "packet.h"

std::queue <std::string> command_queue;

bool running = true;

void keyboard_handler(SOCKET ServerSocket){
    char message[100] = "";
    do{
        std::cin.getline(message, 100);
        send(ServerSocket, message, (int)strlen(message), 0);
        memset(&message[0], 0, sizeof(message));
        
    }while(strcmp(message, "/quit"));       // this will quit when you type /quit ( I think )
    running = false;
    return;
}

int main(){
    WSADATA wsaData;        // this will store information about the winsock implementation

    if (WSAStartup(MAKEWORD(1,1), &wsaData) != 0) { // initialzie winsock and error out if it fails
        std::cerr << "Failed initializing winsock. Exiting...\n";
        WSACleanup();
        exit(1);
    }// else
     //   std::cout << "Initialized winsock.\n";


    addrinfo socket_info;   // used to configure the socket
    addrinfo* result = NULL;

    ZeroMemory(&socket_info, sizeof(socket_info)); // initialize the socket info with zeros

    socket_info.ai_family = AF_UNSPEC;     // set the type of protocol to be used. For client, it's unspecified, as it gets
                                           // this data from the server
    socket_info.ai_socktype = SOCK_STREAM; // sets how data is transmitted. for TCP we use stream. for UDP we use dgram
    socket_info.ai_protocol = IPPROTO_TCP; // set protocol to TCP
   
    
    if(getaddrinfo("127.0.0.1", OPEN_PORT, &socket_info, &result)){    // resolve localhost port and address, to be used by the 
        std::cerr << "Addrinfo failed! Exiting...\n";           // sockets.
        WSACleanup();
        exit(1);
    }//else
        //std::cout << "Resolved local address and port. \n";
    
    SOCKET ServerSocket = INVALID_SOCKET;  // declare socket, and initialize in failed state ( this changes later )

    ServerSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol); // set the attributes of the socket, this should change
                                                                                        // it to a valid socket
    if(ServerSocket == INVALID_SOCKET){
        std::cerr << "Socket is invalid! Error was " << WSAGetLastError() << '\n';
        freeaddrinfo(result);
        WSACleanup();
        exit(1);
    }

    std::cout << "Connecting to server...\n";
    
    int iResult = connect(ServerSocket, result->ai_addr, (int)result->ai_addrlen);      // finally, connect
    if(iResult == SOCKET_ERROR || ServerSocket == INVALID_SOCKET){
        std::cerr << "Cannot connect!";
        closesocket(ServerSocket);
        freeaddrinfo(result);
        WSACleanup();
        exit(1);
    }//else
        //std::cout << "Connected!\n";
    

    
    int recv_buffer_length = 1024;
    char recv_buffer[1024] = "";


    std::thread sender_thread(keyboard_handler, ServerSocket);  // handle the keyboard inputs in another thread, to allow messages to be displayed
    
    char name[100] = "";

    auth:
    std::cout << "Please enter your name: ";
    std::cin.getline(name, 100);
    send(ServerSocket, name, (int)strlen(name), 0);             // this _could_ be dangerous, since we're not checking for the result of the send operation
    recv(ServerSocket, recv_buffer, recv_buffer_length, 0);
    if(strcmp(recv_buffer, "OK")){                              // Server sends an OK if the user was connected correctly, if not then the username is taken
        std::cout << "Sorry! That username is taken! \n";
        memset(&recv_buffer[0], 0, sizeof(recv_buffer));
        goto auth;
    }
    else{
        system("cls");
        std::cout << "Welcome, " << name << "\n\n";
    }

    u_long mode = 1;  // 1 to enable non-blocking socket
    ioctlsocket(ServerSocket, FIONBIO, &mode);

    while(running){
        do{ // really, this receiver belongs on another thread, that's for the future . TODO!
            iResult = recv(ServerSocket, recv_buffer, recv_buffer_length, 0);
            recv_buffer[iResult] = '\0';
            if (iResult > 0)
                std::cout << recv_buffer << '\n';           // this just prints whatever it gets
            else if (iResult == 0)
                std::cout << "Connection closing.\n";
            memset(&recv_buffer[0], 0, sizeof(recv_buffer));

        }while(iResult > 0);
    }
    



    std::cout << "Exiting";
    shutdown(ServerSocket, SD_SEND);
    closesocket(ServerSocket);
    WSACleanup();
    return 0;
}