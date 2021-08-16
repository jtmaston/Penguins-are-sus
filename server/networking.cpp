#include "networking.h"

SOCKET init_connection(){    
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
    return ListenerSocket;
}

SOCKET accept_connection(SOCKET ListenerSocket){
    SOCKET ClientSocket = INVALID_SOCKET;
    ClientSocket = accept(ListenerSocket, NULL, NULL);      // accept a connection
    u_long mode = 0;                                        // set the socket to block. This is done because the sockets are used as flow control
    ioctlsocket(ClientSocket, FIONBIO, &mode);
    
    return ClientSocket;
}