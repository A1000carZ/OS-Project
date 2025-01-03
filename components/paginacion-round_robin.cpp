#include<iostream>
#include<stdlib.h>
#include<time.h>
#include<stdio.h>
#include<conio.h>
#include<math.h>
#include<vector>
#include<algorithm>
#include <iomanip.h>

#define quantum 3
#define mem 1.5
#define ma 64
#define lineas 100
#define N 20
#define tso 20

// Combined structure definitions
struct PCB {
    char proceso[20];
    int tiempo_llegada;
    int ciclos;
    int estado;  // 1: New, 2: Ready, 3: Running, 4: Blocked, 5: Finished
    int tiempo_retorno;
    int ntar; // Task number for memory management
    PCB* siguiente;
};

struct MMT {
    int marco, loc;
    int edo;
    struct MMT* sig;
};

struct JT {
    int ntar;
    int tamtar;
    int lpmt;
    int secuencia[8];
    struct JT* sig;
};

struct PMT {
    int pagina;
    int marco;
    int estado;
    int referencia;
    int modificacion;
    struct PMT* sig;
};

// Global pointers
MMT* pmmt = NULL, * qmmt, * nuevommt, * auxmmt;
JT* pjt = NULL, * qjt, * nuevojt, * auxjt;
PCB* ppcb = NULL, * qpcb, * nuevopcb, * auxpcb;
PMT* pp[N] = { NULL }, * qp[N], * nuevop[N], * auxp[N];

// Function prototypes
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
void imprimirListaPCB();
void Round_robin();
void inicializar_sistema();

// Main integration function

