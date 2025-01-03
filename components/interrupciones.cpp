#include <iostream>
#include <stdlib.h>
#include <conio.h>
#include <cstring>
#include <stdio.h>
#include <string.h>
#define quantum 3
#define maxmem 50 // memoria en kb

struct Interrupciones {
    int codigo ;
    string desc;
}A[15]; 

struct PCB {
    char proceso[20];
    int tiempo_llegada;
    int ciclos;
    int estado;  // Cambiado a int para facilitar el manejo de estados
    int CicInt;
    int IniInt;
    int CodInt; 
    int Duracion;
    int tiempo_retorno;
    bool Interrpcion;
    int ciclos_transcurridos;
    PCB* siguiente;
};

struct tar {
    int estado;
    int tar_tar;
    tar* siguiente;
};

struct tareas {
    tar* subtareas;
    tareas* siguiente;
};

void crearJT();
void imprimirJT();
void crearListaPCB();
void imprimirListaPCB();
void Round_robin();  
void imprimirTiemposRetorno();
void Crear_vector(Interrupciones A[]);
void Eliminamiembro();

tareas* ptar = NULL, * qtar, * nuevotar, * auxtar;
PCB* ppcb = NULL, * qpcb, * nuevopcb, * auxpcb;

int main() {
    srand(time(NULL));
    crearJT();
    imprimirJT();

    crearListaPCB();
    imprimirListaPCB();
    system ("pause");
    Crear_vector(A);
    Round_robin();  // Llama a la nueva subrutina para procesar tareas
    imprimirTiemposRetorno();
    system("pause");
    return 0;
}

void crearJT() {
    int elementos =10;
    for (int i = 0; i < elementos; i++) {
        if (ptar == NULL) {
            ptar = new tareas;
            ptar->subtareas = NULL;
            int num_subtareas = 5;

            tar* ultimo_subtarea = NULL;
            for (int j = 0; j < num_subtareas; j++) {
                tar* nueva_subtarea = new tar;
                nueva_subtarea->tar_tar = j ;
                if(j==0){
                	nueva_subtarea->estado=1;
				}
				else{
                nueva_subtarea->estado = rand()%2;
                }
                nueva_subtarea->siguiente = NULL;

                if (ptar->subtareas == NULL) {
                    ptar->subtareas = nueva_subtarea;
                } else {
                    ultimo_subtarea->siguiente = nueva_subtarea;
                }
                ultimo_subtarea = nueva_subtarea;
                
            }

            ptar->siguiente = NULL;
            qtar = ptar;
        } else {
            nuevotar = new tareas;
            nuevotar->subtareas = NULL;
            int num_subtareas = 5;

            tar* ultimo_subtarea = NULL;
            for (int j = 0; j < num_subtareas; j++) {
                tar* nueva_subtarea = new tar;
                nueva_subtarea->tar_tar = j;
                if(j==0){
                	nueva_subtarea->estado=1;
				}
				else{
                nueva_subtarea->estado = rand()%2;
                }
                nueva_subtarea->siguiente = NULL;

                if (nuevotar->subtareas == NULL) {
                    nuevotar->subtareas = nueva_subtarea;
                } else {
                    ultimo_subtarea->siguiente = nueva_subtarea;
                }
                ultimo_subtarea = nueva_subtarea;
            }

            nuevotar->siguiente = NULL;
            qtar->siguiente = nuevotar;
            qtar = nuevotar;
        }
    }
}

void imprimirJT() {                                                                                                 
    cout << "Tabla de Tareas" << endl;
    int contador_tarea = 1;

    tareas* actual = ptar;

    while (actual != NULL) {
        cout << "Tarea " << contador_tarea << ":" << endl;
        int contador_seleccion = 0;

        tar* sub_actual = actual->subtareas;
        while (sub_actual != NULL) {
            cout << "  J" << contador_tarea << "P" << sub_actual->tar_tar
                 << "    " << sub_actual->estado << endl;

            if (sub_actual->estado == 1) {
                contador_seleccion++;
            }

            sub_actual = sub_actual->siguiente;
        }

        cout << "\nSeleccion: " << contador_seleccion << endl << endl;
        actual = actual->siguiente;
        contador_tarea++;
    }
}

void crearListaPCB() {
    int contador_tarea = 1;
    int cont=0;
    tareas* actual_tarea = ptar;

    while (actual_tarea != NULL) {
        tar* sub_actual = actual_tarea->subtareas;
        while (sub_actual != NULL) {
            if (sub_actual->estado == 1) {
                nuevopcb = new PCB;
                sprintf(nuevopcb->proceso, "J%dP%d", contador_tarea, sub_actual->tar_tar);
                nuevopcb->tiempo_llegada = cont;
                nuevopcb->ciclos = 2 + rand() % (7-2+1);
                nuevopcb->estado = 1;  // Inicializa todas las tareas en estado 2 (espera)
                nuevopcb->CicInt = 0+rand()%3; 
                nuevopcb->Interrpcion=0+rand()%2;
                if(nuevopcb->Interrpcion==1){
                    nuevopcb->CodInt = 0+rand ()%14;
                    nuevopcb->IniInt=2+rand()%(nuevopcb->ciclos-2+1);
                }else if(nuevopcb->Interrpcion==0){
                    nuevopcb->IniInt=0;
                    nuevopcb->CodInt = -1;
                }
                nuevopcb->CicInt=0;
                nuevopcb->siguiente = NULL;

                if (ppcb == NULL) {
                    ppcb = nuevopcb;
                    qpcb = ppcb;
                } else {
                    qpcb->siguiente = nuevopcb;
                    qpcb = nuevopcb;
                }
                cont++;
            }
            sub_actual = sub_actual->siguiente;
        }
        actual_tarea = actual_tarea->siguiente;
        contador_tarea++;
    }
}

