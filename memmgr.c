/*!
/******************************************************************************/
/*//file memmgr.c
//author Musa B Sesay  (Song Shiming)
//email I201621168@hust.edu.cn
//ID I201621168
//Course AOS2017
//Assign #5
//date 294/4/2017
/*
brief  
    Time spent on this assignment: 8 hours
    Rudimentary memory manager for dynamically allocated memory.
    Hence the memory manager use a singly-linked list in other to keep track of
    allocated and unallocated blocks of memory.
*/
/**************************************************************************/
#include "memmgr.h"
#include <stdlib.h> /* free, malloc */
#include <string.h> /* strcpy */

static MMPolicy gPolicy = mmpFirstFit;

block_info *mm_create(size_t bytes)
{
    block_info * newHeap;

    
    /* Allocate memory block */
    newHeap= (block_info *) malloc(sizeof(block_info));
    if(newHeap == NULL)
    {
        /* Out of memory */
        return NULL;
    }

    /* Allocate memory heap */
    newHeap->address = (char *) malloc(bytes);
    if(newHeap->address == NULL)
    {
        /* Out of memory */
        free(newHeap);
        return NULL;
    }

    /* Setup memory block */
    newHeap->allocated = false;
    newHeap->size = bytes;
    newHeap->next = 0;
    strcpy(newHeap->label, "START");

    return newHeap;
}

/*************************************************************************/
/*!
  \brief
    Get a block of memory of a specified size (in bytes) from the heap.
  \param heap
    Head of memory heap
  \param bytes
    Size of memoryblock
  \param label
    Label for the memory block
  \return
    Memory Address that just allocated
*/
/*************************************************************************/
void *mm_allocate(block_info *heap, size_t bytes, char *label)
{
    block_info * newBlock = NULL;
    block_info * curBlock = heap;
    block_info * bestBlock = NULL;
    size_t remainMem;

    /* Search for memory block */
    while(true)
    {
        if(gPolicy == mmpFirstFit)
        {
            /* Find Non-allocated Block and Block with enough memory */
            if(curBlock->allocated == false && curBlock->size >= bytes)
            {
                bestBlock = curBlock;
                break;
            }

            /* No More Blocks */
            if(curBlock->next == NULL)
            {
                return NULL;
            }
        }
        else if(gPolicy == mmpBestFit)
        {
            /* Find Non-allocated Block and Block with enough memory */
            if(curBlock->allocated == false && curBlock->size >= bytes)
            {
                /* Use the first matching size memory block */
                if(curBlock->size == bytes)
                {
                    bestBlock = curBlock;
                    break;
                }
                /* Looks for alternative memory block */
                if(bestBlock == NULL || bestBlock->size > curBlock->size)
                    bestBlock = curBlock;
            }

            if(curBlock->next == NULL)
            {
                /* all searched, use the best location we have found */
                if(bestBlock != NULL)
                    break;

                return NULL;
            }
        }

        curBlock = curBlock->next;
    }

    /* Setup memory block */
    remainMem = bestBlock->size - bytes;
    if(remainMem > 0)
    {
        newBlock= (block_info *) malloc(sizeof(block_info));
        if(newBlock == NULL)
        {
            /* Out of memory */
            return NULL;
        }

        newBlock->allocated = false;
        newBlock->size = remainMem;
        newBlock->address = (char*)(bestBlock->address + bytes);
        newBlock->next = bestBlock->next;
        strcpy(newBlock->label, "FREE");

        bestBlock->next = newBlock;
    }

    bestBlock->allocated = true;
    bestBlock->size = bytes;
    strcpy(bestBlock->label, label);

    return bestBlock->address;
}

/*************************************************************************/
/*!
  \brief
    Frees a block of memory
    (that was allocated using mm_allocate) from the heap.
  \param heap
    Head of memory heap
  \param address
    a non-NULL pointer that was returned by the mm_allocate function
  \return
    Status SUCCESS or FAILURE
*/
/*************************************************************************/
int mm_deallocate(block_info *heap, void *address)
{
    block_info * prevBlock = NULL;
    block_info * curBlock = heap;

    /* Find The Block */
    while(true)
    {
        /* Find Allocated Block */
        if(curBlock->allocated == true)
        {
            /* Is the same as address, we found the block */
            if((void *)curBlock->address == address)
                break;

            prevBlock = NULL;
        }
        else
        {
            /* Remember Adjucent Unallocated Block */
            prevBlock = curBlock;
        }

        /* Fail to find the block */
        if ((void *)curBlock->address > address || curBlock->next == NULL)
        {
            return FAILURE;
        }

        curBlock = curBlock->next;
    }

    /* If previous block is free memory, we combine them */
    if(prevBlock != NULL)
    {
        block_info * temp = curBlock;
        curBlock = prevBlock;
        curBlock->size += temp->size;
        curBlock->next = temp->next;
        free(temp);
    }
    else
    {
        curBlock->allocated = false;
    }

    /* If next block is free memory, we combine them */
    if(curBlock->next != NULL && curBlock->next->allocated == false)
    {
        block_info * temp = curBlock->next;
        curBlock->size += temp->size;
        curBlock->next = temp->next;
        free(temp);
    }

    return SUCCESS;
}

/*************************************************************************/
/*!
  \brief
    Destroys the memory manager by freeing the block of memory allocated
    in the mm_create function
  \param heap
    Head of memory heap
*/
/*************************************************************************/
void mm_destroy(block_info *heap)
{
    block_info * curBlock = heap;
    void* memHeap = heap->address;

    /* Delete All Nodes */
    while(curBlock != NULL)
    {
        block_info * temp = curBlock;
        curBlock = curBlock->next;
        free(temp);
    }

    /* Delete Memory Heap */
    if(memHeap != NULL)
        free(memHeap);
}

