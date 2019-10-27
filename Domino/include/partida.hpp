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

    bool canPlay(const bool isJ1) const;
    bool isTurnJ1() const { return _turnJ1; }
    void changeTurn() { _turnJ1 = !_turnJ1; }
    bool jugarFicha(const Ficha& f1, const bool extremoDerecho, const bool player1);
    Ficha robarFicha(const bool player1);
    void start()
    {
        for (int i = 0; i < 7; ++i) {
            _j1.robar(_m);
            _j2.robar(_m);
        }
    }

    std::string showBoard() const { return _t.toString(); }
    std::string showHand(const bool player1) const { return player1 ? _j1.handToString() : _j2.handToString(); }
    std::string jugadaInicial();
    int isFinished() const
    {
        if (_j1.getMano().empty())
            return 1;
        if (_j2.getMano().empty())
            return -1;
        int extDerCount = 0, extIzqCount = 0;
        for (auto& f : _t.getFichas()) {
            if (f.getN1() == _t.getExtremoDerecho() || f.getN2() == _t.getExtremoDerecho())
                extDerCount++;
            if (f.getN1() == _t.getExtremoIzquierdo() || f.getN2() == _t.getExtremoIzquierdo())
                extIzqCount++;
        }
        if (extDerCount == 7 && extIzqCount == 7) {
            int sumJ1 = 0, sumJ2 = 0;
            for (auto& f : _j1.getMano())
                sumJ1 += f.getN1() + f.getN2();

            for (auto& f : _j2.getMano())
                sumJ2 += f.getN1() + f.getN2();
            return (sumJ2 > sumJ1 ? 1 : -1);
        }
        return 0;
    }
};

#endif