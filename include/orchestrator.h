#ifndef ULA_CLOUD_H
#define ULA_CLOUD_H

#include <pthread.h>
#include <sys/types.h>
#include <stddef.h>

/* --- Constantes y Límites --- */
#define MAX_SERVICES 10
#define MAX_NAME_LEN 32
#define DEFAULT_MEM_LIMIT (50 * 1024 * 1024) // 50MB por defecto

/* --- Tipos de Datos --- */

/**
 * Estados posibles de un microservicio en el orquestador.
 */
typedef enum {
    STATE_IDLE,      // No ha sido lanzado
    STATE_RUNNING,   // En ejecución
    STATE_CRASHED,   // Terminó con error (exit code != 0)
    STATE_KILLED,    // Terminado por señal (ej: OOM Killer o SIGSEGV)
    STATE_STOPPED    // Terminado normalmente (exit code 0)
} service_state_t;

//

/**
 * Estructura principal de un servicio (El Dashboard).
 */
typedef struct {
    pid_t pid;                      // ID del proceso hijo
    char name[MAX_NAME_LEN];        // Nombre identificador
    char path[256];                 // Ruta al binario del servicio
    size_t mem_limit;               // Límite de memoria en bytes (setrlimit)
    service_state_t state;          // Estado actual
    int exit_status;                // Código de salida o señal recibida
    pthread_t monitor_thread;       // Hilo watchdog asignado
} service_t;

/* --- Variables Globales (Externas) --- */

// Array global que actúa como Dashboard de estado
extern service_t dashboard[MAX_SERVICES];
extern int num_services;

// Mutex para proteger el acceso al dashboard desde los hilos
extern pthread_mutex_t dashboard_mutex;

/* --- Prototipos de Funciones --- */

/**
 * Lanza un proceso hijo para un servicio y aplica sus límites.
 * Implementado en src/orchestrator.c
 */
int spawn_service(int index);

/**
 * Rutina del hilo monitor (watchdog).
 * Implementado en src/monitor.c
 */
void* monitor_service(void *arg);

/**
 * Configura los límites de recursos (setrlimit).
 * Implementado en src/resources.c
 */
void apply_resource_limits(size_t mem_limit);

/**
 * Muestra el estado actual de todos los servicios en consola.
 */
void print_dashboard();

#endif /* ULA_CLOUD_H */
