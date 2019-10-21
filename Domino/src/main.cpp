#include "server.hpp"

void handleSigint(int sig);

Server server(AF_INET, 8000, 2);
int main()
{
    signal(SIGINT, handleSigint);
    char buffer[100];
    int max;
    server.startListening();
    while (1) {
        max = server.recreateFDSet();
        pselect(max + 1, &server.readset_, NULL, NULL, &server.timeout_, NULL);
        for (auto i = server.clientBegin(); i != server.clientEnd(); ++i) {
            if (FD_ISSET(*i, &server.readset_)) {
                if ((recv(*i, &buffer, 100, 0) > 0))
                    server.handleMessage(i, buffer);
                else
                    server.handleDisconnection(i);
            }
        }

        if (FD_ISSET(server.sd_, &server.readset_)) {
            server.acceptClient();
        }
    }
}

void handleSigint(int sig)
{
    delete &server;
    exit(1);
}