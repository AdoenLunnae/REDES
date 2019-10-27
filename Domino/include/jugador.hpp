#ifndef _JUGADOR_HPP_
#define _JUGADOR_HPP_
#include "ficha.hpp"
#include "monton.hpp"
#include <algorithm>
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
    Ficha robar(Monton& m);
    bool tieneFicha(const Ficha& f) const
    {
        for (auto& ficha : _mano)
            if (ficha == f)
                return true;
        return false;
    }
    void soltarFicha(const Ficha& f)
    {
        _mano.erase(std::find(_mano.begin(), _mano.end(), f));
    }
    std::string handToString() const;
};

#endif