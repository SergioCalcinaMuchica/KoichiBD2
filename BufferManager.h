#pragma once
#include "DISCO.h"
#include "MicroControlador.h"
#include "Bloque.h"
#include "BufferPool.h"
#include <vector>
#include <unordered_map>
using namespace std;
class BufferManager {
public:
    unordered_map<int, Bloque> Bloques;
    Disco* disco;
    MicroControlador* micro;
    BufferPool* bufferPool;
    int tamSector;
    int sectoresPorBloque;

    BufferManager(Disco* d, MicroControlador* mc);
    vector<char> cargarBloque(int LBA);
    void escribirBloque(int LBA, vector<char> datos);

    Bloque& obtenerBloque(int LBA, char modo, bool pin);  // modo: 'L' o 'E'
    void liberarBloque(int LBA, bool modificado); // true si se modificó
    void liberarBloqueSinEscribir(int LBA); //libera bloque sin la necesidad de escribir en el disco 

};
