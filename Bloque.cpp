#include "Bloque.h"
#include <iostream>
using namespace std;

void Bloque::ObtenerDatosCabecera(){ //Obtiene datos de la cabecera del bloque
    int i=0;
    string aux="";

    while(datos[i]!='#'){
        i++;
    }
    i++;
    aux=aux+datos[i];
    i++;
    disponible=stoi(aux);
    aux="";
    i++;
    while(datos[i]!='#'){
        if(datos[i]=='-'){
            while(datos[i]!='#'){
                i++;
            }
            break;
        }
        aux=aux+datos[i];
        i++;
    }
    i++;
    if(aux==""){idsiguiente=0;}else{idsiguiente=stoi(aux);}
    aux="";
    while(datos[i]!='\n'){
        if(datos[i]=='-'){
            while(datos[i]!='\n'){
                i++;
            }
            break;
        }
        aux=aux+datos[i];
        i++;
    }
    espacio_disponible=stoi(aux);
    cout<<id<<"-"<<disponible<<"-"<<idsiguiente<<"-"<<espacio_disponible<<endl;
    /*
    while(datos[i]!='\n'){
        while(datos[i]!='#'){
            if(datos[i]=='-'||datos[i]=='\n'){
                if(cond==1){
                    cout<<"bb1"<<endl;
                    disponible=stoi(aux);
                }
                if(cond==2){
                    if(aux==""){
                        idsiguiente=0;
                    }else{
                        cout<<"bb2"<<endl;
                        idsiguiente=stoi(aux);
                    }
                }
                if(cond==3){
                    cout<<"bb3"<<endl;
                    cout<<aux;
                    espacio_disponible=stoi(aux);
                }
                break;
            }
            aux=aux+datos[i];
            cout<<aux<<endl;
            i++;
        }
        cond++;
        aux="";
        i++;
    }
    */
}

Bloque::Bloque(int id2, vector<char> dates){
    id=id2;
    datos=dates;
    ObtenerDatosCabecera();
}

void Bloque::putDisponible(int n){ //modifica el campo de Disponibilidad(0/1) de la cabecera del bloque
    disponible=n;
    int i=0;
    string dispo=to_string(n);
    int delimitadores = 0;
    while (delimitadores < 1 && datos[i] != '\n') {
        if (datos[i] == '#') {
            delimitadores++;
        }
        i++;
    }
    for(int j=0;j<dispo.size();j++){
        datos[i]=dispo[j];
        i++;
    }
    while(datos[i]!='#'){
        datos[i]='-';
        i++;
    }
}

void Bloque::putIdSiguiente(int n){ //modifica el campo de isSiguiente de la cabecera del bloque
    idsiguiente=n;
    int i=0;
    string idSIG=to_string(n);
    int delimitadores = 0;
    while (delimitadores < 2 && datos[i] != '\n') {
        if (datos[i] == '#') {
            delimitadores++;
        }
        i++;
    }
    for(int j=0;j<idSIG.size();j++){
        datos[i]=idSIG[j];
        i++;
    }
    while(datos[i]!='#'){
        datos[i]='-';
        i++;
    }
}

void Bloque::putEspacio_disponible(int n){ //modifica el campo de espacio disponible de la cabecera del bloque
    espacio_disponible=n;
    int i=0;
    string esp_disponible=to_string(espacio_disponible);
    int delimitadores = 0;
    while (delimitadores < 3 && datos[i] != '\n') {
        if (datos[i] == '#') {
            delimitadores++;
        }
        i++;
    }
    for(int j=0;j<esp_disponible.size();j++){
        datos[i]=esp_disponible[j];
        i++;
    }
    while(datos[i]!='\n'){
        datos[i]='-';
        i++;
    }
}
