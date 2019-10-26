cmake_minimum_required(VERSION 3.0.0)

project(Domino VERSION 0.1.0)

include_directories(include/)

include(CTest)
enable_testing()

list(APPEND FILES ficha monton tablero jugador)

foreach(FILE ${FILES})
    list(APPEND SRC src/${FILE}.cpp)
endforeach(FILE)

add_library(sources OBJECT ${SRC} src/partida.cpp )

foreach(FILE ${FILES})
    add_executable(${FILE}Test tests/${FILE}Test.cpp $<TARGET_OBJECTS:sources>)
    add_test(${FILE}Test ${FILE}Test)
endforeach(FILE)

add_executable(server src/main.cpp server/server.cpp $<TARGET_OBJECTS:sources>)
