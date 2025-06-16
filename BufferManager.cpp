#include "BufferManager.h"
//buffer basico para manejar bloques de disco
BufferManager::BufferManager(Disco* disco, MicroControlador* mc)
    : disco(disco), micro(mc) {}

void BufferManager::cargarBloque(int LBA) { //aqui pasamos el numero logico de bloque
    int indice = LBA;

    if(Bloques.find(LBA) == Bloques.end()){
        vector<char> bloqueCompleto;
        vector<char> aux;
        for(int i=0;i<disco->sectoresPorBloque;i++){
            int sectorLBA = LBA + i;
            micro->ObtenerRuta(sectorLBA);
            bloqueCompleto.insert(bloqueCompleto.end(), aux.begin(), aux.end());
        }
        Bloques[LBA] = Bloque(LBA);
        Bloques[LBA].datos = bloqueCompleto; 
    }
}

void BufferManager::escribirBloque(int LBA) {
    int tamSector = disco->capSector;
    int offset = 0;
    vector<char>& bloqueCompleto = Bloques[LBA].datos;
    int tamTotal = bloqueCompleto.size();

    for(int i=0;i<disco->sectoresPorBloque;i++){
        int sectorLBA = LBA + i;
        micro->ObtenerRuta(sectorLBA);
        int bytesRestantes = tamTotal - offset;
        int bytesAEscribir = (bytesRestantes >= tamSector) ? tamSector : bytesRestantes;

        // Solo escribimos los datos que quedan disponibles
        vector<char> datosSector(bloqueCompleto.begin() + offset, bloqueCompleto.begin() + offset + bytesAEscribir);
        disco->escribirSector(datosSector, micro->ruta);
        offset += bytesAEscribir;
    }
}
