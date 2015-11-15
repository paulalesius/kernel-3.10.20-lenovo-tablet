/*
 * Support for Intel Camera Imaging ISP subsystem.
 *
 * Copyright (c) 2010 - 2014 Intel Corporation. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version
 * 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *
 */

#include "ia_css_queue.h"
#include <system_types.h>
#include <ia_css_circbuf.h>
#include "queue_access.h"

/*****************************************************************************
 * Queue Public APIs
 *****************************************************************************/
int ia_css_queue_local_init(
			ia_css_queue_t *qhandle,
			ia_css_queue_local_t *desc)
{
	if (NULL == qhandle || NULL == desc
		|| NULL == desc->cb_elems || NULL == desc->cb_desc) {
		/* Invalid parameters, return error*/
		return EINVAL ;
	}

	/* Mark the queue as Local */
	qhandle->type = IA_CSS_QUEUE_TYPE_LOCAL;

	/* Create a local circular buffer queue*/
	ia_css_circbuf_create(&qhandle->desc.cb_local,
	      desc->cb_elems,
	      desc->cb_desc);

	return 0;
}

int ia_css_queue_remote_init(
			ia_css_queue_t *qhandle,
			ia_css_queue_remote_t *desc)
{
	if (NULL == qhandle || NULL == desc) {
		/* Invalid parameters, return error*/
		return EINVAL;
	}

	/* Mark the queue as remote*/
	qhandle->type = IA_CSS_QUEUE_TYPE_REMOTE;

	/* Copy over the local queue descriptor*/
	qhandle->location = desc->location;
	qhandle->proc_id = desc->proc_id;
	qhandle->desc.remote.cb_desc_addr = desc->cb_desc_addr;
	qhandle->desc.remote.cb_elems_addr = desc->cb_elems_addr;

	/* If queue is remote, we let the local processor
	 * do its init, before using it. This is just to get us
	 * started, we can remove this restriction as we go ahead
	 */

	return 0;
}

int ia_css_queue_uninit(
			ia_css_queue_t *qhandle)
{
	if (!qhandle)
		return EINVAL;

	/* Load the required queue object */
	if (qhandle->type == IA_CSS_QUEUE_TYPE_LOCAL) {
		/* Local queues are created. Destroy it*/
		ia_css_circbuf_destroy(&qhandle->desc.cb_local);
	}

	return 0;
}

int ia_css_queue_enqueue(
			ia_css_queue_t *qhandle,
			uint32_t item)
{
	if (0 == qhandle)
		return EINVAL;

	/* 1. Load the required queue object */
	if (qhandle->type == IA_CSS_QUEUE_TYPE_LOCAL) {
		/* Directly de-ref the object and
		 * operate on the queue
		 */
		if (ia_css_circbuf_is_full(&qhandle->desc.cb_local)) {
			/* Cannot push the element. Return*/
			return ENOBUFS;
		}

		/* Push the element*/
		ia_css_circbuf_push(&qhandle->desc.cb_local, item);
	} else if (qhandle->type == IA_CSS_QUEUE_TYPE_REMOTE) {
		ia_css_circbuf_desc_t cb_desc;
		ia_css_circbuf_elem_t cb_elem;
		uint32_t ignore_desc_flags = QUEUE_IGNORE_STEP_FLAG;

		/* a. Load the queue cb_desc from remote */
		QUEUE_CB_DESC_INIT(&cb_desc);
		if (0 != ia_css_queue_load(qhandle, &cb_desc, ignore_desc_flags))
			return ENOSYS;

		/* b. Operate on the queue */
		if (ia_css_circbuf_desc_is_full(&cb_desc))
			return ENOBUFS;

		cb_elem.val = item;

		if (0 != ia_css_queue_item_store(qhandle, cb_desc.end, &cb_elem))
			return ENOSYS;

		cb_desc.end = (cb_desc.end + 1) % cb_desc.size;

		/* c. Store the queue object */
		/* Set only fields requiring update with
		 * valid value. Avoids uncessary calls
		 * to load/store functions
		 */
		ignore_desc_flags = QUEUE_IGNORE_SIZE_START_STEP_FLAGS;

		if (0 != ia_css_queue_store(qhandle, &cb_desc, ignore_desc_flags))
			return ENOSYS;
	}

	return 0;
}

