/***************************************************************************//**
 *   @file   list.c
 *   @brief  List library implementation
 *   @author Mihail Chindris (mihail.chindris@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "list.h"
#include "error.h"
#include <stdlib.h>

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct list_elem
 * @brief Format of each element of the list
 */
struct list_elem {
	/** User data */
	void			*data;
	/** Reference to previous element */
	struct list_elem	*prev;
	/** Reference to next element */
	struct list_elem	*next;
};

/**
 * @struct list_iterator
 * @brief Structure used to iterate through the list
 */
struct iterator {
	/** List reference */
	struct _list_desc	*list;
	/** Current element reference */
	struct list_elem	*elem;
};

/**
 * @struct _list_desc
 * @brief List descriptor
 */
struct _list_desc {
	/** Reference to first element in the list */
	struct list_elem	*first;
	/** Reference to last element in the list*/
	struct list_elem	*last;
	/** Number of elements in the list */
	uint32_t		nb_elements;
	/** Function used to compare elements */
	f_cmp			comparator;
	/** Number of current active iterators */
	uint32_t		nb_iterators;
	/** Internal list iterator */
	struct iterator		l_it;
};

/** @brief Default function used to compare element in the list ( \ref f_cmp) */
static int32_t default_comparator(void *data1, void *data2)
{
	return (int32_t)(data1 - data2);
}

/**
 * @brief Creates a new list elements an configure its value
 * @param data - To set list_elem.data
 * @param prev - To set list_elem.prev
 * @param next - To set list_elem.next
 * @return Address of the new element or NULL if allocation fails.
 */
static inline struct list_elem *create_element(void *data,
		struct list_elem *prev,
		struct list_elem *next)
{
	struct list_elem *elem;

	elem = (struct list_elem *)calloc(1, sizeof(*elem));
	if (!elem)
		return NULL;
	elem->data = data;
	elem->prev = prev;
	elem->next = next;

	return (elem);
}

/**
 * @brief Updates the necesary link on the list elements to add or remove one
 * @param prev - Low element
 * @param elem - Middle element
 * @param next - High element
 */
static inline void update_links(struct list_elem *prev, struct list_elem *elem,
				struct list_elem *next)
{
	if (prev)
		prev->next = elem ? elem : next;
	if (elem) {
		elem->prev = prev;
		elem->next = next;
	}
	if (next)
		next->prev = elem ? elem : prev;
}

/**
 * @brief Update according to modification the list descriptor
 * @param list - List reference
 * @param new_first - New first element
 * @param new_last - New last element
 */
static inline void update_desc(struct _list_desc *list,
			       struct list_elem *new_first,
			       struct list_elem *new_last)
{
	if (new_first == list->first) {
		list->last = new_last;
		if (new_first == NULL || new_last == NULL)
			list->first = new_last;
	} else {	/* if (new_last == list->last) */
		list->first = new_first;
		if (new_last == NULL || new_first == NULL)
			list->last = new_first;
	}
}

/**
 * @brief Set the adapter functions acording to the adapter type
 * @param ad - Reference of the adapter
 * @param type - Type of the adapter
 */
static inline void set_adapter(struct list_desc *ad, enum adapter_type type)
{
	switch (type) {
	case LIST_PRIORITY_LIST:
		ad->push = list_add_find;
		ad->pop = list_get_first;
		ad->top_next = list_read_first;
		ad->back = list_read_last;
		ad->swap = list_edit_first;
		break;
	case LIST_QUEUE:
		ad->push = list_add_last;
		ad->pop = list_get_first;
		ad->top_next = list_read_first;
		ad->back = list_read_last;
		ad->swap = list_edit_first;
		break;
	case LIST_DEFAULT:
	case LIST_STACK:
	default:
		ad->push = list_add_last;
		ad->pop = list_get_last;
		ad->top_next = list_read_last;
		ad->back = list_read_first;
		ad->swap = list_edit_last;
		break;
	}
}

