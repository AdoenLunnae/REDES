#ifndef _PARTIDA_HPP_
#define _PARTIDA_HPP_
#include "jugador.hpp"
#include "monton.hpp"
#include "tablero.hpp"
#include <cassert>
#include <cstdlib>
#include <ctime>
#include <list>
#include <ostream>
#include <string>
#include <vector>

class Partida {
private:
    Jugador _j1, _j2;
    Tablero _t;
    Monton _m;
    bool _turnJ1;

public:
    inline Partida()
        : _j1()
        , _j2()
        , _t()
        , _m()
        , _turnJ1(true){};
    std::string showBoard() { return _t.toString(); }
    bool canPlay(const bool isJ1);
    bool isTurnJ1() const { return _turnJ1; }
    void changeTurn() { _turnJ1 = !_turnJ1; }
    bool jugarFicha(const Ficha& f1, const bool extremoDerecho);
};

#endif