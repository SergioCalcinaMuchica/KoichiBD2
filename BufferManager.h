#pragma once
#include "DISCO.h"
#include "MicroControlador.h"
#include "Bloque.h"
#include <vector>
#include <unordered_map>
using namespace std;
class BufferManager {
public:
    unordered_map<int, Bloque> Bloques;
    Disco* disco;
    MicroControlador* micro;
    int tamSector;
    int sectoresPorBloque;

    BufferManager(Disco* d, MicroControlador* mc);
    void cargarBloque(int LBA);
    void escribirBloque(int LBA);

    Bloque& obtenerBloque(int LBA, char modo, bool pin);  // modo: 'L' o 'E'
    void liberarBloque(int LBA, bool modificado); // true si se modificó
};