/**
 * @brief Create a new empty list
 * @param list_desc - Where to store the reference of the new created list
 * @param type - Type of adapter to use.
 * @param comparator - Used to compare item when using an ordered list or when
 * using the \em find functions.
 * @return
 *  - \ref SUCCESS : On success
 *  - \ref FAILURE : Otherwise
 */
int32_t list_init(struct list_desc **list_desc, enum adapter_type type,
		  f_cmp comparator)
{
	struct list_desc	*l_desc;
	struct _list_desc	*list;


	if (!list_desc)
		return FAILURE;
	l_desc = (struct list_desc *)calloc(1, sizeof(*l_desc));
	if (!l_desc)
		return FAILURE;
	list = (struct _list_desc *)calloc(1, sizeof(*list));
	if (!list) {
		free(l_desc);
		return FAILURE;
	}

	*list_desc = l_desc;
	l_desc->priv_desc = list;
	list->comparator = comparator ? comparator : default_comparator;

	/* Configure wrapper */
	set_adapter(l_desc, type);
	list->l_it.list = list;

	return SUCCESS;
}

/**
 * @brief Remove the created list.
 *
 * All its elements will be cleared and the data inside each will be lost. If
 * not all iterators have been removed, the list will not be removed.
 * @param list_desc - Reference to the list
 * @return
 *  - \ref SUCCESS : On success
 *  - \ref FAILURE : Otherwise
 */
int32_t list_remove(struct list_desc *list_desc)
{
	void			*data;
	struct _list_desc	*list;

	if (!list_desc)
		return FAILURE;

	list = list_desc->priv_desc;
	if (list->nb_iterators != 0)
		return FAILURE;

	/* Remove all the elements */
	while (SUCCESS == list_get_first(list_desc, &data))
		;
	free(list_desc->priv_desc);
	free(list_desc);

	return SUCCESS;
}

/**
 * @brief Get the number of elements inside the list
 * @param list_desc - List reference
 * @param out_size - Where to store the number of elements
 * @return
 *  - \ref SUCCESS : On success
 *  - \ref FAILURE : Otherwise
 */
int32_t list_get_size(struct list_desc *list_desc, uint32_t *out_size)
{
	struct _list_desc	*list;

	if (!list_desc || !out_size)
		return FAILURE;

	list = list_desc->priv_desc;
	*out_size = list->nb_elements;

	return SUCCESS;
}

/** @brief Add element at the begining of the list. Refer to \ref f_add */
int32_t list_add_first(struct list_desc *list_desc, void *data)
{
	struct list_elem	*prev;
	struct list_elem	*next;
	struct list_elem	*elem;
	struct _list_desc	*list;

	if (!list_desc)
		return FAILURE;

	list = list_desc->priv_desc;

	prev = NULL;
	next = list->first;
	elem = create_element(data, prev, next);
	if (!elem)
		return FAILURE;

	update_links(prev, elem, next);

	update_desc(list, elem, list->last);

	list->nb_elements++;

	return SUCCESS;
}

/** @brief Add element at the end of the list. Refer to \ref f_add */
int32_t list_add_last(struct list_desc *list_desc, void *data)
{
	struct list_elem	*prev;
	struct list_elem	*next;
	struct list_elem	*elem;
	struct _list_desc	*list;

	if (!list_desc)
		return FAILURE;
	list = list_desc->priv_desc;

	prev = list->last;
	next = NULL;
	elem = create_element(data, prev, next);
	if (!elem)
		return FAILURE;

	update_links(prev, elem, next);

	update_desc(list, list->first, elem);

	list->nb_elements++;

	return SUCCESS;
}

/** @brief Add element at the specified idx. Refer to \ref f_add */
int32_t list_add_idx(struct list_desc *list_desc, void *data, uint32_t idx)
{
	struct _list_desc	*list;

	if (!list_desc)
		return FAILURE;
	list = list_desc->priv_desc;

	/* If there are no elements the creation of an iterator will fail */
	if (list->nb_elements == 0 || idx == 0)
		return list_add_first(list_desc, data);
	if (list->nb_elements == idx)
		return list_add_last(list_desc, data);

	list->l_it.elem = list->first;
	if (SUCCESS != iterator_move(&(list->l_it), idx))
		return FAILURE;

	return iterator_insert(&(list->l_it), data, 0);
}

