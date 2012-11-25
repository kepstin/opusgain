/* 
 * opusgain - Calculate EBU R128 and ReplayGain for Ogg Opus files
 * Copyright © 2012 Calvin Walton <calvin.walton@kepstin.ca>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef OGG_H
#define OGG_H

#include <stdint.h>
#include <stdio.h>

/**
 * ogg_error_codes:
 * @OGG_SUCCESS: Everything is fine
 * @OGG_INVALID: An incoming Ogg stream contains invalid data
 */
typedef enum {
	OGG_SUCCESS = 0,
	OGG_INVALID
} ogg_error_codes;

/**
 * ogg_page_type:
 * @OGG_PAGE_TYPE_CONTINUED: This page contains a packet continued from a previous page
 * @OGG_PAGE_TYPE_BOS: This page is the first in a logical bitstream
 * @OGG_PAGE_TYPE_EOS: This page is the last in a logical bitstream
 *
 * Flags for the #ogg_page.type field
 */
typedef enum {
	OGG_PAGE_TYPE_CONTINUED = 0x01,
	OGG_PAGE_TYPE_BOS = 0x02,
	OGG_PAGE_TYPE_EOS = 0x04,
} ogg_page_type;

/**
 * ogg_page:
 * @version: Ogg stream structure revision
 * @type: Flag indicating page's context in the bitstream (See #ogg_page_type)
 * @granule_pos: Position indicator for data contained in the page
 * @serial: Logical bitstream identification serial number
 * @seq: Page counter
 * @data: The data contained within this page
 * @data_len: The number of bytes of data
 * @offset: The offset of this page from the start of the file
 *
 * Structure representing a page in an Ogg stream
 */
typedef struct ogg_page {
	uint64_t granule_pos;
	uint32_t serial;
	uint32_t seq;
	uint16_t data_len;
	uint8_t version;
	uint8_t type;
	uint8_t *data;
	long offset;
} ogg_page;

/**
 * ogg_error:
 * 
 * A human-readable string error from the last failed Ogg function
 */
extern const char *ogg_error;

/**
 * ogg_page_read:
 * @page: #ogg_page structure to save the read data into
 * @file: File to read the Ogg page from
 * 
 * Read the contents of an Ogg page from a file
 *
 * Returns: 0 on success, otherwise a value from #ogg_error_codes and #ogg_error
 * will contain a message
 */
int ogg_page_read(ogg_page *page, FILE *file);

#endif
