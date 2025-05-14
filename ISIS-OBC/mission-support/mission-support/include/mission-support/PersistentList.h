#ifndef _PERSISTENT_LIST_H
#define _PERSISTENT_LIST_H

#include <hal/boolean.h>

#define PERSISTENTLST_MAX_NO_LISTS        50

//Error codes
#define E_PLIST_FRAMLOCK        -1 //<! Failed to obtain lock for accessing FRAM
#define E_PLIST_FRAMRANGE       -2 //<! FRAM address out of range
#define E_PLIST_FRAMVERIFY      -3 //<! FRAM data verification failed (read data not identical to written data)
#define E_PLIST_FRAMOVERLAP     -5 //<! List would conflict with another FRAM user
#define E_PLIST_MALLOC          -6 //!< Failed to allocate memory
#define E_PLIST_MAXLISTS        -7 //!< No more lists can be initialized
#define E_PLIST_MAXNODES        -8 //!< Adding node would exceed list size
#define E_PLIST_LISTID          -9 //!< Incorrect list ID
#define E_PLIST_POSITION        -10 //!< Incorrect position or position not found in list
#define E_PLIST_NODESIZE        -11 //!< Node size too large
#define E_PLIST_FRAMCORRUPTION  -12 //!< Node seems to be corrupt in FRAM
#define E_PLIST_FRAM_ERROR      -13 //!< FRAM communication is not working as expected
#define E_PLIST_EXISTING_LIST   -14 //!< There is an existing list with a different node size present in FRAM
#define E_PLIST_ITERATOR_ID    -100 //<! Invalid iterator ID
#define E_PLIST_MAXITERATORS   -101 //<! List can not accommodate any new iterators
#define E_PLIST_ITERATORINACT  -102 //<! Specified iterator is inactive and can not be used
#define E_PLIST_ITERATOR_EOL   -103 //<! Iterator has reached the end of the list
#define E_PLIST_INACCESSIBLE   -104 //<! Some data is inaccessible and not able to be returned
#define E_PLIST_INVALID_INPUT  -105 //<! Generic error code for invalid input parameters

#define PLIST_START 0
#define PLIST_END -1

typedef enum _iterator_direction_t
{
    iterator_forward = 0,
    iterator_backward = 1,
} iterator_direction_t;

/**
 * Start an instance of a persistent list.
 *
 * @param[in] FRAMbaseaddress Lowest FRAM address from where to start storing this list
 * @param[in] item_size Size of the data items in the list, in bytes. This can currently not be
 * larger than XXX bytes
 * @param[in] max_no_items Maximum number of items that the list can contain
 * @param[in] max_iterators Maximum number of iterators that can be active for this list
 * @param[in] force_empty Force the list to empty on start, regardless of contents. If there
 * is no list in FRAM then this option not matter, if a list does exist then it will be overwritten
 * with an empty list
 * @param[out] listid List ID that needs to be used when referencing this list in other functions in this module
 *
 * @returns A negative number on error, 0 when successful
 */
int PersistentListStart(unsigned int FRAMbaseaddress, unsigned int item_size, unsigned int max_no_items, unsigned int max_iterators, Boolean force_empty, int* listid);

/**
 * Stops an instance of a persistent list. This will remove it from RAM only.
 * FRAM is kept untouched.
 * NOTE: Only last list is currently supported!
 *
 * @param[in] listid List ID to the list that is to be removed from RAM
 *
 * @returns A negative number on error, 0 when successful
 */
int PersistentListStop(int listid);

/**
 * Add a node with data to a list
 *
 * @param[in] listid ID of the list to which the node should be added
 * @param[in] position Position in the list where the node should be inserted. Position 0
 * means at the head, a position larger than the number of nodes in the list or a negative
 * position will mean it will be added at the end of the list
 * @param[in] data Pointer to the data that should be stored in the new node (by copy)
 *
 * @returns A negative number on error, 0 when successful
 */
int PersistentListNodeAdd(int listid, int position, void* data);

/**
 * Get a a node's data from the list
 *
 * @param[in] listid ID of the list from which the data should be retrieved
 * @param[in] position Position of the node within the list
 * @param[out] data Pointer to memory where the data can be copied
 *
 * @returns A negative number on error, 0 when successful
 */
int PersistentListNodeGetdata(int listid, int position, void* data);

/**
 * Update the data associated with a node in the list
 *
 * @param[in] listid ID of the list
 * @param[in] position Position of the node within the list
 * @param[out] data Pointer to memory where the data to be stored in the node resides
 *
 * @returns A negative number on error, 0 when successful
 */
