#ifndef _PARTIDA_HPP_
#define _PARTIDA_HPP_
#include <cassert>
#include <list>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <ostream>
#include "monton.hpp"
#include "tablero.hpp"
#include "jugador.hpp"

class Partida{
    private:
        Jugador _j1, _j2;
        Tablero _t;
        Monton _m;
        bool showBoard();
        bool sendHands();
        bool sendTile();
        bool drawTile();
        bool pass();
        bool checkFinished();

    public:
        inline  Partida() : _j1(), _j2(), _t(), _m(){};


};

#endif