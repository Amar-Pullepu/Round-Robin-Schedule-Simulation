#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<malloc.h>
#include<time.h>
#include<pthread.h>
#include<semaphore.h>
struct Process {							//DETAILS OF PROCESS IS STORED IN A STRUCTURE
	int Rtime,Atime,Btime,Qtime,id;			//ARRIVALTIME,REMAINING TIME,BURST TIME,ID OF PROCESS
	clock_t arrival;						//IT STORES THE VALUE OF CPU CLOCK CYCLES WHEN IT ENTERED INTO THE PROCESS
	int flag,completed,Arrival_Flag;		//FEW FLAGS TO RUN THE PROGRAM PROPERLY
	sem_t se;								//SEMOPHORE OF THAT PROCESS
	struct Process *next;					//LINKLIST NEXT NODE ADDRESS
}; 
int i=0,z=1,l=1,TQ;							//TIME QUANTUM	
float WT=0,TAT=0;							//AVERAGE WAITING TIME AND TURN AROUND TIME
clock_t Start_Time,count;					//IT STORES THE CPU CLOCK CYCLES WHEN IT START EXECUTING THE PROCESSES AND THE VALUES AFTER EACH SECOND 
struct Process *front=NULL,*temp,*Front_P=NULL,*Rear_P=NULL;		//HEADERS AND TAILERS OF QUEUES
typedef struct Process node;				
void *processor(node *S) {					//PTHREAD FUNCTION OF OUR PROCESS
	int Preemtion_Flag=0,Null_Flag=0;		//TWO FLAG BITS TO RUN THE PROCESS CLEARLY
	while(1) {
		sem_wait(&S->se);					//SEMAPHORE WAIT FUNCTION SO THAT NO TWO PROCESSES WILL CONTEXT SWITCH IN BETWEEN
		if((S->Atime<=(clock()-Start_Time)/CLOCKS_PER_SEC && S->Arrival_Flag==1) || Preemtion_Flag==1) {
			S->Arrival_Flag=0;
			Preemtion_Flag=0;
			count=clock();
		} 
		if(S->flag==1) {
			printf("\nProcess-%d Started \t\t\t\t\tTimer :%d",S->id,(clock()-Start_Time)/CLOCKS_PER_SEC);
			S->flag=0;
			S->arrival=clock();
		}
		if((clock()-count)/CLOCKS_PER_SEC==1) {	//CHECKS THE VALUE PER SECOND
			count=clock();
			printf("\nProcess-%d Running\t\t\t\t\tTimer :%d",S->id,(clock()-Start_Time)/CLOCKS_PER_SEC);
			S->Rtime-=1;
			S->Qtime+=1;
			if(S->Rtime==0) {					//PROCESS TERMINATION CONDITION
				TAT+=(clock()-Start_Time)/CLOCKS_PER_SEC-S->Atime;			//ADDING TURNAROUND TIME OF THIS PROCESS
				WT+=((clock()-Start_Time)/CLOCKS_PER_SEC)-S->Atime-S->Btime;	//ADDING WAITING TIME OF THIS PROCESS
				if(Front_P!=Rear_P) {
					Null_Flag=1;
					Front_P=Front_P->next;
					sem_post(&Front_P->se);
				}
				S->completed=1;
				printf("\nProcess-%d completed executing, ",S->id);
				if(Front_P==Rear_P && Null_Flag==0){				
					Rear_P=NULL;
					Front_P=NULL;
					l=1;
				}
				else{
					printf("next Process is: %d",Front_P->id);
				}
			}
			else if(S->Qtime>=TQ && Front_P!=Rear_P) {				//CONTEXT SWITCHING CONDITION
				S->Qtime=0;
				Front_P=Front_P->next;
				Rpush(S);
				printf("\nProcess-%d Context Switched to Process-%d",S->id,Front_P->id);
				Preemtion_Flag=1;
				sem_post(&Front_P->se);			//INCREASING SEMAPHORE VALUE THAT WILL PREEMPT THE RUNNING PROCESS
				sem_wait(&S->se);				//MAKING THE PROCESS TO WAIT UNTIL IT REACHES ITS TURN IN QUEUE
			}
		}
		if(S->completed==1) {					//PROCESS COMPLETED FLAG IS TURNED TO '1' AND PROCESS TERMINATED
			break;
		}
		sem_post(&S->se);					//INCREASING SEMAPHORE TO RUN NEXT ITERATION
	}
}
void Rpush(node *temp) {					//PROCESSOR QUEUE WHERE ROUNDROBIN IS PERFORMED
	node *start=Front_P;
	if(Front_P==NULL) {
		Front_P=temp;
		Rear_P=temp;
		temp->next=NULL;
	}
	else {
		temp->next=NULL;
		Rear_P->next=temp;
		Rear_P=temp;
	}
}
void push() {							//THIS FUNCTION CREATES A QUEUE WHICH SORTS WRT ARRIVAL TIME
	temp=(node *)malloc(sizeof(node));
	printf("\nEnter Arrival Time of Process-%d:",i+1);
	scanf("%d",&temp->Atime);
	temp->id=i+1;
	printf("Enter Burst Time :");
	scanf("%d",&temp->Rtime);
	temp->Btime=temp->Rtime;
	sem_init(&temp->se,0,0);			//SET SEMAPHORE VALUE OF EVERY PROCESS AS ZERO INITIALLY
	int t=temp->Atime;
	temp->flag=1;
	temp->Qtime=0;
	temp->Arrival_Flag=1;
	node *Start=front;
	if ((Start->Atime > t)){
        	temp->next = front;
        	front=temp;
    	}
    	else {
        	while (Start->next != NULL && Start->next->Atime <= t) {
				Start = Start->next;
        	}
        temp->next = Start->next;
        Start->next = temp;
    }
}
void main() {
	int n,m=1;
	pthread_t p[10];
	printf("Enter No.of Processes :");				
	scanf("%d",&n);
	printf("Enter Time Quantum :");		//TO SHOW THE PROCESSES ARE RUNNING WE CONVERTED 1 MILLI SECOND = ONE SECOND
	scanf("%d",&TQ);
	while(i<n) {
		if(front==NULL) {
			front=(node *)malloc(sizeof(node));
			printf("\nEnter Arrival Time of Process-%d:",i+1);
			scanf("%d",&front->Atime);
			printf("Enter Burst Time :");
			scanf("%d",&front->Rtime);
			front->flag=1;
			front->Qtime=0;
			front->id=i+1;
			front->Arrival_Flag=1;
			front->Btime=front->Rtime;
			sem_init(&front->se,0,0);
			front->next=NULL;
		}
		else {
			push();
		}
	 	i++;
	}
	Start_Time=clock();					//NOTES THE TIME OF EXECUTION INTO START
	count=clock();
	i=0;		
	printf("\n\t\t\t\t\t\t\tTimer :0");
	while(front!=NULL) {
		temp=front;
		if(temp->Atime<0) {			//IF ARRIVAL TIME IS LESS THAN ZERO PROCESS WON'T EXECUTE
			printf("\nInvalid Arrival Time for Process-%d",temp->id);
			i++;
			front=front->next;
		}
		if((clock()-Start_Time)/CLOCKS_PER_SEC==temp->Atime) {		//PROCESS CREATED WHEN ITS ARRIVAL TIME IS REACHED
			if(l==1) {
				l=0;
				m=0;
				sem_post(&temp->se);								//SEMAPHORE VALUE OF FIRST PROCESS IS SET TO '1' SO IT WILL EXECUTE
			}
			pthread_create(&p[i],NULL,processor,temp);				//PROCESS IS CREATED HERE WITH AT ITS ARRIVAL TIME
			front=front->next;
			Rpush(temp);
			i++;
		}
		if(((clock()-count)/CLOCKS_PER_SEC==1 && Front_P==NULL)) { //TIMER PRINTS 
			count=clock();
			printf("\nNo Process is Running\t\t\t\t\tTimer :%d",(clock()-Start_Time)/CLOCKS_PER_SEC);
		}
	}
	for(i=0;i<n;i++) {
		pthread_join(p[i],NULL);
	}
	printf("\nAverage Waiting Time :%.2f\nAverage Turn Around Time :%.2f",(float)WT/n,(float)TAT/n);	//AVERAGE WAITING TIME AND TURNAROUND TIME IS CALCULATED
}

