/*
 * File: cvector.c
 * Author: Victor Cheruiyot
 * ----------------------
 *
 */

#include "cvector.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <assert.h>
#include <string.h>
#include <search.h>

// a suggested value to use when given capacity_hint is 0
#define DEFAULT_CAPACITY 16

/* Type: struct CVectorImplementation
 * ----------------------------------
 * This definition completes the CVector type that was declared in
 * cvector.h. You fill in the struct with your chosen fields.
 */
struct CVectorImplementation {
	size_t alloc_depth;
	size_t capacity;
	size_t elems_size;
	void *elements;
	void (*clean_up)(void *);
};


/* The NOT_YET_IMPLEMENTED macro is used as the body for all functions
 * to remind you about which operations you haven't yet implemented.
 * It will report a fatal error if a call is made to an not-yet-implemented
 * function (this is preferable to returning garbage or silently
 * ignoring the call).  Remove the call to this macro as you implement
 * each function and finally you can remove this macro and comment
 * when no longer needed.
 */
#define NOT_YET_IMPLEMENTED printf("%s() not yet implemented!\n", __func__); raise(SIGKILL); exit(107);


CVector *cvec_create(size_t elemsz, size_t capacity_hint, CleanupElemFn fn)
{
	CVector *vec = malloc(sizeof(struct CVectorImplementation));
	assert(vec != NULL);
	vec->alloc_depth = capacity_hint;
	vec->elems_size = elemsz;
	vec->capacity = 0;
	vec->elements = (void *)malloc(elemsz * capacity_hint);
	vec->clean_up = fn; //clean up function
	return vec;
}

void cvec_dispose(CVector *cv)
{
	if(cv->clean_up){
		for(size_t i = 0; i < cv->capacity; i++){
			void * v = cvec_nth(cv, i);
			cv->clean_up(v);
		}	
	}	
	free(cv->elements);
	free(cv);
}

int cvec_count(const CVector *cv)
{
	return cv->capacity;
}

void *re_size(CVector *cv){
	cv->alloc_depth *= 2;
	void *tp = realloc(cv->elements, cv->elems_size * cv->alloc_depth);//how do we catch
        assert(tp != NULL);	
	cv->elements = tp;
	return cv;
}

void *cvec_nth(const CVector *cv, int index)
{
	assert(index >= 0 && index <= cv->capacity);
	void *value = (char *)cv->elements + (cv->elems_size * index);
	return value;
}


void cvec_insert(CVector *cv, const void *addr, int index)
{
	assert(index >= 0 && index <= cv->capacity); 
	if(cv->alloc_depth == cv->capacity + 1){
		re_size(cv);
	}
	void *cur = cvec_nth(cv, index);
	void *cur_next = (char *)cur + cv->elems_size;
	memmove(cur_next, cur, (cv->elems_size * (cv->capacity - index)));
	cv->capacity++;
	cvec_replace(cv, addr, index);
}

void cvec_append(CVector *cv, const void *addr)
{
	if(cv->capacity == cv->alloc_depth){
		re_size(cv);
	}
	void *elms = cvec_nth(cv, cv->capacity);
	memcpy(elms, addr, cv->elems_size);
	(cv->capacity)++;
}

void cvec_replace(CVector *cv, const void *addr, int index)
{
	assert(index >= 0 && index < cv->capacity);
	void *p = cvec_nth(cv, index);//should free
	memcpy(p, addr, cv->elems_size);
}

void cvec_remove(CVector *cv, int index)
{
	assert(index >= 0 && index < cv->capacity);
	void *cur = cvec_nth(cv, index);
	void *next = (char *)cur + cv->elems_size;
	memmove(cur, next, (cv->capacity - index - 1) * cv->elems_size);
	cv->capacity--;
}

int cvec_search(const CVector *cv, const void *key, CompareFn cmp, int start, bool sorted)
{
	assert(start >= 0 && start < cv->capacity);
	void *base = cvec_nth(cv, start);
	if(sorted){
		void *found = bsearch(key, base, cv->capacity, cv->elems_size, cmp);
		if(found){
			return ((char *)found - (char *)cv->elements) / cv->elems_size;
		}else{
			return -1;
		}
		
	}else{
		size_t memb = cv->capacity - start;
		void *elem = lfind(key, base, &memb, cv->elems_size, cmp);
		if(elem){
			return ((char *)elem - (char *)cv->elements) / cv->elems_size;
		}else{
			return -1;
		}
	}

}

void cvec_sort(CVector *cv, CompareFn cmp)
{
	assert(cmp != NULL);
	qsort(cv->elements, cv->capacity, cv->elems_size, cmp);
}

void *cvec_first(const CVector *cv)
{
	return cv->elements;

}

void *cvec_next(const CVector *cv, const void *prev)
{
	void *next = (char *)prev + cv->elems_size;
	size_t len = (char *)prev - (char *)cv->elements;
	if((len / cv->elems_size) == (cv->capacity - 1))return NULL;
	return next;
}

