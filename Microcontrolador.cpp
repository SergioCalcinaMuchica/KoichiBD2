#include "MicroControlador.h"
#include "Disco.cpp"

MicroControlador::MicroControlador(Disco *d) {
    disco = d;
    for (int i = 0; i < 4; i++) {
        ruta[i] = 0;
    }
}

void MicroControlador::ObtenerRuta(int LBA) {
    ruta[0] = LBA / (2*disco->pistas*disco->sectores); // Plato ..sectoresxplato
    LBA = LBA%(2*disco->pistas*disco->sectores);
    ruta[1] = LBA / (disco->pistas*disco->sectores); // Superficie ..sectoresxsuperficie
    LBA = LBA%(disco->pistas*disco->sectores);
    ruta[2] = LBA/disco->sectores; // Pista
    ruta[3] = LBA % disco->sectores; // Sector
}