#include "server.hpp"
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

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

    clients_.push_back(User{ newSd_ });
    cout << "client with fd " << newSd_ << " connected\n";
    return true;
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
        sprintf(buffer_, "-Err. No estás logeado");
        send(client.fd, buffer_, sizeof(buffer_), 0);
        return false;
    }
    if (!shouldBePlaying && (client.playing != NULL)) {
        sprintf(buffer_, "-Err. Ya estás logeado");
        send(client.fd, buffer_, sizeof(buffer_), 0);
        return false;
    }
    return true;
}

bool Server::checkTurn(const User& client)
{
    if (!checkPlaying(client, true)) {
        sprintf(buffer_, "-Err. No es tu turno");
        send(client.fd, buffer_, sizeof(buffer_), 0);
        return false;
    }
    return (!(client.isPlayer1 ^ client.playing->isTurnJ1()));
}

bool Server::handleMessage(vector<User>::iterator clientIndex, const char* message)
{
    /*
    REGISTER –u  usuario –p  password –d  Nombre  y  Apellidos –c  Ciudad:  mensaje mediante  el  cual  el  usuario  solicita  registrarse  para  acceder  al  servicio de  chat  que escucha en el puerto TCP  2050.
    PASO-TURNO: mensaje para indicar que no se dispone de ninguna ficha para colocar en ese turno.
    INICIAR-PARTIDA: mensaje para indicar el interés en jugar una partida de dominó.
    COLOCAR-FICHA |valor1|valor2|,extremo:  mensaje  para  colocar  una  ficha  en  el tablero.
    -> Del servidor: FICHAS |valor1|valor2||...||: mensaje para enviar las fichas de un jugador.
    -> Del servidor: TABLERO |valor1|valor2||...||: mensaje para enviar el tablero de la partida.
    ROBAR-FICHA:mensaje para robaruna ficha del montón.
    -> Del servidor FICHA |valor1|valor2|: mensaje para enviar una ficha a un jugador que haya robado.
    SALIR: mensaje para solicitar salir del juego.
    */
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
            clientIndex->introducedUsername = username;
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
        } else if (clientIndex->introducedUsername == "") {
            sprintf(buffer_, "-Err. No se ha introducido usuario");
        } else if (correct = checkPassword(clientIndex->introducedUsername, password)) {
            sprintf(buffer_, "+Ok. Contraseña correcta");
            clientIndex->loggedIn = true;
        } else {
            sprintf(buffer_, "-Err. Contraseña incorrecta");
        }
        send(clientIndex->fd, buffer_, sizeof(buffer_), 0);
        return correct;
    } else if (command == "SALIR") {
        handleDisconnection(clientIndex);
        return true;
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
            if (clientIndex->playing->canPlay(clientIndex->isPlayer1))
                clientIndex->playing->changeTurn();
        }
    } else if (command == "COLOCAR-FICHA") {
        if (checkLogged(*clientIndex, true) && checkPlaying(*clientIndex, true) && checkTurn(*clientIndex)) {
            std::string fichaStr = strtok(NULL, ",");
            std::string extremo = strtok(NULL, "\n");
            int n1, n2;
            bool correct = false;
            sscanf(fichaStr.c_str(), "|%d|%d|\n", &n1, &n2);
            try {
                if (extremo == "izq") {
                    clientIndex->playing->jugarFicha(Ficha(n1, n2), false);
                    std::string board = clientIndex->playing->showBoard();
                    sprintf(buffer_, "+Ok. Ficha jugada\nTABLERO %s", board.c_str());
                    correct = true;
                } else if (extremo == "der") {
                    clientIndex->playing->jugarFicha(Ficha(n1, n2), true);
                    std::string board = clientIndex->playing->showBoard();
                    sprintf(buffer_, "+Ok. Ficha jugada\nTABLERO %s", board.c_str());
                    correct = true;
                } else {
                    sprintf(buffer_, "-Err. Formato incorrecto");
                }
            } catch (const std::exception& e) {
                sprintf(buffer_, "-Err. La ficha es invalida");
            }
            send(clientIndex->fd, buffer_, sizeof(buffer_), 0);
        }
    }
}

bool Server::handleDisconnection(vector<User>::iterator& clientIndex)
{
    close(clientIndex->fd);
    cout << "client with fd " << clientIndex->fd << " disconnected\n";
    clientIndex = clients_.erase(clientIndex);
    clientIndex--;
}