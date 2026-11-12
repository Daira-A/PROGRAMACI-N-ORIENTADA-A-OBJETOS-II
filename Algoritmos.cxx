#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <string>
using namespace std;
struct Estudiante{
    string nombre;
    int anio;
    double gpa;
    double presupuesto;
};
double indiceUniversidad(const Estudiante& e){
    return fabs(e.gpa-3)*e.presupuesto;
}
bool porAnio(const Estudiante& a,const Estudiante& b){
    return a.anio<b.anio;
}
bool porGpa(const Estudiante& a,const Estudiante& b){
    return a.gpa<b.gpa;
}
bool porPresupuesto(const Estudiante& a,const Estudiante& b){
    return a.presupuesto<b.presupuesto;
}
bool porIndice(const Estudiante& a,const Estudiante& b){
    return indiceUniversidad(a)<indiceUniversidad(b);
}

void imprimir(const vector<Estudiante>& v){
    if(v.empty()){
        cout<<"(vector vacio)"<<endl;
        return;
    }
    for(int i=0;i<(int)v.size();i++){
        cout<<i<<") "<<v[i].nombre<<" - "<<v[i].anio<<" - GPA: "
             <<v[i].gpa<<" - Presupuesto: "<<v[i].presupuesto
             <<" - Indice: "<<indiceUniversidad(v[i])<<endl;
    }
}

bool insertar(vector<Estudiante>& v,bool alFinal,int indice,Estudiante e){
    if(alFinal){
        v.push_back(e);
        return true;
    }
    if(indice<0||indice>(int)v.size()){
        return false;
    }
    v.insert(v.begin()+indice,e);
    return true;
}

bool eliminar(vector<Estudiante>& v,bool ultimo,int indice){
    if(v.empty()){
        return false;
    }
    if(ultimo){
        v.pop_back();
        return true;
    }
    if(indice<0||indice>=(int)v.size()){
        return false;
    }
    v.erase(v.begin()+indice);
    return true;
}

bool actualizar(vector<Estudiante>& v,int indice,Estudiante e){
    if(indice<0||indice>=(int)v.size()){
        return false;
    }
    v[indice]=e;
    return true;
}

void ordenar(vector<Estudiante>& v,int criterio){
    if(criterio==1) stable_sort(v.begin(),v.end(),porAnio);
    else if(criterio==2) stable_sort(v.begin(),v.end(),porGpa);
    else if(criterio==3) stable_sort(v.begin(),v.end(),porPresupuesto);
    else if(criterio==4) stable_sort(v.begin(),v.end(),porIndice);
}

bool estaOrdenado(const vector<Estudiante>& v,int criterio){
    if(criterio==1) return is_sorted(v.begin(),v.end(),porAnio);
    if(criterio==2) return is_sorted(v.begin(),v.end(),porGpa);
    if(criterio==3) return is_sorted(v.begin(),v.end(),porPresupuesto);
    return is_sorted(v.begin(),v.end(),porIndice);
}

Estudiante leerEstudiante(){
    Estudiante e;
    cin.ignore();
    cout<<"Nombre: ";
    getline(cin,e.nombre);
    cout<<"Anio de nacimiento: ";
    cin>>e.anio;
    cout<<"GPA: ";
    cin>>e.gpa;
    cout<<"Presupuesto: ";
    cin>>e.presupuesto;
    return e;
}

Estudiante estudiantePorDefecto(){
    Estudiante e={"NN",0,0,0};
    return e;
}

