#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t mutex;
int sum = 0;


void sem_action(int current_number) {
    sum += current_number;
}
  
void* thread(void* arg)
{
    int current_number = *((int*)arg);

    sem_wait(&mutex);

    sem_action(current_number);
        
    sem_post(&mutex);
}
  
  
int main()
{
    int MAX_NUMER = 100;
    sem_init(&mutex, 0, 1);

    pthread_t t[MAX_NUMER];
    int threadNum[MAX_NUMER];
    for (int i=0; i< MAX_NUMER; i++)
        threadNum[i] = i + 1;

    for(int i=0;i<MAX_NUMER;i++) {
        pthread_create(&t[i],NULL,thread,&threadNum[i]);
    }
    for(int i=0;i<MAX_NUMER;i++)
        pthread_join(t[i],NULL);

    sem_destroy(&mutex);

    printf("the total sum is %d \n", sum);

    return 0;
}