/** @brief Add element in ascending order. Refer to \ref f_add */
int32_t list_add_find(struct list_desc *list_desc, void *data)
{
	struct list_elem	*elem;
	struct _list_desc	*list;

	if (!list_desc)
		return FAILURE;
	list = list_desc->priv_desc;


	/* Based on place iterator */
	elem = list->first;
	while (elem) {
		if (0 < list->comparator(elem->data, data))
			break;
		elem = elem->next;
	}
	if (elem == NULL) {
		list->l_it.elem = list->last;
		return iterator_insert(&(list->l_it), data, 1);
	} else {
		list->l_it.elem = elem;
		return iterator_insert(&(list->l_it), data, 0);
	}

}

/** @brief Edit the first element of the list. Refer to \ref f_edit */
int32_t list_edit_first(struct list_desc *list_desc, void *new_data)
{
	struct _list_desc	*list;

	if (!list_desc)
		return FAILURE;

	list = list_desc->priv_desc;
	list->first->data = new_data;

	return SUCCESS;
}

/** @brief Edit the last element of the list. Refer to \ref f_edit */
int32_t list_edit_last(struct list_desc *list_desc, void *new_data)
{
	struct _list_desc	*list;

	if (!list_desc)
		return FAILURE;

	list = list_desc->priv_desc;
	list->last->data = new_data;

	return SUCCESS;
}

/** @brief Edit the element at the specified idx. Refer to \ref f_edit */
int32_t list_edit_idx(struct list_desc *list_desc, void *new_data, uint32_t idx)
{
	struct _list_desc	*list;

	if (!list_desc)
		return FAILURE;
	list = list_desc->priv_desc;

	list->l_it.elem = list->first;
	if (SUCCESS != iterator_move(&(list->l_it), idx))
		return FAILURE;

	return iterator_edit(&(list->l_it), new_data);
}

/** @brief Edit the element which match with cmp_data. Refer to \ref f_edit */
int32_t list_edit_find(struct list_desc *list_desc, void *new_data,
		       void *cmp_data)
{
	struct _list_desc	*list;

	if (!list_desc)
		return FAILURE;
	list = list_desc->priv_desc;

	list->l_it.elem = list->first;
	if (SUCCESS != iterator_find(&(list->l_it), cmp_data))
		return FAILURE;

	return iterator_edit(&(list->l_it), new_data);
}

/** @brief Read the first element of the list. Refer to \ref f_read */
int32_t list_read_first(struct list_desc *list_desc, void **data)
{
	struct _list_desc	*list;

	if (!list_desc || !data)
		return FAILURE;

	*data = NULL;
	list = list_desc->priv_desc;
	if (!list->first)
		return FAILURE;

	*data = list->first->data;

	return SUCCESS;
}

/** @brief Read the last element of the list. Refer to \ref f_read */
int32_t list_read_last(struct list_desc *list_desc, void **data)
{
	struct _list_desc	*list;

	if (!list_desc || !data)
		return FAILURE;

	*data = NULL;
	list = list_desc->priv_desc;
	if (!list->last)
		return FAILURE;

	*data = list->last->data;

	return SUCCESS;
}

/** @brief Read the element at the specified idx. Refer to \ref f_read */
int32_t list_read_idx(struct list_desc *list_desc, void **data, uint32_t idx)
{
	struct _list_desc	*list;

	if (!list_desc || !data)
		return FAILURE;

	*data = NULL;
	list = list_desc->priv_desc;
	if (!list)
		return FAILURE;

	if (idx >= list->nb_elements)
		return FAILURE;

	list->l_it.elem = list->first;
	if (SUCCESS != iterator_move(&(list->l_it), idx))
		return FAILURE;

	return iterator_read(&(list->l_it), data);
}