int PersistentListNodeUpdatedata(int listid, int position, void* data);

/**
 * Get the number of nodes in a list
 *
 * @param[in] listid ID of the list to which the number of nodes should be returned
 *
 * @returns A negative number on error, and the node count (0 or more) when successful
 */
int PersistentListNodeGetcount(int listid);

/**
 * Removes a node and its data from the list
 *
 * @param[in] listid ID of the list from which the node and data should be removed
 * @param[in] position Position of the node within the list
 *
 * @returns A negative number on error, 0 when successful
 */
int PersistentListNodeRemove(int listid, int position);

/**
 * Remove all nodes and their data from the list
 *
 * @param[in] listid ID of the list from which all nodes and data should be removed
 *
 * @returns A negative number on error, 0 when successful
 */
int PersistentListNodeRemoveAll(int listid);

/**
 * Create a new iterator for a list, which can be used to sequentially go over the nodes
 * in the list sequentially in a certain direction, which is faster than looking up each node
 * individually
 *
 * @param[in] listid ID of the list the iterator will belong to
 * @param[in] position Initial position of the iterator within the list
 * @param[out] iteratorid ID to refer to the iterator, please note that this is not unique across lists!
 *
 * @returns 0 when the iterator was successfully created, error code otherwise
 */
int PersistentListIteratorNew(int listid, int position, int* iteratorid);

/**
 * Stop using an iterator
 *
 * @param[in] listid ID of the list the iterator belongs to
 * @param[in] iteratorid ID of the iterator to be stopped
 *
 * @returns 0 when the iterator was successfully stopped, error code otherwise
 */
int PersistentListIteratorStop(int listid, int iteratorid);

/**
 * Move iterator to next node of the list
 *
 * @param[in] listid ID of the list the iterator belongs to
 * @param[in] iteratorid ID of the iterator to be moved
 * @param[in] direction Direction to move the iterator in: forward (towards the end of the list)
 * or backward (towards the start of the list)
 *
 * @returns 0 when the iterator was successfully moved, error code otherwise
 */
int PersistentListIteratorNext(int listid, int iteratorid, iterator_direction_t direction);

/**
 * Get data from the node in the list the iterator is currently pointing to
 *
 * @param[in] listid ID of the list the iterator belongs to
 * @param[in] iteratorid ID of the iterator to be used
 * @param[out] data Pointer to store the data that belongs to the node in the list the iterator is currently pointing to
 *
 * @returns 0 when the data was successfully retrieved, error code otherwise
 */
int PersistentListIteratorGetNodeData(int listid, int iteratorid, void* data);

/**
 * Convenience function to move iterator to next node of the list and at the same time also get the node
 *
 * @param[in] listid ID of the list the iterator belongs to
 * @param[in] iteratorid ID of the iterator to be used
 * @param[in] direction Direction to move the iterator in: forward (towards the end of the list)
 *  or backward (towards the start of the list)
 * @param[out] data Pointer to store the data that belongs to the node in the list the iterator is currently pointing to
 *
 * @returns 0 when the data was successfully retrieved, error code otherwise
 */
int PersistentListIteratorNextNodeData(int listid, int iteratorid, iterator_direction_t direction, void* data);

/**
 * Retrieves the node position the iterator is currently on
 * NOTE: Please be aware that the position is only valid until an add or remove has occurred i the list
 *
 * @param[in] listid ID of the list the iterator belongs to
 * @param[in] iteratorid ID of the iterator to be used
 * @param[out] position Pointer to store position in the list from which the iterator is currently pointing to
 *
 * @returns 0 when the data was successfully retrieved, error code otherwise
 */
int PersistentListIteratorGetNodePosition(int listid, int iteratorid, int* position);

/**
 * Updates the node in the list the iterator is currently pointing to
 *
 * @param[in] listid ID of the list the iterator belongs to
 * @param[in] iteratorid ID of the iterator to be used
 * @param[in] data Pointer to the new data to store
 *
 * @returns 0 when the data was successfully retrieved, error code otherwise
 */
int PersistentListIteratorUpdateNodeData(int listid, int iteratorid, void* data);

/**
 * Removes the node in the list the iterator is currently pointing to
 *
 * @param[in] listid ID of the list the iterator belongs to
 * @param[in] iteratorid ID of the iterator to be used
 *
 * @returns 0 when the data was successfully retrieved, error code otherwise
 */
int PersistentListIteratorRemoveNode(int listid, int iteratorid);

#endif // _PERSISTENT_LIST_H
