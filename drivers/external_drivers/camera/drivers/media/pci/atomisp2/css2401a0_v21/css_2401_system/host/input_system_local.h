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

#ifndef __INPUT_SYSTEM_LOCAL_H_INCLUDED__
#define __INPUT_SYSTEM_LOCAL_H_INCLUDED__

#include "type_support.h"
#include "input_system_global.h"

#include "ibuf_ctrl.h"
#include "csi_rx.h"
#include "pixelgen.h"
#include "isys_stream2mmio.h"

typedef input_system_err_t input_system_error_t;

typedef enum {
	MIPI_FORMAT_EMBEDDED = 0x12,
	MIPI_FORMAT_YUV420_8 = 0x18,
	MIPI_FORMAT_YUV420_10,
	MIPI_FORMAT_YUV420_8_LEGACY,
	MIPI_FORMAT_YUV420_8_SHIFT = 0x1C,
	MIPI_FORMAT_YUV420_10_SHIFT,
	MIPI_FORMAT_YUV422_8 = 0x1E,
	MIPI_FORMAT_YUV422_10,
	MIPI_FORMAT_RGB444 = 0x20,
	MIPI_FORMAT_RGB555,
	MIPI_FORMAT_RGB565,
	MIPI_FORMAT_RGB666,
	MIPI_FORMAT_RGB888,
	MIPI_FORMAT_RAW6 = 0x28,
	MIPI_FORMAT_RAW7,
	MIPI_FORMAT_RAW8,
	MIPI_FORMAT_RAW10,
	MIPI_FORMAT_RAW12,
	MIPI_FORMAT_RAW14,
	MIPI_FORMAT_CUSTOM0 = 0x30,
	MIPI_FORMAT_CUSTOM1,
	MIPI_FORMAT_CUSTOM2,
	MIPI_FORMAT_CUSTOM3,
	MIPI_FORMAT_CUSTOM4,
	MIPI_FORMAT_CUSTOM5,
	MIPI_FORMAT_CUSTOM6,
	MIPI_FORMAT_CUSTOM7,
	//MIPI_FORMAT_RAW16, /*not supported by 2401*/
	//MIPI_FORMAT_RAW18,
	N_MIPI_FORMAT
} mipi_format_t;

#define MIPI_FORMAT_JPEG	MIPI_FORMAT_CUSTOM0
#define MIPI_FORMAT_BINARY_8	MIPI_FORMAT_CUSTOM0
#define N_MIPI_FORMAT_CUSTOM	8

/* The number of stores for compressed format types */
#define	N_MIPI_COMPRESSOR_CONTEXT	(N_RX_CHANNEL_ID * N_MIPI_FORMAT_CUSTOM)

typedef enum {
	MIPI_PREDICTOR_NONE = 0,
	MIPI_PREDICTOR_TYPE1,
	MIPI_PREDICTOR_TYPE2,
	N_MIPI_PREDICTOR_TYPES
} mipi_predictor_t;

typedef struct input_system_state_s	input_system_state_t;
struct input_system_state_s {
	ibuf_ctrl_state_t	ibuf_ctrl_state[N_IBUF_CTRL_ID];
	csi_rx_fe_ctrl_state_t  csi_rx_fe_ctrl_state[N_CSI_RX_FRONTEND_ID];
	csi_rx_be_ctrl_state_t  csi_rx_be_ctrl_state[N_CSI_RX_BACKEND_ID];
	pixelgen_ctrl_state_t   pixelgen_ctrl_state[N_PIXELGEN_ID];
	stream2mmio_state_t	stream2mmio_state[N_STREAM2MMIO_ID];
};
#endif /* __INPUT_SYSTEM_LOCAL_H_INCLUDED__ */
