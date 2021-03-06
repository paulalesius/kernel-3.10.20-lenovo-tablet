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

#ifndef __IA_CSS_QUEUE_H
#define __IA_CSS_QUEUE_H

#include <platform_support.h>
#include <type_support.h>

#include "ia_css_queue_comm.h"
#include "../src/queue_access.h"

/* Local Queue object descriptor */
struct ia_css_queue_local {
	ia_css_circbuf_desc_t *cb_desc; /*Circbuf desc for local queues*/
	ia_css_circbuf_elem_t *cb_elems; /*Circbuf elements*/
};
typedef struct ia_css_queue_local ia_css_queue_local_t;

/* Handle for queue object*/
typedef struct ia_css_queue ia_css_queue_t;


/*****************************************************************************
 * Queue Public APIs
 *****************************************************************************/
/** @brief Initialize a local queue instance.
 *
 * @param[out] qhandle. Handle to queue instance for use with API
 * @param[in]  desc.   Descriptor with queue properties filled-in
 * @return     0      - Successful init of local queue instance.
 * @return     EINVAL - Invalid argument.
 *
 */
extern int ia_css_queue_local_init(
			ia_css_queue_t *qhandle,
			ia_css_queue_local_t *desc);

/** @brief Initialize a remote queue instance
 *
 * @param[out] qhandle. Handle to queue instance for use with API
 * @param[in]  desc.   Descriptor with queue properties filled-in
 * @return     0      - Successful init of remote queue instance.
 * @return     EINVAL - Invalid argument.
 */
extern int ia_css_queue_remote_init(
			ia_css_queue_t *qhandle,
			ia_css_queue_remote_t *desc);

/** @brief Uninitialize a queue instance
 *
 * @param[in]  qhandle. Handle to queue instance
 * @return     0 - Successful uninit.
 *
 */
extern int ia_css_queue_uninit(
			ia_css_queue_t *qhandle);

/** @brief Enqueue an item in the queue instance
 *
 * @param[in]  qhandle. Handle to queue instance
 * @param[in]  item.    Object to be enqueued.
 * @return     0       - Successful enqueue.
 * @return     EINVAL  - Invalid argument.
 * @return     ENOBUFS - Queue is full.
 *
 */
extern int ia_css_queue_enqueue(
			ia_css_queue_t *qhandle,
			uint32_t item);

/** @brief Dequeue an item from the queue instance
 *
 * @param[in]  qhandle. Handle to queue instance
 * @param[out] item.    Object to be dequeued into this item.

 * @return     0       - Successful dequeue.
 * @return     EINVAL  - Invalid argument.
 * @return     ENODATA - Queue is empty.
 *
 */
extern int ia_css_queue_dequeue(
			ia_css_queue_t *qhandle,
			uint32_t *item);

/** @brief Check if the queue is empty
 *
 * @param[in]  qhandle.  Handle to queue instance
 * @param[in]  is_empty  True if empty, False if not.
 * @return     0       - Successful access state.
 * @return     EINVAL  - Invalid argument.
 * @return     ENOSYS  - Function not implemented.
 *
 */
extern int ia_css_queue_is_empty(
			ia_css_queue_t *qhandle,
			bool *is_empty);

/** @brief Get the usable size for the queue
 *
 * @param[in]  qhandle. Handle to queue instance
 * @param[out] size     Size value to be returned here.
 * @return     0       - Successful get size.
 * @return     EINVAL  - Invalid argument.
 * @return     ENOSYS  - Function not implemented.
 *
 */
extern int ia_css_queue_get_size(
		ia_css_queue_t *qhandle,
		uint32_t *size);

#endif /* __IA_CSS_QUEUE_H */

