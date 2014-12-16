

#ifndef _ORDEREDLIST_H_
#define _ORDEREDLIST_H_

/* must return 0 if equal, < 0 if left is less than right, 
 * or > 0 if left is more than right */
typedef signed int (*orderPredicate_t)(void *, void *);

struct orderedList_s {
	void **array;
	unsigned int allocated;
	unsigned int maxSize;
	orderPredicate_t predicate;
};

struct orderedList_s *orderedListCreate(unsigned int maxSize, orderPredicate_t predicate);
void orderedListInsertItem(struct orderedList_s *list, void *item);
void *orderedListGetItem(struct orderedList_s *list, unsigned int index);

#endif
