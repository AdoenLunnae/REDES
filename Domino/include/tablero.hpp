#ifndef _TABLERO_HPP_
#define _TABLERO_HPP_
#include "ficha.hpp"
#include <list>
#include <sstream>
#include <string>
class Tablero {
private:
    std::list<Ficha> _fichas;

public:
    inline Tablero() { _fichas.clear(); }
    inline int getExtremoIzquierdo() const
    {
        return (_fichas.front().getN1());
    }

    inline int getExtremoDerecho() const
    {
        return (_fichas.back().getN2());
    }

    bool ponerFichaIzq(const Ficha& f);
    bool ponerFichaDer(const Ficha& f);

    inline bool estaVacio() const { return (_fichas.empty()); }
    inline int getNumFichas() const { return (_fichas.size()); }
    std::string toString() const
    {
        std::stringstream ss;
        for (auto& ficha : _fichas) {
            ss << ficha;
        }
        return ss.str();
    }
};

#endif