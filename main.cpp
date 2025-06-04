#include <iostream>
#include "DISCO.h"
using namespace std;

int main() {
    Disco disco;
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

    ejecutando = true;
    while (ejecutando) {
        cout << "\n1) Mostrar informacion del disco\n";
        cout << "2) Salir\n";
        cout << "3) Mostrar arbol del disco\n";
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
            default:
                cout << "Opcion invalida.\n";
        }
    }
    return 0;
}