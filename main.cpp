#include<iostream>
#include<stdlib.h>
#include<time.h>
#include<stdio.h>
#include<conio.h>
#include<math.h>
#include<vector>
#include<algorithm>
#include<cstring>

#define mem 1.5
#define ma 64
#define lineas 100
#define N 20
#define tso 20// Tama?o del sistema operativo en KB
#define quantum 3
using namespace std;

struct Interrupciones {
    int codigo ;
    string desc;
}A[15]; 

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

struct PCB {
     char proceso[20];
    int tiempo_llegada;
    int localidad_i;
    int ciclos;
    int estado;
    int cpu_es;
    int CicInt;
    int IniInt;
    int CodInt;
    int Duracion;
    bool Interrupcion;
    int ciclos_transcurridos;
    int ntar;
    PCB* sig;
};

struct PCBSemaforo {
    char proceso[20];
    int tiempo_llegada;
    int ciclos;
    int estado;
    int CicInt;
    int IniInt;
    int CodInt;
    int Duracion;
    int tiempo_retorno;
    bool Interrpcion;
    int ciclos_transcurridos;
    int ntar;
    PCBSemaforo* siguiente;
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
void crearPCB();
void imprime_PCB();
void ImprimirPCBSemaforo();
void imprimePCBFiltrado(PCB* ppcb);
void Round_robin();
void Crear_vector(Interrupciones A[]);



MMT* pmmt = NULL, * qmmt, * nuevommt, * auxmmt;
JT* pjt = NULL, * qjt, * nuevojt, * auxjt;
PMT* pp[N] = { NULL }, * qp[N], * nuevop[N], * auxp[N]; // Tablas de p?ginas por tarea
PCB* ppcb = NULL, *qpcb, *nuevopcb;
PCBSemaforo* ppcbsem = NULL, *qpcbsem = NULL, *nuevopcbsem = NULL, *auxpcbsem = NULL;

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
    crearPCB();
    imprime_PCB();
    system("pause");
    Crear_vector(A);
    Round_robin();
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
    cout << "\n\nNo.marco     Local.i    Edo\n";
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

void crearPCB() {
    ppcb = NULL; 
    int contador=0;
    for (int i = 0; i < N; i++) {
        PMT* auxp = pp[i]; 
        
        while (auxp != NULL) {
            if (auxp->marco != 0) {
                nuevopcb = new PCB;
                sprintf(nuevopcb->proceso, "J%dP%d", i, auxp->pagina); 
                nuevopcb->tiempo_llegada = contador;    
                nuevopcb->localidad_i= auxp->marco * ma; 
                nuevopcb->ciclos = 2 + rand() % (7-2+1);;       
                nuevopcb->estado = 1;     
                nuevopcb->cpu_es = 0+rand()%3; 
                nuevopcb->IniInt = 0+rand()%2;
                nuevopcb->Interrupcion=0+rand()%2;
                if(nuevopcb->Interrupcion==1){
                    nuevopcb->CodInt = 0+rand ()%14;
                    nuevopcb->IniInt=2+rand()%(nuevopcb->ciclos-2+1);
                    nuevopcb->Duracion=0;
                }else if(nuevopcb->Interrupcion==0){
                    nuevopcb->IniInt=0;
                    nuevopcb->CodInt = -1;
                    nuevopcb->Duracion=77;
                }
                contador++;
                nuevopcb->sig = NULL; 
                
                if (ppcb == NULL) {
                    ppcb = nuevopcb;
                    qpcb = ppcb;
                } else {
                    qpcb->sig = nuevopcb;
                    qpcb = nuevopcb;
                }
            }
            auxp = auxp->sig; 
        }
    }
}

void imprime_PCB() {
    cout << "\n\n\t\tBLOQUE DE CONTROL DE PROCESOS (PCB)\n";
    cout << "-----------------------------------------------------------------------------------------------------------------" << endl;
    cout << "|Proceso|    T. Llegada    |  Localidad  |    Ciclos    |    Estado    |   CPU-E/S     |Ini.Int|Duracion|Interr.|" << endl;
    cout << "-----------------------------------------------------------------------------------------------------------------" << endl;

    PCB* actual = ppcb;
    while (actual != NULL) {
        // Imprimir el proceso si no tiene interrupción activa
        cout << actual->proceso
             << "\t\t" << actual->tiempo_llegada << "t"
             << "\t\t" << actual->localidad_i << "kb"
			 << "\t\t" << actual->ciclos << "ms"
             << "\t\t" << actual->estado;
            if(actual->Interrupcion==true)
                cout<<"\t    E/S";
             else if(actual->Interrupcion==false)
                cout<<"\t    CPU";
        cout << "\t\t   " << actual->IniInt
             << "\t   " << actual->Duracion
             << "\t   " << actual->CodInt
             << endl;

        actual = actual->sig;
    }
    cout << "----------------------------------------------------------------------------------------------------------------" << endl;
}


void ImprimirPCBSemaforo() {
    cout << "\n=== Lista de Procesos en PCBSemaforo ===\n";
    cout << "-------------------------------------------------------------------------------------------------\n";
    cout << "|Proceso|    T. Llegada    |   Ciclos   |    Estado |      E/S      |Ini.Int|Duracion|Interr.|\n";
    cout << "-------------------------------------------------------------------------------------------------\n";

    auxpcbsem = ppcbsem;
    while (auxpcbsem != NULL) {
        cout << auxpcbsem->proceso
             << "\t\t" << auxpcbsem->tiempo_llegada << "t"
             << "\t\t" << auxpcbsem->ciclos << "ms"
             << "\t\t" << auxpcbsem->estado
             << "\t\t E/S"
             << "\t\t   " << auxpcbsem->IniInt
             << "\t   " << auxpcbsem->Duracion
             << "\t   " << auxpcbsem->CodInt
             << endl;

        auxpcbsem = auxpcbsem->siguiente;
    }
    cout << "-------------------------------------------------------------------------------------------------\n";
}

void imprimePCBFiltrado(PCB* ppcb) {
    cout << "\n\n\t\tBLOQUE DE CONTROL DE PROCESOS (PCB)\n";
    cout << "-----------------------------------------------------------------------------------------------------------------" << endl;
    cout << "|Proceso|    T. Llegada    |  Localidad  |    Ciclos    |    Estado    |   CPU-E/S     |Ini.Int|Duracion|Interr.|" << endl;
    cout << "-----------------------------------------------------------------------------------------------------------------" << endl;

    PCB* actual = ppcb;
    while (actual != NULL) {
       
        if (actual->cpu_es != 1 && actual->estado != 4) {
            cout << actual->proceso
                 << "\t\t" << actual->tiempo_llegada << "t"
                 << "\t\t" << actual->localidad_i << "kb"
                 << "\t\t" << actual->ciclos << "ms"
                 << "\t\t" << actual->estado;
            if(actual->Interrupcion)
                cout<<"\t    E/S";
            else
                cout<<"\t    CPU";
            cout << "\t\t   " << actual->IniInt
                 << "\t   " << actual->Duracion
                 << "\t   " << actual->CodInt
                 << endl;
        }
        actual = actual->sig;
    }
    cout << "-----------------------------------------------------------------------------------------------------------------" << endl;
}

void Crear_vector(Interrupciones A[]){
   A[0].codigo= 19; 
   A[0].desc= "Obtener unidad de disco por defecto \n";
   A[1].codigo=23;
   A[1].desc="Invoca los servicios de la impresora de la ROM BIOS\n";
   A[2].codigo=63;
   A[2].desc="Gestor Overlay\n";
   A[3].codigo=75;
   A[3].desc="Reservada para BIOS\n";
   A[4].codigo=94;
   A[4].desc="Obtener nombre de la maquina/instalacion de impresora\n";
   A[5].codigo=22;
   A[5].desc="Escritura aleatoria\n";
   A[6].codigo=30;
   A[6].desc="Apunta a la tabla de parametros de la unidad de disco\n";
   A[7].codigo=47;
   A[7].desc="Interrupcion multiple del DOS\n";
   A[8].codigo= 31; 
   A[8].desc= "Apunta a los  caracteres graficos del CGA\n";
   A[9].codigo= 31; 
   A[9].desc= "Apunta a los  caracteres graficos del CGA\n";
   A[10].codigo= 33; 
   A[10].desc= "Invoca a todos los servicion en funcion a DOS\n";
   A[11].codigo= 40; 
   A[11].desc= "DOS idle\n";
   A[12].codigo= 41; 
   A[12].desc= "Interno DOS. PutChar Rapido\n";
   A[13].codigo= 48; 
   A[13].desc= "Ejecutar comando\n";
   A[14].codigo= 3; 
   A[14].desc= "Leer la posicion del cursor\n";
}   

void Round_robin() {
    PCB* actual;
    int tiempo_total = 0;
    actual = ppcb;
    ppcbsem = NULL; // Initialize PCB semaphore list

    // Initialize all processes to state 2 (Ready)
    while (actual != NULL) {
        actual->estado = 2;
        actual = actual->sig;
    }

    bool hayPendientes;
    bool mostrarInterrupcion = false;
    PCB* procesoInterrumpido = NULL;

    do {
        hayPendientes = false;
        actual = ppcb;

        while (actual != NULL) {
            if (actual->estado == 2) {
                actual->estado = 3; // Set to Running
                clrscr(); // Borland's clear screen function

                int ciclos_a_ejecutar = (actual->ciclos > quantum) ? quantum : actual->ciclos;
                int cont = 0;

                while (cont < ciclos_a_ejecutar && actual->ciclos > 0) {
                    clrscr();
                    
                    // Check if process is E/S type
                    if (actual->cpu_es == 1) {
                        // Create new PCB Semaphore entry
                        nuevopcbsem = new PCBSemaforo;
                        strcpy(nuevopcbsem->proceso, actual->proceso);
                        nuevopcbsem->tiempo_llegada = actual->tiempo_llegada;
                        nuevopcbsem->ciclos = actual->ciclos;
                        nuevopcbsem->estado = actual->estado;
                        nuevopcbsem->CicInt = actual->CicInt;
                        nuevopcbsem->IniInt = actual->IniInt;
                        nuevopcbsem->CodInt = actual->CodInt;
                        nuevopcbsem->Duracion = actual->Duracion;
                        nuevopcbsem->Interrpcion = actual->Interrupcion;
                        nuevopcbsem->ciclos_transcurridos = actual->ciclos_transcurridos;
                        nuevopcbsem->ntar = actual->ntar;
                        nuevopcbsem->siguiente = NULL;

                        // Add to PCB Semaphore list
                        if (ppcbsem == NULL) {
                            ppcbsem = nuevopcbsem;
                            qpcbsem = nuevopcbsem;
                        } else {
                            qpcbsem->siguiente = nuevopcbsem;
                            qpcbsem = nuevopcbsem;
                        }
                        
                        // Set state to 4 to hide it
                        actual->estado = 4;
                    }

                    // Print filtered PCB state
                    imprimePCBFiltrado(ppcb);
                    
                    // Print PCB Semaphore state
                    if (ppcbsem != NULL) {
                        cout << "\nProcesos E/S en PCB Semaforo:\n";
                        ImprimirPCBSemaforo();
                    }

                    actual->ciclos--;
                    tiempo_total++;
                    cont++;
                    actual->ciclos_transcurridos++;

                    // Process PCB Semaphore processes simultaneously
                    PCBSemaforo* semActual = ppcbsem;
                    while (semActual != NULL) {
                        semActual->ciclos--;
                        if (semActual->ciclos <= 0) {
                            // Process completed in PCB Semaphore
                            // Find corresponding process in main PCB and update its state
                            PCB* mainProcess = ppcb;
                            while (mainProcess != NULL) {
                                if (strcmp(mainProcess->proceso, semActual->proceso) == 0) {
                                    mainProcess->estado = 5; // Mark as completed
                                    mainProcess->ciclos = 0;
                                    break;
                                }
                                mainProcess = mainProcess->sig;
                            }
                        }
                        semActual = semActual->siguiente;
                    }

                    // Clean up completed processes in PCB Semaphore
                    PCBSemaforo* semPrev = NULL;
                    semActual = ppcbsem;
                    while (semActual != NULL) {
                        if (semActual->ciclos <= 0) {
                            if (semPrev == NULL) {
                                ppcbsem = semActual->siguiente;
                                delete semActual;
                                semActual = ppcbsem;
                            } else {
                                semPrev->siguiente = semActual->siguiente;
                                delete semActual;
                                semActual = semPrev->siguiente;
                            }
                        } else {
                            semPrev = semActual;
                            semActual = semActual->siguiente;
                        }
                    }

                    cout << "\nPresione cualquier tecla para continuar...";
                    getch();

                    // Check for interruption
                    if (actual->ciclos_transcurridos == actual->IniInt && 
                        actual->Interrupcion) {
                        mostrarInterrupcion = true;
                        procesoInterrumpido = actual;
                        break;
                    }
                }

                if (mostrarInterrupcion) {
                    clrscr();
                    imprimePCBFiltrado(ppcb);
                    cout << "\nCodigo de interrupcion: " << procesoInterrumpido->CodInt
                         << "\nError encontrado en " << procesoInterrumpido->proceso
                         << "\nError encontrado en el tiempo: " << procesoInterrumpido->IniInt
                         << "\nDescripcion: " << A[procesoInterrumpido->CodInt].desc
                         << "\n\nPresione cualquier tecla para continuar...";

                    getch();
                    procesoInterrumpido->estado = 4; // Hide instead of delete
                    mostrarInterrupcion = false;
                }

                if (actual->ciclos == 0) {
                    actual->estado = 5; // Finished
                } else if (actual->estado != 4 && actual->estado != 6) {
                    actual->estado = 4; // Blocked
                }
            }
            actual = actual->sig;
        }

        // Check for pending processes in both PCB and PCB Semaphore
        hayPendientes = false;
        actual = ppcb;
        while (actual != NULL) {
            if (actual->estado == 2 || (actual->estado == 4 && actual->cpu_es != 1)) {
                hayPendientes = true;
                break;
            }
            actual = actual->sig;
        }

        // Also check PCB Semaphore
        if (ppcbsem != NULL) {
            hayPendientes = true;
        }

        // Reset blocked processes to ready (only for non-E/S processes)
        actual = ppcb;
        while (actual != NULL) {
            if (actual->estado == 4 && actual->cpu_es != 1) {
                actual->estado = 2;
            }
            actual = actual->sig;
        }

    } while (hayPendientes);
}