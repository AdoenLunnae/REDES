#include "server.hpp"

Server::Server(sa_family_t family, const int port, int clientCap)
{
    clientCap_ = clientCap;
    sd_ = socket(family, SOCK_STREAM, 0);

    if (sd_ == -1) {
        perror("No se puede abrir el socket cliente\n");
        exit(1);
    }

    newClientLen_ = sizeof(newClient_);

    sockname_.sin_family = family;
    sockname_.sin_port = htons(port);
    sockname_.sin_addr.s_addr = INADDR_ANY;

    if (bind(sd_, (struct sockaddr*)&sockname_, sizeof(sockname_)) == -1) {
        perror("Error en la operación bind");
        exit(1);
    }
}

bool Server::acceptClient()
{
    if ((newSd_ = accept(sd_, (struct sockaddr*)&newClient_, &newClientLen_)) == -1) {
        perror("Error aceptando peticiones");
        return false;
    }

    clients_.push_back(newSd_);
    cout << "client with fd " << newSd_ << " connected\n";
    return true;
}

bool Server::handleMessage(const vector<int>::iterator clientIndex, const char* message)
{
    printf("From fd%d Received: %s", *clientIndex, message);
    sprintf(buffer_, "Recibido");
    send(*clientIndex, buffer_, 100, 0);
}

bool Server::handleDisconnection(vector<int>::iterator& clientIndex)
{
    close(*clientIndex);
    cout << "client with fd " << *clientIndex << " disconnected\n";
    clients_.erase(clientIndex);
    clientIndex--;
}