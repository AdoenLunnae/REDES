#ifndef _PARTIDA_HPP_
#define _PARTIDA_HPP_
#include <cassert>
#include <list>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <ostream>

    class Partida{
    private:
        Jugador _j1, _j2;
        Tablero _t;
        Monton _m;
    public:
        Partida();
    };

#endif