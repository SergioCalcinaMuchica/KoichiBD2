#pragma once
#include "DISCO.h"
#include "MicroControlador.h"
#include <vector>
#include <unordered_map>

class BufferManager {
public:
    unordered_map<int, vector<char>> Bloques;
    Disco* disco;
    MicroControlador* micro;

    BufferManager(Disco* disco, MicroControlador* mc);
};