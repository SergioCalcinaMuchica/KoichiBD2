#pragma once
#include <vector>

class Bloque{
    public:
    int id;
    bool disponible;
    int idsiguiente;
    int espacio_disponible; //bytes incluyendo el espacio que ocupa la cabecera
    std::vector<char> datos;
    void ObtenerDatosCabecera();
    void saltarCabecera();
    Bloque() {} // Constructor por defecto
    Bloque(int);
};