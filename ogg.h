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

/**
 * SECTION:ogg
 * @short_description: A miniature non-conformant ogg library
 * @title: Ogg
 *
 * This library code handles parsing and in-place modification of a limited
 * subset of Ogg streams stored in local files, such as Ogg files containing
 * a single audio logical stream.
 *
 * The purpose of this code is to allow writing an application that can modify
 * stream header packets without requiring that the file be completely
 * re-written, such as would be requires with the official libogg.
 */

#ifndef OGG_H
#define OGG_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

/**
 * ogg_error_codes:
 * @OGG_SUCCESS: Everything is fine
 * @OGG_INVALID: An incoming Ogg stream contains invalid data
 * @OGG_MULTISTREAM: The Ogg container contains multiple streams
 * @OGG_BAD_SIZE: An #ogg_page contains a data amount that isn't allowed by spec
 */
typedef enum {
	OGG_SUCCESS = 0,
	OGG_INVALID,
	OGG_MULTI_STREAM,
	OGG_BAD_SIZE,
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
	off_t offset;
} ogg_page;

typedef struct ogg_packet_page {
	ogg_page page;
	size_t offset;
	size_t length;
	struct ogg_packet_page *next;
} ogg_packet_page;

typedef struct ogg_packet {
	uint64_t data_len;
	ogg_packet_page first;
} ogg_packet;

typedef struct ogg_stream ogg_stream;

typedef struct ogg_stream_io_functions {
	size_t (*read)(ogg_stream *stream, uint8_t *buffer, size_t len);
	size_t (*write)(ogg_stream *stream, const uint8_t *buffer, size_t len);
	off_t (*tell)(ogg_stream *stream);
	int (*seek)(ogg_stream *stream, off_t offset);
} ogg_stream_io_functions;

struct ogg_stream {
	ogg_stream_io_functions *io;
	void *priv;
	ogg_packet *current;
	size_t offset;
};

/**
 * ogg_error:
 * 
 * A human-readable string error from the last failed Ogg function
 */
extern const char *ogg_error;

/**
 * ogg_page_init:
 * @page: An uninitialized #ogg_page structure
 *
 * Initialize a previously unused #ogg_page structure
 */
void ogg_page_init(ogg_page *page);

/**
 * ogg_page_clear:
 * @page: A previously-used #ogg_page structure
 *
 * Free memory internally allocated for the Ogg page, and reinitialize.
 */
void ogg_page_clear(ogg_page *page);

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

/**
 * ogg_page_write:
 * @page: An #ogg_page structure to write to a file
 * @file: The file to write the Ogg page to
 *
 * Write the contents of an Ogg page to a file
 *
 * Returns: 0 on success, otherwise a value from #ogg_error_codes and #ogg_error
 * will contain a message
 */
int ogg_page_write(const ogg_page *page, FILE *file);

/**
 * ogg_packet_init:
 * @packet: An uninitialized #ogg_packet structure
 *
 * Initialize a previously unused #ogg_page structure
 */
void ogg_packet_init(ogg_packet *packet);

/**
 * ogg_packet_clear:
 * @packet: A previously-used #ogg_packet structure
 *
 * Free memory internally allocated for the Ogg packet, and reinitialize.
 */
void ogg_packet_clear(ogg_packet *packet);

/**
 * ogg_stream_file_open:
 * @filename: The path to a local ogg file
 *
 * Open a local file for use with the ogg packet api
 *
 * Returns: A newly allocated #ogg_stream, which must be freed with
 * ogg_stream_file_close() or %NULL on error, in which case #ogg_error will
 * have a message.
 */
ogg_stream *ogg_stream_file_open(const char *filename);

/**
 * ogg_stream_file_close:
 * @stream: The #ogg_stream to close and free
 *
 * Close an ogg stream opened with ogg_stream_file_open() and free allocated
 * memory
 */
void ogg_stream_file_close(ogg_stream *stream);

#endif
