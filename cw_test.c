/*
    I am not really sure the difference between design flaws and style problems so
    generalized style problems to visual anomalies
    generalized design flaws to logical errors: code that will run but not as intended
    generalized programming errors to things likely to break execution
*/



/*  Linked list management subroutine library */

//STYLE: wrong spacing
struct  n    
{
    void        *user_data;
    int         f;
    struct n    *next;
};



/* Insert at end of list */
void insert(struct n *head, void *data, int data_size)
{
    struct n *new_node, *p;

    new_node = malloc(sizeof(new_node));

    new_node->next = NULL;

    new_node->user_data = malloc(data_size);
    memcpy(new_node->user_data, data, sizeof(data));

    /* If list is empty, just insert at the top */
    if (head == NULL)
        head = new_node;
    else
        /*  Else, insert at end of list */
        for (p = head; p; p = p->next)
            /* We're at the end if the next node is NULL, so insert */
            if (p->next == NULL)
                p->next = new_node;
}



/* Traverse the list, invoking a passed in callback function, which
    can optionally tell us to free the node when we return  */
// STYLE: wrong indentations
void traverse(struct n *head, 
        void (*callback_func)(void *user_data, int free_flag))
{
  struct n *p;

  for (p = head; p; p = p->next)    {
    // DESIGN: p->f should be passed by reference
    (*callback_func)(p->user_data, p->f);
    if (p->f)           // If they want to free the node, do that
        free(p);
  }
}


/* Free the list */
void freel(struct n *head)
{
    struct n *p;
    // ERROR: trying to access already freed memory.
    for (p = head; p; p = p->next)
        free(p);
}


/* Quickly sort the list */
void qsort(struct n *head, int compare(void *a, void *b))
{
    struct n *p, *q;

    for (p = head; p; p = p->next);
        for (q = p->next; q; q = q->next)
        {
            if (compare(q->user_data, p->user_data) < 0)
            {
                /* Swap 'em */
                // DESIGN: missing temp storage for swapping. 
                // Needs a temp variable to store one of the values to be swapped
                p->user_data = q->user_data;
                q->user_data = p->user_data;
            }
        }
}
