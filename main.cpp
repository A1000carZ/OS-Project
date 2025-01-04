#include <iostream>
#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <algorithm>
#include <string>
#include <iomanip>

#define quantum 3
#define mem 1.5
#define ma 64
#define lineas 100
#define N 20
#define tso 20
#define maxmem 50

struct Interrupciones {
    int codigo;
    std::string desc;
} A[15];

struct PCB {
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
    PCB* siguiente;
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

struct MMT {
    int marco, loc;
    int edo;
    MMT* sig;
};

struct MMTasig {
    int marco, loc;
    int edo;
    MMTasig* sig;
};

struct JT {
    int ntar;
    int tamtar;
    int lpmt;
    int secuencia[8];
    JT* sig;
};

struct PMT {
    int pagina;
    int marco;
    int estado;
    int referencia;
    int modificacion;
    PMT* sig;
};

// Global pointers
MMT* pmmt = NULL, * qmmt, * nuevommt, * auxmmt;
MMTasig* pmmtasig = NULL, * qmmtasig, * nuevommtasig, * auxmmtasig;
JT* pjt = NULL, * qjt, * nuevojt, * auxjt;
PCB* ppcb = NULL, * qpcb, * nuevopcb, * auxpcb;
PCBSemaforo* ppcbsem= NULL, * qpcbsem, * nuevopcbsem, * auxpcbsem;
PMT* pp[N] = { NULL }, * qp[N], * nuevop[N], * auxp[N];

// Function prototypes
void Crear_vector(Interrupciones A[]);
int calc_marcos();
void crearMMT(int);
void crearMMTAsignado(int);
void crearPMT(int, int);
void crearJT();
void imprime_MMT();
void imprime_MMTasig();
void imprime_PMT();
void imprime_JT();
void asigna_pag(int, int);
void asigna_so();
void asigna_paginas_con_secuencia();
void imprimirListaPCB();
void Round_robin();
void inicializar_sistema();
void manejador_interrupciones(PCB* process, Interrupciones A[], MMT* pmmt, PMT* pp[]);
void Round_robin_con_interrupciones(PCB* &ppcb, Interrupciones A[], MMT* pmmt, PMT* pp[]);
void CrearPCBSemaforo(); 
void ImprimirPCBSemaforo();

int main() {
    srand(time(NULL));
    inicializar_sistema();
    Crear_vector(A);

    cout << "Estado inicial del sistema:\n";
    imprime_MMT();
    imprime_JT();
    imprime_MMTasig();
    imprime_PMT();
    imprimirListaPCB();
     cout << "\n\nPresione cualquier tecla para continuar con round robin...";
                    getch();
    Round_robin_con_interrupciones(ppcb,A,pmmt,pp);
    
    cout << "\nPresione cualquier tecla para salir...";
    getch();
    return 0;
}

void Crear_vector(Interrupciones A[]) {
    A[0].codigo = 19;
    A[0].desc = "Obtener unidad de disco por defecto";
    A[1].codigo = 23;
    A[1].desc = "Invoca los servicios de la impresora";
    A[2].codigo = 63;
    A[2].desc = "Gestor Overlay";
    A[3].codigo = 75;
    A[3].desc = "Reservada para BIOS";
    A[4].codigo = 94;
    A[4].desc = "Obtener nombre de la maquina/instalacion";
    A[5].codigo = 22;
    A[5].desc = "Escritura aleatoria";
    A[6].codigo = 30;
    A[6].desc = "Tabla de parametros de disco";
    A[7].codigo = 47;
    A[7].desc = "Interrupcion multiple del DOS";
    A[8].codigo = 31;
    A[8].desc = "Caracteres graficos del CGA";
    A[9].codigo = 31;
    A[9].desc = "Caracteres graficos del CGA";
    A[10].codigo = 33;
    A[10].desc = "Servicios en funcion a DOS";
    A[11].codigo = 40;
    A[11].desc = "DOS idle";
    A[12].codigo = 41;
    A[12].desc = "Interno DOS. PutChar Rapido";
    A[13].codigo = 48;
    A[13].desc = "Ejecutar comando";
    A[14].codigo = 3;
    A[14].desc = "Leer posicion del cursor";
}
void inicializar_sistema() {
    /* Initialize random seed */
    srand(time(NULL));
    int marcos_totales = calc_marcos();
    cout << marcos_totales << endl;
    
    /* Initialize Memory Management */
    crearMMT(marcos_totales);
    crearJT();
    crearMMTAsignado(marcos_totales);

     // Asignaci?n de p?ginas para cada tarea basada en el tama?o de la tarea
    auxjt = pjt;
    while (auxjt != NULL) {
        asigna_pag(auxjt->ntar, auxjt->tamtar);
        auxjt = auxjt->sig;
    }
   asigna_paginas_con_secuencia(); 


    /* Create test processes and link them to memory tasks */
    int proceso_count = 0;
    auxjt = pjt;
    
    while (auxjt != NULL && proceso_count < 5) {
        int i;
        for (i = 0; i < 5 && proceso_count < 5; i++) {  /* Max 5 processes per task */
            nuevopcb = new PCB;
            /* Clear memory first */
            memset(nuevopcb, 0, sizeof(PCB));
            
            char temp[20];
            sprintf(temp, "J%dP%d", proceso_count + 1, i);  /* Format: J1P0, J1P1, etc. */
            strcpy(nuevopcb->proceso, temp);
            
            nuevopcb->tiempo_llegada = proceso_count;
            nuevopcb->ciclos = 2 + rand() % 6;
            nuevopcb->estado = 1;
            nuevopcb->ntar = proceso_count;
            nuevopcb->CicInt = rand() % 3;
            nuevopcb->Interrpcion = rand() % 2;  /* Random 0 or 1 */
            nuevopcb->ciclos_transcurridos = 0;
            
            if (nuevopcb->Interrpcion) {
                nuevopcb->CodInt = rand() % 14;  /* Random interrupt code */
                nuevopcb->IniInt = 2 + rand() % (nuevopcb->ciclos - 1);
                nuevopcb->Duracion = 1 + rand() % 3;
            } else {
                nuevopcb->IniInt = 0;
                nuevopcb->CodInt = -1;
                nuevopcb->Duracion = 0;
            }
            
            nuevopcb->siguiente = NULL;

            if (ppcb == NULL) {
                ppcb = nuevopcb;
                qpcb = ppcb;
            } else {
                qpcb->siguiente = nuevopcb;
                qpcb = nuevopcb;
            }

            /* Assign memory pages for this process */
            if (auxjt != NULL) {
                asigna_pag(auxjt->ntar, auxjt->tamtar);
            }
            
            proceso_count++;
        }
        auxjt = auxjt->sig;
    }

    asigna_paginas_con_secuencia();
}

int calc_marcos() {
    return (mem * 1024) / ma;
}

void crearMMT(int a) {
    int mar = 0;
    while (mar != a) {
        if (pmmt == NULL) {
            pmmt = new MMT;
            pmmt->marco = mar;
            pmmt->loc = mar * ma;
            pmmt->edo = 0;
            pmmt->sig = NULL;
            qmmt = pmmt;
        } else {
            nuevommt = new MMT;
            nuevommt->marco = mar;
            nuevommt->loc = mar * ma;
            nuevommt->edo = 0;
            nuevommt->sig = NULL;
            qmmt->sig = nuevommt;
            qmmt = nuevommt;
        }
        mar++;
    }
}

void crearMMTAsignado(int a) {
    int marasig = 0;
    while (marasig != a) {
        if (pmmtasig == NULL) {
            pmmtasig = new MMTasig;
            pmmtasig->marco = marasig;
            pmmtasig->loc = marasig * ma;
            pmmtasig->edo = 1;
            pmmtasig->sig = NULL;
            qmmtasig = pmmtasig;
        } else {
            nuevommtasig = new MMTasig;
            nuevommtasig->marco = marasig;
            nuevommtasig->loc = marasig * ma;
            nuevommtasig->edo = 1;
            nuevommtasig->sig = NULL;
            qmmtasig->sig = nuevommtasig;
            qmmtasig = nuevommtasig;
        }
        marasig++;
    }
}

void crearPMT(int ntar, int npaginas) {
    auxmmt = pmmt;
    pp[ntar] = NULL;

    for (int i = 0; i < npaginas; i++) {
        if (pp[ntar] == NULL) {
            pp[ntar] = new PMT;
            pp[ntar]->pagina = i;
            pp[ntar]->marco = 0;
            pp[ntar]->estado = 0;
            pp[ntar]->referencia = 0;
            pp[ntar]->modificacion = 0;
            pp[ntar]->sig = NULL;
            qp[ntar] = pp[ntar];
        } else {
            nuevop[ntar] = new PMT;
            nuevop[ntar]->pagina = i;
            nuevop[ntar]->marco = 0;
            nuevop[ntar]->estado = 0;
            nuevop[ntar]->referencia = 0;
            nuevop[ntar]->modificacion = 0;
            nuevop[ntar]->sig = NULL;
            qp[ntar]->sig = nuevop[ntar];
            qp[ntar] = nuevop[ntar];
        }
    }
}

void crearJT() {
    float tamanios_en_lineas[N];
    for(int i = 0; i < N; i++) {
        tamanios_en_lineas[i] = 120 + rand() % 450;
    }
    
    for (int i = 0; i < N; i++) {
        if (pjt == NULL) {
            pjt = new JT;
            pjt->ntar = i;
            pjt->tamtar = tamanios_en_lineas[i];
            pjt->lpmt = ceil((float)tamanios_en_lineas[i] / lineas);
            pjt->secuencia[0] = 0;
            for(int j = 1; j < 9; j++) {
                pjt->secuencia[j] = rand() % pjt->lpmt;
            }
            pjt->sig = NULL;
            qjt = pjt;
        } else {
            nuevojt = new JT;
            nuevojt->ntar = i;
            nuevojt->tamtar = tamanios_en_lineas[i];
            nuevojt->lpmt = ceil((float)tamanios_en_lineas[i] / lineas);
            nuevojt->secuencia[0] = 0;
            for(int j = 1; j < 9; j++) {
                nuevojt->secuencia[j] = rand() % nuevojt->lpmt;
            }
            nuevojt->sig = NULL;
            qjt->sig = nuevojt;
            qjt = nuevojt;
        }
    }
}

void imprime_MMT() {
    cout << "\n+----------+-----------+-------+\n";
    cout << "| No.Marco | Local.i   | Estado|\n";
    cout << "+----------+-----------+-------+\n";
    
    auxmmt = pmmt;
    while (auxmmt != NULL) {
        cout << "| " << setw(8) << auxmmt->marco
             << " | " << setw(9) << auxmmt->loc
             << " | " << setw(5) << auxmmt->edo << " |\n";
        auxmmt = auxmmt->sig;
    }
    cout << "+----------+-----------+-------+\n";
}

void imprime_MMTasig() {
    cout << "\n+----------+-----------+-------+\n";
    cout << "| No.Marco | Local.i   | Estado|\n";
    cout << "+----------+-----------+-------+\n";
    
    auxmmtasig = pmmtasig;
    while (auxmmtasig != NULL) {
        cout << "| " << setw(8) << auxmmtasig->marco
             << " | " << setw(9) << auxmmtasig->loc
             << " | " << setw(5) << auxmmtasig->edo << " |\n";
        auxmmtasig = auxmmtasig->sig;
    }
    cout << "+----------+-----------+-------+\n";
}


void imprime_JT() {
    cout << "\n+----------+----------+----------+-----------------+\n";
    cout << "| No.Tarea | Lineas   | Loc.PMT  | Secuencia      |\n";
    cout << "+----------+----------+----------+-----------------+\n";

    auxjt = pjt;
    while (auxjt != NULL) {
        cout << "| J" << setw(7) << auxjt->ntar
             << " | " << setw(8) << auxjt->tamtar
             << " | " << setw(8) << "101" << auxjt->ntar
             << " | [";
        
        for(int i = 0; i < 8; i++) {        
            cout << "P" << auxjt->secuencia[i];
            if(i < 7) cout << " ";
        }
        cout << "] |\n";
        auxjt = auxjt->sig;
    }
    cout << "+----------+----------+----------+-----------------+\n";
}

void imprime_PMT() {
    for (int i = 0; i < N; i++) {
        if (pp[i] == NULL) { // Verificar si la PMT existe
            cout << "PMT para la tarea J" << i << " no fue creada.\n";
            continue;
        }

        PMT* auxp = pp[i];
        bool tieneDatos = false;

        // Verificar si la PMT tiene datos relevantes
        while (auxp != NULL) {
            if (auxp->marco != 0) {
                tieneDatos = true;
                break;
            }
            auxp = auxp->sig;
        }

        cout << "\n=== PMT de la tarea " << i << " ===\n";

        JT* auxjt = pjt;
        while (auxjt != NULL && auxjt->ntar != i) {
            auxjt = auxjt->sig;
        }

        if (auxjt != NULL) {
            cout << "J" << i << ": Secuencia=[";
            for (int j = 0; j < 8; j++) {
                cout << "P" << auxjt->secuencia[j];
                if (j < 7) cout << " ";
            }
            cout << "]\n\n";
        } else {
            cout << "Error: No se encontró tarea J" << i << " en la lista de tareas.\n";
        }

        cout << "+----------+-------+---------+--------+-----------+-------------+\n";
        cout << "| No.Pag   | Marco | Local.i | Estado | Referenc. | Modificac. |\n";
        cout << "+----------+-------+---------+--------+-----------+-------------+\n";

        auxp = pp[i]; // Reiniciar para recorrer la lista
        while (auxp != NULL) {
            cout << "| " << setw(8) << auxp->pagina
                 << " | " << setw(5) << auxp->marco
                 << " | " << setw(7) << (auxp->marco * ma)
                 << " | " << setw(6) << auxp->estado
                 << " | " << setw(9) << auxp->referencia
                 << " | " << setw(11) << auxp->modificacion << " |\n";
            auxp = auxp->sig;
        }
        cout << "+----------+-------+---------+--------+-----------+-------------+\n";
    }
}


void asigna_pag(int ntar, int tam_tarea) {
    int paginas_necesarias = ceil((float)tam_tarea / lineas);
    crearPMT(ntar, paginas_necesarias);
}

void asigna_so() {
    int marcos_so = ceil((float)tso / ma);
    int marcos_ocupados = 0;
    auxmmt = pmmt;

    while (auxmmt != NULL && marcos_ocupados < marcos_so) {
        if (auxmmt->edo == 0) {
            auxmmt->edo = 1;
            marcos_ocupados++;
        }
        auxmmt = auxmmt->sig;
    }
}

void asigna_paginas_con_secuencia() {
    // Primero, asignar el marco del SO
    auxmmt = pmmt;
    if (auxmmt != NULL) {
        auxmmt->edo = 1;  // Marco para SO
        auxmmt = auxmmt->sig;  // Mover al siguiente marco disponible después del SO
    }

    // Comenzar la asignación de páginas después del marco del SO
    auxjt = pjt;

    while (auxjt != NULL) {
        int marcos_asignados = 0;
        std::vector<int> paginas_asignadas;
        paginas_asignadas.push_back(0);  // Siempre comenzar con P0

        // Recolectar páginas únicas de la secuencia
        for (int i = 1; i < 8 && paginas_asignadas.size() < 3; i++) {
            int pagina_actual = auxjt->secuencia[i];
            if (std::find(paginas_asignadas.begin(), paginas_asignadas.end(), pagina_actual) == paginas_asignadas.end()) {
                paginas_asignadas.push_back(pagina_actual);
            }
        }

        // Asignar marcos a las páginas recolectadas
        for (size_t i = 0; i < paginas_asignadas.size() && auxmmt != NULL; i++) {
            int pagina_logica = paginas_asignadas[i];
            PMT* auxp = pp[auxjt->ntar];

            // Encontrar la página correcta en la PMT
            while (auxp != NULL && auxp->pagina != pagina_logica) {
                auxp = auxp->sig;
            }

            if (auxp != NULL && auxmmt != NULL && auxmmt->edo == 0) {
                auxp->marco = auxmmt->marco;
                auxp->estado = 1;
                // La localidad inicial es el marco * 64 (tamaño del marco)
                int localidad_inicial = auxp->marco * 64;
                
                // Asignar la información a la PCB correspondiente
                PCB* auxpcb = ppcb;
                while (auxpcb != NULL) {
                    if (auxpcb->ntar == auxjt->ntar) {
                        // Aquí podrías agregar la información del marco y localidad
                        // a la PCB si es necesario
                        break;
                    }
                    auxpcb = auxpcb->siguiente;
                }

                auxmmt = auxmmt->sig;
                marcos_asignados++;
            }
        }
        auxjt = auxjt->sig;
    }
}

void imprimirListaPCB() {
    cout << endl;
    cout << "\t\tBLOQUE DE CONTROL DE PROCESOS (PCB)\n";
    cout << "-------------------------------------------------------------------------------------------------" << endl;
    cout << "|Proceso|    T. Llegada    |   Ciclos   |    Estado |      CPU-E/S     |Ini.Int|Duracion|Interr.|" << endl;
    cout << "-------------------------------------------------------------------------------------------------" << endl;

    PCB* actual = ppcb;
    while (actual != NULL) {
        // Imprimir el proceso si no tiene interrupción activa
        cout << actual->proceso
             << "\t\t" << actual->tiempo_llegada << "t"
             << "\t\t" << actual->ciclos << "ms"
             << "\t\t" << actual->estado;
            if(actual->Interrpcion==true)
                cout<<"\t    E/S";
             else if(actual->Interrpcion==false)
                cout<<"\t    CPU";
        cout << "\t\t   " << actual->IniInt
             << "\t   " << actual->Duracion
             << "\t   " << actual->CodInt
             << endl;

        actual = actual->siguiente; // Avanzar al siguiente proceso
    }
    cout << "-------------------------------------------------------------------------------------------------" << endl;
}

void manejador_interrupciones(PCB* process, Interrupciones A[], MMT* pmmt, PMT* pp[]) {
    // system("cls");
    cout << "\n=== MANEJADOR DE INTERRUPCIONES ===\n";
    cout << "Codigo de interrupcion: " << process->CodInt << endl;
    cout << "Proceso: " << process->proceso << endl;
    cout << "Error encontrado en el tiempo: " << process->IniInt << endl;
    cout << "Descripcion: " << A[process->CodInt].desc << endl;
    
    // Liberar memoria asignada al proceso
    PMT* auxp = pp[process->ntar];
    while (auxp != NULL) {
        if (auxp->estado == 1) {
            // Encontrar el marco correspondiente en MMT y liberarlo
            MMT* auxmmt = pmmt;
            while (auxmmt != NULL) {
                if (auxmmt->marco == auxp->marco) {
                    auxmmt->edo = 0; // Liberar el marco
                    break;
                }
                auxmmt = auxmmt->sig;
            }
            auxp->estado = 0;
            auxp->marco = 0;
        }
        auxp = auxp->sig;
    }
    
    cout << "\nMemoria liberada para el proceso interrumpido\n";
    cout << "\nPresione cualquier tecla para continuar...";
    getch();
}

void Round_robin_con_interrupciones(PCB* &ppcb, Interrupciones A[], MMT* pmmt, PMT* pp[]) {
    PCB* actual;
    int tiempo_total = 0;
    actual = ppcb;

    /* Initialize all processes to Ready state */
    while (actual != NULL) {
        actual->estado = 2;  /* Set to Ready state */
        actual = actual->siguiente;
    }

    int hayPendientes;  /* Changed from bool to int for C++98 */
    int mostrarInterrupcion = 0;  /* Changed from bool to int */
    PCB* procesoInterrumpido = NULL;
    PCB* anterior = NULL;

    do {
        hayPendientes = 0;
        actual = ppcb;
        anterior = NULL;

        while (actual != NULL) {
            if (actual->estado == 2) {  /* If process is Ready */
                actual->estado = 3;  /* Set to Running */
                clrscr();  /* Borland specific */

                int ciclos_a_ejecutar = (actual->ciclos > quantum) ? quantum : actual->ciclos;
                int cont = 0;

                while (cont < ciclos_a_ejecutar && actual->ciclos > 0) {
                    clrscr();  /* Borland specific */
                    cout << "\t\tBLOQUE DE CONTROL DE PROCESOS (PCB)\n";
                    cout << "-------------------------------------------------------------------------------------------------\n";
                    cout << "|Proceso|    T. Llegada    |   Ciclos   |    Estado |      CPU-E/S     |Ini.Int|Duracion|Interr.|\n";
                    cout << "-------------------------------------------------------------------------------------------------\n";
                   
                    PCB* listaE_S = NULL;  // Puntero para procesos de E/S
                    PCB* finE_S = NULL;    // Puntero al final de la lista de E/S
                    /* Display current state of all processes */
                    PCB* temp = ppcb;
                    int contador = 0; // para saber cuando llegue al # de ciclos del quantum
                    while (temp != NULL) {
                        cout << temp->proceso;
                        cout << "\t\t" << temp->tiempo_llegada << "t";
                        cout << "\t\t" << temp->ciclos << "ms";
                        cout << "\t\t" << temp->estado;
                        
                           if (temp->Interrpcion) { // Si es un proceso de E/S
                           cout << "\t    E/S";

                           // Incrementar el contador
                           contador++;

                              // Si el contador alcanza el quantum, se envía a PCBSemaforo
        if (contador == quantum) {
            // Crear y agregar a la lista PCBSemaforo
            nuevopcbsem = new PCBSemaforo;
            strcpy(nuevopcbsem->proceso, temp->proceso);
            nuevopcbsem->tiempo_llegada = temp->tiempo_llegada;
            nuevopcbsem->ciclos = temp->ciclos;
            nuevopcbsem->estado = temp->estado;
            nuevopcbsem->CicInt = temp->CicInt;
            nuevopcbsem->IniInt = temp->IniInt;
            nuevopcbsem->CodInt = temp->CodInt;
            nuevopcbsem->Duracion = temp->Duracion;
            nuevopcbsem->tiempo_retorno = temp->tiempo_retorno;
            nuevopcbsem->Interrpcion = temp->Interrpcion;
            nuevopcbsem->ciclos_transcurridos = temp->ciclos_transcurridos;
            nuevopcbsem->ntar = temp->ntar;
            nuevopcbsem->siguiente = NULL;

            if (ppcbsem == NULL) { // Lista vacía
                ppcbsem = nuevopcbsem;
                qpcbsem = nuevopcbsem;
            } else { // Agregar al final
                qpcbsem->siguiente = nuevopcbsem;
                qpcbsem = nuevopcbsem;
            }

            // Reiniciar el contador despues de mandar al semafotro
            contador = 0;
        }
        ImprimirPCBSemaforo();

    } else {
        cout << "\t    CPU";
    }
                        
                        cout << "\t\t   " << temp->IniInt;
                        cout << "\t   " << temp->Duracion;
                        cout << "\t   " << temp->CodInt;
                        cout << "\n";
                        
                        temp = temp->siguiente;
                    }
                    cout << "-------------------------------------------------------------------------------------------------\n";

                    actual->ciclos--;
                    tiempo_total++;
                    cont++;
                    actual->ciclos_transcurridos++;

                    /* Check for interruption */
                    if (actual->Interrpcion && 
                        actual->ciclos_transcurridos == actual->IniInt) {
                        mostrarInterrupcion = 1;
                        procesoInterrumpido = actual;
                        break;
                    }

                    cout << "\nProceso en ejecucion: " << actual->proceso;
                    cout << "\nCiclos restantes: " << actual->ciclos;
                    cout << "\nTiempo total: " << tiempo_total;
                    cout << "\n\nPresione cualquier tecla para continuar...";
                    getch();  /* Borland specific */
                }

                if (mostrarInterrupcion) {
                    clrscr();
                    cout << "\nCodigo de interrupcion: " << procesoInterrumpido->CodInt;
                    cout << "\nError encontrado en " << procesoInterrumpido->proceso;
                    cout << "\nError encontrado en el tiempo: " << procesoInterrumpido->IniInt;
                    cout << "\nDescripcion: " << A[procesoInterrumpido->CodInt].desc;
                    
                    /* Liberar memoria asignada al proceso */
                    PMT* auxp = pp[procesoInterrumpido->ntar];
                    while (auxp != NULL) {
                        if (auxp->estado == 1) {
                            MMT* auxmmt = pmmt;
                            while (auxmmt != NULL) {
                                if (auxmmt->marco == auxp->marco) {
                                    auxmmt->edo = 0;
                                    break;
                                }
                                auxmmt = auxmmt->sig;
                            }
                            auxp->estado = 0;
                            auxp->marco = 0;
                        }
                        auxp = auxp->sig;
                    }
                    
                    cout << "\n\nPresione cualquier tecla para continuar...";
                    getch();
                    
                    /* Remove interrupted process from list */
                    if (procesoInterrumpido == ppcb) {
                        ppcb = ppcb->siguiente;
                    } else {
                        anterior->siguiente = procesoInterrumpido->siguiente;
                    }
                    
                    delete procesoInterrumpido;
                    mostrarInterrupcion = 0;
                    actual = (anterior != NULL) ? anterior->siguiente : ppcb;
                    continue;
                }

                if (actual->ciclos == 0) {
                    actual->estado = 5;  /* Finished */
                    actual->tiempo_retorno = tiempo_total - actual->tiempo_llegada;
                } else {
                    actual->estado = 4;  /* Blocked */
                }
            }
            
            anterior = actual;
            actual = actual->siguiente;
        }

        /* Check for pending processes */
        actual = ppcb;
        hayPendientes = 0;
        while (actual != NULL) {
            if (actual->estado == 2 || actual->estado == 4) {
                hayPendientes = 1;
                break;
            }
            actual = actual->siguiente;
        }

        /* Reset blocked processes to ready */
        actual = ppcb;
        while (actual != NULL) {
            if (actual->estado == 4) {
                actual->estado = 2;
            }
            actual = actual->siguiente;
        }

    } while (hayPendientes);

    /* Print final statistics */
    cout << "\n=== RESUMEN DE EJECUCION ===\n";
    cout << "Tiempo total: " << tiempo_total << " ms\n\n";
    cout << "Tiempos de retorno por proceso:\n";
    cout << "-----------------------------------------\n";
    cout << "| Proceso | Tiempo de Retorno (ms) |\n";
    cout << "-----------------------------------------\n";
    
    actual = ppcb;
    float total_retorno = 0;
    int num_procesos = 0;

    while (actual != NULL) {
        cout << "| " << actual->proceso << "\t|\t" << actual->tiempo_retorno << " ms |\n";
        total_retorno += actual->tiempo_retorno;
        num_procesos++;
        actual = actual->siguiente;
    }

    cout << "-----------------------------------------\n";
    if (num_procesos > 0) {
        float promedio = total_retorno / num_procesos;
        cout << "Tiempo de retorno promedio: " << promedio << " ms\n";
    }
    cout << "Tiempo total de ejecucion: " << tiempo_total << " ms\n\n";
}



void ImprimirPCBSemaforo() {
    cout << "\n=== Lista de Procesos en PCBSemaforo ===\n";
    cout << "-------------------------------------------------------------------------------------------------\n";
    cout << "|Proceso|    T. Llegada    |   Ciclos   |    Estado |      E/S      |Ini.Int|Duracion|Interr.|\n";
    cout << "-------------------------------------------------------------------------------------------------\n";

    auxpcbsem = ppcbsem; // Puntero auxiliar para recorrer la lista
    while (auxpcbsem != NULL) {
        cout << auxpcbsem->proceso;
        cout << "\t\t" << auxpcbsem->tiempo_llegada << "t";
        cout << "\t\t" << auxpcbsem->ciclos << "ms";
        cout << "\t\t" << auxpcbsem->estado;
        cout << "\t\t   " << auxpcbsem->IniInt;
        cout << "\t   " << auxpcbsem->Duracion;
        cout << "\t   " << auxpcbsem->CodInt;


        auxpcbsem = auxpcbsem->siguiente; // Avanzar al siguiente proceso
    }
    cout << "-------------------------------------------------------------------------------------------------\n";
}