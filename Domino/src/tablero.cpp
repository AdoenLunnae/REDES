#include "tablero.hpp"
#include "ficha.hpp"
#include<list>


bool Tablero::ponerFichaIzq(Ficha &f){
    if(estaVacio()){
        _fichas.push_front(f);
        return true;
    }
    if(getExtremoIzquierdo() == f.getN2()){
        _fichas.push_front(f);
        return true;
    }
    else if(getExtremoIzquierdo() == f.getN1()){
        f.rotar();
        _fichas.push_front(f);
        return true;
    }
    else{
        return false;
    }
    
}

bool Tablero::ponerFichaDer(Ficha &f){
    if(estaVacio()){
        _fichas.push_back(f);
        return true;
    }
    if(getExtremoDerecho() == f.getN1()){
        _fichas.push_back(f);
        return true;
    }
    else if(getExtremoDerecho() == f.getN2()){
        f.rotar();
        _fichas.push_back(f);
        return true;
    }
    else{
        return false;
    }
    
}