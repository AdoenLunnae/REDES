#ifndef SERVER_HPP
#define SERVER_HPP
#include "partida.hpp"
#include <iostream>
#include <list>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

using std::cout;
using std::vector;

typedef struct {
    int fd;
    std::string introducedUsername = "";
    bool loggedIn = false;
    bool lookingForMatch = false;
    Partida* playing = NULL;
    bool isPlayer1 = false;
} User;

class Server {
private:
    struct sockaddr_in sockname_, newClient_;
    struct hostent* host_;
    int clientCap_;
    std::list<Partida> partidasActivas_;
    char buffer_[100];
    vector<User> clients_;

    int newSd_;
    socklen_t newClientLen_;

public:
    fd_set readset_;
    timespec timeout_{ 3, 0 };
    int sd_;

    Server(sa_family_t family, const int port, int clientCap);
    ~Server()
    {
        close(sd_);
        exit(1);
    }

    bool startListening()
    {
        if (listen(sd_, clientCap_) == -1) {
            perror("Error en la operación de listen");
            return false;
        }
        return true;
    }

    bool acceptClient();

    bool checkLogged(const User& client, const bool shouldBeLogged);

    bool checkPlaying(const User& client, const bool shouldBePlaying);

    bool checkUsername(const std::string& username);

    bool checkPassword(const std::string& username, const std::string& password);

    bool checkTurn(const User& client);

    bool handleMessage(const vector<User>::iterator clientIndex, const char* message);

    bool handleDisconnection(vector<User>::iterator& clientIndex);

    bool createMatch(User& j1, User& j2)
    {
        partidasActivas_.emplace_front();
        j1.lookingForMatch = false;
        j2.lookingForMatch = false;
        j1.isPlayer1 = true;
        j1.isPlayer1 = false;
    }

    vector<User>::iterator clientBegin()
    {
        return clients_.begin();
    }
    vector<User>::iterator clientEnd() { return clients_.end(); }

    int recreateFDSet()
    {
        FD_ZERO(&readset_);
        FD_SET(sd_, &readset_);
        int max = -1;
        for (auto i = clients_.begin(); i != clients_.end(); ++i) {
            FD_SET(i->fd, &readset_);
            if (i->fd > max)
                max = i->fd;
        }
        return max;
    }
};

#endif