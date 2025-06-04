#ifndef MICROCONTROLADOR_H
#define MICROCONTROLADOR_H
using namespace std;

class MicroControlador {
    public:
    int ruta[4];
    MicroControlador();
    void ObtenerRuta(int, Disco &) {}
};

#endif