#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define NUM_THREADS 4
#define NUM_ACCOUNTS 1
#define TRANSACTIONS_PER_TELLER 5
#define INITIAL_BALANCE 1000.0

// Shared data structure
typedef struct {
    int account_id;
    double balance;
    int transaction_count;
} Account;

// Global accounts array (shared resource)
Account accounts[NUM_ACCOUNTS];

// Thread function
void* teller_thread(void* arg) {
    int teller_id = *(int*)arg;
    // Unique random seed per thread
    unsigned int seed = time(NULL) + teller_id; 

    for (int i = 0; i < TRANSACTIONS_PER_TELLER; i++) {
        // 0 = deposit, 1 = withdraw
	int action = rand_r(&seed) % 2; 
        // $1–$200 range
	double amount = (rand_r(&seed) % 200) + 1;

        double old_balance = accounts[0].balance;

        if (action == 0) {
            accounts[0].balance = old_balance + amount;
            printf("Thread %d: Depositing %.2f (old: %.2f → new: %.2f)\n",
                   teller_id, amount, old_balance, accounts[0].balance);
        } else {
            accounts[0].balance = old_balance - amount;
            printf("Thread %d: Withdrawing %.2f (old: %.2f → new: %.2f)\n",
                   teller_id, amount, old_balance, accounts[0].balance);
        }

        accounts[0].transaction_count++;
	// small delay to make race conditions visible
        usleep(100000); 
    }

    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];

    // Initialize account
    accounts[0].account_id = 1;
    accounts[0].balance = INITIAL_BALANCE;
    accounts[0].transaction_count = 0;

    printf("Initial balance: %.2f\n\n", accounts[0].balance);

    // Create threads
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i + 1;
        pthread_create(&threads[i], NULL, teller_thread, &thread_ids[i]);
    }

    // Wait for all threads to finish
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("\nFinal balance: %.2f (Expected value will vary due to race conditions)\n",
           accounts[0].balance);

    return 0;
}
