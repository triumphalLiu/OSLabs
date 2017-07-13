#ifndef __lib_h__
#define __lib_h__
#include "types.h"

void printf(char *format,...);
int sleep(unsigned int);
int fork();
int exit();
int sem_init(sem_t *sem, uint32_t value);
int sem_post(sem_t *sem);
int sem_wait(sem_t *sem);
int sem_destroy(sem_t *sem);
#endif
