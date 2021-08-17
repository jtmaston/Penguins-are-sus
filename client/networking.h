#pragma once
#include <iostream>

#ifndef WIN32_LEAN_AND_MEAN         // this prevents conflicts with Windows.h, which is imported
#define WIN32_LEAN_AND_MEAN         // internally in the winsock libraries.
#endif

#include <windows.h>
#include <winsock2.h>               // winsock lib
#include <ws2tcpip.h>               // winsock TCP/IP stack library
#include <iphlpapi.h>               // IP helper tools
#include "packet.h"


#define OPEN_PORT "8123"
SOCKET init_connection();           // initialize the connection parameters for winsock. Returns a socket connected to the server
bool login(SOCKET);                 // handles the login, asks for a username and passes it to server. Returns true on login ok, false on login fail
bool logout(SOCKET);                // logs out ( tells the server to close the socket ). Returns 0 on logout ok, ( 1 on logout fail, NOT IMPLEMENTED )