int ia_css_queue_dequeue(
			ia_css_queue_t *qhandle,
			uint32_t *item)
{
	if (qhandle == 0 || NULL == item)
		return EINVAL;

	/* 1. Load the required queue object */
	if (qhandle->type == IA_CSS_QUEUE_TYPE_LOCAL) {
		/* Directly de-ref the object and
		 * operate on the queue
		 */
		if (ia_css_circbuf_is_empty(&qhandle->desc.cb_local)) {
			/* Nothing to pop. Return empty queue*/
			return ENODATA;
		}

		*item = ia_css_circbuf_pop(&qhandle->desc.cb_local);
	} else if (qhandle->type == IA_CSS_QUEUE_TYPE_REMOTE) {
		/* a. Load the queue from remote */
		ia_css_circbuf_desc_t cb_desc;
		ia_css_circbuf_elem_t cb_elem;
		uint32_t ignore_desc_flags = QUEUE_IGNORE_STEP_FLAG;

		QUEUE_CB_DESC_INIT(&cb_desc);

		if (0 != ia_css_queue_load(qhandle, &cb_desc, ignore_desc_flags))
			return ENOSYS;

		/* b. Operate on the queue */
		if (ia_css_circbuf_desc_is_empty(&cb_desc))
			return ENODATA;

		if (0 != ia_css_queue_item_load(qhandle, cb_desc.start, &cb_elem))
			return ENOSYS;

		*item = cb_elem.val;

		cb_desc.start = (cb_desc.start + 1) % cb_desc.size;

		/* c. Store the queue object */
		/* Set only fields requiring update with
		 * valid value. Avoids uncessary calls
		 * to load/store functions
		 */
		ignore_desc_flags = QUEUE_IGNORE_SIZE_END_STEP_FLAGS;
		if (0 != ia_css_queue_store(qhandle, &cb_desc, ignore_desc_flags))
			return ENOSYS;
	}
	return 0;
}

int ia_css_queue_is_empty(
			ia_css_queue_t *qhandle,
			bool *is_empty)
{
	if (!qhandle || !is_empty)
		return EINVAL;

	/* 1. Load the required queue object */
	if (qhandle->type == IA_CSS_QUEUE_TYPE_LOCAL) {
		/* Directly de-ref the object and
		 * operate on the queue
		 */
		*is_empty = ia_css_circbuf_is_empty(&qhandle->desc.cb_local);
		return 0;
	} else if (qhandle->type == IA_CSS_QUEUE_TYPE_REMOTE) {
		/* a. Load the queue from remote */
		ia_css_circbuf_desc_t cb_desc;
		uint32_t ignore_desc_flags = QUEUE_IGNORE_STEP_FLAG;

		QUEUE_CB_DESC_INIT(&cb_desc);
		if (0 != ia_css_queue_load(qhandle, &cb_desc, ignore_desc_flags))
			return ENOSYS;

		/* b. Operate on the queue */
		*is_empty = ia_css_circbuf_desc_is_empty(&cb_desc);
		return 0;
	}

	return EINVAL;
}

int ia_css_queue_get_size(
			ia_css_queue_t *qhandle,
			uint32_t *size)
{
	if (qhandle == 0 || size == NULL)
		return EINVAL;

	/* 1. Load the required queue object */
	if (qhandle->type == IA_CSS_QUEUE_TYPE_LOCAL) {
		/* Directly de-ref the object and
		 * operate on the queue
		 */
		/* Return maximum usable capacity */
		*size = ia_css_circbuf_get_size(&qhandle->desc.cb_local);
	} else if (qhandle->type == IA_CSS_QUEUE_TYPE_REMOTE) {
		/* a. Load the queue from remote */
		ia_css_circbuf_desc_t cb_desc;
		uint32_t ignore_desc_flags = QUEUE_IGNORE_START_END_STEP_FLAGS;

		QUEUE_CB_DESC_INIT(&cb_desc);

		if (0 != ia_css_queue_load(qhandle, &cb_desc, ignore_desc_flags))
			return ENOSYS;

		/* Return maximum usable capacity */
		*size = cb_desc.size;
	}

	return 0;
}