vector<Estudiante>datosPrueba(){
    vector<Estudiante>v;
    v.push_back({"Ana",2003,3.8,500});
    v.push_back({"Luis",2001,2.5,900});
    v.push_back({"Rosa",2002,3.0,300});
    v.push_back({"Juan",2000,3.5,100});
    return v;
}
void pruebas(){
    vector<Estudiante>v;
    cout<<"\nPRUEBA 1: Insertar sin datos al final"<<endl;
    cout<<"Entrada: vector vacio, insertar por defecto al final"<<endl;
    cout<<"Salida esperada: 0) NN - 0 - GPA: 0 - Presupuesto: 0"<<endl;
    insertar(v,true,0,estudiantePorDefecto());
    imprimir(v);

    cout<<"\nPRUEBA 2: Insertar con datos en indice 0"<<endl;
    cout<<"Entrada: insertar Ana, 2003, 3.8, 500 en indice 0"<<endl;
    cout<<"Salida esperada: Ana en la posicion 0 y NN en la posicion 1"<<endl;
    insertar(v,false,0,{"Ana",2003,3.8,500});
    imprimir(v);

    cout<<"\nPRUEBA 3: Eliminar el ultimo"<<endl;
    cout<<"Entrada: eliminar el ultimo del vector anterior"<<endl;
    cout<<"Salida esperada: queda solo Ana"<<endl;
    eliminar(v,true,0);
    imprimir(v);

    cout<<"\nPRUEBA 4: Eliminar con indice"<<endl;
    v=datosPrueba();
    cout<<"Entrada: vector con Ana, Luis, Rosa, Juan; eliminar indice 1"<<endl;
    cout<<"Salida esperada: quedan Ana, Rosa, Juan"<<endl;
    eliminar(v,false,1);
    imprimir(v);

    cout<<"\nPRUEBA 5: Eliminar con vector vacio"<<endl;
    v.clear();
    cout<<"Entrada: vector vacio, eliminar el ultimo"<<endl;
    cout<<"Salida esperada: no se elimina nada, sigue vacio"<<endl;
    if(!eliminar(v,true,0)){
        cout<<"No se puede eliminar: el vector esta vacio"<<endl;
    }
    imprimir(v);

    cout<<"\nPRUEBA 6: Actualizar"<<endl;
    v=datosPrueba();
    cout<<"Entrada: actualizar indice 2 (Rosa) a Rosa, 2002, 3.9, 350"<<endl;
    cout<<"Salida esperada: posicion 2 con GPA 3.9 y presupuesto 350"<<endl;
    actualizar(v,2,{"Rosa",2002,3.9,350});
    imprimir(v);

    cout<<"\nPRUEBA 7: Ordenar por anio de nacimiento"<<endl;
    v=datosPrueba();
    cout<<"Salida esperada: Juan(2000), Luis(2001), Rosa(2002), Ana(2003)"<<endl;
    ordenar(v,1);
    imprimir(v);
    cout<<"Verificacion con is_sorted: "<<(estaOrdenado(v,1)?"ordenado":"NO ordenado")<<endl;

    cout<<"\nPRUEBA 8: Ordenar por GPA"<<endl;
    v=datosPrueba();
    cout<<"Salida esperada: Luis(2.5), Rosa(3.0), Juan(3.5), Ana(3.8)"<<endl;
    ordenar(v,2);
    imprimir(v);
    cout<<"Verificacion con is_sorted: "<<(estaOrdenado(v,2)?"ordenado":"NO ordenado")<<endl;

    cout<<"\nPRUEBA 9: Ordenar por presupuesto"<<endl;
    v=datosPrueba();
    cout<<"Salida esperada: Juan(100), Rosa(300), Ana(500), Luis(900)"<<endl;
    ordenar(v,3);
    imprimir(v);
    cout<<"Verificacion con is_sorted: "<<(estaOrdenado(v,3)?"ordenado":"NO ordenado")<<endl;

    cout<<"\nPRUEBA 10: Ordenar por indice universidad"<<endl;
    v=datosPrueba();
    cout<<"Salida esperada: Rosa(0), Juan(50), Ana(400), Luis(450)"<<endl;
    ordenar(v,4);
    imprimir(v);
    cout<<"Verificacion con is_sorted: "<<(estaOrdenado(v,4)?"ordenado":"NO ordenado")<<endl;
}

int main(){
    vector<Estudiante>estudiantes;
    int opcion;

    do{
        cout<<"\nMENU ESTUDIANTES"<<endl;
        cout<<"1. Insertar estudiante"<<endl;
        cout<<"2. Eliminar estudiante"<<endl;
        cout<<"3. Actualizar estudiante"<<endl;
        cout<<"4. Ordenar"<<endl;
        cout<<"5. Imprimir"<<endl;
        cout<<"6. Ejecutar pruebas"<<endl;
        cout<<"0. Salir"<<endl;
        cout<<"Opcion: ";
        cin>>opcion;
        if(opcion==1){
            int donde,tipo,indice=0;
            cout<<"1. Al final  2. En un indice: ";
            cin>>donde;
            if(donde==2){
                cout<<"Indice: ";
                cin>>indice;
            }
            cout<<"1. Con datos  2. Por defecto (NN,0,0,0): ";
            cin>>tipo;
            Estudiante e=estudiantePorDefecto();
            if(tipo==1){
                e=leerEstudiante();
            }
            if(insertar(estudiantes,donde==1,indice,e)){
                cout<<"Estudiante insertado"<<endl;
            }else{
                cout<<"Indice no valido"<<endl;
            }
        }
        else if(opcion==2){
            if(estudiantes.empty()){
                cout<<"El vector esta vacio, no se puede eliminar"<<endl;
            }else{
                int donde,indice=0;
                cout<<"1. El ultimo  2. Desde un indice: ";
                cin>>donde;
                if(donde==2){
                    cout<<"Indice: ";
                    cin>>indice;
                }
                if(eliminar(estudiantes,donde==1,indice)){
                    cout<<"Estudiante eliminado"<<endl;
                }else{
                    cout<<"Indice no valido"<<endl;
                }
            }
        }
        else if(opcion==3){
            int indice;
            cout<<"Indice a actualizar: ";
            cin>>indice;
            if(indice>=0&&indice<(int)estudiantes.size()){
                Estudiante e=leerEstudiante();
                actualizar(estudiantes,indice,e);
                cout<<"Estudiante actualizado"<<endl;
            }else{
                cout<<"Indice no valido"<<endl;
            }
        }
        else if(opcion==4){
            int criterio;
            cout<<"1. Por anio  2. Por GPA  3. Por presupuesto  4. Por indice universidad: ";
            cin>>criterio;
            ordenar(estudiantes,criterio);
            imprimir(estudiantes);
        }
        else if(opcion==5){
            imprimir(estudiantes);
        }
        else if(opcion==6){
            pruebas();
        }
    }while(opcion!=0);
    return 0;
}