/** @brief Read the element which match with cmp_data. Refer to \ref f_read */
int32_t list_read_find(struct list_desc *list_desc, void **data,
		       void *cmp_data)
{
	struct _list_desc	*list;

	if (!list_desc || !data)
		return FAILURE;

	*data = NULL;
	list = list_desc->priv_desc;
	if (!list)
		return FAILURE;

	list = list_desc->priv_desc;
	list->l_it.elem = list->first;
	if (SUCCESS != iterator_find(&(list->l_it), cmp_data))
		return FAILURE;

	return iterator_read(&(list->l_it), data);
}

/** @brief Read and delete the first element of the list. Refer to \ref f_get */
int32_t list_get_first(struct list_desc *list_desc, void **data)
{
	struct list_elem 	*prev;
	struct list_elem 	*next;
	struct list_elem 	*elem;
	struct _list_desc	*list;

	if (!list_desc || !data)
		return FAILURE;

	*data = NULL;
	list = list_desc->priv_desc;
	if (!list->nb_elements)
		return FAILURE;

	elem = list->first;
	prev = elem->prev;
	next = elem->next;

	update_links(prev, NULL, next);
	update_desc(list, next, list->last);
	list->nb_elements--;

	*data = elem->data;
	free(elem);

	return SUCCESS;
}

/** @brief Read and delete the last element of the list. Refer to \ref f_get */
int32_t list_get_last(struct list_desc *list_desc, void **data)
{
	struct list_elem 	*prev;
	struct list_elem	 *next;
	struct list_elem 	*elem;
	struct _list_desc	*list;

	if (!list_desc || !data)
		return FAILURE;

	*data = NULL;
	list = list_desc->priv_desc;
	if (!list->nb_elements)
		return FAILURE;

	elem = list->last;
	prev = elem->prev;
	next = elem->next;

	update_links(prev, NULL, next);
	update_desc(list, list->first, prev);
	list->nb_elements--;

	*data = elem->data;
	free(elem);

	return SUCCESS;
}

/** @brief Read and delete the element at idx. Refer to \ref f_get */
int32_t list_get_idx(struct list_desc *list_desc, void **data, uint32_t idx)
{
	struct _list_desc	*list;

	if (!list_desc || !data)
		return FAILURE;

	*data = NULL;
	list = list_desc->priv_desc;
	list->l_it.elem = list->first;
	if (SUCCESS != iterator_move(&(list->l_it), idx))
		return FAILURE;

	return iterator_get(&(list->l_it), data);
}

/**
 * @brief Read and delete the element which match with cmp_data.
 * Refer to \ref f_get
 */
int32_t list_get_find(struct list_desc *list_desc, void **data, void *cmp_data)
{
	struct _list_desc	*list;

	if (!list_desc || !data)
		return FAILURE;

	*data = NULL;
	list = list_desc->priv_desc;
	list->l_it.elem = list->first;
	if (SUCCESS != iterator_find(&(list->l_it), cmp_data))
		return FAILURE;

	return iterator_get(&(list->l_it), data);
}

/**
 * @brief Create a new iterator
 * @param iter - Where to store the reference for the new iterator
 * @param list_desc - Reference of the list the iterator will be used for
 * @param start - If it is true the iterator will be positioned at the first
 * element of the list, else it will be positioned at the last.
 * @return
 *  - \ref SUCCESS : On success
 *  - \ref FAILURE : Otherwise
 */
int32_t iterator_init(struct iterator **iter, struct list_desc *list_desc,
		      bool start)
{
	struct iterator	*it;

	if (!list_desc)
		return FAILURE;

	it = (struct iterator *)calloc(1, sizeof(*it));
	if (!it)
		return FAILURE;
	it->list = list_desc->priv_desc;
	it->list->nb_iterators++;
	it->elem = start ? it->list->first : it->list->last;
	*iter = it;

	return SUCCESS;
}

/**
 * @brief Remove the created iterator
 * @param iter - Reference of the iterator
 * @return
 *  - \ref SUCCESS : On success
 *  - \ref FAILURE : Otherwise
 */
int32_t iterator_remove(struct iterator *iter)
{
	struct iterator *it = iter;

	if (!it)
		return FAILURE;

	it->list->nb_iterators--;
	free(it);

	return SUCCESS;
}

