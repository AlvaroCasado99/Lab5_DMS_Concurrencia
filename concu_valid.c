#include <stdatomic.h>
#include <stddef.h>
#include <threads.h>
#include <stdio.h>

mtx_t lock;
/* Atomic so multiple threads can increment safely */
atomic_int completed = ATOMIC_VAR_INIT(0);
enum { max_threads = 5 };
 
int do_work(void *arg) {
    int i = *(int *)arg;

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
 
    return 0;
}
 
int main(void) {
    thrd_t threads[max_threads];
    int indexes[max_threads];
  
    printf("Inicializando el mutex.\n"); 
    if (thrd_success != mtx_init(&lock, mtx_plain)) {
        printf("Error al inicializar el mutex.\n");
        return -1;
    }

    for (size_t i = 0; i < max_threads; i++) {
        indexes[i] = i;
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
 
    printf("Destruyendo el mutex.\n");
    mtx_destroy(&lock);
    return 0;
}
