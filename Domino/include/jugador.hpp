#ifndef _JUGADOR_HPP_
#define _JUGADOR_HPP_
#include "ficha.hpp"
#include "monton.hpp"
#include <cstdlib>
#include <ctime>
#include <vector>

class Jugador {
private:
    std::vector<Ficha> _mano;

public:
    inline Jugador()
    {
        _mano.clear();
        srand(time(nullptr));
    }
    std::vector<Ficha> getMano() const { return _mano; }
    bool robar(Monton& m);
    bool tieneFicha(const Ficha& f)
    {
        for (auto& ficha : _mano)
            if (ficha == f)
                return true;
        return false;
    }
};

#endif