#pragma comment(lib, "Ws2_32.lib")  // This will link the required wisock lib
#include "networking.h"
#include <thread>
#include <signal.h>


bool running = true;

void keyboard_handler(SOCKET ServerSocket, std::string username){
    std::string message;
    do{
        std::getline(std::cin, message);
        Packet message_packet (username, "message", message);
        
    }while(message != "/quit");       // this will quit when you type /quit ( I think )
    running = false;
    return;
}

SOCKET ServerSocket;

void kbInterrupt(int signum){
    std::cout << "Caught keyboard interrupt";
    logout(ServerSocket);
    exit(0);
}

int main(){

    signal(SIGINT, kbInterrupt);

    ServerSocket = std::move(init_connection());
                         // send the login command
    
    
    bool login_ok = false;

    while(!login_ok){
        login_ok = login(ServerSocket);
        if (! login_ok )
            std::cout << "Sorry! Username is taken! \n";
    }

    while(1){}

    logout(ServerSocket);
    return 0;
}