#include "BufferPool.h"
#include "BufferManager.h"
#include <iostream>
using namespace std;

void BufferPool::imprimirPagina(int m){
    for(int i=0;i<bufferSize;i++){
        if(frames[i].pageID==m){
            m=i;
            break;
        }
    }
    std::cout << "PageID: " << frames[m].pageID << "\n";
    std::cout << "Datos del bloque:\n";
    for (char c : frames[m].bloque.datos) {
        std::cout << c;
    }
    std::cout << "\n";
}

BufferPool::BufferPool(BufferManager* bm, int size) : bufferManager(bm), bufferSize(size), clock(0) {
    for (int i = 0; i < bufferSize; i++) {
        frames[i].frameID = i;
        frames[i].pageID = -1; // Inicialmente no hay páginas
        frames[i].modo = '-'; // Por defecto modo lectura
        frames[i].dirtyBit = 0;
        frames[i].pinCount = 0;
        frames[i].lastUsed = 0;
        frames[i].pinned = 0;
    }
}

Bloque& BufferPool::accederPagina(int LBA, char modo, bool pinear){
    clock++;

    //La página ya está cargada?
    for (int i = 0; i < bufferSize; i++) {
        if (frames[i].pageID == LBA) {
            Page& p = frames[i];
            p.pinCount++;
            p.lastUsed = clock;
            
            //DETALLES como si su modo era E(escritura) preguntar si se quiere guardar para poder guardarlo en el disco o simplemente cambiar el modo
            if(p.modo=='E'){
                cout<<"Desea guardar los cambios en el disco? (si/no): ";
                std::string respuesta;
                std::cin >> respuesta;
                if (respuesta == "si") {
                    cout << "Cambios GUARDADOS en el disco.\n";
                    bufferManager->escribirBloque(p.pageID, p.bloque.datos);
                }else{
                    cout << "Cambios NO GUARDADOS en el disco.\n";
                }
                std::vector<char> datos = bufferManager->cargarBloque(LBA);
                p.bloque = Bloque(LBA, datos);
                p.pinCount--;
            }
            p.modo = modo;
            p.dirtyBit = (modo == 'E') ? 1 : 0;
            if (pinear) p.pinned = 1;
            imprimirEstado();
            return p.bloque;
        }
    }

    // Paso 2: Buscar un frame libre
    for (int i = 0; i < bufferSize; i++) {
        if (frames[i].pageID == -1) {
            Page& p = frames[i];

            p.frameID = i;
            p.pageID = LBA;
            p.modo = modo;
            p.dirtyBit = (modo == 'E') ? 1 : 0;
            p.pinCount = 1;
            p.lastUsed = clock;
            if (pinear) p.pinned = 1;

            std::vector<char> datos = bufferManager->cargarBloque(LBA);
            p.bloque = Bloque(LBA, datos);

            imprimirEstado();
            return p.bloque;
        }
    }

    // Paso 3: Si no hay espacio libre → aplicar reemplazo
    reemplazarPagina(LBA, modo, pinear);

     // Después del reemplazo, uno de los frames ya tiene la página
    for (int i = 0; i < bufferSize; i++) {
        if (frames[i].pageID == LBA) {
            imprimirEstado();
            return frames[i].bloque;
        }
    }
    throw std::runtime_error("Error al acceder a la página: no se pudo cargar la página solicitada.");
}

