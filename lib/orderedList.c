/**
 * \file
 * \author James Johns
 */

#include <printh.h>
#include <types.h>
#include <malloc.h>
#include <orderedList.h>

/**
 * \fn createOrderedList(unsigned int maxSize, orderPredicate_t predicate)
 *
 * Create an ordered list of maximum length maxSize, using predicate to sort entries.
 */
struct orderedList_s *createOrderedList(unsigned int maxSize, orderPredicate_t predicate)
{
	struct orderedList_s *toRet = malloc(sizeof(struct orderedList_s));
	toRet->array = malloc(sizeof(void *)*maxSize);
	toRet->maxSize = maxSize;
	toRet->allocated = 0;
	toRet->predicate = predicate;
	return toRet;
}

/**
 * \fn orderedListInsertItem(struct orderedList_s *list, void *item)
 *
 * Insert item into list.
 */
void orderedListInsertItem(struct orderedList_s *list, void *item)
{
	unsigned int i = 0;
	if (list->allocated >= list->maxSize)
		return;
	/* find first entry in list that is more than the new item */
	while (i < list->allocated && list->array[i] != NULL && list->predicate(item, list->array[i]) > 0)
		i++;
	/* place new item into list, moving everything along one */
	void *tmp, *tmp2 = item;
	while (i <= list->allocated && i < list->maxSize) {
		tmp = list->array[i];
		list->array[i] = tmp2;
		tmp2 = tmp;
		i++;
	}
	list->allocated++;
}

/**
 * \fn orderedListGetItem(struct orderedList_s *list, unsigned int index)
 *
 * Return item at index in list
 */
void *orderedListGetItem(struct orderedList_s *list, unsigned int index)
{
	if (index >= list->allocated)
		return NULL;
	return list->array[index];
}

/**
 * \fn orderedListFindItem(struct orderedList_s *list, void *item)
 *
 * Return entry in list at the same position item would be if entered into list.
 *
 * item need only be unique enough for the predicate used by the list object to compare
 * with each entry in the list.
 */
void *orderedListFindItem(struct orderedList_s *list, void *item)
{
	unsigned int i = 0;
	while ((i < list->allocated) && (list->predicate(item, list->array[i]) != 0))
		i++;
	if (i >= list->allocated)
		return NULL;

	if (list->predicate(item, list->array[i]) == 0) 
		return list->array[i];
	else
		return NULL;

}
