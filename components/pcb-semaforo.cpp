#include<iostream>
#include<stdlib.h>
#include<time.h>
#include<stdio.h>
#include<conio.h>
#include<math.h>
#include <vector>
#include <algorithm>

#define mem 1.5
#define ma 64
#define lineas 100
#define N 20
#define tso 20// Tama?o del sistema operativo en KB


struct MMT {
    int marco, loc;
    int edo; // 0 si el marco est? libre, 1 si est? ocupado
    struct MMT* sig;
};

struct JT {
    int ntar;
    int tamtar; // Tama?o de la tarea en bytes
    int lpmt;   // N?mero de p?ginas en la PMT
    int secuencia[8];
    struct JT* sig;
};

struct PMT {
    int pagina; // N?mero de la p?gina l?gica
    int marco;  // N?mero del marco f?sico asignado
    int estado;  // Estado (0 por defecto)
    int referencia; // Referencia (0 por defecto)
    int modificacion; // Modificaci?n (0 por defecto)
    struct PMT* sig;
};

int calc_marcos(void);
void crearMMT(int);
void crearPMT(int, int);
void crearJT(void);
void imprime_MMT();
void imprime_PMT();
void imprime_JT();
void asigna_pag(int, int);
 int asigna_so();
void asigna_paginas_con_secuencia(); 
void imprime_JT_sin_asignadas();
MMT* pmmt = NULL, * qmmt, * nuevommt, * auxmmt;
JT* pjt = NULL, * qjt, * nuevojt, * auxjt;
PMT* pp[N] = { NULL }, * qp[N], * nuevop[N], * auxp[N]; // Tablas de p?ginas por tarea

int main() {
     srand(time(NULL));
    int marcos_totales = calc_marcos();
 
    crearMMT(marcos_totales); // Crea la tabla MMT
    asigna_so(); // Asigna marcos al SO
    imprime_MMT();           // Imprime la tabla MMT inicial
    
    crearJT();               // Crea la tabla JT con las tareas
    
    // Asignaci?n de p?ginas para cada tarea basada en el tama?o de la tarea
    auxjt = pjt;
    while (auxjt != NULL) {
        asigna_pag(auxjt->ntar, auxjt->tamtar);
        auxjt = auxjt->sig;
    }
   asigna_paginas_con_secuencia(); 
    imprime_JT();            // Imprime la tabla JT actualizada
    imprime_MMT();           // Imprime la tabla MMT despu?s de la asignaci?n
    imprime_PMT();           // Imprime todas las PMTs para cada tarea
    //Imprime_JT_sin_asignadas();
    system("pause");
    return 0;
}

int calc_marcos(void) {
    int cap_mem, cant_marcos;
    cap_mem = mem * 1024;
    cant_marcos = cap_mem / ma; // Capacidad total de memoria dividida por el tama?o de un marco
    return cant_marcos;
}

void crearMMT(int a) {
    int mar = 0;

    while (mar != a) {
        if (pmmt == NULL) {
            pmmt = new MMT;
            pmmt->marco = mar;
            pmmt->loc = mar * ma;
            pmmt->edo = 0; // Marco libre
            pmmt->sig = NULL;
            qmmt = pmmt;
        }
        else {
            nuevommt = new MMT;
            nuevommt->marco = mar;
            nuevommt->loc = mar * ma;
            nuevommt->edo = 0; // Marco libre
            nuevommt->sig = NULL;
            qmmt->sig = nuevommt;
            qmmt = nuevommt;
        }
        mar++;
    }
}

void imprime_MMT() {
    auxmmt = pmmt;
    cout << "No.marco     Local.i    Edo\n";
    while (auxmmt != NULL) {
        cout << auxmmt->marco << "\t\t" << auxmmt->loc << "\t" << auxmmt->edo << "\n";
        auxmmt = auxmmt->sig;
    }
}

