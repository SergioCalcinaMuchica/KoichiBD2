#include "MicroControlador.h"
#include "Disco.cpp"

MicroControlador::MicroControlador() {
    for (int i = 0; i < 4; i++) {
        ruta[i] = 0;
    }
}

void MicroControlador::ObtenerRuta(int LBA, Disco &d) {
    ruta[0] = LBA / (2*d.pistas*d.sectores); // Plato ..sectoresxplato
    LBA = LBA%(2*d.pistas*d.sectores);
    ruta[1] = LBA / (d.pistas*d.sectores); // Superficie ..sectoresxsuperficie
    LBA = LBA%(d.pistas*d.sectores);
    ruta[2] = LBA/d.sectores; // Pista
    ruta[3] = LBA % d.sectores; // Sector
}