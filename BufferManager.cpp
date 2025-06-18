#include "BufferManager.h"
#include <iostream>
//buffer basico para manejar bloques de disco
BufferManager::BufferManager(Disco* d, MicroControlador* mc){
    disco=d;
    micro=mc;
    tamSector = disco->capSector;
    sectoresPorBloque = disco->sectoresPorBloque;
}

void BufferManager::cargarBloque(int LBA) {
    if (Bloques.find(LBA) == Bloques.end()) {
        vector<char> bloqueCompleto;

        for (int i = 0; i < disco->sectoresPorBloque; i++) {
            int sectorLBA = (LBA*sectoresPorBloque) + i;
            cout<<"sectorLBA: "<<sectorLBA<<endl;
            micro->ObtenerRuta(sectorLBA);
            // Leer el contenido del sector desde el archivo
            vector<char> aux = disco->leerSector(micro->ruta);
            if(aux.size()!=0){
                bloqueCompleto.insert(bloqueCompleto.end(), aux.begin(), aux.end());
            }
        }

        Bloques[LBA] = Bloque(LBA,bloqueCompleto);
    }
}

void BufferManager::escribirBloque(int LBA) {
    int offset = 0;
    cout<<LBA<<endl;
    vector<char>& bloqueCompleto = Bloques[LBA].datos;
    for(int i=0;i<Bloques[LBA].datos.size();i++){
        cout<<Bloques[LBA].datos[i];
    }
    cout<<endl;
    int tamTotal = bloqueCompleto.size();
    cout<<"tamTotal"<<tamTotal<<endl;
    for(int i=0;i<sectoresPorBloque;i++){
        int sectorLBA = (LBA*sectoresPorBloque) + i;
        micro->ObtenerRuta(sectorLBA);
        int bytesRestantes = tamTotal - offset;
        int bytesAEscribir = (bytesRestantes >= tamSector) ? tamSector : bytesRestantes;
        // Solo escribimos los datos que quedan disponibles
        vector<char> datosSector(bloqueCompleto.begin() + offset, bloqueCompleto.begin() + offset + bytesAEscribir);
        disco->escribirSector(datosSector, micro->ruta);
        cout<<"p1"<<endl;
        offset += bytesAEscribir;
    }
}

Bloque& BufferManager::obtenerBloque(int LBA, char modo, bool pin) { //Lectura o Escritura
    cargarBloque(LBA); // si ya está no hace nada
    return Bloques[LBA];
}

void BufferManager::liberarBloque(int LBA, bool modificado) { //true si esta modificado o algo
    if (modificado) {
        escribirBloque(LBA);
        Bloques.erase(LBA);
    }
}

void BufferManager::liberarBloqueSinEscribir(int LBA){
  Bloques.erase(LBA);
}
