#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "tablero.hpp"
#include "ficha.hpp"

TEST_CASE("Constructor"){
    Tablero t;
    SUBCASE("The board is empty")
        CHECK(t.estaVacio());
}

TEST_CASE("Place tile to the left"){
    Tablero t;
    Ficha f(1,3);
    CHECK(t.ponerFichaIzq(f));
    
    SUBCASE("There is one more tile")
        CHECK(t.getNumFichas() == 1);
    
    SUBCASE("Can place only a tile that fits"){
        f = *new Ficha(1, 1);
        CHECK(t.ponerFichaIzq(f));
        f = *new Ficha(2, 2);
        CHECK_FALSE(t.ponerFichaIzq(f));
    }
}

TEST_CASE("Place tile to the rigth"){
    Tablero t;
    Ficha f(3, 1);
    CHECK(t.ponerFichaDer(f));
    
    SUBCASE("There is one more tile")
        CHECK(t.getNumFichas() == 1);
    
    SUBCASE("Can place only a tile that fits"){
        f = *new Ficha(1, 1);
        CHECK(t.ponerFichaDer(f));
        f = *new Ficha(2, 2);
        CHECK_FALSE(t.ponerFichaDer(f));
    }
}