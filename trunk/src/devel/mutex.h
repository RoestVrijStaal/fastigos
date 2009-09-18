#define MUTEX_OK 0
#define MUTEX_ERROR 1

void *mutex_create();
void mutex_free(void *dir);
uint8_t mutex_try_lock(uint32_t *dir);
uint8_t mutex_try_unlock(uint32_t *dir);
