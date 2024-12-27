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
                    nuevopcb->IniInt=1+rand()%(nuevopcb->ciclos-1+1);
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
    cout << "----------------------------------------------------------------------------------------------" << endl;
    cout << "|Proceso|    T. Llegada    |   Ciclos  |      CPU-E/S     |   Estado |Ini.Int|Duracion|Interr.|" << endl;
    cout << "----------------------------------------------------------------------------------------------" << endl;

    PCB* actual = ppcb;
    while (actual != NULL) {
        // Imprimir el proceso si no tiene interrupción activa
        cout << actual->proceso
             << "\t\t" << actual->tiempo_llegada << "t"
             << "\t\t" << actual->ciclos << "ms";
             if(actual->Interrpcion==true)
                cout<<"\t\tE/S";
             else if(actual->Interrpcion==false)
                cout<<"\t\tCPU";
        cout << "\t\t" << actual->estado
             << "\t" << actual->IniInt
             << "\t" << actual->Duracion
             << "\t" << actual->CodInt
             << endl;

        actual = actual->siguiente; // Avanzar al siguiente proceso
    }
    cout << "----------------------------------------------------------------------------------------------" << endl;
}



void Crear_vector(Interrupciones A[]){
   A[0].codigo= 10; 
   A[0].desc= "Leer caracter ampliado e/s \n";
   A[1].codigo=2;
   A[1].desc="Interrupcion no enmascarable\n";
   A[2].codigo=63;
   A[2].desc="Gestor Overlay\n";
   A[3].codigo=75;
   A[3].desc="Reservada para BIOS\n";
   A[4].codigo=9;
   A[4].desc="Generados por accion del teclado\n";
   A[5].codigo=8;
   A[5].desc="Obtener atributo y caracter en el cursor\n";
   A[6].codigo=11;
   A[6].desc="Asignar paleta de colores\n";
   A[7].codigo=15;
   A[7].desc="Obtener buffer del video\n";
   A[8].codigo= 31; 
   A[8].desc= "Apuinta a los  caracteres graficos del CGA\n";
   A[9].codigo= 31; 
   A[9].desc= "Apuinta a los  caracteres graficos del CGA\n";
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

        while (actual != NULL) {
            if (actual->estado == 2 || actual->estado == 4) {
                actual->estado = 3;  
                system("cls");

                if (actual->ciclos > quantum) {
                    int cont = 0;
                    while (cont < quantum) {
                        system("cls");
                        imprimirListaPCB();
                        actual->ciclos--;
                        tiempo_total++;
                        cont++;
                        actual->ciclos_transcurridos++;
                        system("pause");
                        
                        if (actual->ciclos_transcurridos == actual->IniInt) {
                            mostrarInterrupcion = true;
                            procesoInterrumpido = actual;
                            break; 
                        }
                    }

                    
                    if (mostrarInterrupcion) break;

                    actual->estado = 4;  
                } else {  
                    while (actual->ciclos > 0) {
                        system("cls");
                        imprimirListaPCB();
                        actual->ciclos--;
                        tiempo_total++;
                        actual->ciclos_transcurridos++;
                        system("pause");
                        if (actual->ciclos_transcurridos == actual->IniInt) {
                            mostrarInterrupcion = true;
                            procesoInterrumpido = actual;
                            break;  
                        }
                    }
                    if (mostrarInterrupcion) break;

                    actual->estado = 5;   
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

                
                if (ppcb == procesoInterrumpido) {
                    ppcb = ppcb->siguiente;
                } else {
                    PCB* prev = ppcb;
                    while (prev->siguiente != procesoInterrumpido) {
                        prev = prev->siguiente;
                    }
                    prev->siguiente = procesoInterrumpido->siguiente;
                }
                delete procesoInterrumpido;
                procesoInterrumpido = NULL;
                mostrarInterrupcion = false;
                break;   
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