void crearJT() {
    float tamanios_en_lineas[N];
    for(int i=0;i<N;i++){
    	tamanios_en_lineas[i]=120+rand()%(450);
	}
    int bytes = sizeof(tamanios_en_lineas);
    int elementos = bytes / sizeof(float);
    float red;

    for (int i = 0; i < elementos; i++) {
        if (pjt == NULL) {
            pjt = new JT;
            pjt->ntar = i;
            pjt->tamtar = tamanios_en_lineas[i];
            red = (float)tamanios_en_lineas[i] / lineas;
            pjt->lpmt = ceil(red); // Calcula n?mero de p?ginas
            pjt->secuencia[0]=0;
            for(int i=1;i<8;i++){
            	pjt->secuencia[i]=0+rand()%(pjt->lpmt);
			}
            pjt->sig = NULL;
            qjt = pjt;
        }
        else {
            nuevojt = new JT;
            nuevojt->ntar = i;
            nuevojt->tamtar = tamanios_en_lineas[i];
            red = (float)tamanios_en_lineas[i] / lineas;
            nuevojt->lpmt = ceil(red); // Calcula n?mero de p?ginas
            nuevojt->secuencia[0]=0;
            for(int i=1;i<8;i++){
            	nuevojt->secuencia[i]=0+rand()%(nuevojt->lpmt);
			}
            nuevojt->sig = NULL;
            qjt->sig = nuevojt;
            qjt = nuevojt;
        }
    }
}

void imprime_JT() {
    auxjt = pjt;
    cout << "\n\nNo.tarea   Lineas    Loc.PMT   Secuencia \n";
    while (auxjt != NULL) {
        cout << "J" << auxjt->ntar << "\t\t" << auxjt->tamtar << "\t" <<"101"<< auxjt->ntar<<"   [";
		for(int i=0;i<8;i++){
		cout<<"P"<<auxjt->secuencia[i]<<" ";
		} cout<<"] \n";
        auxjt = auxjt->sig;
    }
}

void crearPMT(int ntar, int npaginas) {
    // Reseteamos el puntero para la MMT
    auxmmt = pmmt;
    pp[ntar] = NULL; // Inicializa la PMT de la tarea ntar
    
    for (int i = 0; i < npaginas; i++) {
        if (pp[ntar] == NULL) {
            pp[ntar] = new PMT;
            pp[ntar]->pagina = i;
            pp[ntar]->marco = 0; // Inicialmente sin marco asignado
            pp[ntar]->estado = 0;
            pp[ntar]->referencia = 0;
            pp[ntar]->modificacion = 0;
            pp[ntar]->sig = NULL;
            qp[ntar] = pp[ntar]; // Inicio de la lista
        } else {
            nuevop[ntar] = new PMT;
            nuevop[ntar]->pagina = i;
            nuevop[ntar]->marco = 0; // Inicialmente sin marco asignado
            nuevop[ntar]->estado = 0;
            nuevop[ntar]->referencia = 0;
            nuevop[ntar]->modificacion = 0;
            nuevop[ntar]->sig = NULL;
            qp[ntar]->sig = nuevop[ntar]; // A?adir al final de la lista
            qp[ntar] = nuevop[ntar]; // Actualizar puntero
        }
    }
}




void imprime_PMT() {
    for (int i = 0; i < N; i++) {
        PMT* auxp = pp[i]; // Inicializar un puntero para recorrer PMT
        
        // Verificar si hay al menos una p?gina con datos
        bool tieneDatos = false;
        while (auxp != NULL) {
            if (auxp->marco != 0 ) {
                tieneDatos = true;
                break;
            }
            auxp = auxp->sig; // Avanza al siguiente en la PMT
        }
     
        
        // Si hay datos, imprime la PMT
        if (tieneDatos==true ) {
            cout << "\n\nPMT de la tarea " << i << "\n";

            // Imprimir la secuencia de la tarea
            JT* auxjt = pjt; // Inicializar un puntero para recorrer JT
            while (auxjt != NULL && auxjt->ntar != i) { // Buscar la tarea correspondiente
                auxjt = auxjt->sig;
            }
            if (auxjt != NULL) { // Si se encuentra la tarea
                cout << "J" << i << ": Secuencia=["; // Imprimir la secuencia
                for (int j = 0; j < 8; j++) {
                    cout << "P" << auxjt->secuencia[j] << " ";
                }
                cout << "]\n";
            }

            cout << "No.pagina\tMarco\tLocal.i\tEstado\tReferencia\tModificacion\n"; // Imprimir encabezados
            
            auxp = pp[i]; // Reiniciar auxp para imprimir valores
            while (auxp != NULL) { // Imprimir los valores de PMT
                cout << auxp->pagina << "\t\t" << auxp->marco << "\t" << (auxp->marco * ma) << "\t" 
                     << auxp->estado << "\t" << auxp->referencia << "\t\t" << auxp->modificacion << "\n"; // Imprimir valores
                auxp = auxp->sig; // Avanza al siguiente en la PMT
            }
        }
    }
}




