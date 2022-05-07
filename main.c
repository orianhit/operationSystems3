#include <stdio.h>
#include <sys/mman.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/sem.h>
#define MAX_NUMER 100

union semun
{
    int val;               /* Value for SETVAL */
    struct semid_ds *buf;  /* Buffer for IPC_STAT, IPC_SET */
    unsigned short *array; /* Array for GETALL, SETALL */
    struct seminfo *__buf; /* Buffer for IPC_INFO
                              (Linux-specific) */
};

sem_t mutex;
static int *sum;

void sem_action(int current_number)
{
    *sum += current_number;
    printf("proc\t%d\tsum\t%d\n", current_number, *sum);
}

void wait_until_value_max(int semid)
{
    struct sembuf *sops = (struct sembuf *)malloc(1 * sizeof(struct sembuf *));
    sops[0].sem_num = 0;
    sops[0].sem_flg = 0;
    sops[0].sem_op = -100;

    semop(semid, sops, 1);
    free(sops);
}

void increase_sem(int semid)
{
    struct sembuf *sops = (struct sembuf *)malloc(1 * sizeof(struct sembuf *));
    sops[0].sem_num = 0;
    sops[0].sem_flg = 0;
    sops[0].sem_op = 1;

    semop(semid, sops, 1);
    free(sops);
}

int init_sems()
{
    union semun smearg1;
    int semid = semget(IPC_PRIVATE, 1, 0600);
    smearg1.val = 0;

    semctl(semid, 1, SETVAL, smearg1);
    return semid;
}

void calc_sum(int semid, int current_number)
{
    sem_wait(&mutex);

    sem_action(current_number);

    sem_post(&mutex);
    increase_sem(semid);
}

int main()
{
    sem_init(&mutex, 0, 1);

    sum = mmap(NULL, sizeof *sum, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *sum = 0;

    int semid = init_sems();

    for (int i = 1; i <= MAX_NUMER; i++)
    {
        if (fork() == 0)
        {
            calc_sum(semid, i);
            exit(1);
        }
    }

    wait_until_value_max(semid);
    sem_destroy(&mutex);

    printf("the total sum is %d \n", *sum);
    munmap(sum, sizeof *sum);
    exit(1);

    return 0;
}