/**
 * @brief Move the position of the iteration through the list.
 *
 * If the required position is outside the list, the call will fail and the
 * iterator will keep its position.
 * @param iter - Reference of the iterator
 * @param idx - Number of positions to be move. If positive, it will be moved
 * forward, otherwise backwords.
 * @return
 *  - \ref SUCCESS : On success
 *  - \ref FAILURE : Otherwise
 */
int32_t iterator_move(struct iterator *iter, int32_t idx)
{
	struct iterator		*it = iter;
	struct list_elem	*elem;
	int32_t			dir = (idx < 0) ? -1 : 1;

	if (!it)
		return FAILURE;

	idx = abs(idx);
	elem = it->elem;
	while (idx > 0 && elem) {
		elem = dir > 0 ? elem->next : elem->prev;
		idx--;
	}
	if (!elem)
		return FAILURE;

	it->elem = elem;

	return SUCCESS;
}

/**
 * @brief Place the iterator where cmp_data if found.
 * @param iter - Reference to the iterator
 * @param cmp_data - Data to be found
 * @return
 *  - \ref SUCCESS : On success
 *  - \ref FAILURE : Otherwise
 */
int32_t iterator_find(struct iterator *iter, void *cmp_data)
{
	struct iterator		*it = iter;
	struct list_elem	*elem;

	if (!it)
		return FAILURE;

	elem = it->list->first;
	while (elem) {
		if (0 == it->list->comparator(elem->data, cmp_data)) {
			it->elem = elem;
			return SUCCESS;
		}
		elem = elem->next;
	}

	return FAILURE;
}

/**
 * @brief Replace the data at the current position. Refer to \ref f_edit
 */
int32_t iterator_edit(struct iterator *iter, void *new_data)
{
	struct iterator *it = iter;

	if (!it)
		return FAILURE;

	it->elem->data = new_data;

	return SUCCESS;
}

/**
 * @brief Read and remove the data at the current position. Refer to \ref f_get.
 *
 * If the current item is the last one, the iterator will be moved to the
 * previous one.
 */
int32_t iterator_get(struct iterator *iter, void **data)
{
	struct iterator		*it = iter;
	struct list_elem	*next;


	if (!it || !it->elem || !data)
		return FAILURE;

	update_links(it->elem->prev, NULL, it->elem->next);
	if (it->elem == it->list->first)
		update_desc(it->list, it->elem->next, it->list->last);
	else if (it->elem == it->list->last)
		update_desc(it->list, it->list->first, it->elem->prev);
	it->list->nb_elements--;

	*data = it->elem->data;
	if (it->elem == it->list->last)
		next = it->elem->prev;
	else
		next = it->elem->next;
	free(it->elem);
	it->elem = next;

	return SUCCESS;
}

/**
 * @brief Read the data at the current position. Refer to \ref f_read
 */
int32_t iterator_read(struct iterator *iter, void **data)
{
	struct iterator *it = iter;

	if (!it || !it->elem || !data)
		return FAILURE;

	*data = it->elem->data;

	return SUCCESS;
}

/**
 * @brief Insert an item in the list. Refer to \ref f_add
 * @param iter
 * @param data
 * @param after - If true, the item will be inserted after the current position.
 * Otherwise it will be inserted before.
 */
int32_t iterator_insert(struct iterator *iter, void *data, bool after)
{
	struct iterator		*it = iter;
	struct list_elem	*elem;
	struct list_desc	list_desc;

	if (!it)
		return FAILURE;

	list_desc.priv_desc = iter->list;
	if (after && it->elem == it->list->last)
		return list_add_last(&list_desc, data);
	if (!after && it->elem == it->list->first)
		return list_add_first(&list_desc, data);

	if (after)
		elem = create_element(data, it->elem, it->elem->next);
	else
		elem = create_element(data, it->elem->prev, it->elem);
	if (!elem)
		return FAILURE;

	update_links(elem->prev, elem, elem->next);

	it->list->nb_elements++;

	return SUCCESS;
}
