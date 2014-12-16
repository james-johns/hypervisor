
#include <printh.h>
#include <types.h>
#include <malloc.h>
#include <orderedList.h>

struct orderedList_s *orderedListCreate(unsigned int maxSize, orderPredicate_t predicate)
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
	if (list->allocated == list->maxSize)
		printh("List is full, cannot add another item\r\n");
	/* find first entry in list that is more than the new item */
	while (list->predicate(item, list->array[i]) > 0)
		i++;
	/* place new item into list, moving everything along one */
	void *tmp;
	while (i <= list->allocated) {
		tmp = list->array[i];
		list->array[i] = item;
		item = tmp;
	}
	list->allocated++;
}

void *orderedListGetItem(struct orderedList_s *list, unsigned int index)
{
	if (index >= list->allocated)
		return NULL;
	return list->array[index];
}

