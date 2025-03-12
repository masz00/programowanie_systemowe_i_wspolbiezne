#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#define MAX_CEGLA_NA_TASMIE 10
#define MAX_WAGA_NA_TASMIE 10
#define WAGA_CEGLA_1 1
#define WAGA_CEGLA_2 2
#define LICZBA_CEGLA 20

volatile int tasma[MAX_CEGLA_NA_TASMIE];
volatile int w;
volatile int r; 
volatile int liczba_cegla_na_tasma;
volatile int waga_tasmy;
volatile int waga_cegly;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cProducent = PTHREAD_COND_INITIALIZER;
pthread_cond_t cKonsument = PTHREAD_COND_INITIALIZER;

void* producent(void *p) {
    for (int i = 1; i <= LICZBA_CEGLA; i++) {
        if (rand() % 2 == 0) {
            waga_cegly = WAGA_CEGLA_1;
        }
        else {
            waga_cegly = WAGA_CEGLA_2;
        }
        usleep((rand() % 10 + 1) * 10000);
        
        pthread_mutex_lock(&mutex);
        while (liczba_cegla_na_tasma == MAX_CEGLA_NA_TASMIE || waga_tasmy + waga_cegly > MAX_WAGA_NA_TASMIE) {
            pthread_cond_wait(&cProducent, &mutex);
        }
        
        tasma[w] = waga_cegly;
        
        w = (w + 1) % MAX_CEGLA_NA_TASMIE;
        liczba_cegla_na_tasma++;
        waga_tasmy += waga_cegly;
        printf("Cegla%d o wadze %d kg została UMIESZCZONA na tasmie, aktualna waga tasmy to: %d, kg.\n", i, waga_cegly, waga_tasmy);
        
        pthread_cond_signal(&cKonsument);
        pthread_mutex_unlock(&mutex);
    }
}

void* konsument(void *p) {
    for (int i = 1; i <= LICZBA_CEGLA; i++) {
        usleep((rand() % 10 + 1) * 10000);
        
        pthread_mutex_lock(&mutex);
        while (liczba_cegla_na_tasma == 0) {
            pthread_cond_wait(&cKonsument, &mutex);
        }
        
        int waga_cegly = tasma[r];
        
        r = (r + 1) % MAX_CEGLA_NA_TASMIE;
        liczba_cegla_na_tasma--;
        waga_tasmy -= waga_cegly;
        printf("Cegla%d o wadze %d kg została ZDJETA z tasmy, aktualna waga tasmy to: %d, kg. \n", i, waga_cegly, waga_tasmy);
         
        pthread_cond_signal(&cProducent);
        pthread_mutex_unlock(&mutex);
    }
}

int main() {
    pthread_t id1, id2;
    w = 0;
    r = 0;
    liczba_cegla_na_tasma = 0;
    waga_tasmy = 0;

    pthread_create(&id1, NULL, producent, NULL);
    pthread_create(&id2, NULL, konsument, NULL);
    pthread_exit(NULL);
}




