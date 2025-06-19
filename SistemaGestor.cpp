#include "SistemaGestor.h"
SistemaGestor::SistemaGestor(BufferManager* bf){
    buffer=bf;
    tamSector = buffer->tamSector;
    sectoresPorBloque = buffer->sectoresPorBloque;
}

vector<string> SistemaGestor::parseCSVLine(string& linea) {
    vector<string> resultado;
    string campo;
    bool dentroComillas = false;

    for (size_t i = 0; i < linea.size(); ++i) {
        char c = linea[i];
        if (c == '"') {
            dentroComillas = !dentroComillas; // Entrar/salir de comillas
        } else if (c == ',' && !dentroComillas) {
            resultado.push_back(campo);
            campo.clear();
        } else {
            campo += c;
        }
    }
    resultado.push_back(campo);
    return resultado;
}

int SistemaGestor::BloqueNDisponible(){ //devolvera el indice del siguiente bloque disponible y lo llenara con 1 indicando q esta ocupado
  int lba=0;
  bool encontrado=false;
  int indice=0; //posicion, LBA del bloque p
  int i=0;
  while(encontrado==false){
    i=0;
    Bloque& busq= buffer->obtenerBloque(0,'W', 0);
    while(busq.datos[i]!='\n') i++;
    i++;
    while(busq.datos[i]!='\n') i++;
    i++;
    while(busq.datos[i]!='0'){
      i++;
      indice++;
      if(i==busq.datos.size()){
        break; //se termino el bloque, hay que verificar si sigue el bitmap en otro bloque y si no es así entonces ya no hay bloques disponibles
      }
      if(busq.datos[i]=='0'){
        busq.datos[i]='1';
        cout<<"Indice:"<<indice<<endl;
        buffer->liberarBloque(lba,true);
        return indice;
      }
    }
    if(busq.idsiguiente==0){
      cout<<"YA NO HAY BLOQUES DISPONIBLES";
      return 0; //indicador, 0 es si no hay bloques disponibles
    }
    buffer->liberarBloqueSinEscribir(lba);
    lba=busq.idsiguiente;
  }
  cout<<"ERROR BLOQUENDISPONIBle"<<endl;   
  return -1;
}

bool SistemaGestor::verificarEsquemaExiste(string nombreEsquema, int* LBAesquema){
    int lba=0;
    Bloque& busq= buffer->obtenerBloque(0,'L', 0); //LBA, L, pin empezamos buscando desde bloque 0 porq capaz esta ahí quien sabe :)
    int i=0;
    while(i<busq.datos.size()){
        if(busq.datos[i]==nombreEsquema[0]){
            for(int j=1;j<nombreEsquema.size();j++){
                i++;
                if(busq.datos[i]!=nombreEsquema[j]){
                    break;
                }
                if(busq.datos[i+1]=='#' && (j+1)==nombreEsquema.size()){
                    i++;i++;
                    string aux="";
                    while(busq.datos[i]!='\n'){
                        aux=aux+busq.datos[i];
                        i++;
                    }
                    *LBAesquema = stoi(aux);
                    buffer->liberarBloqueSinEscribir(lba);
                    return true;
                }
            }
        }
        i++;
        if(i==busq.datos.size()){
            if(busq.idsiguiente==0){
                buffer->liberarBloqueSinEscribir(lba);
                return false;
            }else{
                buffer->liberarBloqueSinEscribir(lba);
                lba=busq.idsiguiente;
                busq= buffer->obtenerBloque(lba,'L', 0);
                i=0;
            }
        }
    }
    buffer->liberarBloqueSinEscribir(lba);
    return false;
}

