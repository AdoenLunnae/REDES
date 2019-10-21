#ifndef SERVER_HPP
#define SERVER_HPP
#include <iostream>
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

class Server {
private:
    struct sockaddr_in sockname_, newClient_;
    struct hostent* host_;
    int clientCap_;

    char buffer_[100];
    vector<int> clients_;

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

    bool handleMessage(const vector<int>::iterator clientIndex, const char* message);

    bool handleDisconnection(vector<int>::iterator& clientIndex);

    vector<int>::iterator clientBegin() { return clients_.begin(); }
    vector<int>::iterator clientEnd() { return clients_.end(); }

    int recreateFDSet()
    {
        FD_ZERO(&readset_);
        FD_SET(sd_, &readset_);
        int max = -1;
        for (auto i = clients_.begin(); i != clients_.end(); ++i) {
            FD_SET(*i, &readset_);
            if (*i > max)
                max = *i;
        }
        return max;
    }
};

#endif