int main() {
    inicializar_sistema();
    
    cout << "Estado inicial del sistema:\n";
    imprime_MMT();
    imprime_JT();
    imprime_PMT();
    imprimirListaPCB();
    Round_robin();
    
    cout << "\nPresione cualquier tecla para salir...";
    getch();
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
    cout << "\n+----------+-----------+-------+\n";
    cout << "| No.Marco | Local.i   | Estado|\n";
    cout << "+----------+-----------+-------+\n";
    
    while (auxmmt != NULL) {
        cout << "| " << setw(8) << auxmmt->marco 
             << " | " << setw(9) << auxmmt->loc 
             << " | " << setw(5) << auxmmt->edo << " |\n";
        auxmmt = auxmmt->sig;
    }
    cout << "+----------+-----------+-------+\n";
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
    cout << "\n+----------+----------+----------+-----------------+\n";
    cout << "| No.Tarea | Lineas   | Loc.PMT  | Secuencia      |\n";
    cout << "+----------+----------+----------+-----------------+\n";
    
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
        PMT* auxp = pp[i];
        bool tieneDatos = false;
        
        while (auxp != NULL) {
            if (auxp->marco != 0) {
                tieneDatos = true;
                break;
            }
            auxp = auxp->sig;
        }
        
        if (tieneDatos) {
            cout << "\n=== PMT de la tarea " << i << " ===\n";
            
            JT* auxjt = pjt;
            while (auxjt != NULL && auxjt->ntar != i) {
                auxjt = auxjt->sig;
            }
            
            if (auxjt != NULL) {
                cout << "J" << i << ": Secuencia=[";
                for (int j = 0; j < 8; j++) {
                    cout << "P" << auxjt->secuencia[j];
                    if(j < 7) cout << " ";
                }
                cout << "]\n\n";
            }

            cout << "+----------+-------+---------+--------+-----------+-------------+\n";
            cout << "| No.Pag   | Marco | Local.i | Estado | Referenc. | Modificac. |\n";
            cout << "+----------+-------+---------+--------+-----------+-------------+\n";
            
            auxp = pp[i];
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

void inicializar_sistema() {
    srand(time(NULL));
    int marcos_totales = calc_marcos();
    
    // Initialize Memory Management
    crearMMT(marcos_totales);
    asigna_so();
    crearJT();
    
    // Create test processes and link them to memory tasks
    for(int i = 0; i < 5; i++) {
        nuevopcb = new PCB;
        sprintf(nuevopcb->proceso, "P%d", i+1);
        nuevopcb->tiempo_llegada = i;
        nuevopcb->ciclos = 2 + rand() % 6;
        nuevopcb->estado = 1;
        nuevopcb->ntar = i;  // Link to memory task
        nuevopcb->siguiente = NULL;

        if (ppcb == NULL) {
            ppcb = nuevopcb;
            qpcb = ppcb;
        } else {
            qpcb->siguiente = nuevopcb;
            qpcb = nuevopcb;
        }
        
        // Assign memory pages for this process in the pcb
        auxjt = pjt;
        while (auxjt != NULL && auxjt->ntar != i) {
            auxjt = auxjt->sig;
        }
        if (auxjt != NULL) {
            asigna_pag(auxjt->ntar, auxjt->tamtar);
        }
    }
    
    asigna_paginas_con_secuencia();
}
void imprimirListaPCB() {
    cout << "\n+----------+-----------+--------+--------+\n";
    cout << "| Proceso | T.Llegada | Ciclos | Estado |\n";
    cout << "+----------+-----------+--------+--------+\n";

    PCB* actual = ppcb;
    while (actual != NULL) {
        cout << "| " << setw(8) << actual->proceso 
             << " | " << setw(9) << actual->tiempo_llegada << "t"
             << " | " << setw(6) << actual->ciclos << "ms"
             << " | " << setw(6) << actual->estado << " |\n";
        actual = actual->siguiente;
    }
    cout << "+----------+-----------+--------+--------+\n";
}

// Modified Round Robin to include memory management information ...
void Round_robin() {
    PCB* actual;
    int tiempo_total = 0;
    actual = ppcb;

    while (actual != NULL) {
        actual->estado = 2;
        actual = actual->siguiente;
    }

    bool hayPendientes;
    do {
        hayPendientes = false;
        actual = ppcb;

        while (actual != NULL) {
            if (actual->estado == 2) {
                actual->estado = 3;

                int ciclos_a_ejecutar = (actual->ciclos > quantum) ? quantum : actual->ciclos;

                for(int i = 0; i < ciclos_a_ejecutar; i++) {
                    
                    cout << "\n=== ESTADO DEL SISTEMA ===\n";
                    imprimirListaPCB();
                    
                    // Display memory information for current process
                    cout << "\n=== INFORMACION DE MEMORIA DEL PROCESO ===\n";
                    cout << "Proceso: " << actual->proceso << "\n";
                    cout << "Numero de tarea en memoria: " << actual->ntar << "\n";
                    
                    // Show assigned pages
                    PMT* auxp = pp[actual->ntar];
                    if (auxp != NULL) {
                        cout << "Paginas asignadas:\n";
                        while (auxp != NULL) {
                            if (auxp->estado == 1) {
                                cout << "Pagina " << auxp->pagina << " en marco " << auxp->marco << "\n";
                            }
                            auxp = auxp->sig;
                        }
                    }
                    
                    actual->ciclos--;
                    tiempo_total++;
                    
                    cout << "\nCiclos restantes: " << actual->ciclos;
                    cout << "\nTiempo total: " << tiempo_total;
                    cout << "\n\nPresione cualquier tecla para continuar...";
                    getch();
                }

                if (actual->ciclos == 0) {
                    actual->estado = 5;
                    actual->tiempo_retorno = tiempo_total - actual->tiempo_llegada;
                } else {
                    actual->estado = 4;
                }
            }
            actual = actual->siguiente;
        }

        actual = ppcb;
        while (actual != NULL) {
            if (actual->estado == 2 || actual->estado == 4) {
                hayPendientes = true;
                break;
            }
            actual = actual->siguiente;
        }

        actual = ppcb;
        while (actual != NULL) {
            if (actual->estado == 4) {
                actual->estado = 2;
            }
            actual = actual->siguiente;
        }

    } while (hayPendientes);

    cout << "\n=== RESUMEN DE EJECUCION ===\n";
    cout << "Tiempo total: " << tiempo_total << " ms\n\n";
    cout << "Tiempos de retorno por proceso:\n";
    cout << "--------------------------------\n";
    actual = ppcb;
    float total_retorno = 0;
    int num_procesos = 0;
    
    while (actual != NULL) {
        cout << actual->proceso << ": " << actual->tiempo_retorno << " ms\n";
        total_retorno += actual->tiempo_retorno;
        num_procesos++;
        actual = actual->siguiente;
    }
    
    cout << "--------------------------------\n";
    cout << "Tiempo promedio de retorno: " << total_retorno/num_procesos << " ms\n";
}

