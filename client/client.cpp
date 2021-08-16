#pragma comment(lib, "Ws2_32.lib")  // This will link the required wisock lib
#include "packet.h"
#include "networking.h"
#include <thread>


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

int main(){

    SOCKET ServerSocket = std::move(init_connection());
    std::string name;
    std::cout << "Please enter your name: ";
    std::getline(std::cin, name);
    
    Packet loginPacket(name, "login", "");

    loginPacket.send_packet(ServerSocket);                      // send the login command
    //loginPacket.should_block = true;
    //loginPacket.recv_into_packet(ServerSocket);                 // and get the server's response
    //loginPacket.print();
    loginPacket.should_block = false;

    //if(strcmp(recv_buffer, "OK")){                              // Server sends an OK if the user was connected correctly, if not then the username is taken
    //    std::cout << "Sorry! That username is taken! \n";
    //    memset(&recv_buffer[0], 0, sizeof(recv_buffer));
    //}
    //else{
    //    system("cls");
    //    std::cout << "Welcome, " << name << "\n\n";
    //}

    /*std::thread sender_thread(keyboard_handler, ServerSocket);  // handle the keyboard inputs in another thread, to allow messages to be displayed
                                                                // note: this thread is now activated later, it doesn't make sense to start b4 auth

    u_long mode = 1;  // 1 to enable non-blocking socket
    ioctlsocket(ServerSocket, FIONBIO, &mode);

    int iResult;
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
    }*/
    



    std::cout << "Exiting";
    shutdown(ServerSocket, SD_SEND);
    closesocket(ServerSocket);
    WSACleanup();
    return 0;
}