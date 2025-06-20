#pragma once
#include <iomanip>
#include <algorithm>
#include "Bloque.h"
class BufferManager;

struct Page {
    int frameID;
    int pageID; //LBA 
    char modo;           // 'L' o 'E'
    int dirtyBit;
    int pinCount;
    int lastUsed;
    int pinned;
    Bloque bloque;
};

class BufferPool{
    public:
    BufferManager* bufferManager;
    int bufferSize;
    Page frames[4];
    int clock;
    Bloque& accederPagina(int LBA, char modo, bool pinear); 
    void reemplazarPagina(int LBA, char modo, bool pinear); //remover pagina ultima usada
    void imprimirEstado();
    void imprimirPagina(int m);
    void liberarPagina(int m); //libera pagina sin escribir en disco
    void pinearPagina(int m); //pinea pagina
    void despinearPagina(int m); //despinea pagina
    BufferPool(BufferManager* bm, int size);
};