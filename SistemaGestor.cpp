#include "SistemaGestor.h"
SistemaGestor::SistemaGestor(BufferManager* bf){
    buffer=bf;
    tamSector = buffer->tamSector;
    sectoresPorBloque = buffer->sectoresPorBloque;
}

int BloqueNDisponible(){ //devolvera el indice del siguiente bloque disponible
  int lba=0;
  bool encontrado=false;
  int indice=0; //posicion, LBA del bloque p
  int i=0;
  while(encontrado==false){
    i=0;
    Bloque& busq = buffer->obtenerBloque(lba,'L',0);
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
      if(busq.datos[i]==1){
        return indice;
      }
    }
    if(busq.idsiguiente==0){
      cout<<"YA NO HAY BLOQUES DISPONIBLES";
      return 0; //indicador, 0 es si no hay bloques disponibles
    }
    busq.liberarBloqueSinEscribir(lba);
    lba=busq.idsiguiente;
  }
}

//contador del bitmap para saber q bloque esta disponible funcion que devolveria 
string SistemaGestor::extraerBitmap(vector<char>& datos){ //
    int indice = 0;
    // Saltar la primera línea (cabecera)
    while (indice < datos.size() && datos[indice] != '\n') indice++;
    indice++;
    // Saltar la segunda línea (cabecera)
    while (indice < datos.size() && datos[indice] != '\n') indice++;
    indice++;
    // Extraer el bitmap hasta el siguiente salto de línea
    string bitmap;
    while (indice < datos.size() && datos[indice] != '\n') {
        bitmap += datos[indice];
        indice++;
    }
    return bitmap;
}

void SistemaGestor::actualizarBitmapEnDatos(vector<char>& datos, const string& nuevoBitmap){
    int indice = 0;
    // Saltar la primera línea (cabecera)
    while (indice < datos.size() && datos[indice] != '\n') indice++;
    indice++;
    // Saltar la segunda línea (cabecera)
    while (indice < datos.size() && datos[indice] != '\n') indice++;
    indice++;
    // Ahora 'indice' apunta al inicio del bitmap
    for (size_t i = 0; i < nuevoBitmap.size() && (indice + i) < datos.size(); ++i) {
        datos[indice + i] = nuevoBitmap[i];
    }
}

bool SistemaGestor::verificarEsquemaExiste(string nombreEsquema, int* LBAesquema){
    int lba=0;
    Bloque& busq= buffer->obtenerBloque(0,'L', 0); //LBA, L, pin empezamos buscando desde bloque 0 porq capaz esta ahí quien sabe :)
    int i=0;
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
        //revisar si hay bloque disponible en base al bitmap
        buffer->cargarBloque(0); //cargar bloque 0 que es el bitmap
        vector<char>& datos = buffer->Bloques[0].datos;
        string bitmapt= extraerBitmap(datos);
        if(bitmapt.find('0') == string::npos){
            cout<<"TODOS BLOQUES LLENOS"<<endl;
            return;
        }else{
            int indice=bitmapt.find('0');
            bitmapt[indice]='1';
            actualizarBitmapEnDatos(datos,bitmapt);
            string lineaMeta = nombreEsquema + "#" + to_string(indice) + "\n";
            if(datos.size())
            datos.insert(datos.end(), lineaMeta.begin(), lineaMeta.end());
            buffer->escribirBloque(0);
        }
    }else{
        //logica para variable, aun no implementada
        return;
    }
}

int SistemaGestor::cargarEsquema(string nombreEsquema){

} // Busca el esquema y va a la direccion del bloque donde empieza dicho esquema, devuelve el LBA

void SistemaGestor::insertarNRegistro(int n, string nameArchivo, string nameEsquema){

}
