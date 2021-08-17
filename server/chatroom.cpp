#include "chatroom.h"
#include "random"

ChatRoom::ChatRoom(){
    const std::string CHARACTERS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    std::random_device random_device;
    std::mt19937 generator(random_device());
    std::uniform_int_distribution<> distribution(0, CHARACTERS.size() - 1);
    
    this->identifier = "";
    for( int i = 0; i < 6; i++ )
        this -> identifier += CHARACTERS[distribution(generator)];
    
    this->player_count = 0;
    this->clients.clear();
}

void ChatRoom::start_game(){
    
}

bool ChatRoom::add_client(Client client){
    if(this->player_count == 6)
        return 0;

    this->clients.push_back(client);
    this->player_count++;

    if( this->player_count == 6)
        this->start_game();

    return 1;
}