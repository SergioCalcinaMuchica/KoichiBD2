#pragma once
#include <vector>
#include <string>

class Bloque{
    public:
    int id;
    int disponible;
    int idsiguiente=0;
    int espacio_disponible; //bytes incluyendo el espacio que ocupa la cabecera
    std::vector<char> datos;
    void ObtenerDatosCabecera();
    void putDisponible(int);
    void putIdSiguiente(int);
    void putEspacio_disponible(int);
    Bloque() {} // Constructor por defecto
    Bloque(int);
};