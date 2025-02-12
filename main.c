#include <stdio.h>
#include <stdlib.h>

#define SIZE 2000

typedef struct metadata_tag
{
    int size;
    struct metadata_tag *next;
} metadata;

char memory[SIZE];

metadata *free_list_ptr = (metadata *)memory;

void *allocate_memory(int blockSize)
{
    metadata *ptr = NULL;
    metadata *current = free_list_ptr;
    metadata *prev = NULL;

    // Search for a suitable free block
    while (current != NULL && current->size < (blockSize + sizeof(metadata)))
    {
        prev = current;
        current = current->next;
    }

    if (current == NULL)
    {
        printf("No Sufficient Memory Available\n");
        return NULL;
    }

    // If the block is exactly the requested size, remove it from the free list
    if (current->size == blockSize + sizeof(metadata))
    {
        if (prev == NULL) // First block
            free_list_ptr = current->next;
        else
            prev->next = current->next;
    }
    else // Split the block
    {
        metadata *newBlock = (metadata *)((char *)current + blockSize + sizeof(metadata));
        newBlock->size = current->size - blockSize - sizeof(metadata);
        newBlock->next = current->next;

        if (prev == NULL) // First block
        {
            free_list_ptr = newBlock;
        }
        else
        {
            prev->next = newBlock;
        }

        current->size = blockSize;
    }

    current->next = NULL;
    printf("Memory allocated: Address = %p, Size = %d\n", (void *)current, blockSize);
    return (void *)((char *)current + sizeof(metadata));
}

void merge_adjacent_free_blocks()
{
    metadata *current = free_list_ptr;

    while (current != NULL && current->next != NULL)
    {
        if ((char *)current + current->size + sizeof(metadata) == (char *)current->next)
        {
            // Merge current block with next
            current->size += current->next->size + sizeof(metadata);
            current->next = current->next->next; // Skip merged block
        }
        else
        {
            current = current->next;
        }
    }
}

void deallocate_memory(void *ptr)
{
    if (ptr == NULL)
    {
        printf("Block is not valid\n");
        return;
    }

    metadata *block = (metadata *)((char *)ptr - sizeof(metadata));
    metadata *current = free_list_ptr;
    metadata *prev = NULL;

    // Find the correct place to insert while keeping the list sorted
    while (current != NULL && current < block)
    {
        prev = current;
        current = current->next;
    }

    // Insert the block in the correct position
    block->next = current;
    if (prev == NULL)
    {
        free_list_ptr = block; // Insert at head
    }
    else
    {
        prev->next = block; // Insert between prev and current
    }

    printf("Memory deallocated: Address = %p, Size = %d\n", (void *)block, block->size);

    // Merge free blocks
    merge_adjacent_free_blocks();
}

void print_free_list()
{
    printf("\nCurrent Free Memory Blocks:\n");
    metadata *current = free_list_ptr;
    while (current != NULL)
    {
        printf("Address: %p, Size: %d\n", (void *)current, current->size);
        current = current->next;
    }
}

int main()
{
    // Initialize the free list
    free_list_ptr->size = SIZE - sizeof(metadata);
    free_list_ptr->next = NULL;

    printf("Initial Free Memory Block: Address: %p, Size: %d\n", (void *)free_list_ptr, free_list_ptr->size);

    // Step 1: Allocate memory
    printf("\nAllocating 300 bytes...\n");
    void *ptr1 = allocate_memory(300);
    print_free_list();

    printf("\nAllocating 500 bytes...\n");
    void *ptr2 = allocate_memory(500);
    print_free_list();

    printf("\nAllocating 200 bytes...\n");
    void *ptr3 = allocate_memory(200);
    print_free_list();

    // Step 2: Deallocate memory with Before/After merging
    printf("\nDeallocating 500 bytes block...\n");
    printf("Before Merging:\n");
    print_free_list();
    deallocate_memory(ptr2);
    printf("After Merging:\n");
    print_free_list();

    printf("\nDeallocating 300 bytes block...\n");
    printf("Before Merging:\n");
    print_free_list();
    deallocate_memory(ptr1);
    printf("After Merging:\n");
    print_free_list();

    printf("\nDeallocating 200 bytes block...\n");
    printf("Before Merging:\n");
    print_free_list();
    deallocate_memory(ptr3);
    printf("After Merging:\n");
    print_free_list();

    return 0;
}
