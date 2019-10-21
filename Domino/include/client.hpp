#ifndef CLIENT_HPP
#define CLIENT_HPP
#include <arpa/inet.h>
#include <cstdio>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

class Client {
private:
    int sd_;
    struct sockaddr_in servername_;
    char buffer_[100];
    socklen_t lenServer_;

public:
    Client(sa_family_t family, const int port, const char* addr)
    {
        servername_.sin_family = family;
        servername_.sin_port = htons(port);
        servername_.sin_addr.s_addr = inet_addr(addr);
        lenServer_ = sizeof(servername_);
        if ((sd_ = socket(family, SOCK_STREAM, 0)) == -1) {
            perror("Error opening socket\n");
            exit(1);
        }
    }

    ~Client()
    {
        close(sd_);
    }

    bool startConnection()
    {
        if (connect(sd_, (sockaddr*)&servername_, lenServer_) == -1) {
            perror("Error de conexión");
            return false;
        }
        return true;
    }

    bool startListening()
    {
        do {
            puts("Teclee el mensaje a transmitir");
            fgets(buffer_, 100, stdin);

            if (send(sd_, buffer_, 100, 0) == -1) {
                perror("Error enviando datos");
                return false;
            }

        } while (strcmp(buffer_, "FIN") != 0);
        return true;
    }
};

#endif