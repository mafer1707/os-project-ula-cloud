#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

// Bandera global para un cierre controlado
volatile sig_atomic_t keep_running = 1;

/**
 * Manejador de señales para demostración.
 * Permite que el servicio cierre archivos antes de morir.
 */
void handle_sigterm(int sig) {
    (void)sig;
    keep_running = 0;
}

int main() {
    // Configurar el manejador de SIGTERM (enviado por el orquestador)
    signal(SIGTERM, handle_sigterm);

    FILE *log_file = fopen("service_logger.log", "a");
    if (log_file == NULL) {
        perror("Error al abrir el archivo de log");
        return 1;
    }

    printf("[Logger] Servicio iniciado. PID: %d\n", getpid());
    fprintf(log_file, "--- Sesión iniciada el %ld ---\n", (long)time(NULL));
    fflush(log_file);

    while (keep_running) {
        time_t now = time(NULL);
        struct tm *t = localtime(&now);

        // Escribir timestamp en el archivo
        fprintf(log_file, "[%02d:%02d:%02d] Logger vivo y reportando.\n", 
                t->tm_hour, t->tm_min, t->tm_sec);
        
        // Es vital hacer fflush para que los alumnos vean los cambios en tiempo real
        fflush(log_file);

        // Dormir 2 segundos
        sleep(2);
    }

    fprintf(log_file, "--- Servicio cerrado correctamente ---\n");
    fclose(log_file);
    printf("[Logger] Finalizando de forma limpia...\n");

    return 0;
}
