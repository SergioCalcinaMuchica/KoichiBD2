#pragma once
#include <vector>
#include <string>
#include <iostream>
#include "BufferManager.h" // Para usar el buffer
using namespace std;

class SistemaGestor {
public:
    BufferManager* buffer;
    int tamSector; //para las restricciones maybe 
    int sectoresPorBloque; //para las restricciones maybe
    SistemaGestor(BufferManager* b); // constructor

    // Funciones de administración
    int BloqueNDisponible();
    bool verificarEsquemaExiste(string nombreEsquema, int* LBAesquema);
    void insertarEsquema();
    int cargarEsquema(string nombreEsquema); // Busca el esquema y va a la direccion del bloque donde empieza dicho esquema, devuelve el LBA
    void insertarNRegistro(int n, string nameArchivo, string nameEsquema);
    void eliminarRegistro(int n, string nameEsquema);
    void seleccionarRegistros(string nombreEsquema); // SELECT *
    // void seleccionarWhere(string columna, string valor); a futuro
};
