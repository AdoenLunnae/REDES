#ifndef _MONTON_HPP_
#define _MONTON_HPP_
#include <vector>
#include <algorithm>
#include "ficha.hpp"
    class Monton{
    private:
        std::vector<Ficha> _restantes;
    public:
        Monton();
        std::vector<Ficha> getRestantes() const {return _restantes;}
        int getTamano() const {return _restantes.size();}
        bool cogerFicha(int n1, int n2);
    };
#endif