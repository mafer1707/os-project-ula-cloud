#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include "orchestrator.h"

/**
 * TODO: Implementar la restricción de recursos del sistema.
 * * Requisitos:
 * 1. Definir los límites (soft y hard) para el espacio de direccionamiento virtual.
 * 2. Aplicar el límite utilizando la syscall 'setrlimit'.
 * 3. Garantizar que si la configuración falla, el proceso no continúe su ejecución.
 * * Ayuda: Revisar la constante RLIMIT_AS en la documentación de Linux.
 */
void apply_resource_limits(size_t mem_limit) {
    
    struct rlimit rl;

    rl.rlim_cur = mem_limit;
    rl.rlim_max = mem_limit;
    int res = setrlimit(RLIMIT_AS, &rl);

    if(res < 0){
        exit(1);
    }

    // TODO: Configurar la estructura rlimit y ejecutar la syscall.
    
    // Casos a considerar:
    // - ¿Qué diferencia hay entre rlim_cur y rlim_max?
    // - ¿Qué sucede si el límite solicitado es menor al tamaño del propio binario?
}
