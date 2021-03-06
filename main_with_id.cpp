#include <iostream>
#include <thread>
#include <semaphore.h>
#include <chrono>
#include <queue>
#include<unistd.h>
using namespace std;

#define MAX_CAPACITY 20
#define SOFA_SIZE 4
#define NUM_BARBER 3
#define MAX_TIME 50

// countering Semaphores
sem_t max_capacity;
sem_t sofa;
sem_t barber_chair;
sem_t coord;

// Mutex or Binary Semaphores
sem_t cust_ready;
sem_t finished[MAX_TIME];
sem_t leave_b_chair;
sem_t payment;
sem_t receipt;

// Mutex Semaphore
sem_t mutex1;
sem_t mutex2;

// Identity of Customer
int counter = 0;

// Queue of Locks
queue<int> q;

void init_semaphore() {
    sem_init(&max_capacity, 0, MAX_CAPACITY);
    sem_init(&sofa, 0, SOFA_SIZE);
    sem_init(&barber_chair, 0, NUM_BARBER);
    sem_init(&coord, 0, NUM_BARBER);

    sem_init(&cust_ready, 0, 0);
    for(int i=0; i<MAX_TIME; i++)
        sem_init(&finished[i], 0, 0);
    sem_init(&leave_b_chair, 0, 0);
    sem_init(&payment, 0, 0);
    sem_init(&receipt, 0, 0);

    sem_init(&mutex1, 0, 1);
    sem_init(&mutex2, 0, 1);
}

void destroy_semaphore() {
    sem_destroy(&max_capacity);
    sem_destroy(&sofa);
    sem_destroy(&barber_chair);
    sem_destroy(&coord);

    sem_destroy(&cust_ready);
    for(int i=0; i<MAX_TIME; i++)
        sem_destroy(&finished[i]);
    sem_destroy(&leave_b_chair);
    sem_destroy(&payment);
    sem_destroy(&receipt);

    sem_destroy(&mutex1);
    sem_destroy(&mutex2);
}

void customer() {
    sem_wait(&mutex1);
    int id = counter++;
    sem_post(&mutex1);
    sem_wait(&max_capacity);
    printf("Enter Shop: %d\n", id);
    sem_wait(&sofa);
    printf("Sit on Sofa: %d\n", id);
    sem_wait(&barber_chair);
    printf("Get up from Sofa: %d\n", id);
    sem_post(&sofa);
    printf("Sit in Barber Chair: %d\n", id);
    sem_wait(&mutex2);
    q.push(id);
    sem_post(&mutex2);
    sem_post(&cust_ready);
    sem_wait(&finished[id]);
    printf("Leave Barber Chair: %d\n", id);
    sem_post(&leave_b_chair);
    printf("Payment Processing: %d\n", id);
    sem_post(&payment);
    sem_wait(&receipt);
    printf("Good Bye: %d\n", id);
    sem_post(&max_capacity);
    cout << flush;
}

void barber() {
    while(true) {
        sem_wait(&cust_ready);
        printf("Customer is Ready\n");
        sem_wait(&coord);
        sem_wait(&mutex2);
        int id = q.front();
        q.pop();
        sem_post(&mutex2);
        printf("Cutting Hair %d\n", id);
        sem_post(&coord);
        sem_post(&finished[id]);
        sem_wait(&leave_b_chair);
        sem_post(&barber_chair);
        printf("One cut completed\n");
        cout << flush;
    }
}

void cashier() {
    while(true) {
        printf("Inside Payment Section\n");
        sem_wait(&payment);
        printf("Payment Started\n");
        sem_wait(&coord);
        printf("Payment Accepting\n");
        sem_post(&coord);
        sem_post(&receipt);
        printf("One Payment Processed\n");
        cout << flush;
    }
}

int main() {

    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);

    init_semaphore();

    thread barber_thread(barber);
    thread cashier_thread(cashier);

    for(int _time=0; _time<MAX_TIME; _time++) {
        thread customer_thread(customer);
        customer_thread.detach();
        if(_time%10==0)
            sleep(1);
    }
    
    barber_thread.join();
    cashier_thread.join();

    destroy_semaphore();

    return 0;
}