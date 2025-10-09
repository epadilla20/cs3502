#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_ACCOUNTS 2
#define INITIAL_BALANCE 1000.0

typedef struct {
    int account_id;
    double balance;
    pthread_mutex_t lock;
} Account;

Account accounts[NUM_ACCOUNTS];

void transfer(int from_id, int to_id, double amount) {
    printf("Thread %ld: Attempting transfer from %d to %d\n",
           pthread_self(), from_id, to_id);

    // Lock the first account
    pthread_mutex_lock(&accounts[from_id].lock);
    printf("Thread %ld: Locked account %d\n", pthread_self(), from_id);

    // Simulate processing delay to allow other thread to cause deadlock
    usleep(100000);

    printf("Thread %ld: Waiting for account %d\n", pthread_self(), to_id);
    pthread_mutex_lock(&accounts[to_id].lock);

    // If this line is reached, no deadlock occurred this time
    accounts[from_id].balance -= amount;
    accounts[to_id].balance += amount;

    pthread_mutex_unlock(&accounts[to_id].lock);
    pthread_mutex_unlock(&accounts[from_id].lock);
}

void* thread_func(void* arg) {
    int* ids = (int*)arg;
    int from_id = ids[0];
    int to_id = ids[1];
    transfer(from_id, to_id, 100.0);
    return NULL;
}

int main() {
    pthread_t t1, t2;

    // Initialize accounts
    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        accounts[i].account_id = i;
        accounts[i].balance = INITIAL_BALANCE;
        pthread_mutex_init(&accounts[i].lock, NULL);
    }

    // Thread argument arrays
    // Transfer A to B, B to A
    int args1[2] = {0, 1}; 
    int args2[2] = {1, 0};

    printf("Initial Balances: A = %.2f, B = %.2f\n\n",
           accounts[0].balance, accounts[1].balance);

    // Create two threads that will deadlock
    pthread_create(&t1, NULL, thread_func, args1);
    pthread_create(&t2, NULL, thread_func, args2);

    // Wait for threads (the program will hang due to deadlock)
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("\nFinal Balances: A = %.2f, B = %.2f\n",
           accounts[0].balance, accounts[1].balance);

    // Clean up (won’t be reached if deadlock occurs)
    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        pthread_mutex_destroy(&accounts[i].lock);
    }

    return 0;
}
