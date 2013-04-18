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
#include "oggopus.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
	struct ogg_stream *infile = NULL, *outfile = NULL;
	struct ogg_page header_page, tags_page;
	int ret, err = 0;
	uint32_t serial;
	
	ogg_page_init(&header_page);
	ogg_page_init(&tags_page);
	
	if (argc != 3) {
		fprintf(stderr, "Usage: %s input.opus output.opus\n", argv[0]);
		err = 1;
		goto error;
	}
	
	infile = ogg_stream_file_open_read(argv[1]);
	if (!infile) {
		fprintf(stderr, "Failed to open input file: %s\n", ogg_error);
		err = 1;
		goto error;
	}
	
	ret = ogg_page_read(&header_page, infile);
	if (ret != OGG_SUCCESS) {
		fprintf(stderr, "Failed to read Ogg Page: %s\n", ogg_error);
		err = 1;
		goto error;
	}
	fprintf(stderr, "Page contains %d data bytes\n", header_page.data_len);

	if (oggopus_recognize(&header_page)) {
		fprintf(stderr, "File is not OggOpus!\n");
		err = 1;
		goto error;
	}
	serial = header_page.serial;
	printf("OggOpus found in stream serial %#x\n", header_page.serial);


	ret = ogg_page_read(&tags_page, infile);
	if (ret != OGG_SUCCESS) {
		fprintf(stderr, "Failed to read Ogg Page: %s\n", ogg_error);
		err = 1;
		goto error;
	}
	if (tags_page.serial != serial) {
		fprintf(stderr, "Multi-stream files are not supported\n");
		err = 1;
		goto error;
	}
	
	
	fprintf(stderr, "Page contains %d data bytes\n", tags_page.data_len);

	outfile = ogg_stream_file_open(argv[2]);
	if (!outfile) {
		fprintf(stderr, "Failed to open output file: %s\n", ogg_error);
		err = 1;
		goto error;
	}
	
	ret = ogg_page_write(&header_page, outfile);
	if (ret != OGG_SUCCESS) {
		fprintf(stderr, "Failed to write Ogg Page: %s\n", ogg_error);
		err = 1;
		goto error;
	}
	ret = ogg_page_write(&tags_page, outfile);
	if (ret != OGG_SUCCESS) {
		fprintf(stderr, "Failed to write Ogg Page: %s\n", ogg_error);
		err = 1;
		goto error;
	}

error:

	if (outfile) {
		ogg_stream_file_close(outfile);
		outfile = NULL;
	}
	
	if (infile) {
		ogg_stream_file_close(infile);
		infile = NULL;
	}

	ogg_page_clear(&header_page);
	ogg_page_clear(&tags_page);
	
	return err;
}
