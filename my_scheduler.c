#include "threads.h"
#define ERROR(Str) fprintf(stderr, "%s\n", Str), exit(1)
extern int scheduling_type;
extern thread_queue_t *ready_list;

void thread_enqueue(thread_t *t, thread_queue_t *q){
    //allocate node and give it the thread
    thread_node_t *node = malloc(sizeof(*node));
    node->thread = t;
    
    //check if queue is empty
    if(q->size == 0){
      q->head = node;
      q->tail = node;
    }
    
    else{
      q->tail->next = node;
      q->tail = node;
    }
    
    q->size++;
}

thread_t *thread_dequeue(thread_queue_t *q){
    if(q->size == 0){
      ERROR("List empty, cannot dequeue element");
    }
    
    //grab next element after head
    thread_node_t *newHead = malloc(sizeof(*newHead));
    newHead = q->head->next;
    
    //remove the head
    thread_node_t *oldHead = malloc(sizeof(*oldHead));
    oldHead = q->head;
    
    //check case where list was only one element
    if(q->size == 1){
      q->tail = NULL;
      q->size = 0;
      return oldHead->thread;
    }
    
    //set new head
    q->head = newHead;
    q->size--;
    return oldHead->thread;
}

thread_t* scheduler(){
    switch(scheduling_type)
    {
        case 0: {          // Round Robin
	    //select the head node to run, and recycle it to the tail of the list
			thread_t *evictedHead = thread_dequeue(ready_list);
            thread_enqueue(evictedHead, ready_list);
            return evictedHead;
	    }	
	      
        case 1: {          // Lottery - Based on Lottery algorithm in 3 Easy Pieces textbook
					
			int totalWeight = 0;
			int ticketCount = 0;
			thread_node_t *current = malloc(sizeof(*current));
			//grab the head to traverse the list
			current = ready_list->head;
	    
			//sum the thread weights
			while(current != NULL) {
				if(current->thread->priority < 0) {
					ERROR("Cannot have a negative weight");
				}
				totalWeight += current->thread->priority;
				current = current->next;
			}
	    
				//winner = some number between 0 and totalWeight-1
				int winner = random() % (totalWeight);
				//printf("Winner = %i\n", winner);

				//reset current to head
				current = ready_list->head;
				//increment the total until you find the winner
				while(current != NULL){
					ticketCount += current->thread->priority;
					
					if(ticketCount > winner){
						return current->thread;
					}
	    
					current = current->next;

				}
		}
        case 2:
          //First come first serve
            return ready_list->head->thread;
        default:
            return NULL;
    }
}
