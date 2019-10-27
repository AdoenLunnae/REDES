#include "server.hpp"
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

bool operator==(const Matchmaking& m1, const Matchmaking& m2)
{
    return m1.match == m2.match;
}

Server::Server(sa_family_t family, const int port, int clientCap, int matchCap)
{
    clientCap_ = clientCap;
    matchCap_ = matchCap;
    sd_ = socket(family, SOCK_STREAM, 0);

    if (sd_ == -1) {
        perror("No se puede abrir el socket cliente\n");
        exit(1);
    }

    newClientLen_ = sizeof(newClient_);

    sockname_.sin_family = family;
    sockname_.sin_port = htons(port);
    sockname_.sin_addr.s_addr = INADDR_ANY;
    int opt = 1;
    setsockopt(sd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
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
    if (!isFull()) {
        clients_.push_back(User{ newSd_ });
        cout << "client with fd " << newSd_ << " connected\n";
        return true;
    } else {
        sprintf(buffer_, "CONNECTION_NOT_ALLOWED");
        send(newSd_, buffer_, sizeof(buffer_), 0);
        close(newSd_);
        return false;
    }
}

bool Server::checkUsername(const std::string& username)
{
    std::string line;
    std::fstream f("users.txt", std::ios::in);
    while (getline(f, line, '\n')) {
        line = line.substr(0, line.find(' '));
        if (username == line) {
            return true;
        }
    }
    return false;
}

bool Server::checkPassword(const std::string& username, const std::string& password)
{
    std::string line;
    std::string aux;
    std::fstream f("users.txt", std::ios::in);
    while (getline(f, line, '\n')) {
        aux = line.substr(0, line.find(' '));
        if (username == aux) {
            aux = line.substr(line.find(' ') + 1);
            if (password == aux) {
                return true;
            }
        }
    }
    return false;
}

bool Server::checkLogged(const User& client, const bool shouldBeLogged)
{
    if (shouldBeLogged && !client.loggedIn) {
        sprintf(buffer_, "-Err. No estás logeado");
        send(client.fd, buffer_, sizeof(buffer_), 0);
        return false;
    }
    if (!shouldBeLogged && client.loggedIn) {
        sprintf(buffer_, "-Err. Ya estás logeado");
        send(client.fd, buffer_, sizeof(buffer_), 0);
        return false;
    }
    return true;
}

bool Server::checkPlaying(const User& client, const bool shouldBePlaying)
{
    if (shouldBePlaying && !(client.playing != NULL)) {
        sprintf(buffer_, "-Err. No estás jugando");
        send(client.fd, buffer_, sizeof(buffer_), 0);
        return false;
    }
    if (!shouldBePlaying && (client.playing != NULL)) {
        sprintf(buffer_, "-Err. Ya estás jugando");
        send(client.fd, buffer_, sizeof(buffer_), 0);
        return false;
    }
    return true;
}

bool Server::checkTurn(const User& client)
{
    bool correct = (client.isPlayer1 ^ client.playing->match->isTurnJ1());
    if (!correct) {
        sprintf(buffer_, "-Err. No es tu turno");
        send(client.fd, buffer_, sizeof(buffer_), 0);
    }
    return correct;
}

void Server::createMatch(User& j1, User& j2)
{
    partidasActivas_.push_back({ &j1, &j2, new Partida, true });
    j1.lookingForMatch = false;
    j2.lookingForMatch = false;
    j1.isPlayer1 = true;
    j2.isPlayer1 = false;
    j1.playing = &partidasActivas_.back();
    j2.playing = &partidasActivas_.back();
    partidasActivas_.back().match->start();
    sprintf(buffer_, "+Ok. Partida encontrada");
    send(j1.fd, buffer_, sizeof(buffer_), 0);
    send(j2.fd, buffer_, sizeof(buffer_), 0);
    sprintf(buffer_, partidasActivas_.back().match->jugadaInicial().c_str());
    send(j1.fd, buffer_, sizeof(buffer_), 0);
    send(j2.fd, buffer_, sizeof(buffer_), 0);
}

void Server::sendUpdate(Matchmaking& m)
{
    int status = m.match->isFinished();
    if (!status) {
        sprintf(buffer_, "Es el turno de %s\nTABLERO %s", (m.match->isTurnJ1() ? m.j1->username : m.j2->username).c_str(), m.match->showBoard().c_str());
        send(m.j1->fd, buffer_, sizeof(buffer_), 0);
        send(m.j2->fd, buffer_, sizeof(buffer_), 0);

        sprintf(buffer_, "FICHAS %s", m.match->showHand(true).c_str());
        send(m.j1->fd, buffer_, sizeof(buffer_), 0);

        sprintf(buffer_, "FICHAS %s", m.match->showHand(false).c_str());
        send(m.j2->fd, buffer_, sizeof(buffer_), 0);
        return;
    }
    if (status == 1) {
        sprintf(buffer_, "Ha ganado %s", m.j1->username.c_str());
    }
    if (status == -1) {
        sprintf(buffer_, "Ha ganado %s", m.j2->username.c_str());
    }
    send(m.j1->fd, buffer_, sizeof(buffer_), 0);
    send(m.j2->fd, buffer_, sizeof(buffer_), 0);
    endMatch(&m);
}

void Server::endMatch(Matchmaking* m)
{
    delete m->match;
    partidasActivas_.remove(*m);
    m->j1->playing = NULL;
    m->j2->playing = NULL;
    m->j1->isPlayer1 = false;
    m->j2->isPlayer1 = false;
    sprintf(buffer_, "PARTIDA FINALIZADA");
    send(m->j1->fd, buffer_, sizeof(buffer_), 0);
    send(m->j2->fd, buffer_, sizeof(buffer_), 0);
}
bool Server::handleMessage(vector<User>::iterator clientIndex, const char* message)
{
    /*
    REGISTER –u  usuario –p  password:  mensaje mediante  el  cual  el  usuario  solicita  registrarse  para  acceder  al  servicio de  chat  que escucha en el puerto TCP  2050.

    ROBAR-FICHA:mensaje para robaruna ficha del montón.
    -> Del servidor FICHA |valor1|valor2|: mensaje para enviar una ficha a un jugador que haya robado.
    */
    if (strlen(message) == 1) {
        send(clientIndex->fd, "", 0, 0);
        return false;
    }
    char copy[strlen(message)];
    strcpy(copy, message);
    std::string command(strtok(copy, " \n"));

    if (command == "USUARIO") {
        std::string username(strtok(NULL, "\n"));
        std::string line;
        bool correct = false;
        if (clientIndex->loggedIn) {
            sprintf(buffer_, "-Err. Already logged in.");
        } else if (correct = checkUsername(username)) {
            sprintf(buffer_, "+Ok. Usuario correcto.");
            clientIndex->username = username;
        } else {
            sprintf(buffer_, "-Err. Usuario incorrecto.");
        }
        send(clientIndex->fd, buffer_, sizeof(buffer_), 0);
        return correct;
    } else if (command == "PASSWORD") {
        std::string password(strtok(NULL, "\n"));
        std::string line;
        bool correct = false;
        if (!checkLogged(*clientIndex, false)) {
        } else if (clientIndex->username == "") {
            sprintf(buffer_, "-Err. No se ha introducido usuario");
        } else if (correct = checkPassword(clientIndex->username, password)) {
            sprintf(buffer_, "+Ok. Contraseña correcta");
            clientIndex->loggedIn = true;
        } else {
            sprintf(buffer_, "-Err. Contraseña incorrecta");
        }
        send(clientIndex->fd, buffer_, sizeof(buffer_), 0);
        return correct;
    } else if (command == "INICIAR-PARTIDA") {
        bool correct = false;
        if (checkLogged(*clientIndex, true) && checkPlaying(*clientIndex, false)) {
            clientIndex->lookingForMatch = true;
            sprintf(buffer_, "+Ok. Buscando partida");
            correct = true;
        }
        send(clientIndex->fd, buffer_, sizeof(buffer_), 0);
    } else if (command == "PASO-TURNO") {
        if (checkLogged(*clientIndex, true) && checkPlaying(*clientIndex, true) && checkTurn(*clientIndex)) {
            if (!clientIndex->playing->match->canPlay(clientIndex->isPlayer1)) {
                clientIndex->playing->match->changeTurn();
                clientIndex->playing->requiresUpdate = true;
                return true;
            } else {
                sprintf(buffer_, "-Err. Puedes jugar ficha");
                send(clientIndex->fd, buffer_, sizeof(buffer_), 0);
                return false;
            }
        }
        return false;
    } else if (command == "COLOCAR-FICHA") {
        if (checkLogged(*clientIndex, true) && checkPlaying(*clientIndex, true) && checkTurn(*clientIndex)) {
            std::string fichaStr, extremo;
            std::string rest(strtok(NULL, "\n"));
            fichaStr = rest.substr(0, rest.find(','));
            extremo = rest.substr(rest.find(',') + 1);

            int n1, n2;
            bool correct = false;
            if (sscanf(fichaStr.c_str(), "|%d|%d|", &n1, &n2) <= 0) {
                sprintf(buffer_, "-Err. Formato incorrecto");
                send(clientIndex->fd, buffer_, sizeof(buffer_), 0);
                return false;
            }
            try {
                if (extremo == "izq") {
                    if (clientIndex->playing->match->jugarFicha(Ficha(n1, n2), false, clientIndex->isPlayer1)) {
                        sprintf(buffer_, "+Ok. Ficha jugada\n");
                        correct = true;
                    } else {
                        sprintf(buffer_, "-Err. No puedes jugar esa ficha en ese extremo");
                    }
                } else if (extremo == "der") {
                    if (clientIndex->playing->match->jugarFicha(Ficha(n1, n2), true, clientIndex->isPlayer1)) {
                        sprintf(buffer_, "+Ok. Ficha jugada\n");
                        correct = true;
                    } else {
                        sprintf(buffer_, "-Err. No puedes jugar esa ficha en ese extremo");
                    }
                } else {
                    sprintf(buffer_, "-Err. Formato incorrecto");
                }
            } catch (const char* e) {
                sprintf(buffer_, "-Err. La ficha es invalida");
            }
            send(clientIndex->fd, buffer_, sizeof(buffer_), 0);
            clientIndex->playing->requiresUpdate = correct;
            return correct;
        }
    } else if (command == "ROBAR-FICHA") {
        if (checkLogged(*clientIndex, true) && checkPlaying(*clientIndex, true) && checkTurn(*clientIndex)) {
            if (!clientIndex->playing->match->canPlay(clientIndex->isPlayer1)) {
                Ficha f = clientIndex->playing->match->robarFicha(clientIndex->isPlayer1);
                if (f.getN1() == -1) {
                    sprintf(buffer_, "-Err. El montón está vacío");
                    send(sd_, buffer_, sizeof(buffer_), 0);
                    return false;
                }
                std::stringstream ss;
                ss << f;
                sprintf(buffer_, "FICHA %s", ss.str().c_str());
                send(sd_, buffer_, sizeof(buffer_), 0);
                clientIndex->playing->requiresUpdate = true;
                return true;
            } else {
                sprintf(buffer_, "-Err. Puedes jugar ficha");
                send(clientIndex->fd, buffer_, sizeof(buffer_), 0);
                return false;
            }
        }
        return false;
    } else {
        sprintf(buffer_, "-Err. Comando no reconocido");
        send(clientIndex->fd, buffer_, sizeof(buffer_), 0);
        return false;
    }
}

bool Server::handleDisconnection(vector<User>::iterator& clientIndex)
{
    if (clientIndex->playing != NULL)
        endMatch(clientIndex->playing);
    close(clientIndex->fd);
    cout << "client with fd " << clientIndex->fd << " disconnected\n";
    clientIndex = clients_.erase(clientIndex) - 1;
}