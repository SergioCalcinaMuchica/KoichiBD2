#ifndef MICROCONTROLADOR_H
#define MICROCONTROLADOR_H
using namespace std;

class MicroControlador {
    public:
    Disco* disco;
    int ruta[4];
    MicroControlador(Disco *d);
    void ObtenerRuta(int) {}
};

#endif