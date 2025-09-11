#include "DataStructures.h"


void push(SnakeBelly *belly, bool poison){
    if(belly == NULL) return;

    struct Cell *cell = malloc(sizeof(struct Cell));
    if(cell == NULL){return;}           /*TO DO*/

    cell->poison = poison;
    if(!poison){/*TO DO*/}
    else{cell->color = belly->color_food;}
    cell->val = 0;

    cell->next = belly->head;
    belly->head = cell;
    belly->count++;
}

bool pop(SnakeBelly *belly){
    /*TO DO*/
}


