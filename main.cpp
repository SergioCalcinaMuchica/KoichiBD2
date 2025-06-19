#include <iostream>
#include "DISCO.h"
#include "MicroControlador.h"
#include "BufferManager.h"
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
    ejecutando = true;
    while (ejecutando) {
        cout << "\n1) Mostrar informacion del disco\n";
        cout << "2) Salir\n";
        cout << "3) Mostrar arbol del disco\n";
        cout << "4) Insertar esquema\n";
        cout << "5) Insertar datos\n";
        cout << "6) Pruebas varias\n";
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
                cout<<koichiBD.BloqueNDisponible();
                break;
            }
            default:
                cout << "Opcion invalida.\n";
        }
    }
    return 0;
}

//g++ main.cpp Microcontrolador.cpp Disco.cpp BufferManager.cpp Bloque.cpp SistemaGestor.cpp -o main