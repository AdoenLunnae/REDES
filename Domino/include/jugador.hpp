#ifndef _JUGADOR_HPP_
#define _JUGADOR_HPP_
#include <vector>
#include <cstdlib>
#include <ctime>
#include "ficha.hpp"
#include "monton.hpp"

    class Jugador{
    private:
        std::vector<Ficha> _mano;
    public:
        inline Jugador(){
            _mano.clear();
            srand(time(nullptr));
        }
        std::vector<Ficha> getMano() const{return _mano;}
        bool robar(Monton &m);
    };

#endif