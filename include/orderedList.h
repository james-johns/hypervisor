

#ifndef _ORDEREDLIST_H_
#define _ORDEREDLIST_H_

typedef void * type_t;

/* must return 0 if equal, < 0 if left is less than right, 
 * or > 0 if left is more than right */
typedef signed int (*orderPredicate_t)(type_t, type_t);
struct orderedList_s {
	type_t *array;
	unsigned int allocated;
	unsigned int maxSize;
	orderPredicate_t predicate;
};

struct orderedList_s *createOrderedList(unsigned int maxSize, orderPredicate_t predicate);
void orderedListInsertItem(struct orderedList_s *list, type_t item);
void *orderedListGetItem(struct orderedList_s *list, unsigned int index);
void *orderedListFindItem(struct orderedList_s *list, type_t item);

#endif
