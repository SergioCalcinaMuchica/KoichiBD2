#include <iostream>
#include <cstdio>
#include <fstream>
#include <windows.h>
using namespace std;

bool existeCarpetaDisco() {
    DWORD atributos = GetFileAttributesA("Disco");
    return (atributos != INVALID_FILE_ATTRIBUTES) && (atributos & FILE_ATTRIBUTE_DIRECTORY);
}

class Disco {
public:
    int platos;
    int pistas;
    int sectores;
    int capSector; // en MB
    int sectoresPorBloque;
    int espacioTotal;

    Disco() : platos(0), pistas(0), sectores(0), capSector(0), sectoresPorBloque(0), espacioTotal(0) {}

    void pedirDatos() {
        cout << "Ingrese el numero de platos: ";
        cin >> platos;
        cout << "Ingrese el numero de pistas: ";
        cin >> pistas;
        cout << "Ingrese el numero de sectores: ";
        cin >> sectores;
        cout << "Ingrese la capacidad de cada sector (en MB): ";
        cin >> capSector;
        cout << "Ingrese el numero de sectores por bloque: ";
        cin >> sectoresPorBloque;
        espacioTotal = platos * 2 * pistas * sectores * capSector;
    }

    void crearCarpeta(const char* ruta) {
        CreateDirectoryA(ruta, NULL);
    }

    void crearDisco() {
        crearCarpeta("Disco");
        char ruta_temporal[300];
        for (int p = 0; p < platos; p++) {
            sprintf(ruta_temporal, "Disco\\Plato%d", p);
            crearCarpeta(ruta_temporal);
            for (int sup = 0; sup < 2; sup++) {
                sprintf(ruta_temporal, "Disco\\Plato%d\\Superficie%d", p, sup);
                crearCarpeta(ruta_temporal);
                for (int pista = 0; pista < pistas; pista++) {
                    sprintf(ruta_temporal, "Disco\\Plato%d\\Superficie%d\\Pista%d", p, sup, pista);
                    crearCarpeta(ruta_temporal);
                    for (int sector = 0; sector < sectores; sector++) {
                        sprintf(ruta_temporal, "Disco\\Plato%d\\Superficie%d\\Pista%d\\Sector%d.txt", p, sup, pista, sector);
                        ofstream archivo(ruta_temporal);
                        archivo.close();
                    }
                }
            }
        }
    }

    void borrarTodoEnCarpeta(const char* path) { //recursivo
        char search_path[300];
        sprintf(search_path, "%s\\*", path);

        WIN32_FIND_DATAA fd; //estrucutra q tiene informacion de los archivos encontrados
        HANDLE hFind = FindFirstFileA(search_path, &fd); //puntero o identificador del proceso de busqueda

        if (hFind == INVALID_HANDLE_VALUE) return;

        do {
            if (strcmp(fd.cFileName, ".") == 0 || strcmp(fd.cFileName, "..") == 0) //evitar bucles infinitos 
                continue;

            char ruta_completa[300];
            sprintf(ruta_completa, "%s\\%s", path, fd.cFileName);

            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) { //elementos FILE_.. activado? entonces es una carpeta
                borrarTodoEnCarpeta(ruta_completa);           // recursivo
                RemoveDirectoryA(ruta_completa);               // elimina carpeta vacía
            } else {
                DeleteFileA(ruta_completa);                    // elimina archivo
            }

        } while (FindNextFileA(hFind, &fd));

        FindClose(hFind);
    }

    void borrarDisco() {
        const char* disco = "Disco";
        borrarTodoEnCarpeta(disco);
        RemoveDirectoryA(disco);
    }

    void mostrarArbol(const char* path, int nivel = 0) {
        char search_path[300];
        sprintf(search_path, "%s\\*", path);
        WIN32_FIND_DATAA fd;
        HANDLE hFind = FindFirstFileA(search_path, &fd);
        if (hFind == INVALID_HANDLE_VALUE) return;

        do {
            const char* nombre = fd.cFileName;
            if (strcmp(nombre, ".") == 0 || strcmp(nombre, "..") == 0)
                continue;

            // Sangría básica con espacios y |
            for (int i = 0; i < nivel; ++i) std::cout << "|   ";

            // Usar +-- o |- como ramas
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                cout << "+-- " << nombre << "/" << std::endl;
                char nueva_ruta[300];
                sprintf(nueva_ruta, "%s\\%s", path, nombre);
                mostrarArbol(nueva_ruta, nivel + 1); // Recursivo
            } else {
                cout << "+-- " << nombre << std::endl;
            }

        } while (FindNextFileA(hFind, &fd));
        FindClose(hFind);
    }

    void mostrarInfo() {
        cout << "Capacidad del disco: " << espacioTotal << " MB" << endl;
        cout << "Capacidad del bloque: " << sectoresPorBloque * capSector << " MB" << endl;
        cout << "Numero de bloques por pista: " << sectores / sectoresPorBloque << endl;
        cout << "Numero de bloques por plato: " << (pistas * sectores / sectoresPorBloque) * 2 << endl;
    }
};

int main() {
    Disco disco;
    int opc=0;
    bool ejecutando = true;
    string entrada;
    cout<<"Bienvenido al sistema de gestion de discos\n";
    cout<<"1)Crear disco\n";
    cout<<"2)Continuar en disco\n";
    cout<<"Seleccione una opcion: ";
    cin >> opc;
    while(ejecutando){
        switch(opc){
            case 1:
                disco.borrarDisco(); // Limpia antes de crear
                disco.pedirDatos();
                disco.crearDisco();
                opc=0;
                ejecutando=false;
                break;
            case 2:
                if(!existeCarpetaDisco()){
                    cout<<"NINGUN DISCO EXISTE, CREE UNO PRIMERO\n";
                    opc=1;
                }
                cout << "Continuando en disco existente...\n";
                //funcion a hacer para recuperar datos del disco
                opc=0;
                ejecutando=false; // Salir del bucle
                break;
            default:
                cout << "Opcion invalida. Saliendo.\n";
                opc=1;
        }
    }

    ejecutando=true;    
    while(ejecutando){
        cout << "Bienvenido al sistema de gestion de discos\n";
        cout << "1) Mostrar informacion del disco\n";
        cout<<  "2) Salir\n";
        cout << "3) Adicionar CSV\n";
        cout << "4) Mostrar arbol del disco\n";
        cout << "Seleccione una opcion: ";
        cin >> opc;
        switch (opc){
            case 1:
                cout<<"----------------------------\n";
                disco.mostrarInfo();
                cout<<"----------------------------\n";
                break;
            case 2:
                ejecutando=false;
                break;
            case 3:
                cin>>entrada;
                break;
            case 4:
                cout << "Arbol del disco:\n";
                disco.mostrarArbol("Disco");
                break;
            default:
                cout << "Opcion invalida. Saliendo.\n";
                return 1;
        }
    }
    return 0;
}