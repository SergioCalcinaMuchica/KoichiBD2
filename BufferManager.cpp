#include "BufferManager.h"
//buffer basico para manejar bloques de disco
BufferManager::BufferManager(Disco* disco, MicroControlador* mc)
    : disco(disco), micro(mc) {}

void BufferManager::cargarBloque(int LBA) { //leer
    int secxpista = disco->sectores * disco->pistas;
    int indice = LBA;
    if (Bloques.find(LBA) == Bloques.end()) { // Si el bloque no está en el buffer
        vector<char> bloqueCompleto;
        vector<char> aux;
        for(int i=0;i<disco->platos;i++){
            for (int j = 0; j < 2; j++) {
                int sectorLBA = LBA + (i * 2 + j) * secxpista;
                micro->ObtenerRuta(sectorLBA);
                aux = disco->leerSector(micro->ruta);
                bloqueCompleto.insert(bloqueCompleto.end(), aux.begin(), aux.end());
            }
        }
        Bloques[indice] = bloqueCompleto;
    }
}

void BufferManager::escribirBloque(int LBA) {
    int secxpista = disco->sectores * disco->pistas;
    int sectoresPorBloque = disco->platos * 2;
    int tamSector = disco->capSector;
    int offset = 0;

    vector<char>& bloqueCompleto = Bloques[LBA];
    int tamTotal = bloqueCompleto.size();

    for (int i = 0; i < disco->platos; i++) {
        for (int j = 0; j < 2; j++) {
            int sectorLBA = LBA + (i * 2 + j) * secxpista;
            micro->ObtenerRuta(sectorLBA);

            int bytesRestantes = tamTotal - offset;
            int bytesAEscribir = (bytesRestantes >= tamSector) ? tamSector : bytesRestantes;

            // Solo escribimos los datos que quedan disponibles
            vector<char> datosSector(bloqueCompleto.begin() + offset, bloqueCompleto.begin() + offset + bytesAEscribir);

            disco->escribirSector(datosSector, micro->ruta);

            offset += bytesAEscribir;
        }
    }
}