void imprimirListaPCB() {
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

    // Inicializar todos los procesos a estado 2 (Listo)
    while (actual != NULL) {
        actual->estado = 2;
        actual = actual->siguiente;
    }

    bool hayPendientes;
    bool mostrarInterrupcion = false;
    PCB* procesoInterrumpido = NULL;

    do {
        hayPendientes = false;
        actual = ppcb;
        PCB* anterior = NULL; // Para manejar la eliminación de nodos

        while (actual != NULL) {
            if (actual->estado == 2) {
                actual->estado = 3; // Ejecutando
                system("cls");

                int ciclos_a_ejecutar = (actual->ciclos > quantum) ? quantum : actual->ciclos;
                int cont = 0;

                while (cont < ciclos_a_ejecutar && actual->ciclos > 0) {
                    system("cls");
                    imprimirListaPCB();
                    actual->ciclos--;
                    tiempo_total++;
                    cont++;
                    actual->ciclos_transcurridos++;

                    system("pause");

                    if (actual->ciclos_transcurridos == actual->IniInt || 
                        (actual->ciclos == 0 && actual->ciclos_transcurridos == actual->IniInt)) {
                        
                        mostrarInterrupcion = true;
                        procesoInterrumpido = actual;
                        break; 
                    }
                }

                if (mostrarInterrupcion) {
                    system("cls");
                    imprimirListaPCB();
                    cout << "\nCodigo de interrupcion: " << procesoInterrumpido->CodInt
                         << "\nError encontrado en " << procesoInterrumpido->proceso
                         << "\nError encontrado en el tiempo: " << procesoInterrumpido->IniInt
                         << "\nDescripcion: " << A[procesoInterrumpido->CodInt].desc
                         << "\n\nPresione cualquier tecla para continuar y eliminar la interrupcion...";

                    getch();

                    // Eliminar el proceso interrumpido de la lista
                    if (procesoInterrumpido == ppcb) {
                        ppcb = ppcb->siguiente; // Eliminar el primer nodo
                    } else {
                        anterior->siguiente = procesoInterrumpido->siguiente;
                    }

                    delete procesoInterrumpido; // Liberar memoria
                    procesoInterrumpido = NULL;
                    mostrarInterrupcion = false;

                    // Continuar con el siguiente nodo después de eliminar
                    actual = (anterior != NULL) ? anterior->siguiente : ppcb;
                    continue;
                }

                if (actual->ciclos == 0) {
                    actual->estado = 5; // Finalizado
                } else if (actual->estado != 6) {
                    actual->estado = 4; // Bloqueado
                }
            }

            anterior = actual;
            actual = actual->siguiente;
        }

        // Verificar si quedan procesos pendientes
        hayPendientes = false;
        actual = ppcb;
        while (actual != NULL) {
            if (actual->estado == 2 || actual->estado == 4) {
                hayPendientes = true;
                break;
            }
            actual = actual->siguiente;
        }

        // Reiniciar solo los procesos bloqueados (estado 4 a estado 2)
        actual = ppcb;
        while (actual != NULL) {
            if (actual->estado == 4) {
                actual->estado = 2;  // Reiniciar de Bloqueado a Listo
            }
            actual = actual->siguiente;
        }

    } while (hayPendientes);

    cout << "\nEjecucion completada. Tiempo total: " << tiempo_total << " ms.\n";
}

void imprimirTiemposRetorno() {
    cout << "\n\t\tTIEMPO DE RETORNO DE PROCESOS\n";
    cout << "-----------------------------------------\n";
    cout << "| Proceso | Tiempo de Retorno (ms) |\n";
    cout << "-----------------------------------------\n";

    int totalTiempoRetorno = 0;
    int cantidadProcesos = 0;
    PCB* actual = ppcb;

    while (actual != NULL) {
        cantidadProcesos++;
        totalTiempoRetorno += actual->tiempo_retorno;
        cout << "| " << actual->proceso << "\t|\t" << actual->tiempo_retorno << " ms |\n";
        actual = actual->siguiente;
    }

    cout << "-----------------------------------------\n";
        float promedioTiempoRetorno = static_cast<float>(totalTiempoRetorno) / cantidadProcesos;
        cout << "Tiempo de retorno promedio: " << promedioTiempoRetorno << " ms\n";
    

    cout << "Tiempo total de ejecucion: " << totalTiempoRetorno << " ms\n\n";
}