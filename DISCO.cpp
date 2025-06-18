#include "DISCO.h"
#include "MicroControlador.h"
#include <iostream>
#include <cstdio>
#include <fstream>
#include <vector>
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
    int indiceBloque = 0;
    int espacioBloque = sectoresPorBloque * capSector; 
    int cantdBloques = (pistas * sectores * platos * 2) / sectoresPorBloque;
    cantdBloques = contarDigitos(cantdBloques);
    int tamcabeceraBloque = cantdBloques + 3+ cantdBloques+1+contarDigitos(espacioBloque);
    int contadorB=sectoresPorBloque;
    
    long posEspacioDisponibleBloque;
    long posidSiguiente;
    long posDisponible;
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
                    FILE* archivo = fopen(ruta_temporal, "wb");
                    if(contadorB==sectoresPorBloque){
                        fprintf(archivo,"%i", indiceBloque); //indice bloque
                        int aux= cantdBloques - contarDigitos(indiceBloque);
                        for(int i=0; i<aux; i++){
                            fprintf(archivo,"-");
                        }
                        posDisponible=ftell(archivo)+1;
                        fprintf(archivo,"#0#");//indicador si esta ocupado o no
                        posidSiguiente=ftell(archivo);
                        for(int i=0; i<cantdBloques; i++){ //bloque siguiente
                            fprintf(archivo,"-");
                        }
                        posEspacioDisponibleBloque=ftell(archivo)+1;
                        fprintf(archivo, "#%i",espacioBloque-tamcabeceraBloque);
                        aux=contarDigitos(espacioBloque)-contarDigitos(espacioBloque-tamcabeceraBloque);
                        for(int i=0;i<aux;i++){
                            fprintf(archivo,"-");
                        }
                        fprintf(archivo,"\n");
                        indiceBloque++;
                        contadorB=0;
                    }
                    contadorB++;
                    fclose(archivo);
                }
            }
        }
    }
    
    FILE* metadata = fopen("Disco\\Plato0\\Superficie0\\Pista0\\Sector0.txt", "ab");
    if(metadata){
        espacioTotal= espacioTotal-(indiceBloque*tamcabeceraBloque)-6-contarDigitos(platos)-contarDigitos(pistas)-contarDigitos(sectores)-contarDigitos(capSector)-contarDigitos(sectoresPorBloque);
        espacioTotal=espacioTotal - contarDigitos(espacioTotal)-indiceBloque;
        fprintf(metadata,"%i#%i#%i#%i#%i#%i\n", platos, pistas, sectores, capSector, sectoresPorBloque, espacioTotal);//espacio disponible
        long posBit2=ftell(metadata);
        cout<<"pos"<<posBit2<<endl;
        fclose(metadata);
        metadata = fopen("Disco\\Plato0\\Superficie0\\Pista0\\Sector0.txt", "rb"); // Modo binario
        long bytesOcupados;
        //saber cuantos bytes ocupados van
        if (metadata) {
            fseek(metadata, 0, SEEK_END);          // Moverse al final
            bytesOcupados=ftell(metadata); 
            fclose(metadata);
        }

        int LBA=0;
        bool unsoloBloque=true;
        char direccion[300];
        MicroControlador pre = MicroControlador(this);
        long capcdbloque=capSector*sectoresPorBloque;
        long acumBloque=bytesOcupados;
        int contadorBloque=1;
        pre.ObtenerRuta(0);
        sprintf(direccion, "Disco\\Plato%d\\Superficie%d\\Pista%d\\Sector%d.txt", pre.ruta[0], pre.ruta[1], pre.ruta[2], pre.ruta[3]);
        metadata = fopen(direccion, "ab");
        //llenado de bitmap, metadata sobre bloques disponibles o no
        int espacioOcupadoASumarAlDisco = 0; //bytes que van s
        int LBAmin = 0;
        for(int i=0;i<indiceBloque;i++){
            bytesOcupados++;
            acumBloque++;
            if(bytesOcupados>capSector){
                fclose(metadata);
                LBA++;
                if(LBA%sectoresPorBloque==0){ //cambio de bloque aqui hacer los cambios en los campos correspondientes
                    contadorBloque++;
                    unsoloBloque=false;

                    if(LBA-sectoresPorBloque==LBAmin){
                        LBAmin=LBA;
                    }

                    pre.ObtenerRuta(LBA-sectoresPorBloque);
                    sprintf(direccion, "Disco\\Plato%d\\Superficie%d\\Pista%d\\Sector%d.txt", pre.ruta[0], pre.ruta[1], pre.ruta[2], pre.ruta[3]);
                    metadata = fopen(direccion, "r+b");
                    fseek(metadata,posDisponible,SEEK_SET);
                    fputc('1', metadata);
                    fseek(metadata,posEspacioDisponibleBloque,SEEK_SET);
                    string espaux= to_string(capcdbloque - (acumBloque-1));
                    int espaxiosaux=contarDigitos(capcdbloque)-contarDigitos(capcdbloque - (acumBloque-1));
                    for(int i=0;i<espaux.size();i++){
                        fputc(espaux[i],metadata);
                    }
                    for(int i=0;i<espaxiosaux;i++){
                        fputc('-',metadata);
                    }
                    fseek(metadata,posidSiguiente,SEEK_SET);
                    espaux=to_string(contadorBloque-1);
                    for(int i=0;i<espaux.size();i++){
                        fputc(espaux[i],metadata);
                    }
                    fclose(metadata);
                    pre.ObtenerRuta(LBA);
                    sprintf(direccion, "Disco\\Plato%d\\Superficie%d\\Pista%d\\Sector%d.txt", pre.ruta[0], pre.ruta[1], pre.ruta[2], pre.ruta[3]);
                    metadata = fopen(direccion, "r+b");
                    fseek(metadata,posDisponible,SEEK_SET);
                    fputc('1', metadata);
                    fclose(metadata);
                }

                pre.ObtenerRuta(LBA);
                sprintf(direccion, "Disco\\Plato%d\\Superficie%d\\Pista%d\\Sector%d.txt", pre.ruta[0], pre.ruta[1], pre.ruta[2], pre.ruta[3]);
                metadata = fopen(direccion, "rb");
                if (metadata) {
                    fseek(metadata, 0, SEEK_END);          // Moverse al final
                    bytesOcupados=ftell(metadata);
                    if(LBA%sectoresPorBloque==0){
                        acumBloque=bytesOcupados;
                    }
                    fclose(metadata);
                }
                
                metadata = fopen(direccion, "ab");//cambio de sector logico
                fprintf(metadata,"0");
                bytesOcupados++;
                if(LBA%sectoresPorBloque==0){
                    acumBloque++;
                }
            }else{
                fprintf(metadata,"0");
            }
        }
        fclose(metadata);
        if(unsoloBloque==true){
                pre.ObtenerRuta(0);
                sprintf(direccion, "Disco\\Plato%d\\Superficie%d\\Pista%d\\Sector%d.txt", pre.ruta[0], pre.ruta[1], pre.ruta[2], pre.ruta[3]);
                metadata = fopen(direccion, "r+b");
                fseek(metadata,posDisponible,SEEK_SET);
                fputc('1', metadata);
                fseek(metadata,posEspacioDisponibleBloque,SEEK_SET);
                string espaux= to_string(capcdbloque - acumBloque);
                int espaxiosaux=contarDigitos(capcdbloque)-contarDigitos(capcdbloque - acumBloque);
                for(int i=0;i<espaux.size();i++){
                    fputc(espaux[i],metadata);
                }
                for(int i=0;i<espaxiosaux;i++){
                    fputc('-',metadata);
                }
        }
        fclose(metadata);
        //LBAmin
        pre.ObtenerRuta(LBAmin);
        sprintf(direccion, "Disco\\Plato%d\\Superficie%d\\Pista%d\\Sector%d.txt", pre.ruta[0], pre.ruta[1], pre.ruta[2], pre.ruta[3]);
        metadata = fopen(direccion, "r+b");
        fseek(metadata,posDisponible,SEEK_SET);
        fputc('1', metadata);
        fseek(metadata,posEspacioDisponibleBloque,SEEK_SET);
        string espaux= to_string(capcdbloque - acumBloque);
        int espaxiosaux=contarDigitos(capcdbloque)-contarDigitos(capcdbloque - acumBloque);
        for(int i=0;i<espaux.size();i++){
            fputc(espaux[i],metadata);
        }
        for(int i=0;i<espaxiosaux;i++){
            fputc('-',metadata);
        }
        
        fclose(metadata);

        int contZ=posBit2; //va sumando la cantidad de unos que se estan agregando para veridicar que no sobrepase le tamaño de un sector
        int c;
        LBA=0;
        metadata = fopen("Disco\\Plato0\\Superficie0\\Pista0\\Sector0.txt", "r+b");
        fseek(metadata,posBit2,SEEK_SET);
        while(contadorBloque!=0){ //provisional pero deberia mejorar, imaginate que toque poner 1 a bits que esten a otro bloque. PENDIENTE
            if(contZ%espacioBloque==0){
                fclose(metadata);
                LBA++;
                pre.ObtenerRuta(LBA);
                sprintf(direccion, "Disco\\Plato%d\\Superficie%d\\Pista%d\\Sector%d.txt", pre.ruta[0], pre.ruta[1], pre.ruta[2], pre.ruta[3]);
                metadata = fopen(direccion, "r+b");
                int auxindice=0;
                while((c = getc(metadata))!='\n'){
                    auxindice++;
                    contZ++;
                }
                contZ++;
                fseek(metadata, 1, SEEK_CUR);
                fseek(metadata, -1, SEEK_CUR);
            }else if(contZ%capSector==0){
                fclose(metadata);
                LBA++;
                pre.ObtenerRuta(LBA);
                sprintf(direccion, "Disco\\Plato%d\\Superficie%d\\Pista%d\\Sector%d.txt", pre.ruta[0], pre.ruta[1], pre.ruta[2], pre.ruta[3]);
                metadata = fopen(direccion, "r+b");
            }
            fputc('1', metadata);
            contZ++;
            contadorBloque--;
        }
        fclose(metadata);
    }
}

