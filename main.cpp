#include <iostream>
#include <thread>
#include <semaphore.h>
#include <chrono>
using namespace std;

#define MAX_CAPACITY 20
#define SOFA_SIZE 4
#define NUM_BARBER 3
#define MAX_TIME 100

// Counting Semaphores
sem_t max_capacity;
sem_t sofa;
sem_t barber_chair;
sem_t coord;

// Mutex or Binary Semaphores
sem_t cust_ready;
sem_t finished;
sem_t leave_b_chair;
sem_t payment;
sem_t receipt;

void init_semaphore() {
    sem_init(&max_capacity, 0, MAX_CAPACITY);
    sem_init(&sofa, 0, SOFA_SIZE);
    sem_init(&barber_chair, 0, NUM_BARBER);
    sem_init(&coord, 0, NUM_BARBER);

    sem_init(&cust_ready, 0, 0);
    sem_init(&finished, 0, 0);
    sem_init(&leave_b_chair, 0, 0);
    sem_init(&payment, 0, 0);
    sem_init(&receipt, 0, 0);
}

void destroy_semaphore() {
    sem_destroy(&max_capacity);
    sem_destroy(&sofa);
    sem_destroy(&barber_chair);
    sem_destroy(&coord);

    sem_destroy(&cust_ready);
    sem_destroy(&finished);
    sem_destroy(&leave_b_chair);
    sem_destroy(&payment);
    sem_destroy(&receipt);
}

class Customer {
    int id;
public:
    Customer(int &_id) : id(_id) {
        printf("Welcome Customer: %d\n", id);
    }

    void operator()(int _flag) {
        sem_wait(&max_capacity);
        printf("Enter Shop: %d\n", id);
        sem_wait(&sofa);
        printf("Sit on Sofa: %d\n", id);
        sem_wait(&barber_chair);
        printf("Get up from Sofa: %d\n", id);
        sem_post(&sofa);
        printf("Sit in Barber Chair: %d\n", id);
        sem_post(&cust_ready);
        sem_wait(&finished);
        printf("Leave Barber Chair: %d\n", id);
        sem_post(&leave_b_chair);
        printf("Payment Processing: %d\n", id);
        sem_post(&payment);
        sem_wait(&receipt);
        printf("Good Bye: %d\n", id);
        sem_post(&max_capacity);
    }
};

class Barber {
public:
    Barber(string &&msg) {
        cout << msg << endl;
    }

    void operator()() {
        while(true) {
            sem_wait(&cust_ready);
            printf("Customer is Ready\n");
            sem_wait(&coord);
            printf("Cutting Hair\n");
            sem_post(&coord);
            sem_post(&finished);
            sem_wait(&leave_b_chair);
            sem_post(&barber_chair);
            printf("One Cut Completed\n");
        }
    }
};

class Cashier {
public:
    Cashier(int _flag) {}

    void operator()() {
        while(true) {
            printf("Inside Payment Function\n");
            sem_wait(&payment);
            printf("Payment Started\n");
            sem_wait(&coord);
            printf("Payment Accepting\n");
            sem_post(&coord);
            sem_post(&receipt);
            printf("One Payment Processed\n");
        }
    }
};

int main() {

    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);

    init_semaphore();

    thread barber(Barber("This is a the Barber Shop"));
    thread cashier(Cashier(0));

    for(int _time=0; _time<MAX_TIME; _time++) {
        thread customer(Customer(_time), 0);
        customer.detach();
    }
    
    barber.join();
    cashier.join();

    destroy_semaphore();

    return 0;
}