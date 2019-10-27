#include "server.hpp"

void handleSigint(int sig);
void handleSigsegv(int sig);
Server server(AF_INET, 8000, 2);
int main()
{
    signal(SIGINT, handleSigint);
    signal(SIGSEGV, handleSigsegv);
    char buffer[100];
    int max;
    std::vector<std::vector<User>::iterator> lookingForMatch;
    server.startListening();
    while (1) {
        lookingForMatch.clear();
        max = server.recreateFDSet();
        pselect(max + 1, &server.readset_, NULL, NULL, &server.timeout_, NULL);
        for (auto i = server.clientBegin(); i != server.clientEnd(); ++i) {
            if (FD_ISSET(i->fd, &server.readset_)) {
                if ((recv(i->fd, &buffer, 100, 0) > 0) && (strcmp(buffer, "SALIR") != 0))
                    server.handleMessage(i, buffer);
                else
                    server.handleDisconnection(i);
            }
        }
        for (auto i = server.clientBegin(); i != server.clientEnd(); ++i) {
            if (i->lookingForMatch)
                lookingForMatch.push_back(i);
        }
        for (auto i = lookingForMatch.begin(); i != lookingForMatch.end(); ++i) {
            if (i + 1 != lookingForMatch.end()) {
                server.createMatch(**i, **(i + 1));
                ++i;
            }
        }

        if (FD_ISSET(server.sd_, &server.readset_)) {
            server.acceptClient();
        }
    }
}

void handleSigint(int sig)
{
    for (auto i = server.clientBegin(); i != server.clientEnd(); ++i)
        server.handleDisconnection(i);
    delete &server;
    exit(1);
}

void handleSigsegv(int sig)
{
    exit(1);
}