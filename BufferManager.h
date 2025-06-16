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

    BufferManager(Disco* disco, MicroControlador* mc);
    void cargarBloque(int LBA);
    void escribirBloque(int LBA);
};