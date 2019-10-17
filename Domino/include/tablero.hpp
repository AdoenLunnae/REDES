#ifndef _TABLERO_HPP_
#define _TABLERO_HPP_
#include "ficha.hpp"
#include <list>

    class Tablero{
    private:
        std::list<Ficha> _fichas;
    public:
        inline Tablero(){_fichas.clear();}
        inline int getExtremoIzquierdo() const{
            return(_fichas.front().getN1());
        }
        
        inline int getExtremoDerecho() const {
            return (_fichas.back().getN2());
        }
        
        bool ponerFichaIzq(Ficha &f);
        bool ponerFichaDer(Ficha &f);
        
        inline bool estaVacio() const {return(_fichas.empty());}
        inline int getNumFichas() const {return(_fichas.size());}
    };

#endif