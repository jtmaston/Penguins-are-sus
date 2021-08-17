#pragma once
#ifndef WIN32_LEAN_AND_MEAN         // this prevents conflicts with Windows.h, which is imported
#define WIN32_LEAN_AND_MEAN         // internally in the winsock libraries.
#endif

#include <windows.h>
#include <winsock2.h>               // winsock lib
#include <ws2tcpip.h>               // winsock TCP/IP stack library
#include <iphlpapi.h>               // IP helper tools
#include <iostream>

#define OPEN_PORT "8123"

SOCKET init_connection();           // initialize winsock components. Returns a passive ( listener ) socket to be used for accepting conns
SOCKET accept_connection(SOCKET);   // accept a client. Returns an active socket, to be used for comms