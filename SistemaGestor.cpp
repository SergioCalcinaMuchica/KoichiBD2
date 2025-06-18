#include "SistemaGestor.h"
SistemaGestor::SistemaGestor(BufferManager* bf){
    buffer=bf;
    tamSector = buffer->tamSector;
    sectoresPorBloque = buffer->sectoresPorBloque;
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
    cout<<"inicio verificacion"<<endl;
    int lba=0;
    Bloque& busq= buffer->obtenerBloque(0,'L', 0); //LBA, L, pin empezamos buscando desde bloque 0 porq capaz esta ahí quien sabe :)
    int i=0;
    cout<<"b1"<<endl;
    while(i<busq.datos.size()){
        if(busq.datos[i]==nombreEsquema[0]){
            for(int j=1;j<nombreEsquema.size();j++){
                i++;
                cout<<busq.datos[i]<<endl;
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
                    cout<<"aux:"<<aux;
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
        for(int i=0;i<contadorAtributos;i++){ //int,float,varchar
            cout<<"Ingrese nombre del atributo:";
            cin>>aux;
            ingreso = ingreso + aux + "#";
            cout<<"Ingrese tipo del atributo:";
            cin>>aux;
            ingreso = ingreso + aux + "#";
            cout<<"Ingrese cuantos caracteres(bytes) ocupa el atributo:"; //cantidad maxima de caracteres
            cin>>aux;
            ingreso = ingreso + aux + "@"; //fin de esquema
        }
        //int indice = BloqueNDisponible(); CAMBIAR
        int indice=1;
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
            cout<<"BAN1"<<endl;
            if((busq.espacio_disponible-(lineaMeta.size()))<0){ //+1
                lba= busq.idsiguiente;
            }
            buffer->liberarBloqueSinEscribir(lba);
            busq= buffer->obtenerBloque(lba,'W', 0);
            for(int i=0;i<lineaMeta.size();i++){
                busq.datos.push_back(lineaMeta[i]);
            }
            cout<<"BAN2"<<endl;
            cout<<"Espacio:"<<busq.espacio_disponible<<endl;
            cout<<busq.espacio_disponible-lineaMeta.size()<<endl;
            busq.putEspacio_disponible(busq.espacio_disponible-(lineaMeta.size()));
            buffer->liberarBloque(lba,true);

            //ingresar esquema de la relacion
            cout<<"BAN3"<<endl;
            lba=indice;
            cout<<indice;
            busq= buffer->obtenerBloque(lba,'W', 0);
            for(int i=0;i<ingreso.size();i++){
                busq.datos.push_back(ingreso[i]);
            }
            cout<<busq.espacio_disponible-ingreso.size()<<endl;
            busq.putEspacio_disponible(busq.espacio_disponible-(ingreso.size()));
            for(int i=0;i<busq.datos.size();i++){
                cout<<busq.datos[i];
            }
            cout<<endl;
            buffer->liberarBloque(lba,true);
            cout<<"er"<<endl;
            return;
        }
        return;
    }else{
        //logica para variable, aun no implementada
        return;
    }
    return;
}

int SistemaGestor::cargarEsquema(string nombreEsquema){

} // Busca el esquema y va a la direccion del bloque donde empieza dicho esquema, devuelve el LBA

void SistemaGestor::insertarNRegistro(int n, string nameArchivo, string nameEsquema){

}
