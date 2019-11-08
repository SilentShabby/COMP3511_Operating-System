#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * Data structure of meta_data (32 bytes in 64-bit Linux OS)
 */
struct meta_data 
{
    size_t size;             // 8 bytes
    int free;                // 8 bytes 
    struct meta_data *next;  // 8 bytes
    struct meta_data *prev;  // 8 bytes
};

// calculate the meta data size and store as a constant
const size_t meta_data_size = sizeof(struct meta_data); 

/**
 * Global variables
 */
void *start_heap = NULL; // pointing to the start of the heap, initialize in main()
struct meta_data dummy_head_node; // dummy head node of a doubly linked list, initaiize in main()
struct meta_data *head = &dummy_head_node;

// This helper function should be used after sbrk()
// Given the pointer returned after calling sbrk(), it checks whether sbrk() is failed or not
// Reference: http://man7.org/linux/man-pages/man2/brk.2.html
//
// Usage:
//   void *p = sbrk(size);
//   if ( !sbrk_fail(p) ) {
//       // okay!
//   } else {
//      // failed
//   }
inline int sbrk_fail(void *ret) 
{
    return ret == sbrk(0);
}

// This helper function shift the sizeof(meta_data) bytes and cast the pointer to (void*)
//
// Usage:
//    struct meta_data *mb = // suppose you have a pointer pointing to a meta_data struct
//    void *data = meta_to_data(mb); // this helper function helps shift by sizeof(meta_data) bytes and cast to void*
//
inline void *meta_to_data(struct meta_data *mb) {
    return (void*) (mb+1);
}

// The implementation of the following functions are given:
struct meta_data *find_free_block(struct meta_data *head, size_t size);
void list_add(struct meta_data *new, struct meta_data *prev, struct meta_data *next);
void list_add_tail(struct meta_data *new, struct meta_data *head);
void init_list(struct meta_data *list);
void mm_print();

// Students are required to implement these functions below
void *mm_malloc(size_t size);
void mm_free(void *p);


int main()
{
    // initialization of start_heap and the linked list
    start_heap = sbrk(0);
    init_list(head);

    // step 1: allocate 1000 bytes
    void *p = mm_malloc(1000);

    printf("=== After step 1 ===\n");
    mm_print();

    // step 2: free 1000 bytes
    mm_free(p);

    printf("=== After step 2 ===\n");
    mm_print();

    // step 3: allocate 500 bytes, expected to split the free slot into 2
    void *q = mm_malloc(500);

    printf("=== After step 3 ===\n");
    mm_print();


    // step 4: allocate 462 bytes (not enough to split, occupy the whole free block = 468 bytes)
    void *r  = mm_malloc(462);
    printf("=== After step 4 ===\n");
    mm_print();

    // step 5: allocate 5000 bytes
    void *s = mm_malloc(5000);
    printf("=== After step 5 ===\n");
    mm_print();

    // step 6: free "r" pointer 
    mm_free(r);
    printf("=== After step 6 ===\n");
    mm_print();

    // step 7: free "q" pointer 
    mm_free(q);
    printf("=== After step 7 ===\n");
    mm_print();

    // step 8: free "s" pointer
    mm_free(s);
    printf("=== After step 8 ===\n");
    mm_print();

    return 0;
}


void init_list(struct meta_data *list) 
{
    list->next = list;
    list->prev = list;
}

void list_add(struct meta_data *new,
            struct meta_data *prev,
            struct meta_data *next)
{
    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}

void list_add_tail(struct meta_data *new,
                    struct meta_data *head)
{
    list_add(new, head->prev, head);
}

struct meta_data *find_free_block(struct meta_data *head, size_t size)
{
    struct meta_data *cur = head->next;
    while ( cur != head ) {
        if ( cur->free && cur->size >= size)
            return cur;
        cur = cur->next;
    }
    return NULL;
}

/** 
 * TODO: Clearly explain how you implement mm_malloc in point form. For example:
    Step 1: ....
    Step 2: ....
        Step 2.1: .....
        Step 2.2: .....
            Step 2.2.1: .....
    Step 3: ....
 */
void *mm_malloc(size_t size)
{
    /* TODO: Implement mm_malloc */
    return NULL;
}

/** 
 * TODO: Clearly explain how you implement mm_free in point form. For example:
    Step 1: ....
    Step 2: ....
        Step 2.1: .....
        Step 2.2: .....
            Step 2.2.1: .....
    Step 3: ....
 */
void mm_free(void *p)
{
    /* TODO: Implement mm_free */
}
void mm_print()
{
    printf("start_heap = %p\n", start_heap);
    printf(">>>\n");
    struct meta_data *cur = head->next;
    int i = 1;
    while ( cur != head ) {
        printf("Block %02d: [%s] size = %d bytes\n", i, cur->free ? "FREE" : "OCCP", cur->size );
        i = i+1;
        cur = cur->next;
    }
    printf(">>>\n");
    printf("brk = %p\n", sbrk(0));
}