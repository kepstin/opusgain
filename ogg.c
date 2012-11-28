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

#include "ogg.h"
#include "bits.h"

#include <errno.h>
#include <string.h>
#include <stdlib.h>

/* Size of an Ogg page header, minus the segment table */
#define OGG_PAGE_HEADER_SIZE 27

/* Maximum number of segments that can be in an Ogg page */
#define OGG_PAGE_MAX_SEGMENTS 255

#define OGG_GRANULE_POS_NO_PACKET 0xffffffffffffffff

/* The 'OggS' sync indicator for Ogg pages */
static const uint8_t OGG_PAGE_MAGIC[] = {
	0x4f, 0x67, 0x67, 0x53
};

/* Lookup table for the Ogg page CRC32 verification */
static const uint32_t crc_lookup[256]={
	0x00000000,0x04c11db7,0x09823b6e,0x0d4326d9,
	0x130476dc,0x17c56b6b,0x1a864db2,0x1e475005,
	0x2608edb8,0x22c9f00f,0x2f8ad6d6,0x2b4bcb61,
	0x350c9b64,0x31cd86d3,0x3c8ea00a,0x384fbdbd,
	0x4c11db70,0x48d0c6c7,0x4593e01e,0x4152fda9,
	0x5f15adac,0x5bd4b01b,0x569796c2,0x52568b75,
	0x6a1936c8,0x6ed82b7f,0x639b0da6,0x675a1011,
	0x791d4014,0x7ddc5da3,0x709f7b7a,0x745e66cd,
	0x9823b6e0,0x9ce2ab57,0x91a18d8e,0x95609039,
	0x8b27c03c,0x8fe6dd8b,0x82a5fb52,0x8664e6e5,
	0xbe2b5b58,0xbaea46ef,0xb7a96036,0xb3687d81,
	0xad2f2d84,0xa9ee3033,0xa4ad16ea,0xa06c0b5d,
	0xd4326d90,0xd0f37027,0xddb056fe,0xd9714b49,
	0xc7361b4c,0xc3f706fb,0xceb42022,0xca753d95,
	0xf23a8028,0xf6fb9d9f,0xfbb8bb46,0xff79a6f1,
	0xe13ef6f4,0xe5ffeb43,0xe8bccd9a,0xec7dd02d,
	0x34867077,0x30476dc0,0x3d044b19,0x39c556ae,
	0x278206ab,0x23431b1c,0x2e003dc5,0x2ac12072,
	0x128e9dcf,0x164f8078,0x1b0ca6a1,0x1fcdbb16,
	0x018aeb13,0x054bf6a4,0x0808d07d,0x0cc9cdca,
	0x7897ab07,0x7c56b6b0,0x71159069,0x75d48dde,
	0x6b93dddb,0x6f52c06c,0x6211e6b5,0x66d0fb02,
	0x5e9f46bf,0x5a5e5b08,0x571d7dd1,0x53dc6066,
	0x4d9b3063,0x495a2dd4,0x44190b0d,0x40d816ba,
	0xaca5c697,0xa864db20,0xa527fdf9,0xa1e6e04e,
	0xbfa1b04b,0xbb60adfc,0xb6238b25,0xb2e29692,
	0x8aad2b2f,0x8e6c3698,0x832f1041,0x87ee0df6,
	0x99a95df3,0x9d684044,0x902b669d,0x94ea7b2a,
	0xe0b41de7,0xe4750050,0xe9362689,0xedf73b3e,
	0xf3b06b3b,0xf771768c,0xfa325055,0xfef34de2,
	0xc6bcf05f,0xc27dede8,0xcf3ecb31,0xcbffd686,
	0xd5b88683,0xd1799b34,0xdc3abded,0xd8fba05a,
	0x690ce0ee,0x6dcdfd59,0x608edb80,0x644fc637,
	0x7a089632,0x7ec98b85,0x738aad5c,0x774bb0eb,
	0x4f040d56,0x4bc510e1,0x46863638,0x42472b8f,
	0x5c007b8a,0x58c1663d,0x558240e4,0x51435d53,
	0x251d3b9e,0x21dc2629,0x2c9f00f0,0x285e1d47,
	0x36194d42,0x32d850f5,0x3f9b762c,0x3b5a6b9b,
	0x0315d626,0x07d4cb91,0x0a97ed48,0x0e56f0ff,
	0x1011a0fa,0x14d0bd4d,0x19939b94,0x1d528623,
	0xf12f560e,0xf5ee4bb9,0xf8ad6d60,0xfc6c70d7,
	0xe22b20d2,0xe6ea3d65,0xeba91bbc,0xef68060b,
	0xd727bbb6,0xd3e6a601,0xdea580d8,0xda649d6f,
	0xc423cd6a,0xc0e2d0dd,0xcda1f604,0xc960ebb3,
	0xbd3e8d7e,0xb9ff90c9,0xb4bcb610,0xb07daba7,
	0xae3afba2,0xaafbe615,0xa7b8c0cc,0xa379dd7b,
	0x9b3660c6,0x9ff77d71,0x92b45ba8,0x9675461f,
	0x8832161a,0x8cf30bad,0x81b02d74,0x857130c3,
	0x5d8a9099,0x594b8d2e,0x5408abf7,0x50c9b640,
	0x4e8ee645,0x4a4ffbf2,0x470cdd2b,0x43cdc09c,
	0x7b827d21,0x7f436096,0x7200464f,0x76c15bf8,
	0x68860bfd,0x6c47164a,0x61043093,0x65c52d24,
	0x119b4be9,0x155a565e,0x18197087,0x1cd86d30,
	0x029f3d35,0x065e2082,0x0b1d065b,0x0fdc1bec,
	0x3793a651,0x3352bbe6,0x3e119d3f,0x3ad08088,
	0x2497d08d,0x2056cd3a,0x2d15ebe3,0x29d4f654,
	0xc5a92679,0xc1683bce,0xcc2b1d17,0xc8ea00a0,
	0xd6ad50a5,0xd26c4d12,0xdf2f6bcb,0xdbee767c,
	0xe3a1cbc1,0xe760d676,0xea23f0af,0xeee2ed18,
	0xf0a5bd1d,0xf464a0aa,0xf9278673,0xfde69bc4,
	0x89b8fd09,0x8d79e0be,0x803ac667,0x84fbdbd0,
	0x9abc8bd5,0x9e7d9662,0x933eb0bb,0x97ffad0c,
	0xafb010b1,0xab710d06,0xa6322bdf,0xa2f33668,
	0xbcb4666d,0xb8757bda,0xb5365d03,0xb1f740b4
};

