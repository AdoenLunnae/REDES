#include "ficha.hpp"
#include <iostream>

using std::ostream;

ostream& operator<<(ostream &os, const Ficha &f){
    os << "|" << f.getN1() << "·" << f.getN2() <<"|";
}
