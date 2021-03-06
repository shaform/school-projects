#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <cstdlib>
#include <cstdio>


using namespace std;
const int SIZE = 5;
const int GURU = 3;

class Monitor {
	friend class Condition;

	protected:
		sem_t mutex;
		sem_t next;
		int next_count;
		int in_mo;


		void print_critical()
		{
			printf("#########################\n");
			printf("%d process is in critical section\n", in_mo);
		}
		void entry_section()
		{
			sem_wait(&mutex);
			++in_mo;
		}
		void exit_section()
		{
			--in_mo;
			if (next_count > 0) {
				sem_post(&next);
			} else {
				sem_post(&mutex);
			}
		}


		
		Monitor()
			: next_count(0), in_mo(0)
		{
			sem_init(&mutex, 0, 1);
			sem_init(&next, 0, 0);
		}
		~Monitor()
		{
			sem_destroy(&mutex);
			sem_destroy(&next);
		}
};

class Condition {
	private:
		sem_t sem;
		int count;

		sem_t *mutex;
		sem_t *next;
		int *next_count;
		int *in_mo;
	public:
		void init(Monitor &mo)
		{
			mutex = &mo.mutex;
			next = &mo.next;
			next_count = &mo.next_count;
			in_mo = &mo.in_mo;
		}

		void signal()
		{
			if (count > 0) {
				++*next_count;
				sem_post(&sem);
			}
		}

		void wait()
		{
			--*in_mo;
			++count;
			if (*next_count > 0) {
				sem_post(next);
			} else {
				sem_post(mutex);
			}
			sem_wait(&sem);
			sem_wait(next);
			--*next_count;
			--count;
			++*in_mo;
		}

		Condition()
			: next(0), mutex(0), count(0)
		{
			sem_init(&sem, 0, 0);
		}
		~Condition()
		{
			sem_destroy(&sem);
		}
};

const char* PSTRING[] = {"THINKING", "HUNGRY", "EATING"};

class DinningPhilosopher : public Monitor {
	public:
		enum {THINKING, HUNGRY, EATING} state[SIZE];
		Condition self[SIZE];

		void pickup(int i)
		{
			entry_section();


			state[i] = HUNGRY;
			test(i);
			if (state[i] != EATING) {
				self[i].wait();
			}

			print();

			exit_section();
		}

		void putdown(int i)
		{
			entry_section();


			state[i] = THINKING;
			test((i + SIZE-1) % SIZE);
			test((i + 1) % SIZE);

			print();

			exit_section();
		}


		DinningPhilosopher()
		{
			for (int i = 0; i < SIZE; i++) {
				state[i] = THINKING;
				self[i].init(*this);
			}
		}
	private:
		void test(int i)
		{
			if ((state[(i + SIZE-1) % SIZE] != EATING) &&
					(state[i] == HUNGRY) &&
					(state[(i + 1) % SIZE] != EATING)) {
				state[i] = EATING;

				printf("#########################\nsignal");
				self[i].signal();
				printf(" and continue\n");
			}
		}
		void print()
		{
			printf("#########################\n");
			for (int i=0; i<SIZE; ++i) {
				printf("#%d philosopher is %s\n", i, PSTRING[state[i]]);
			}
			print_critical();
		}
};



DinningPhilosopher dp;

void *philosopher(void *t) {

	int *who = (int *) t;
	int guru = GURU;

	while (guru--) {
		sleep(rand() % 5 + 1);
		dp.pickup((*who));
		sleep(rand() % 5 + 1);
		dp.putdown((*who));
	}

	pthread_exit(0);
}

pthread_attr_t attr;

int main()
{
	printf("Testing the monitor with dinning philosophers...\n");
	printf("Press Enter to start.\n");
	getchar();

	pthread_attr_init(&attr);

	pthread_t pts[SIZE];
	int who[SIZE];
	for (int i=0; i<SIZE; ++i) {
		who[i] = i;
		pthread_create(&pts[i], &attr, philosopher, (void *) &who[i]);
	}

	for (int i=0; i<SIZE; ++i) {
		pthread_join(pts[i], 0);
	}

	printf("Testing finished...\n");

	pthread_exit(0);
}
