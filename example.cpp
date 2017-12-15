#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

int full;
pthread_mutex_t mx1;
pthread_mutex_t mx2;
pthread_cond_t cond;
int data;
FILE *f;

int produce()
{
    nanosleep((const struct timespec[]){{0, 500000000L}}, NULL);
    return rand() % 200;
}
void writeToFile(int cons, int processing, bool prod);

void *producer(void *)
{
    while (1)
    {
        int t = produce();
        pthread_mutex_lock(&mx1);
        while (full)
        {
            //printf("Wait consumer(Producer)\n");
            pthread_cond_wait(&cond, &mx1);
        }
        data = t;
        full = 1;

        writeToFile(t, 0, true);

        //printf("Work(Producer)\n");
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mx1);
    }
    return NULL;
}

void *consumer(void *)
{
    while (1)
    {
        int t;
        pthread_mutex_lock(&mx2);
        while (!full)
        {
            //printf("Wait producer(Consumer)\n");
            pthread_cond_wait(&cond, &mx2);
        }
        t = data;
        writeToFile(t, t * t * t, false);
        full = 0;

        //printf("Work(Consumer)\n");

        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mx2);
    }
    return NULL;
}

void writeToFile(int cons, int processing, bool prod)
{

    if (prod)
    {
        printf("produced %d\n", cons);
        fprintf(f, "produced %d\n", cons);
    }
    else
    {
        fprintf(f, "consumed %d\t\tprocessed %d\n", cons, processing);
        printf("consumed %d\t\tprocessed %d\n", cons, processing);
    }
}

int main()
{
    srand(time(0));
    pthread_t threads[2];

    f = fopen("out.txt", "w");
    printf("Enter any char to close the program\n");
    pthread_create(&threads[0], NULL, producer, NULL);
    pthread_create(&threads[1], NULL, consumer, NULL);

    getchar();
    fclose(f);

    return 0;
}