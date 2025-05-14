/*
 * PersistentListDemo.c
 *
 *  Created on: Oct 5, 2020
 *      Author: obar
 */

#include <mission-support/PersistentList.h>
#include <mission-support/FRAMRegistry.h>
#include <stdio.h>

#define PLISTDEMO_ITEM_SIZE sizeof(unsigned char)
#define PLISTDEMO_MAX_ITERATORS 1

static const unsigned int plist_demo_FRAM_base = 0x6000;
static const unsigned int plist_demo_max_items = 24;

void PListDemo(void)
{
	unsigned int i;
	unsigned char data;
	int err, plist_demo_id = 1, iterator_id = 1;


	err = PersistentListStart(plist_demo_FRAM_base, PLISTDEMO_ITEM_SIZE, plist_demo_max_items, PLISTDEMO_MAX_ITERATORS, 1, &plist_demo_id);
	//Create the new PLIST
	if(err < 0)
	{
		printf("\nPersistentListStart failed with error code: %d\n", err);
		return;
	}

	printf("\nAdding some data to the persistent list.\n");

	data = 'a';

	for(i = 0; i < plist_demo_max_items/2; i++)
	{
		printf("\n Adding %c to node slot %d\n", data, i);

		err = PersistentListNodeAdd(plist_demo_id, i, &data);
		if(err < 0)
		{
			printf("\nPersistentListNodeAdd failed with error code: %d\n", err);
			return;
		}
		data = 'a' + i;

	}

	printf("\nData added to the persistent list total nodes: %d (expected 12).\n", PersistentListNodeGetcount(plist_demo_id));

	err = PersistentListNodeGetdata(plist_demo_id, 5, &data);
	if(err < 0)
	{
		printf("\nPersistentListNodeGetdata failed with error code: %d\n", err);
		return;
	}

	printf("\nThe data contained in node 5 is: %c (expected e)\n", data);
	printf("\nReplacing node 5 with data 'a'\n");

	data = 'a';

	err = PersistentListNodeUpdatedata(plist_demo_id, 5, &data);
	if(err < 0)
	{
		printf("\nPersistentListNodeUpdatedata failed with error code: %d\n", err);
		return;
	}

	printf("\nRemoving node 1 'a' from the list\n");

	err = PersistentListNodeRemove(plist_demo_id, 1);
	if(err < 0)
	{
		printf("\nPersistentListNodeRemove failed with error code: %d\n", err);
		return;
	}

	//Now we will use the iterator to print out the contents of the list

	err = PersistentListIteratorNew(plist_demo_id, 0, &iterator_id);

	for(;;)
	{
		PersistentListIteratorGetNodeData(plist_demo_id, iterator_id, &data);
		printf("\n %c \n", data);

		if(PersistentListIteratorNext(plist_demo_id, iterator_id, iterator_forward) != 0)
			break;
	}
	//Clean up
	PersistentListIteratorStop(plist_demo_id, iterator_id);
	PersistentListNodeRemoveAll(plist_demo_id);
	//Only removed the list from RAM not FRAM
	PersistentListStop(plist_demo_id);
}