void SistemaGestor::insertarEsquema(){
    string nombreEsquema;
    char variableFijo;
    int contadorAtributos = 0;
    cout << "Ingrese el nombre del esquema: ";
    cin >> nombreEsquema;
    int lbaesquema;
    if(verificarEsquemaExiste(nombreEsquema, &lbaesquema)==true){
        cout<<"NOMBRE DE ESQUEMA YA EXISTE, ERROR"<<endl;
        cout<<lbaesquema;
        return;
    }
    cout<<"Fijo o variable? (f/v): ";
    cin>> variableFijo;
    cout<<"Cuantos atributos tendra el esquema?";
    cin>> contadorAtributos;
    string ingreso=nombreEsquema+"#"+variableFijo+"#"; //nombre del esquema + tipo de variable
    string aux;
    if(variableFijo=='f'){
        int contTAMregistro=0;
        for(int i=0;i<contadorAtributos;i++){ //int,float,varchar
            cout<<"Ingrese nombre del atributo:";
            cin>>aux;
            ingreso = ingreso + aux + "#";
            cout<<"Ingrese tipo del atributo:";
            cin>>aux;
            ingreso = ingreso + aux + "#";
            cout<<"Ingrese cuantos caracteres(bytes) ocupa el atributo:"; //cantidad maxima de caracteres
            cin>>aux;
            ingreso = ingreso + aux + "#"; //fin de esquema
            contTAMregistro = contTAMregistro + stoi(aux); //sumamos el tamaño de los atributos
        }
        ingreso[ingreso.size()-1] = '\n'; //fin de esquema
        int indice = BloqueNDisponible();
        if(indice==0){
            cout<<"TODOS BLOQUES LLENOS, ERROR"<<endl;
            return;
        }else{
            string lineaMeta = nombreEsquema + "#" + to_string(indice) + "\n";
            
            int lba=0;
            Bloque& busq= buffer->obtenerBloque(lba,'L', 0);
            while(busq.idsiguiente!=0){ //VAMOS AL ID DEL ULTIMO BLOQUE DE LA METADATA por asi decirlo, donde deberiamos añadir el esquema
                buffer->liberarBloqueSinEscribir(lba);
                lba=busq.idsiguiente;
                busq= buffer->obtenerBloque(lba,'L', 0);
            }
            if((busq.espacio_disponible-(lineaMeta.size()))<0){ //ns +1
                lba= busq.idsiguiente;
            }
            buffer->liberarBloqueSinEscribir(lba);
            busq= buffer->obtenerBloque(lba,'W', 0);
            for(int i=0;i<lineaMeta.size();i++){
                busq.datos.push_back(lineaMeta[i]);
            }

            busq.putEspacio_disponible(busq.espacio_disponible-(lineaMeta.size()));
            buffer->liberarBloque(lba,true);
            lba=indice;
            Bloque& busq2= buffer->obtenerBloque(lba,'W', 0);
            for(int i=0;i<ingreso.size();i++){
                busq2.datos.push_back(ingreso[i]);
            }
            busq2.putEspacio_disponible(busq2.espacio_disponible-(ingreso.size()));
            buffer->liberarBloque(lba,true);

            //BITMAP
            contTAMregistro++; //para añadir /n para que se vea ordenado los registros
            cout<<contTAMregistro<<endl;
            Bloque& busq3= buffer->obtenerBloque(lba,'W', 0);
            int cantRegistros = busq3.espacio_disponible/contTAMregistro; //capacidad de registros por bloque
            cantRegistros = (busq3.espacio_disponible-cantRegistros-1)/contTAMregistro; //capacidad de registros por bloque, -1 por el @
            for(int i=0;i<cantRegistros;i++){
                busq3.datos.push_back('0'); //inicializamos el bitmap con 0s
            }
            busq3.datos.push_back('@');
            busq3.putEspacio_disponible(busq3.espacio_disponible-cantRegistros-1);
            
            buffer->liberarBloque(lba,true);
            
            return;
        }
        return;
    }else{
        //logica para variable, aun no implementada
        return;
    }
    return;
}

