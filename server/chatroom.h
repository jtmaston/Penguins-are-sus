#pragma once
#include <string>
#include <vector>
#include <thread>
#include "client.h"

class ChatRoom{
    public:
        std::string identifier;

        ChatRoom();

        bool add_client(Client);        // adds client to the room. Returns 0 on success, 1 on failure ( room is full )
        void remove_client(Client);     // remove client from room.
        void start_game();
        void stop();
        
    //private:
        int player_count = 0;
        std::vector<Client> clients;
        std::thread listener;
};