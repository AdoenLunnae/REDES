#include "jugador.hpp"
#include "ficha.hpp"
#include "monton.hpp"
#include <sstream>
#include <string>

Ficha Jugador::robar(Monton& m)
{
    if (m.getTamano() == 0)
        return (*new Ficha());
    Ficha f = m.getRestantes()[rand() % m.getTamano()];
    m.cogerFicha(f.getN1(), f.getN2());
    _mano.push_back(f);
    return f;
}

std::string Jugador::handToString() const
{
    std::stringstream ss;
    for (auto& ficha : getMano()) {
        ss << ficha;
    }
    return ss.str();
}