void SistemaGestor::insertarNRegistro(int n){ //cantidad de registros a insertar, nombre del archivo y nombre del esquema
    int CantdadRegistros = n; //cantidad de registros a insertar
    if(CantdadRegistros==0){
        CantdadRegistros=-1;
    }
    //Si n vale cero es que se insertara todo el archivo
    string nombreArchivo; //titanic.csv
    string nombreEsquema; //nombre del esquema
    cout<<"Ingrese nombre del archivo de donde importar archivos: ";
    cin>>nombreArchivo;
    cout<<"Ingrese nombre del esquema: ";
    cin>>nombreEsquema;
    int LBAesquema;
    if(verificarEsquemaExiste(nombreEsquema, &LBAesquema)==false){
        cout<<"ESQUEMA NO EXISTE, ERROR"<<endl;
        return;
    }
    Bloque& busq= buffer->obtenerBloque(LBAesquema,'W', 0);
    //-----------------------CONSEGUIMOS DATOS DEL ESQUEMA-----------------------
    char tipodeRegistro;
    int tamRegistroTotal=1; //tamaño total del registro
    vector<string> atributos; //nombre de los atributos
    vector<string> tipos; //tipo de los atributos 
    vector<int> bytesMax; //bytes maximos que ocupa cada atributo
    int i=0; //puntero para recorrer el bloque
    while(busq.datos[i]!='\n'){ //buscamos el nombre del esquema
        i++;
    }
    i++;
    while(busq.datos[i]!='#'){ //llegamos al f o v
        i++;
    }
    i++;
    tipodeRegistro = busq.datos[i]; //f o v
    i=i+2;
    string aux="";
    if(tipodeRegistro=='f'){ //si ingresaremos tamaño fijo
        /*
        while(busq.datos[i+1]!='#'){
            aux=aux+busq.datos[i];
            i++;
        }
        aux=""; //reiniciamos aux
        i=i+2;*/
        while(busq.datos[i]!='\n'){ //mientras no lleguemos al final del esquema
            while(busq.datos[i]!='#'){ //nombre del atributo
                aux=aux+busq.datos[i];
                i++;
            }
            atributos.push_back(aux); 
            aux=""; //reiniciamos aux
            i++; //saltamos el #
            while(busq.datos[i]!='#'){ //tipo del atributo
                aux=aux+busq.datos[i];
                i++;
            }
            tipos.push_back(aux);
            i++; //saltamos el #
            aux="";
            while(busq.datos[i]!='#' &&busq.datos[i]!='\n'){ //bytes maximos que ocupa el atributo
                aux=aux+busq.datos[i];
                i++;
            }
            bytesMax.push_back(stoi(aux));
            aux="";
            if(busq.datos[i]=='#'){
                i++; //saltamos el #
            }
        }
        for(int k=0;k<atributos.size();k++){
            cout<<"Atributo: "<<atributos[k]<<" Tipo: "<<tipos[k]<<" Bytes Max: "<<bytesMax[k]<<endl;
        }
        cout<<"datos  del esquema obtenidos"<<endl;
    //---------------------------------------------------------------------------
        i++; //saltamos el \n y llegamos al bitmap
        int indiceBitmap=i; //puntero para recorrer el bitmap

        while(busq.datos[i]!='@'){
            i++;
        } //posicion donde esta @ ahora es como el cero
        i++; //saltamos el @ y llegamos al final de las cabeceras, ahora tiene que empezar los registros
        int indicebase=i;
        tamRegistroTotal; //tamaño total del registro
        for(int k=0;k<atributos.size();k++){
            tamRegistroTotal = tamRegistroTotal + bytesMax[k]; //sumamos el tamaño de los atributos
        }
        bool TrabajoTerminado = false; //para saber si hemos terminado añadir los n registros;
        string linea;
        ifstream archivo(nombreArchivo); //abrimos el archivo
        getline(archivo,linea); //saltamos la primera linea porque ya hemos llenado con esquema (cabecera)
        cout<<"BAN1"<<endl;
        while(getline(archivo,linea) && TrabajoTerminado==false){ //mientras no lleguemos al final del archivo y no hayamos terminado de insertar los registros
            CantdadRegistros--;
            vector<string> campos = parseCSVLine(linea);
            int indiceLogicBitmap=0;
            cout<<"ANTESCONTindiceBitmap:"<<busq.datos[indiceBitmap-1]<<endl;
            bool insertado = false;
            while(insertado==false){
                cout<<"indiceBitmap:"<<indiceBitmap<<endl;
                cout<<"CONTindiceBitmap:"<<busq.datos[indiceBitmap]<<endl;
                if(busq.datos[indiceBitmap]=='@'){ //significa que el bloque esta lleno y no hay espacio para mas registros 
                    //hay que buscar otro bloque, actualizar el "bloque siguiente", actualizar espacio,reiniciar el indice, crear bitmap en el nuevo bloque
                    break;
                }

                if(busq.datos[indiceBitmap]=='0'){ //encontramos espacio para un registro disponible, usarlo y actualizar bitmap
                    cout<<"BAN3"<<endl;
                    busq.datos[indiceBitmap]='1'; //actualizamos el bitmap
                    int ind_auxi=indicebase+(tamRegistroTotal*indiceLogicBitmap); //posicion donde se guardara el registro
                    cout<<"ind_auxi:"<<ind_auxi<<endl;
                    cout<<busq.datos.size()<<endl;
                    if(ind_auxi>(busq.datos.size()-1)){ //se añade mediante push_back
                        for(int m=0;m<atributos.size();m++){
                            int auxiespacios=0;
                            for(int j=0;j<campos[m].size();j++){
                                busq.datos.push_back(campos[m][j]); //añadimos el campo al registro
                                cout<<campos[m][j];
                            }
                            auxiespacios=bytesMax[m]-campos[m].size(); //espacios que faltan para completar el campo
                            for(int j=0;j<auxiespacios;j++){
                                busq.datos.push_back(' '); //rellenamos con espacios
                                cout<<' ';
                            }
                        }
                        busq.datos.push_back('\n'); //separador de registros
                        cout<<'\n';
                        cout<<"REGISTRO AÑADIDO CON PUSH_BACK"<<endl;
                        insertado = true; //registro insertado
                    }else{//no se añade mediante push_back, se añade directamente en la posicion
                        for(int m=0;m<atributos.size();m++){
                            int auxiespacios=0;
                            for(int j=0;j<campos[m].size();j++){
                                busq.datos[ind_auxi]=campos[m][j]; //añadimos el campo al registro
                                ind_auxi++;
                            }
                            auxiespacios=bytesMax[m]-campos[m].size(); //espacios que faltan para completar el campo
                            for(int j=0;j<auxiespacios;j++){
                                busq.datos[ind_auxi]=' '; //rellenamos con espacios
                                ind_auxi++;
                            }
                        }
                        busq.datos[ind_auxi]='\n'; //separador de registros
                        insertado = true;
                    }
                    break;
                }
                indiceBitmap++;    
                indiceLogicBitmap++;
            }
            if(CantdadRegistros==0){ //si hemos insertado n registros, salimos del bucle
                TrabajoTerminado=true;
                break;
            }
        }
        //actualizar espacio del bloque
        //busq.putEspacio_disponible(buffer->+busq.espacio_disponible-(tamRegistroTotal*indiceLogicBitmap)); //actualizamos el espacio disponible del bloque
        cout<<"MENSAJE ESCRITO"<<endl;
        for(int k=0;k<busq.datos.size();k++){
            cout<<busq.datos[k];
        }
        buffer->liberarBloque(LBAesquema,true); //liberamos el bloque y lo escribimos
        return;
    }else{ //variable
        //logica de variable aun no implementada
        cout<<"ESQUEMA VARIABLE NO IMPLEMENTADO AUN, ERROR"<<endl;
        buffer->liberarBloqueSinEscribir(LBAesquema);
        return;
    }
    buffer->liberarBloque(LBAesquema,true);
    cout<<"ARCHIVO NO EXISTE, ERROR"<<endl;
    return;
}
void SistemaGestor::eliminarRegistro(int n, string nameEsquema){ //n es el registro a eliminar, nombre del esquema
    return;
}