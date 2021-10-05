#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

// The maximum number of customer threads.
#define MAX_CUSTOMERS 25
//Number of chairs
#define NUM_CHAIRS 10
// Function prototypes…
void *customer(void *num);
void *barber(void *);

void randwait(int secs);
void cuttime(int num);
void timearrive(int time);

// Define the semaphores.

// waitingRoom Limits the # of customers allowed
// to enter the waiting room at one time.
sem_t waitingRoom;

// barberChair ensures mutually exclusive access to
// the barber chair.
sem_t barberChair;

// barberPillow is used to allow the barber to sleep
// until a customer arrives.
sem_t barberPillow;

// seatBelt is used to make the customer to wait until
// the barber is done cutting his/her hair.
sem_t seatBelt;

// Stop program when all customers leave the barber
int allDone = 0;

int main(int argc, char *argv[]) {
  pthread_t btid;
  pthread_t tid[MAX_CUSTOMERS];
  long RandSeed;
  int i, numCustomers, numChairs;
  int Number[MAX_CUSTOMERS];

  printf("Enter the number of Custmors : "); scanf("%d",&numCustomers);
  
  numChairs = NUM_CHAIRS;
  // Make sure the number of threads is less than the number of customers 
  if (numCustomers > MAX_CUSTOMERS) {
  printf("The maximum number of Customers is %d.\n", MAX_CUSTOMERS);
  exit(-1);
  }

  // Initialize the numbers array.
  for (i=0; i<MAX_CUSTOMERS; i++) {
  Number[i] = i;
  }

  // Initialize the semaphores
  sem_init(&waitingRoom, 0, numChairs);
  sem_init(&barberChair, 0, 1);
  sem_init(&barberPillow, 0, 0);
  sem_init(&seatBelt, 0, 0);

  // Create the barber.
  pthread_create(&btid, NULL, barber, NULL);

  // Create the customers.
  for (i=0; i<numCustomers; i++) {
  pthread_create(&tid[i], NULL, customer, (void *)&Number[i]);
  sleep(1);
  }

  // Join each of the threads to wait for them to finish.
  for (i=0; i<numCustomers; i++) {
  pthread_join(tid[i],NULL);
  sleep(1);
  }

  // When all of the customers are finished, kill the
  // barber thread.
  allDone = 1;
  sem_post(&barberPillow); // Wake the barber so he will exit.
  pthread_join(btid,NULL);
  return 0;
}

void *customer(void *number) {
  int num = *(int *)number;
  int chairs;
  int wake;
  // Leave for the shop and take some random amount of
  printf("Customer %d is going to the barber shop.\n", num);
  // random time to arrive (0 - 3 seg).
  timearrive(rand() %3);
  printf("Customer %d arrived at barber shop.\n", num);

  // Wait for space to open up in the waiting room
  if (sem_trywait(&waitingRoom) == -1) {
      printf("Waiting room full. Customer %d is leaving.\n",num);
      return 0;
  }
  sem_getvalue(&waitingRoom,&chairs);
  printf("The number of empty chairs is %d\n",chairs);
  printf("Customer %d entering waiting room.\n", num);
  
  // Wait for the barber chair to become free.
  sem_wait(&barberChair);

  // The chair is free so give up your spot in the
  // waiting room.
  sem_post(&waitingRoom);
  
  // Wake up the barber if it is sleeping
  sem_getvalue(&barberPillow,&wake);
  if(wake==0){
      printf("Customer %d waking the barber.\n", num);
  sem_post(&barberPillow);
  }

  // Wait for the barber to finish cutting your hair.
  sem_wait(&seatBelt);

  // Give up the chair.
  sem_post(&barberChair);
  printf("Customer %d leaving barber shop.\n", num);
}

void *barber(void *junk) {
  // While there are still customers to be serviced the barber will be in the barber shop
  while (!allDone) {
  
    int customers;
    // If there are no customers to be served, the barber goes to sleep
    sem_getvalue(&waitingRoom,&customers);
    if(customers==10){
      printf("The barber is sleeping \n");
      sem_wait(&barberPillow);
    }
    
    if (!allDone) {
    // customer’s hair.
    printf("The barber is asking the hair cut style\n");
    //tThe cut hair style must be generated randomly for each client.
    cuttime(rand() %4);
    printf("The barber has finished cutting hair.\n");
    // Release the customer when done cutting
    sem_post(&seatBelt);
    }
    else {
    printf("The barber is going home for the day.\n");
    }
  }
}

void randwait(int secs) {
  int len;

  // Generate a random number…
  len = (int) ((1 * secs) + 1);
  sleep(len);
}

//time for a customer to arrive
void timearrive(int time){
  sleep(time);
}
//cutting hair time for each type there is a diferent waitng time
void cuttime(int num){
  printf("The customer want the haircut number %d\n",num);
  if (num==1){
    printf("The barber is cutting hair\n Waiting time is 500ms\n");
    sleep(500/1000);

  }else if (num==2){
     printf("The barber is cutting hair\n Waiting time is 1000ms\n");
    sleep(1);
  }else if (num==3){
     printf("The barber is cutting hair\n Waiting time is 2000ms\n");
    sleep(2);
  }else if (num==4){
     printf("The barber is cutting hair\n Waiting time is 3000ms\n");
    sleep(3);
  }
}