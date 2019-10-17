#include "jugador.hpp"
#include "ficha.hpp"
#include "monton.hpp"
bool Jugador::robar(Monton &m){
    if(m.getTamano() == 0){
        return(false);
    }
    Ficha f = m.getRestantes()[rand() % m.getTamano()];
    m.cogerFicha(f.getN1(), f.getN2());
    _mano.push_back(f);
    return true;
}