const char *ogg_error;

static uint32_t ogg_page_checksum(
	const uint8_t *page_header,
	uint8_t segments,
	const ogg_page *page
) {
	uint32_t crc_reg = 0;
	size_t i;
	
	for (i = 0; i < OGG_PAGE_HEADER_SIZE + segments; i++)
		crc_reg = (crc_reg<<8)^crc_lookup[((crc_reg>>24)&0xff)^page_header[i]];
	for (i = 0; i < page->data_len; i++)
		crc_reg = (crc_reg<<8)^crc_lookup[((crc_reg>>24)&0xff)^page->data[i]];
	
	return crc_reg;
}

void ogg_page_init(ogg_page *page) {
	memset(page, 0, sizeof (ogg_page));
	page->offset = -1;
}

void ogg_page_clear(ogg_page *page) {
	if (page->data)
		free(page->data);
	ogg_page_init(page);
}

int ogg_page_read(ogg_page *page, FILE *file) {
	size_t read;
	int i, err;
	uint32_t crc32;
	uint8_t page_header[OGG_PAGE_HEADER_SIZE + OGG_PAGE_MAX_SEGMENTS];
	uint8_t segments;
	
	/* Save the current file offset (for in-place rewrites) */
	page->offset = ftell(file);
	
	/* Read the first (fixed) part of the page header */
	read = fread(page_header, 1, OGG_PAGE_HEADER_SIZE, file);
	if (read < OGG_PAGE_HEADER_SIZE) {
		ogg_error = "Error reading page header";
		err = OGG_INVALID;
		goto error;
	}
	
	/* Check synchronization */
	if (memcmp(page_header, OGG_PAGE_MAGIC, 4)) {
		ogg_error = "Page signature does not match";
		err = OGG_INVALID;
		goto error;
	}
	
	/* Parse fixed header data fields */
	page->version = page_header[4];
	page->type = page_header[5];
	page->granule_pos = read_le64(&page_header[6]);
	page->serial = read_le32(&page_header[14]);
	page->seq = read_le32(&page_header[18]);
	crc32 = read_le32(&page_header[22]);
	segments = page_header[26];
	
	/* Read in the segment table */
	read = fread(&page_header[OGG_PAGE_HEADER_SIZE], 1, segments, file);
	if (read < segments) {
		ogg_error = "Error reading segment table";
		err = OGG_INVALID;
		goto error;
	}
	
	/* Parse data length from the segment table */
	page->data_len = 0;
	for (i = 0; i < segments; i++) {
		page->data_len += page_header[OGG_PAGE_HEADER_SIZE + i];
	}
	
	/* Read in the packet data */
	page->data = malloc(page->data_len);
	read = fread(page->data, 1, page->data_len, file);
	if (read < page->data_len) {
		ogg_error = "Error reading page data";
		err = OGG_INVALID;
		goto error;
	}
	
	/* Verify the page checksum */
	write_le32(&page_header[22], 0); /* CRC32 field must be blank */
	if (crc32 != ogg_page_checksum(page_header, segments, page)) {
		ogg_error = "Checksum mismatch";
		err = OGG_INVALID;
		goto error;
	}

	return OGG_SUCCESS;

error:
	if (page->data) {
		free(page->data);
		page->data = NULL;
	}

	return err;
}

