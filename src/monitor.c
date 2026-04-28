#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <pthread.h>
#include "orchestrator.h"

/**
 * TODO: Implementar la rutina del hilo monitor (Watchdog).
 * * Requisitos:
 * 1. Recuperar la estructura del servicio desde el argumento 'arg'.
 * 2. Utilizar 'waitpid' de forma BLOQUEANTE para esperar al proceso hijo.
 * 3. Analizar el estado de terminación (éxito, error o señal del sistema).
 * 4. Actualizar el Dashboard Global de forma SEGURA (evitar condiciones de carrera).
 */
void* monitor_service(void *arg) {

    service_t *service = (service_t *)arg;

    pid_t pid = service->pid;
    int status;
    int exit_res;
    service_state_t state_res;

    int res = waitpid(pid, &status, 0);

    if(res == -1){
        return NULL;
    }

    if(WIFEXITED(status)){

        exit_res = WEXITSTATUS(status);
        state_res = exit_res == 0 ? STATE_STOPPED : STATE_CRASHED;
    }
    else if(WIFSIGNALED(status)){

        exit_res = WTERMSIG(status);
        state_res = STATE_KILLED;

    }else{
        
        exit_res = -1;
        state_res = STATE_CRASHED;
    }
    

    pthread_mutex_lock(&dashboard_mutex);
        service->exit_status = exit_res;
        service->state = state_res;
    pthread_mutex_unlock(&dashboard_mutex);

    // TODO: Castear el argumento al tipo de dato correcto.
    
    // TODO: Implementar la espera del proceso específico.
    // Ayuda: Revisar el uso de waitpid(pid, &status, 0).

    /* * Una vez que waitpid retorna, el proceso hijo ha cambiado de estado.
     * TODO: Analizar el 'status' usando las macros de sys/wait.h:
     * - WIFEXITED: ¿Terminó normalmente?
     * - WEXITSTATUS: ¿Cuál fue su código de retorno?
     * - WIFSIGNALED: ¿Fue terminado por una señal (Segfault, OOM Killer)?
     * - WTERMSIG: ¿Qué señal lo mató?
     */

    /*
     * TODO: Actualizar el dashboard global.
     * ¡CRÍTICO!: El acceso al array 'dashboard' debe estar protegido. 
     * No olvides liberar el mecanismo de sincronización al terminar.
     */
    return NULL;
}
