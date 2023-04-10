#include<stdio.h>
#include <stdlib.h>

typedef struct Process {
	int pid;
	int arrival;
	int burst;
	int remaining;
	int tat;
	int wait;
} Process;

typedef struct Node {
	Process *p;
	struct Node * next;
} Node;

typedef struct Queue {
	struct Node* front ;
	struct Node* rear ;
} Queue;

void enqueue(Queue *q, Process *p) {

	Node* newnode = (Node*) malloc(sizeof(Node));

	newnode->p = p;
	newnode->next = NULL;

	if (q->front == NULL)
		q->front = newnode;
	else
		q->rear->next = newnode;

	q->rear = newnode;
}

Process* dequeue(Queue *q) {
	if (q->front == NULL)
		return NULL;

	Process *p = q->front->p;

	Node *todelete = q->front;

	q->front = q->front->next;
	if (q->front == NULL)
		q->rear = NULL;

	free(todelete);

	return p;
}

void sortByArrival(int n, Process *p) {
	Process temp;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n - 1 - i; j++ ) {
			if (p[j].arrival > p[j + 1].arrival) {
				temp = p[j];
				p[j] = p[j + 1];
				p[j + 1] = temp;
			}
		}
	}
}


int main() {

	Queue *rq = (Queue*) malloc(sizeof(Queue));
	rq->front = NULL;
	rq->rear = NULL;

	int n, quantum;
	printf("Enter the no. of processes: ");
	scanf("%d", &n);

	printf("Enter the quantum: ");
	scanf("%d", &quantum);


	Process p[n];
	for (int i = 0; i < n ; i++) {

		printf("Enter the arrival and burst time: ");
		scanf("%d %d", &p[i].arrival, &p[i].burst);

		p[i].pid = i + 1;
		p[i].remaining = p[i].burst;

	}

	sortByArrival(n, p);

	int count = n, current_time = 0, i_enq = 0;
	float avgtat = 0, avgwait = 0;
	int context_switches = 0;

//enqueueing all the processes arriving at time 0
	for ( i_enq = 0; i_enq < n; i_enq ++) {
		if (p[i_enq].arrival == 0)
			enqueue(rq, &p[i_enq]);
		else break;
	}

	printf("\nPID\tAT\tBT\tTAT\tWT\t");

	while (count > 0) {

		if (rq->front == NULL) //if no process is arrived yet
		{
			current_time++;
			for ( ; i_enq < n; i_enq ++) {
				if (p[i_enq].arrival <= current_time)
					enqueue(rq, &p[i_enq]);
				else break;
			}

			continue;
		}

		Process *current_proc = rq->front->p;

		if (current_proc->remaining <= quantum) //process completed within one quantum
		{
			current_time += current_proc->remaining ;
			for ( ; i_enq < n; i_enq ++) {
				if (p[i_enq].arrival <= current_time)
					enqueue(rq, &p[i_enq]);
				else break;
			}

			current_proc->remaining = 0;

			current_proc->tat = current_time - current_proc->arrival;
			current_proc->wait = current_proc->tat - current_proc->burst;

			printf("\n%d\t%d\t%d\t%d\t%d\t",
			       current_proc->pid, current_proc->arrival, current_proc->burst, current_proc->tat, current_proc->wait);

			avgtat += current_proc->tat;
			avgwait += current_proc->wait;

			dequeue(rq);
			if (rq->front != NULL)
				context_switches++;

			count--;
		}
		else  {
			current_time += quantum;

			for ( ; i_enq < n; i_enq ++) {
				if (p[i_enq].arrival <= current_time)
					enqueue(rq, &p[i_enq]);
				else break;
			}

			current_proc->remaining -= quantum;

			Process *toEnq = dequeue(rq);
			if (rq->front != NULL)
				context_switches++;
			enqueue(rq, toEnq );
		}


	}

	avgtat /= n;
	avgwait /= n;

	printf("\nAverage TAT: %f", avgtat);
	printf("\nAverage Wait: %f", avgwait);
	printf("\nContext switches: %d", context_switches);

	free(rq);
	return 0;
}
