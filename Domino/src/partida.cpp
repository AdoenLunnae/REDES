#include "partida.hpp"
#include "ficha.hpp"
#include "jugador.hpp"
#include <vector>

bool Partida::canPlay(bool isJ1) const
{
    Jugador j = isJ1 ? _j1 : _j2;
    std::vector<Ficha> mano = j.getMano();
    int izq = _t.getExtremoIzquierdo(), der = _t.getExtremoDerecho();
    for (auto& ficha : mano) {
        if (ficha.getN1() == izq || ficha.getN1() == der || ficha.getN2() == izq || ficha.getN2() == der)
            return true;
    }
    return false;
}

bool Partida::jugarFicha(const Ficha& f1, const bool extemoDerecho, const bool player1)
{
    return true;
}
