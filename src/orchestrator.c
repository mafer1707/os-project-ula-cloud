#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "orchestrator.h"

/**
 * TODO: Implementar el despliegue del microservicio.
 * * Requisitos:
 * 1. Crear un nuevo proceso.
 * 2. En el proceso HIJO:
 * - Configurar las restricciones de recursos (memoria).
 * - Reemplazar la imagen del proceso por el binario del servicio.
 * 3. En el proceso PADRE:
 * - Registrar el PID y el estado inicial en el dashboard.
 * - Retornar el PID asignado.
 */
int spawn_service(int index) {

    pid_t pid;

    char *path = dashboard[index].path;
    int mem_limit = dashboard[index].mem_limit;

    pid = fork();

    if(pid > 0){    // Proceso Padre

        pthread_mutex_lock(&dashboard_mutex);
            dashboard[index].pid = pid;
            dashboard[index].state = STATE_RUNNING;
        pthread_mutex_unlock(&dashboard_mutex);
    }
    else if(pid == 0){     // Proceso Hijo

        apply_resource_limits(mem_limit);

        char *args[] = {path, NULL};
        int res = execvp(path, args);

        if(res == -1){
            _exit(1);
        }
    }
    else{       //Error 

        pthread_mutex_lock(&dashboard_mutex);
            dashboard[index].pid = -1;
            dashboard[index].state = STATE_CRASHED;
        pthread_mutex_unlock(&dashboard_mutex);

        return -1;
    }

    // TODO: Invocar la creación del proceso hijo.

    // Casos a manejar:
    // - Error en la creación del proceso.
    // - Lógica del proceso HIJO (Setup de límites y Ejecución).
    // - Lógica del proceso PADRE (Gestión del dashboard).

    return pid; // Cambiar por el PID real
}
