#ifndef BUFFERS_C_
#define BUFFERS_C_

// ================================================
// Buffer for keeping track of records
// ================================================
typedef struct {
    uint32_t *records;
    size_t head;
} record_buf_t;


void record_buf_reset(record_buf_t *buffer) {
    buffer->head = 0;
}

// ================================================
// END Buffer for keeping track of records
// ================================================

// ====================================================================
// CIRCULAR BUFFER Structure for use with the g2 algorithm based on it
// ====================================================================
typedef struct {
    uint64_t *buffer;
    int head;
    int count;
    int size; //of the buffer
} circular_buf_t;


void circular_buf_reset(circular_buf_t * cbuf);
static inline void circular_buf_put(circular_buf_t * cbuf, uint64_t data);
static inline void circular_buf_oldest(circular_buf_t * cbuf, uint64_t * data);

circular_buf_t circular_buf_allocate(int size) {
    circular_buf_t cbuf;
    cbuf.size = size;
    circular_buf_reset(&cbuf);
    cbuf.buffer = calloc(2*cbuf.size, sizeof(uint64_t)); // set memory to zero so we have a proper

    return cbuf;
}

void circular_buf_reset(circular_buf_t * cbuf)
{
    if(cbuf) {
        cbuf->head = cbuf->size;
        cbuf->count = 0;
    }
}

static inline void circular_buf_put(circular_buf_t * cbuf, uint64_t data)
{
    cbuf->buffer[cbuf->head] = data;
    cbuf->buffer[cbuf->head - cbuf->size] = data;
    cbuf->head = ((cbuf->head + 1) % cbuf->size) + cbuf->size;
    if(cbuf->count < cbuf->size) {
        cbuf->count = cbuf->count + 1;
    }
}

static inline void circular_buf_oldest(circular_buf_t * cbuf, uint64_t * data) {
    // CAUTION: Even if the buffer is empty oldest will return whatever
    // is in buffer[0]. We do so because it is conveninet for our specific
    // application but it can be catastrophic.
    // TAKE HOME MESSAGE: Don't use this implementation as is for anything
    // other than computing g2.
    if(cbuf && data) {
        if(cbuf->count < cbuf->size) {
            *data = cbuf->buffer[0];
        } else {
            *data = cbuf->buffer[cbuf->head];
        }
    }
}

// ============================
// END OF CIRCULAR BUFFER
// ============================

// ====================================================================
// CHAINED LIST BUFFER Structure for use with the dummy g2 algorithm (calculate_g2)
// ====================================================================

// this 'node' structure is a chained list to be used with the buffers. It is easy to add an item
// at the end and read+remove an item at the beginning (see the three functions below).
typedef struct node {
    uint64_t val;
    struct node * next;
} node_t;

void head_init(node_t** head, int* length) {
    // initialize a chained list with value 0 and length 0.
    *head = malloc(sizeof(node_t));
    (*head)->val = 0;
    (*head)->next = NULL;
    *length = 0;
}

void push(node_t * head, uint64_t val, int* length) {
    // add an item at the end of the list
    node_t * current = head;
    while (current->next != NULL) {
        current = current->next;
    }
    
    // now we can add a new variable
    current->next = malloc(sizeof(node_t));
    current->next->val = val;
    current->next->next = NULL;
    *length += 1;
}

uint64_t pop(node_t * head, int* length) {
    // remove the first info item (second in the list) from the list, returning its value
    // remember that, for simplicity, we keep the very first item of the list
    // alive so we don't need to recreate one each time the list becomes empty.
    uint64_t retval = 0;
    node_t * next_node = NULL;
    
    if (head->next == NULL) {
        return 0;
    }
    
    next_node = head->next->next;
    retval = head->next->val;
    free(head->next);
    head->next = next_node;
    *length = *length - 1;
    
    return retval;
}

// ============================
// END OF CHAINED LIST BUFFER
// ============================

#endif /* BUFFERS_C_ */