void Disco::recuperarDatosDisco() {
    FILE* metadata = fopen("Disco\\Plato0\\Superficie0\\Pista0\\Sector0.txt", "rb");
    if (metadata) {
        char buffer[256];
        fgets(buffer, sizeof(buffer), metadata); // Leer la primera línea
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
    cout << "Cantidad de pistas: " <<this->pistas << endl;
    cout << "Cantidad de platos: "<< this->platos << endl;
    cout << "Cantidad de sectores: " << this->sectores << endl;
}

void Disco::escribirSector(vector<char> &datos, int* ruta) {
    char direccion[300];
    sprintf(direccion, "Disco\\Plato%d\\Superficie%d\\Pista%d\\Sector%d.txt", ruta[0], ruta[1], ruta[2], ruta[3]);
    FILE* archivo = fopen(direccion, "wb");
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
    FILE* archivo = fopen(direccion, "rb");
    vector<char> datos;
    if (archivo) {
        char byte;
        while (fread(&byte, sizeof(char), 1, archivo)) {
            datos.push_back(byte);
        }
        fclose(archivo);
        return datos;
    } else {
        cout << "Error al abrir el archivo.\n";
        return datos; //sector corrupto o no encontrado
    }
}

/*
vector<char> Disco::leerBloque(vector<int*> rutasSectores) {
    vector<char> bloqueCompleto;

    for (int* ruta : rutasSectores) {
        vector<char> sectorDatos = leerSector(ruta);
        bloqueCompleto.insert(bloqueCompleto.end(), sectorDatos.begin(), sectorDatos.end());
    }

    return bloqueCompleto;
}

void Disco::escribirBloque(vector<int*> rutasSectores, vector<char>& bloqueCompleto) {
    int contador = 0;
    for (int* ruta : rutasSectores) {
        vector<char> sectorDatos;
        for (int i = 0; i < capSector && contador < bloqueCompleto.size(); i++, contador++) {
            sectorDatos.push_back(bloqueCompleto[contador]);
        }
        escribirSector(sectorDatos, ruta);
    }
}
*/