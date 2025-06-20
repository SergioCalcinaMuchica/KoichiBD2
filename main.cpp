#include <iostream>
#include "DISCO.h"
#include "MicroControlador.h"
#include "BufferManager.h"
#include "BufferPool.h"
#include "Bloque.h"
#include "SistemaGestor.h"
using namespace std;
Disco disco;

int main() {
    int opc = 0;
    bool ejecutando = true;

    cout << "Bienvenido al sistema de gestion de discos\n";
    cout << "1) Crear disco\n";
    cout << "2) Continuar en disco\n";
    cout << "Seleccione una opcion: ";
    cin >> opc;

    switch(opc) {
        case 1:
            disco.borrarDisco();
            disco.pedirDatos();
            disco.crearDisco();
            break;
        case 2:
            disco.recuperarDatosDisco();
            break;
        default:
            cout << "Opcion invalida. Saliendo.\n";
            return 1;
    }
    MicroControlador microControlador = MicroControlador(&disco);
    BufferManager buffer = BufferManager(&disco, &microControlador);
    SistemaGestor koichiBD = SistemaGestor(&buffer);
    BufferPool bufferPool = BufferPool(&buffer, 4);
    ejecutando = true;
    while (ejecutando) {
        cout << "\n1) Mostrar informacion del disco\n";
        cout << "2) Salir\n";
        cout << "3) Mostrar arbol del disco\n";
        cout << "4) Insertar esquema\n";
        cout << "5) Insertar datos\n";
        cout << "6) SIMULADOR BUFFERPOOL\n";
        cout << "Seleccione una opcion: ";
        cin >> opc;
        switch (opc) {
            case 1:
                disco.mostrarInfo();
                break;
            case 2:
                ejecutando = false;
                break;
            case 3:
                disco.mostrarArbol("Disco");
                break;
            case 4:
                koichiBD.insertarEsquema();
                break;
            case 5:{
                cout<<"cuantos registros desea insertar? (ingrese 0 para insertar todo el archivo): ";
                int n;
                cin>>n;
                koichiBD.insertarNRegistro(n);
                break;
            }
            case 6:{
                int opcBuffer = 0;
                while (opcBuffer != 7) {
                    cout << "\n1) Acceder a pagina\n";
                    cout << "2) Pinear pagina\n";
                    cout << "3) Imprimir estado del buffer pool\n";
                    cout << "4) Liberar pagina\n";
                    cout << "5) Despinear pagina\n";
                    cout << "6) Imprimir pagina\n";
                    cout << "7) Saliendo del simulador\n";
                    cout << "Seleccione una opcion: ";
                    cin >> opcBuffer;
                    switch (opcBuffer) {
                        case 1: {
                            int LBA;
                            char modo;
                            bool pinear;
                            cout << "Ingrese ID de pagina: ";
                            cin >> LBA;
                            cout << "Ingrese modo (L/E): ";
                            cin >> modo;
                            cout << "Pinear? (1 para si, 0 para no): ";
                            cin >> pinear;
                            bufferPool.accederPagina(LBA, modo, pinear);
                            break;
                        }
                        case 2: {
                            int ID;
                            cout << "Ingrese el ID de la pagina a pinear: ";
                            cin >> ID;
                            bufferPool.pinearPagina(ID);
                            break;
                        }
                        case 3:
                            bufferPool.imprimirEstado();
                            break;
                        case 4: {
                            int m;
                            cout << "Ingrese el indice de la pagina a liberar: ";
                            cin >> m;
                            bufferPool.liberarPagina(m);
                            break;
                        }
                        case 5:
                            int ID;
                            cout << "Ingrese el ID de la pagina a despinear: ";
                            cin >> ID;
                            bufferPool.despinearPagina(ID);
                            break;
                        case 6:
                            int ID2;
                            cout << "Ingrese el ID de la pagina a imprimir: ";
                            cin >> ID2;
                            bufferPool.imprimirPagina(ID2);
                            break;
                        case 7:
                            cout << "Saliendo del simulador.\n";
                            break;
                        default:
                            cout << "Opcion invalida.\n";
                    }
                }
                break;
            }
            default:
                cout << "Opcion invalida.\n";
        }
    }
    return 0;
}

//g++ main.cpp Microcontrolador.cpp Disco.cpp BufferManager.cpp Bloque.cpp SistemaGestor.cpp BufferPool.cpp -o main