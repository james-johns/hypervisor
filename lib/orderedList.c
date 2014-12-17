
#include <printh.h>
#include <types.h>
#include <malloc.h>
#include <orderedList.h>

struct orderedList_s *createOrderedList(unsigned int maxSize, orderPredicate_t predicate)
{
	struct orderedList_s *toRet = malloc(sizeof(struct orderedList_s));
	toRet->array = malloc(sizeof(void *)*maxSize);
	toRet->maxSize = maxSize;
	toRet->allocated = 0;
	toRet->predicate = predicate;
	return toRet;
}

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

void *orderedListGetItem(struct orderedList_s *list, unsigned int index)
{
	if (index >= list->allocated)
		return NULL;
	return list->array[index];
}

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