void BufferPool::reemplazarPagina(int LBA, char modo, bool pinear) {
    // Buscar víctima: LRU no pineada
    int victima = -1;
    int minUso = frames[0].lastUsed;

    //PRIMER CASO
    for (int i = 0; i < bufferSize; i++) {
        if (frames[i].pinned == 0 && frames[i].lastUsed <= minUso && frames[i].pinCount == 0) {
            victima = i;
            minUso = frames[i].lastUsed;
        }
    }
    //EN RESUMEN
    //SE BUSCA AL DE MENOR TIEMPO CADA VEZ Y SE VERIFICA SU PINCOUNT
    //SI NO SE ENCUENTRA, SE PREGUNTA AL SIGUIENTE VIEJITO Y ASÍ

    std::vector<int> orden;
    for (int i = 0; i < bufferSize; i++) {
        orden.push_back(i);
    }

    std::sort(orden.begin(), orden.end(), [&](int a, int b) {
        return frames[a].lastUsed < frames[b].lastUsed;
    });

    while(victima==-1){
        imprimirEstado();
        for (int i = 0; i < bufferSize; i++) {
            if (frames[orden[i]].pinCount == 0) {
                victima = orden[i];
                frames[victima].pinned=0; // Despinear
                if(frames[victima].dirtyBit==1){
                    bufferManager->escribirBloque(frames[victima].pageID, frames[victima].bloque.datos);
                }
                break;
            }
        }

        if(victima == -1){
            cout<<"Ingresar ID de la pagina donde se terminara una solicitud:";
            int id;
            std::cin >> id;
            int vaux;
            for(int i = 0; i < bufferSize; i++) {
                if (frames[i].pageID == id) {
                    vaux=i;
                    break;
                }
            }
            if(frames[vaux].modo == 'E' || frames[vaux].dirtyBit == 1){
                bufferManager->escribirBloque(frames[vaux].pageID, frames[vaux].bloque.datos);
                frames[vaux].dirtyBit = 0;
            }
            if(frames[vaux].pinned==1 && frames[vaux].pinCount == 1) {
                frames[vaux].pinned = 0;
            }
            std::cout << "Pagina " << id << " ya estaba en el buffer, decrece pinCount.\n";
            frames[vaux].pinCount--;
        }
    }

    Page& vieja = frames[victima];

    // Si está modificada, escribir al disco
    if (vieja.dirtyBit == 1) {
        bufferManager->escribirBloque(vieja.pageID, vieja.bloque.datos);
    }

    // Reemplazar con la nueva
    Page& nueva = frames[victima];
    nueva.frameID = victima;
    nueva.pageID = LBA;
    nueva.modo = modo;
    nueva.dirtyBit = (modo == 'E') ? 1 : 0;
    nueva.pinCount = 1;
    nueva.lastUsed = clock;
    if (pinear) nueva.pinned = 1;

    // Cargar desde disco
    std::vector<char> datos = bufferManager->cargarBloque(LBA);
    nueva.bloque = Bloque(LBA, datos);
}

void BufferPool::liberarPagina(int m) { //entra indice
    int vaux;
    for(int i = 0; i < bufferSize; i++) {
        if (frames[i].pageID == m) {
            vaux = i;
        }
    }
    if(frames[vaux].modo == 'E' || frames[vaux].dirtyBit == 1){
        bufferManager->escribirBloque(frames[vaux].pageID, frames[vaux].bloque.datos);
        frames[vaux].dirtyBit = 0; // ya se escribio en el disco lo que tenia
    }
    if(frames[vaux].pinned==1 && frames[vaux].pinCount == 1) {
        frames[vaux].pinned = 0;
    }
    frames[vaux].pinCount--;
    std::cout << "Solicitud de pagina terminada.\n";
}

void BufferPool::imprimirEstado() {
    std::cout << "\nEstado actual del Buffer Pool:\n";
    std::cout << "FrameID | PageID | Modo | Dirty | PinCnt | LastUsed | Pinned | Unpinned\n";
    std::cout << "--------|--------|------|--------|--------|----------|--------|----------\n";

    for (int i = 0; i < bufferSize; i++) {
        Page& p = frames[i];

        if (p.pageID == -1) {
            printf("%7d |   -    |  -   |   -    |   -    |    -     |   -    |    -\n", i);
        } else {
            printf("%7d | %6d |  %c   |   %d    |   %2d   |   %6d  |   %d    |    %d\n",
                p.frameID,
                p.pageID,
                p.modo,
                p.dirtyBit,
                p.pinCount,
                p.lastUsed,
                p.pinned,
                !p.pinned
            );
        }
    }
}

void BufferPool::pinearPagina(int m) {
    for(int i = 0; i < bufferSize; i++) {
        if (frames[i].pageID == m) {
            m=i;
            break;
        }
    }
    frames[m].pinned = 1;
    std::cout << "Pagina " << m << " pineada.\n";
}

void BufferPool::despinearPagina(int m) {
    for(int i = 0; i < bufferSize; i++) {
        if (frames[i].pageID == m) {
            m=i;
            break;
        }
    }
    frames[m].pinned = 0;
    std::cout << "Pagina " << m << " despineada.\n";
}