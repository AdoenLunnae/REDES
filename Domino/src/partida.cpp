#include "partida.hpp"
#include "ficha.hpp"
#include "jugador.hpp"
#include <sstream>
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

bool Partida::jugarFicha(const Ficha& f1, const bool extremoDerecho, const bool player1)
{
    Jugador* j = player1 ? &_j1 : &_j2;
    if (j->tieneFicha(f1)) {
        bool ponerFicha = extremoDerecho ? _t.ponerFichaDer(f1) : _t.ponerFichaIzq(f1);
        if (ponerFicha) {
            j->soltarFicha(f1);
            changeTurn();
        }
        return (ponerFicha);
    }
    return false;
}

Ficha Partida::robarFicha(const bool player1)
{
    return player1 ? _j1.robar(_m) : _j2.robar(_m);
}

std::string Partida::jugadaInicial()
{
    Ficha f;
    bool j1Starts = false, j2Starts = false;
    for (int i = 6; i >= 0 && !j1Starts && !j2Starts; --i) {
        if (_j1.tieneFicha({ i, i })) {
            j1Starts = true;
            f = *new Ficha(i, i);
        }
        if (_j2.tieneFicha({ i, i })) {
            j2Starts = true;
            f = *new Ficha(i, i);
        }
    }
    for (int i = 6; i >= 0 && !j1Starts && !j2Starts; --i) {
        for (int j = i - 1; i >= 0 && !j1Starts && !j2Starts; --j) {
            if (_j1.tieneFicha({ i, j })) {
                j1Starts = true;
                f = *new Ficha(i, i);
            }
            if (_j2.tieneFicha({ i, j })) {
                j2Starts = true;
                f = *new Ficha(i, i);
            }
        }
    }
    jugarFicha(f, true, j1Starts);
    _turnJ1 = !j1Starts;
    std::stringstream ss;
    ss << "Empieza el jugador " << (j1Starts ? "1" : "2") << " con la ficha " << f << "\n";

    return ss.str();
}