int ogg_page_write(const ogg_page *page, FILE *file)
{
	int err;
	uint8_t page_header[OGG_PAGE_HEADER_SIZE + OGG_PAGE_MAX_SEGMENTS];
	uint8_t segments = 0;
	uint16_t data_len;
	uint32_t crc32;
	
	/* Do some basic sanity checks on the data size */
	if (page->granule_pos == OGG_GRANULE_POS_NO_PACKET) {
		if (page->data_len > 255*255) {
			ogg_error = "Continuing Ogg page can hold max 255*255 bytes";
			err = OGG_BAD_SIZE;
			goto error;
		}
	} else {
		if (page->data_len > 255*255 - 1) {
			ogg_error = "Non-continuing Ogg page can hold max 255*255 - 1 bytes";
			err = OGG_BAD_SIZE;
			goto error;
		}
	}
	
	/* Fill in the basic header fields */
	memcpy(page_header, OGG_PAGE_MAGIC, 4);
	page_header[4] = page->version;
	page_header[5] = page->type;
	write_le64(&page_header[6], page->granule_pos);
	write_le32(&page_header[14], page->serial);
	write_le32(&page_header[18], page->seq);
	write_le32(&page_header[22], 0); /* CRC32 field will be filled in later */
	
	/* Generate the segment lacing values */
	data_len = page->data_len;
	while (data_len >= 255) {
		page_header[OGG_PAGE_HEADER_SIZE + segments] = 0xff;
		segments++;
		data_len -= 255;
	}
	if (page->granule_pos == OGG_GRANULE_POS_NO_PACKET) {
		if (data_len > 0) {
			ogg_error = "Continuing Ogg page must contain a multiple of 255 bytes";
			err = OGG_BAD_SIZE;
			goto error;
		}
	} else {
		page_header[OGG_PAGE_HEADER_SIZE + segments] = data_len;
		segments++;
	}
	page_header[26] = segments;
	
	/* Compute the final checksum */
	crc32 = ogg_page_checksum(page_header, segments, page);
	write_le32(&page_header[22], crc32);
	
	/* Write the page header and data */
	fwrite(page_header, 1, OGG_PAGE_HEADER_SIZE + segments, file);
	fwrite(page->data, 1, page->data_len, file);
	
	return OGG_SUCCESS;

error:
	return err;
}

void ogg_packet_init(ogg_packet *packet) {
	packet->data_len = 0;
	ogg_page_init(&packet->first.page);
	packet->first.offset = 0;
	packet->first.length = 0;
	packet->first.next = NULL;
}

void ogg_packet_clear(ogg_packet *packet) {
	/* Need to clear the entire page chain if there are multiple pages */
	if (packet->first.next) {
		ogg_packet_page *current = packet->first.next;
		do {
			ogg_packet_page *next = current->next;
			
			ogg_page_clear(&current->page);
			free(current);
			
			current = next;
		} while (current);
	}
	
	packet->data_len = 0;
	ogg_page_clear(&packet->first.page);
	packet->first.next = NULL;
}

int ogg_packet_read(ogg_packet *packet) {
}

static ogg_stream *ogg_stream_new(void) {
	ogg_stream *stream = calloc(1, sizeof (ogg_stream));
	return stream;
}

static void ogg_stream_free(ogg_stream *stream) {
	free(stream);
}

static size_t ogg_stream_file_read(ogg_stream *stream, uint8_t *buffer, size_t len) {
	FILE *file = stream->priv;
	return fread(buffer, sizeof (uint8_t), len, file);
}

static size_t ogg_stream_file_write(ogg_stream *stream, const uint8_t *buffer, size_t len) {
	FILE *file = stream->priv;
	return fwrite(buffer, sizeof (uint8_t), len, file);
}

static off_t ogg_stream_file_tell(ogg_stream *stream) {
	FILE *file = stream->priv;
	return ftello(file);
}

static int ogg_stream_file_seek(ogg_stream *stream, off_t offset) {
	FILE *file = stream->priv;
	return fseeko(file, offset, SEEK_SET);
}

static ogg_stream_io_functions ogg_stream_file_functions = {
	ogg_stream_file_read,
	ogg_stream_file_write,
	ogg_stream_file_tell,
	ogg_stream_file_seek
};

ogg_stream *ogg_stream_file_open(const char *filename) {
	ogg_stream *stream;
	FILE *file;
	
	file = fopen(filename, "r+");
	if (!file) {
		ogg_error = strerror(errno);
		return NULL;
	}
	
	stream = ogg_stream_new();
	stream->io = &ogg_stream_file_functions;
	stream->priv = file;
	
	return stream;
}

void ogg_stream_file_close(ogg_stream *stream) {
	FILE *file = stream->priv;
	
	fclose(file);
	
	ogg_stream_free(stream);
}
