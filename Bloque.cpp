#include "Bloque.h"
using namespace std;


Bloque::Bloque(int id2){
    id=id2;
}

void Bloque::ObtenerDatosCabecera(){
    int i=0;
    string aux="";
    int cond=0;
    while(datos[i]!='\n'){
        while(datos[i]!='#'){
            if(datos[i]=='-'||datos[i]=='\n'){
                if(cond==1){
                    disponible=stoi(aux);
                }
                if(cond==2){
                    if(aux==""){
                        idsiguiente=0;
                    }else{
                        idsiguiente=stoi(aux);
                    }
                }
                if(cond==3){
                    espacio_disponible=stoi(aux);
                }
                break;
            }
            aux=aux+datos[i];
            i++;
        }
        cond++;
        aux="";
        i++;
    }
}

void Bloque::putDisponible(int n){
    disponible=n;
}

void Bloque::putIdSiguiente(int n){
    idsiguiente=n;
}

void Bloque::putEspacio_disponible(int n){
    espacio_disponible=n;
}
