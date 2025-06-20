#include "BufferManager.h"
#include <iostream>
//buffer basico para manejar bloques de disco
BufferManager::BufferManager(Disco* d, MicroControlador* mc){
    disco=d;
    micro=mc;
    tamSector = disco->capSector;
    sectoresPorBloque = disco->sectoresPorBloque;
}

vector<char> BufferManager::cargarBloque(int LBA) { //carga un bloque completo a partir de su LBA
    vector<char> bloqueCompleto;
    for (int i = 0; i < disco->sectoresPorBloque; i++) {
        int sectorLBA = (LBA*sectoresPorBloque) + i;
        micro->ObtenerRuta(sectorLBA);
        vector<char> aux = disco->leerSector(micro->ruta);
        if (aux.empty()) {
        std::cerr << "Sector vacío: " << micro->ruta << std::endl;
        } else {
            bloqueCompleto.insert(bloqueCompleto.end(), aux.begin(), aux.end());
        }
    }
    return bloqueCompleto;
}

void BufferManager::escribirBloque(int LBA, vector<char> datos) {
    int offset = 0;
    vector<char>& bloqueCompleto = datos;

    int tamTotal = bloqueCompleto.size();
    for(int i=0;i<sectoresPorBloque;i++){
        int sectorLBA = (LBA*sectoresPorBloque) + i;
        micro->ObtenerRuta(sectorLBA);
        int bytesRestantes = tamTotal - offset;
        int bytesAEscribir = (bytesRestantes >= tamSector) ? tamSector : bytesRestantes;
        // Solo escribimos los datos que quedan disponibles
        vector<char> datosSector(bloqueCompleto.begin() + offset, bloqueCompleto.begin() + offset + bytesAEscribir);
        disco->escribirSector(datosSector, micro->ruta);
        offset += bytesAEscribir;
    }
}

Bloque& BufferManager::obtenerBloque(int LBA, char modo, bool pin) { //Lectura o Escritura
    cargarBloque(LBA); // si ya está no hace nada
    return Bloques[LBA];
}

void BufferManager::liberarBloque(int LBA, bool modificado) { //true si esta modificado o algo
    if (modificado) {
        //escribirBloque(LBA);
        Bloques.erase(LBA);
    }
}

void BufferManager::liberarBloqueSinEscribir(int LBA){
  Bloques.erase(LBA);
}
