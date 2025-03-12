#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define TOBACCO 0
#define PAPER 1
#define MATCHES 2
#define AGENT 3

volatile int money_tobacco = 20;
volatile int money_paper = 20;
volatile int money_matches = 20;

volatile int price_tobacco = 0;
volatile int price_paper = 0;
volatile int price_matches = 0;

int sem_id;

void seminit() {
    sem_id = semget(IPC_PRIVATE, 4, IPC_CREAT | 0666);
    semctl(sem_id, TOBACCO, SETVAL, 0);
    semctl(sem_id, PAPER, SETVAL, 0);
    semctl(sem_id, MATCHES, SETVAL, 0);
    semctl(sem_id, AGENT, SETVAL, 1);
}

void P(int sem_num) {
    struct sembuf op;
    op.sem_num = sem_num;
    op.sem_op = -1;
    op.sem_flg = 0;
    semop(sem_id, &op, 1);
}


void V(int sem_num) {
    struct sembuf op;
    op.sem_num = sem_num;
    op.sem_op = 1;
    op.sem_flg = 0;
    semop(sem_id, &op, 1);
}


void semkill() {
    semctl(sem_id, 0, IPC_RMID);
}

void *smokerWithTobacco(void *arg) {
    while (1) {
        P(TOBACCO);
        P(AGENT);
        

        if (money_tobacco < (price_paper + price_matches)) {
            printf("Smoker with tobacco is waiting for a better deal\n");
            V(AGENT);
            usleep((rand() % 10 + 1) * 100000);
            continue;
        }

        money_tobacco -= (price_paper + price_matches);
        money_paper += price_paper;
        money_matches += price_matches;
        printf("Smoker with tobacco bought paper and matches. Remaining money: %d\n", money_tobacco);
        V(MATCHES);
        V(PAPER);
        V(AGENT);
        usleep((rand() % 10 + 1) * 100000);
    }
    return NULL;
}

void *smokerWithPaper(void *arg) {
    while (1) {
        P(PAPER);
        P(AGENT);

        if (money_paper < (price_tobacco + price_matches)) {
            printf("Smoker with paper is waiting for a better deal...\n");
            V(AGENT);
            usleep((rand() % 10 + 1) * 100000);
            continue;
        }

        money_paper -= (price_tobacco + price_matches);
        money_tobacco += price_tobacco;
        money_matches += price_matches;
        printf("Smoker with paper bought tobacco and matches. Remaining money: %d\n", money_paper);
        V(MATCHES);
        V(TOBACCO);
        V(AGENT);
        
        usleep((rand() % 10 + 1) * 100000);
    }
    return NULL;
}

void *smokerWithMatches(void *arg) {
    while (1) {
        P(MATCHES);
        P(AGENT);

        if (money_matches < (price_tobacco + price_paper)) {
            printf("Smoker with matches is waiting for a better deal...\n");
            V(AGENT);
            usleep((rand() % 10 + 1) * 100000);
            continue;
        }

        money_matches -= (price_tobacco + price_paper);
        money_tobacco += price_tobacco;
        money_paper += price_paper;
        printf("Smoker with matches bought tobacco and paper. Remaining money: %d.\n", money_matches);
        V(TOBACCO);
        V(PAPER);
        V(AGENT);
        usleep((rand() % 10 + 1) * 100000);
    }
    return NULL;
}

void *agentFunc(void *arg) {
    while (1) {
        price_tobacco = rand() % 10 + 1;
        price_paper = rand() % 10 + 1;
        price_matches = rand() % 10 + 1;

        printf("Agent set new prices: Tobacco=%d, Paper=%d, Matches=%d\n", price_tobacco, price_paper, price_matches);

        V(TOBACCO);
        V(PAPER);
        V(MATCHES);

        usleep((rand() % 10 + 1) * 100000);
    }
    return NULL;
}

int main() {
    pthread_t smoker1, smoker2, smoker3, agent_thread;

    seminit();

    pthread_create(&smoker1, NULL, smokerWithTobacco, NULL);
    pthread_create(&smoker2, NULL, smokerWithPaper, NULL);
    pthread_create(&smoker3, NULL, smokerWithMatches, NULL);
    pthread_create(&agent_thread, NULL, agentFunc, NULL);

    pthread_join(smoker1, NULL);
    pthread_join(smoker2, NULL);
    pthread_join(smoker3, NULL);
    pthread_join(agent_thread, NULL);

    semkill();  

    return 0;
}