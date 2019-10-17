#include "monton.hpp"

Monton::Monton(){
    for(int i = 0; i < 7; ++i){
        for (int j = 0; j < 7; ++j){
            if(i >= j){
                _restantes.emplace_back(i,j);
            }
        }   
    }
}

bool Monton::cogerFicha(const int n1, const int n2){
    auto pos = std::find(_restantes.begin(), _restantes.end(), Ficha(n1, n2));
    if(pos == _restantes.end()) return false;
    _restantes.erase(pos);
    return true;
}