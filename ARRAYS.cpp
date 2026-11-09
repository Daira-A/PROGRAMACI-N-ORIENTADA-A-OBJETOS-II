#include <iostream>
#include <array>
#include <string>
#include <algorithm>
#include <fstream>
using namespace std;

struct Estudiante{
    string nombre;
    int anio;
    double gpa;
};
const int ANIO_ACTUAL=2026;
void imprimirEstudiante(const Estudiante& e){
    int edad=ANIO_ACTUAL-e.anio;
    cout<<e.nombre<<" - nacimiento: "<<e.anio<<" - edad: "<<edad<<" - GPA: "<<e.gpa
    <<" - beca: "<<(e.gpa>=3.5?"SI":"NO")<<endl;
}

void ejercicio1(){
    cout<<"--GRUPO 1:ENTEROS--"<<endl;
    array<int, 20>n={35, 41, 23, 47, 29,51, 73, 55, 99, 61, 87, 53, 91, 65, 77,3, 15, 7, 19, 11};
    cout<<"a)Todos los elementos: ";
    for (auto it=n.begin(); it!=n.end(); ++it)
    cout<<*it<<" ";
    cout<<endl;
    cout<<"b)Mayores que 20 y menores que 50: ";
    for (int x:n) if (x>20 && x<50)
    cout<<x<<" ";
    cout<<endl;
    sort(n.begin(), n.end());
    cout<<"c)Ordenados: ";
    for(int x:n)
    cout<<x<<" ";
    cout<<endl;
    cout<<"d)Indices impares menores de 10: ";
    for(int i=1; i<10; i+=2)
    cout<<"["<<i<<"]="<<n[i]<<" ";
    cout<<endl<<endl;
}

void ejercicio2(){
    cout<<"--GRUPO 2: PAISES DE SUDAMERICA--"<<endl;
    array<string, 12>p={"Argentina", "Bolivia", "Brasil", "Chile", "Colombia", "Ecuador", "Guyana", "Paraguay", "Peru", "Uruguay"};
    p.at(10)="Surinam";
    p[11]="Venezuela";
    cout<<"a)Se insertaron: "<<p[10]<<" y "<<p[11]<<endl;
    cout<<"b)Paises que inician con C o P: ";
    for(const string& s:p) if(s[0]=='C' || s[0]=='P')
    cout<<s<<" ";
    cout<<endl;
    cout<<"c)Primero: "<<p.front()<<" - Ultimo: "<<p.back()<<endl;
    sort(p.begin(), p.end());
    cout<<"d)Ordenados: ";
    for (const string& s:p)
    cout<<s<<" ";
    cout<<endl<<endl;
}

void ejercicio3(){
    cout<<"--GRUPO 3: ESTUDIANTES--"<<endl;
    array<Estudiante, 10>est={{ {"Juan", 2000, 3.4}, {"Maria", 2005, 2.8} }};
    for (int i=2; i<=3; i++){
        cout<<"Estudiante "<<i+1<<" - nombre: ";
        cin>>est[i].nombre;
        cout<<"Año de nacimiento: ";
        cin>>est[i].anio;
        cout<<"GPA: ";
        cin>>est[i].gpa;
    }
    string nombres[3]={"Gaby", "Patty", "Diana"};
    int nacimientos[3]={2002, 2004, 2006};
    double gps[3]={3.8, 3.0, 2.9};
    for(int i=0; i<2; i++){
        est[4+i]={nombres[i], nacimientos[i], gps[i]};
    }
    ifstream archivo("datos.txt");
    if (!archivo){
        cout<<"No se pudo abrir datos.txt"<<endl;
        return;
    }
    for(int i=6; i<10; i++) {
        archivo>>est[i].nombre>>est[i].anio>>est[i].gpa;
    }
    archivo.close();
    cout<<endl<<"d)Lista de estudiantes:"<< endl;
    for(const Estudiante& e:est) imprimirEstudiante(e);
}

int main(){
    ejercicio1();
    ejercicio2();
    ejercicio3();
    return 0;
}
