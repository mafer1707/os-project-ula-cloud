#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include "orchestrator.h"

/* --- Variables Globales --- */
service_t dashboard[MAX_SERVICES];
int num_services = 0;
pthread_mutex_t dashboard_mutex;

//**  Esto es para imprimir los nombre de los estados y no sus números
const char *state_names[] = {
    "IDLE", 
    "RUNNING", 
    "CRASHED", 
    "KILLED",   
    "STOPPED"
};

/**
 * 
 * Función de utilidad para limpiar la terminal.
 */
void clear_screen() {
    printf("\033[H\033[J");
}

/**
 * TODO: Implementar la visualización del estado actual de los servicios.
 * Se debe garantizar una lectura consistente de los datos compartidos.
 */
void print_dashboard() {
    clear_screen();
    printf("==============================================================\n");
    printf("                ULA-CLOUD MONITORING DASHBOARD               \n");
    printf("==============================================================\n");
    printf("%-15s %-10s %-15s %-10s\n", "SERVICIO", "PID", "ESTADO", "EXIT/SIG");
    printf("--------------------------------------------------------------\n");

    // TODO: Renderizar cada fila del dashboard con la información actualizada.

    pthread_mutex_lock(&dashboard_mutex);
        for (int i = 0; i < num_services; i++){
            printf(" %-12s ",dashboard[i].name);
            printf(" %-10d ",dashboard[i].pid);
            printf(" %-15s ",state_names[dashboard[i].state]);
            printf(" %-10d ",dashboard[i].exit_status);     
            printf("\n");
        }        
    pthread_mutex_unlock(&dashboard_mutex);

    printf("==============================================================\n");
}

/**
 * TODO: Gestión de finalización del orquestador.
 * Implementar una estrategia para evitar la proliferación de procesos huérfanos.
 */
void handle_shutdown(int sig) {
    
    if(sig == SIGINT){
         // TODO: Notificar y limpiar recursos de procesos hijos.
        for (int i = 0; i < num_services; i++){

            if(dashboard[i].state == STATE_RUNNING){
                kill(dashboard[i].pid, SIGTERM);               
            }
        }        
    
        sleep(2);    
        exit(0);
    }
}

int main(int argc, char *argv[]) {
    // 1. Inicialización de mecanismos de sincronización
    if (pthread_mutex_init(&dashboard_mutex, NULL) != 0) {
        perror("Error inicializando mutex");
        return 1;
    }

    // 2. Captura de interrupciones del sistema
    signal(SIGINT, handle_shutdown);

    // --- Este código para leer los argumentos fue generado por Gemini ---

    size_t custom_limit = DEFAULT_MEM_LIMIT; 

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--mem") == 0 && (i + 1) < argc) {
            char *val_str = argv[i + 1];
            long num = atol(val_str); 

            if (strchr(val_str, 'M') || strchr(val_str, 'm')) {
                custom_limit = num * 1024 * 1024;
            } else {
                custom_limit = num; // Asume bytes si no hay letra
            }
            break;
        }
    }

    // ---

    // 3. Configuración de la carga de trabajo (Servicios de prueba)
    num_services = 3;
    
    strcpy(dashboard[0].name, "Logger");
    strcpy(dashboard[0].path, "./bin/logger");
    dashboard[0].mem_limit = custom_limit;

    strcpy(dashboard[1].name, "Chaos");
    strcpy(dashboard[1].path, "./bin/chaos");
    dashboard[1].mem_limit = custom_limit;

    strcpy(dashboard[2].name, "Leak");
    strcpy(dashboard[2].path, "./bin/leak");
    dashboard[2].mem_limit = 20 * 1024 * 1024; // Límite de 20MB

    // 4. Activación del ecosistema
    printf("[ULA-Cloud] Inicializando %d microservicios...\n", num_services);
    
    for (int i = 0; i < num_services; i++) {

        if(spawn_service(i) < 0) continue;

        pthread_t hilo_monitor;
        if(pthread_create(&hilo_monitor,NULL, monitor_service, &dashboard[i]) != 0) continue;

        pthread_detach(hilo_monitor);

        pthread_mutex_lock(&dashboard_mutex);
            dashboard[i].monitor_thread = hilo_monitor;
        pthread_mutex_unlock(&dashboard_mutex);


        /* * TODO: Orquestar el despliegue de servicios y su posterior 
         * monitoreo concurrente. 
         */
    }

    // 5. Ciclo de monitoreo principal
    while (1) {
        print_dashboard();
        sleep(1); 
    }

    pthread_mutex_destroy(&dashboard_mutex);
    return 0;
}
