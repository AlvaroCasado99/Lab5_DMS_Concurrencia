#include <stdatomic.h>
#include <stddef.h>
#include <threads.h>
#include <stdio.h>
  
mtx_t lock;
/* Atomic so multiple threads can modify safely */
atomic_int completed = ATOMIC_VAR_INIT(0);
enum { max_threads = 5 };
 
int do_work(void *arg) {
    int i = *(int *)arg;
 
    if (i == 0) { /* Creation thread */
        printf("Hilo %d: Intentando inicializar el mutex.\n", i);
        
        if (thrd_success != mtx_init(&lock, mtx_plain)) {
            printf("Hilo %d: Error al inicializar el mutex.\n", i);
            return -1;
        }

        atomic_store(&completed, 1);
        printf("Hilo %d: Mutex inicializado.\n", i);

    } else if (i < max_threads - 1) { /* Worker thread */
        
        printf("Hilo %d: Intentando bloquear el mutex.\n", i);
        
        if (thrd_success != mtx_lock(&lock)) {
            printf("Hilo %d: Error al bloquear el mutex.\n", i);
            return -1;
        }
        
        /* Access data protected by the lock */
        printf("Hilo %d: Mutex bloqueado, incrementando contador.\n", i);
        atomic_fetch_add(&completed, 1);

        printf("Hilo %d: Desbloqueando el mutex.\n", i);
        if (thrd_success != mtx_unlock(&lock)) {
            printf("Hilo %d: Error al desbloquear el mutex.\n", i);
            return -1;
        }

    } else { /* Destruction thread */
        printf("Hilo %d: Intentando destruir el mutex.\n", i);
        mtx_destroy(&lock);
        printf("Hilo %d: Mutex destruido.\n", i);
    }
    return 0;
}
  
int main(void) {
    thrd_t threads[max_threads];
    int indexes[max_threads];
   
    for (size_t i = 0; i < max_threads; i++) {
        indexes[i]=i;
        if (thrd_success != thrd_create(&threads[i], do_work, &indexes[i])) {
            printf("Error al crear el hilo %zu.\n", i);
            return -1;
        }
    }

    for (size_t i = 0; i < max_threads; i++) {
        if (thrd_success != thrd_join(threads[i], 0)) {
            printf("Error al unir el hilo %zu.\n", i);
            return -1;
        }
    }
    return 0;
}
