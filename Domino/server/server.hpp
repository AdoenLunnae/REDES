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
struct Matchmaking;
typedef struct {
    int fd;
    std::string username = "";
    bool loggedIn = false;
    bool lookingForMatch = false;
    struct Matchmaking* playing = NULL;
    bool isPlayer1 = false;
} User;

typedef struct Matchmaking {
    User* j1;
    User* j2;
    Partida* match;
    bool requiresUpdate;
} Matchmaking;
bool operator==(const Matchmaking& m1, const Matchmaking& m2);

class Server {
private:
    struct sockaddr_in sockname_, newClient_;
    struct hostent* host_;
    int clientCap_;
    int matchCap_;
    std::list<Matchmaking> partidasActivas_;
    char buffer_[250];
    vector<User> clients_;

    int newSd_;
    socklen_t newClientLen_;

public:
    fd_set readset_;
    timespec timeout_{ 3, 0 };
    int sd_;

    Server(sa_family_t family, const int port, int clientCap, int matchCap);
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

    void sendUpdate(Matchmaking& m);

    void endMatch(Matchmaking* m);

    void createMatch(User& j1, User& j2);

    inline vector<User>::iterator clientBegin() { return clients_.begin(); }
    inline vector<User>::iterator clientEnd() { return clients_.end(); }

    inline std::list<Matchmaking>::iterator matchBegin() { return partidasActivas_.begin(); }
    inline std::list<Matchmaking>::iterator matchEnd() { return partidasActivas_.end(); }

    bool isFull() const { return (clients_.size() == clientCap_); }
    bool fullMatches() const { return partidasActivas_.size() == matchCap_; }

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