#include "DataStructures.h"
#include "print.h"
#include "string.h"
#include "stdint.h"
void push(SnakeBelly *belly, uint16_t val, uint8_t i, uint8_t j, bool poison){
    if(belly == NULL) return;

    struct Cell *cell = malloc(sizeof(struct Cell));
    if(cell == NULL){return;}

    cell->poison = poison;
    cell->prev = NULL; // New head's prev is always NULL
    cell->next = NULL; 
    cell->i = i;
    cell->j = j;
    if(!poison){/*TO DO*/}
    cell->val = val;

    if(belly->count == 0){
        belly->head = cell;
        belly->tail = cell;
    }
    else{
        cell->next = belly->head;
        belly->head->prev = cell; // CRITICAL: Link the old head's prev to the new cell
        belly->head = cell;
    }
    
    belly->count++;
    //sortBelly(belly);
}

void pop(SnakeBelly *belly){
    if(belly == NULL || belly->head == NULL) {
        return;
    }

    struct Cell *temp = belly->head;

    if(belly->head == belly->tail) {
        belly->head = NULL;
        belly->tail = NULL;
    } else {
        belly->head = belly->head->next;
        belly->head->prev = NULL;
    }
    free(temp);
    belly->count--;
}


void popAll(SnakeBelly *belly){

    while(belly->count !=0){
        pop(belly);
    }
}

void insertion_sort(uint16_t arr[], uint16_t n) {
    uint16_t i, key;
    int16_t j; // Change j to a signed integer type
    for (i = 1; i < n; i++) {
        key = arr[i];
        j = i - 1;

        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
    }
}

