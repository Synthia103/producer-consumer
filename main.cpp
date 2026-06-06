#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#define ITER 1000
void *thread_increment(void *arg);
void *thread_decrement(void *arg);
int x;								// global variable for tracking buffer size
pthread_mutex_t m;					// mutual exclusion variable for global variable x
pthread_cond_t c_increment;			// condition variable for thread_increment()
pthread_cond_t c_decrement;			// condition variable for thread_decrement()

int main() {
	/* variable initialization */
	pthread_t tid1, tid2;			// tid1 for thread_increment, tid2 for thread_decrement
	pthread_mutex_init(&m, NULL);
	pthread_cond_init(&c_increment, NULL);
	pthread_cond_init(&c_decrement, NULL);

	pthread_create(&tid1, NULL, thread_increment, NULL);
	pthread_create(&tid2, NULL, thread_decrement, NULL);

	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);

	/* check x; if x == 0, success*/
	if(x != 0) 
		printf("BOOM! counter=%d\n", x);
	else
		printf("OK counter=%d\n", x);

	/* destroy variables */
	pthread_mutex_destroy(&m);
	pthread_cond_destroy(&c_increment);
	pthread_cond_destroy(&c_decrement);
}

/* thread routine */
void *thread_increment (void *arg) {
	int i, val;
	for(i = 0; i < ITER; i++) {
		pthread_mutex_lock(&m);
		/* -----critical section----- */
		while (x > 29) {
			pthread_cond_wait(&c_increment, &m);	// wait until x < 30
		}
		val = x;
		printf("%u: %d\n", (unsigned int)pthread_self(), val);
		x = val + 1;
		pthread_cond_signal(&c_decrement);
		/* -----critical section----- */
		pthread_mutex_unlock(&m);
	}
	pthread_exit(NULL);
}

void *thread_decrement (void *arg) {
	int i, val;
	for(i = 0; i < ITER; i++) {
		pthread_mutex_lock(&m);
		/* -----critical section----- */
		while (x <= 0) {
			pthread_cond_wait(&c_decrement, &m);	// wait until x > 0
		}
		val = x;
		printf("%u: %d\n", (unsigned int)pthread_self(), val);
		x = val - 1;
		pthread_cond_signal(&c_increment);
		/* -----critical section----- */
		pthread_mutex_unlock(&m);
	}
	pthread_exit(NULL);
}
