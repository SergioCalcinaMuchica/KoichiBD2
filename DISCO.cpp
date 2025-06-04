#include "DISCO.h"
#include "MicroControlador.h"
#include <iostream>
#include <cstdio>
#include <fstream>
namespace fs = std::filesystem;
using namespace std;

bool existeCarpetaDisco() {
    return fs::exists("Disco") && fs::is_directory("Disco");
}

int contarDigitos(int n) {
    if (n == 0) return 1;
    int digitos = 0;
    while (n != 0) {
        n /= 10;
        digitos++;
    }
    return digitos;
}

Disco::Disco() : platos(0), pistas(0), sectores(0), capSector(0), sectoresPorBloque(0), espacioTotal(0) {}

void Disco::pedirDatos() {
    cout << "Ingrese el numero de platos: ";
    cin >> platos;
    cout << "Ingrese el numero de pistas: ";
    cin >> pistas;
    cout << "Ingrese el numero de sectores: ";
    cin >> sectores;
    cout << "Ingrese la capacidad de cada sector (en Bytes): ";
    cin >> capSector;
    cout << "Ingrese el numero de sectores por bloque: ";
    cin >> sectoresPorBloque;
    espacioTotal = platos * 2 * pistas * sectores * capSector;
}

void Disco::crearCarpeta(const char* ruta) {
    fs::create_directory(ruta);
}

void Disco::crearDisco() {
    crearCarpeta("Disco");
    char ruta_temporal[300];
    int temporal=0;
    int indiceBloque = 0;
    int espacioBloque = sectoresPorBloque * capSector;
    int contadorBloques = sectoresPorBloque;
    FILE* bloques = fopen("Bloques.txt", "w");
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
                    FILE* archivo = fopen(ruta_temporal, "w");
                    if(contadorBloques==sectoresPorBloque){
                        fprintf(archivo, "%i#0#0#%i\n",indiceBloque,espacioBloque,espacioBloque);
                        fprintf(bloques, "%i#0#0#%i\n",indiceBloque,espacioBloque,espacioBloque);
                        indiceBloque++;
                        contadorBloques=0;
                    }
                    contadorBloques++;
                    fclose(archivo);
                }
            }
        }
    }
    fclose(bloques);
    FILE* metadata = fopen("Disco\\Plato0\\Superficie0\\Pista0\\Sector0.txt", "a");
    if(metadata){
        fprintf(metadata,"%i#%i#%i#%i#%i#%i\n", platos, pistas, sectores, capSector, sectoresPorBloque, espacioTotal);
        fprintf(metadata, "1");
        for(int i=1;i<indiceBloque;i++){
            fprintf(metadata,"0");
        }
        fprintf(metadata, "\n");
        fclose(metadata);
    }
}

void Disco::recuperarDatosDisco() {
    FILE* metadata = fopen("Disco\\Plato0\\Superficie0\\Pista0\\Sector0.txt", "r");
    if (metadata) {
        int platos, pistas, sectores, capSector, sectoresPorBloque, espacioTotal;
        fscanf(metadata, "%d#%d#%d#%d#%d#%d#0", &platos, &pistas, &sectores, &capSector, &sectoresPorBloque, &espacioTotal);
        fclose(metadata);
        cout << "Datos del disco recuperados:\n";
        cout << "Platos: " << platos << "\n";
        cout << "Pistas: " << pistas << "\n";
        cout << "Sectores: " << sectores << "\n";
        cout << "Capacidad de cada sector: " << capSector << " Bytes\n";
        cout << "Sectores por bloque: " << sectoresPorBloque << "\n";
        cout << "Espacio total: " << espacioTotal << " Bytes\n";
        this->platos = platos;
        this->pistas = pistas;
        this->sectores = sectores;
        this->capSector = capSector;
        this->sectoresPorBloque = sectoresPorBloque;
        this->espacioTotal = espacioTotal;
    } else {
        cout << "No se pudo recuperar los datos del disco.\n";
    }
}

void Disco::borrarDisco() {
    const char* disco = "Disco";
    fs::remove_all(disco);
}

void Disco::mostrarArbol(const char* path, int nivel) {
    for (const auto& entry : fs::directory_iterator(path)) {
        for (int i = 0; i < nivel; ++i) cout << "|   ";
        if (fs::is_directory(entry.status())) {
            cout << "+-- " << entry.path().filename().string() << "/" <<endl;
            mostrarArbol(entry.path().string().c_str(), nivel + 1);
        } else {
            cout << "+-- " << entry.path().filename().string() <<endl;
        }
    }
}

void Disco::mostrarInfo() {
    cout << "Capacidad del disco: " << espacioTotal << " Bytes" << endl;
    cout << "Capacidad del bloque: " << sectoresPorBloque * capSector << " Bytes" << endl;
    cout << "Numero de bloques por pista: " << sectores / sectoresPorBloque << endl;
    cout << "Numero de bloques por plato: " << (pistas * sectores / sectoresPorBloque) * 2 << endl;
}

void Disco::escribirSector(vector<char> &datos, int* ruta) {
    char direccion[300];
    sprintf(direccion, "Disco\\Plato%d\\Superficie%d\\Pista%d\\Sector%d.txt", ruta[0], ruta[1], ruta[2], ruta[3]);
    FILE* archivo = fopen(direccion, "w");
    if (archivo) {
        for (char byte : datos) {
            fwrite(&byte, sizeof(char), 1, archivo);
        }
        fclose(archivo);
    } else {
        cout << "Error al abrir el archivo.\n";
    }
}

vector<char> Disco::leerSector(int* ruta) {
    char direccion[300];
    sprintf(direccion, "Disco\\Plato%d\\Superficie%d\\Pista%d\\Sector%d.txt", ruta[0], ruta[1], ruta[2], ruta[3]);
    FILE* archivo = fopen(direccion, "r");
    vector<char> datos;
    if (archivo) {
        char byte;
        while (fread(&byte, sizeof(char), 1, archivo)) {
            datos.push_back(byte);
        }
        return datos;
        fclose(archivo);
    } else {
        cout << "Error al abrir el archivo.\n";
    }
}

vector<char> Disco::leerBloque(vector<int*> rutasSectores) {
    vector<char> bloqueCompleto;

    for (int* ruta : rutasSectores) {
        vector<char> sectorDatos = leerSector(ruta);
        bloqueCompleto.insert(bloqueCompleto.end(), sectorDatos.begin(), sectorDatos.end());
    }

    return bloqueCompleto;
}