void asigna_pag(int ntar, int tam_tarea) {
    int paginas_necesarias = ceil((float)tam_tarea / lineas); // N?mero de p?ginas necesarias
    crearPMT(ntar, paginas_necesarias); // Crea la PMT para la tarea
}

int asigna_so(){
    int marcos_so = ceil((float)tso /ma); // Calcula los marcos necesarios para el SO
    int marcos_ocupados = 0;
    int inicio_so=0;
    auxmmt = pmmt;

    while (auxmmt != NULL && marcos_ocupados < marcos_so) {
        if (auxmmt->marco >= inicio_so && auxmmt->edo == 0) {
            auxmmt->edo = 1; // Marco ocupado por el sistema operativo
            marcos_ocupados++;
        }
        auxmmt = auxmmt->sig;
    }
    return marcos_ocupados; // Devuelve cu?ntos marcos ha ocupado el SO
} 

void asigna_paginas_con_secuencia() {
    auxjt = pjt;  // Iterador de la tabla de tareas
    auxmmt = pmmt; // Iterador de la MMT

    // Avanzar marcos hasta despu?s del sistema operativo (marcos ocupados por el SO)
    while (auxmmt->edo == 1) {
        auxmmt = auxmmt->sig;
    }

    // Para cada tarea en la JT
    while (auxjt != NULL) {
        fflush(stdin);
        int marcos_asignados = 0;  // Lleva el conteo de marcos asignados
        std::vector<int> paginas_asignadas; // Vector din?mico para almacenar hasta 3 p?ginas ?nicas
        paginas_asignadas.push_back(0); // La primera p?gina siempre es la 0

        // Recorrer la secuencia y seleccionar hasta 3 p?ginas ?nicas
        for (int i = 1; i < 8 && paginas_asignadas.size() < 3; i++) {
            int pagina_actual = auxjt->secuencia[i];

            // Verificar que la p?gina no est? ya en el vector
            if (std::find(paginas_asignadas.begin(), paginas_asignadas.end(), pagina_actual) == paginas_asignadas.end()) {
                paginas_asignadas.push_back(pagina_actual); // Agregar la p?gina si no est? duplicada
            }
        }
        // Asignar marcos y localidades a las p?ginas seleccionadas
        for (int i = 0; i < paginas_asignadas.size(); i++) {
            int pagina_logica = paginas_asignadas[i];  // P?gina l?gica a la que vamos a asignar un marco
            PMT* auxp = pp[auxjt->ntar];

            // Buscar la entrada en la PMT que corresponda a esta p?gina l?gica
            while (auxp != NULL && auxp->pagina != pagina_logica) {
                auxp = auxp->sig;
            }

            // Si encontramos una entrada en la PMT y hay marcos disponibles en la MMT
            if (auxp != NULL && auxmmt != NULL && auxmmt->edo == 0) {
                // Asignar el marco y la localidad f?sica
                auxp->marco = auxmmt->marco;
                auxp->estado = 1;  // La p?gina ahora est? ocupada
                auxmmt->edo = 1;  // El marco en la MMT est? ocupado
                auxmmt = auxmmt->sig;  // Pasar al siguiente marco libre en la MMT
                marcos_asignados++;
            }
        }
        auxjt = auxjt->sig;  // Pasar a la siguiente tarea
    }
}
/*
void imprime_JT_sin_asignadas() {
    cout << "\nTabla de Tareas (sin asignadas a PMT):\n";
    cout << "Tarea\tSecuencia\n"; // Encabezados

    JT* auxjt = pjt; // Inicializar un puntero para recorrer JT
    while (auxjt != NULL) {
        bool asignada = false;

        // Verificar si la tarea est? asignada a alguna PMT
        for (int i = 0; i < N; i++) {
            PMT* auxp = pp[i]; // Inicializar puntero para recorrer PMT
            while (auxp != NULL) {
                if (auxp->pagina == auxjt->ntar) { // Verificar si la tarea coincide
                    asignada = true;
                    break; // Salir si se encuentra una coincidencia
                }
                auxp = auxp->sig; // Avanza al siguiente en la PMT
            }
            if (asignada==true) break; // Salir del ciclo si ya est? asignada
        }

        // Imprimir solo si no est? asignada
        if (asignada==false ) {
            cout << "J" << auxjt->ntar << ": Secuencia=["; // Imprimir la tarea y su secuencia
            for (int j = 0; j < 8; j++) {
                cout << "P" << auxjt->secuencia[j] << " ";
            }
            cout << "]\n";
        }

        auxjt = auxjt->sig; // Avanza al siguiente en la JT
    }
}*/