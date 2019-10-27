#ifndef _FICHA_HPP_
#define _FICHA_HPP_
#include <cassert>
#include <ostream>
class Ficha {
private:
    int _n1;
    int _n2;

public:
    inline Ficha(const int& n1, const int& n2)
    {
        if ((n1 < 0) || (n1 >= 7) || (n2 < 0) || (n2 >= 7))
            throw "Invalid Argument";
        _n1 = n1;
        _n2 = n2;
    }
    inline Ficha()
        : _n1(-1)
        , _n2(-1){};
    inline int getN2() const { return _n2; }
    inline int getN1() const { return _n1; }
    inline bool operator==(const Ficha& f) const
    {
        return (((_n1 == f._n1) && (_n2 == f._n2)) || ((_n1 == f._n2) && (_n2 == f._n1)));
    }
    Ficha rotar() const { return *new Ficha(_n2, _n1); }
};
std::ostream& operator<<(std::ostream& os, const Ficha& f);
#endif