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

void safe_transfer(int from_id, int to_id, double amount) {
    printf("Thread %ld: Attempting transfer from %d to %d\n",
           pthread_self(), from_id, to_id);

    // Determine locking order (always lock the lower ID first)
    int first = (from_id < to_id) ? from_id : to_id;
    int second = (from_id < to_id) ? to_id : from_id;

    // Lock in order
    pthread_mutex_lock(&accounts[first].lock);
    printf("Thread %ld: Locked account %d (first)\n", pthread_self(), first);

    usleep(100000); // small delay to simulate real timing

    pthread_mutex_lock(&accounts[second].lock);
    printf("Thread %ld: Locked account %d (second)\n", pthread_self(), second);

    // Perform the transfer
    accounts[from_id].balance -= amount;
    accounts[to_id].balance += amount;
    printf("Thread %ld: Transferred %.2f from %d to %d\n",
           pthread_self(), amount, from_id, to_id);

    // Unlock in reverse order
    pthread_mutex_unlock(&accounts[second].lock);
    pthread_mutex_unlock(&accounts[first].lock);
}

void* thread1_func(void* arg) {
    //Transfer A to B
    safe_transfer(0, 1, 100.0); 
    return NULL;
}

void* thread2_func(void* arg) {
    //Transfer B to A 
    safe_transfer(1, 0, 100.0); 
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

    printf("Initial Balances: A = %.2f, B = %.2f\n\n",
           accounts[0].balance, accounts[1].balance);

    // Create threads
    pthread_create(&t1, NULL, thread1_func, NULL);
    pthread_create(&t2, NULL, thread2_func, NULL);

    // Wait for threads to finish (no deadlock now!)
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("\nFinal Balances: A = %.2f, B = %.2f\n",
           accounts[0].balance, accounts[1].balance);

    // Clean up
    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        pthread_mutex_destroy(&accounts[i].lock);
    }

    return 0;
}
