#include "networking.h"

SOCKET init_connection(){
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
    
    return ServerSocket;
}

bool login(SOCKET sock){
    std::string name;
    std::cout << "Please enter your name: ";
    std::getline(std::cin, name);

    Packet loginPacket(name, "login", "");
    loginPacket.send_packet(sock); 

    loginPacket.should_block = true;
    loginPacket.recv_into_packet(sock);                 // and get the server's response
    loginPacket.should_block = false;
    
    if (loginPacket.command == "OK")
        return true;
    else
        return false;
    
}

bool logout(SOCKET ServerSocket){
    Packet GoodbyePacket ("", "BYE", "");
    GoodbyePacket.should_block = true;
    GoodbyePacket.send_packet(ServerSocket);

    std::cout << "Exiting";
    shutdown(ServerSocket, SD_BOTH);
    closesocket(ServerSocket);
    WSACleanup();
    
    return 0;
}