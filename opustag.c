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

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
	FILE *opusfile;
	struct ogg_page header_page = { 0 }, tags_page = { 0 };
	int ret;
	
	if (argc != 2) {
		fprintf(stderr, "Usage: %s file.opus\n", argv[0]);
		exit(1);
	}
	
	opusfile = fopen(argv[1], "rb");
	if (!opusfile) {
		perror("Failed to open input file");
		exit(1);
	}
	
	ret = ogg_page_read(&header_page, opusfile);
	if (ret != OGG_SUCCESS) {
		fprintf(stderr, "Failed to read Ogg Page: %s\n", ogg_error);
		exit(1);
	}
	fprintf(stderr, "Page contains %d data bytes\n", header_page.data_len);
	
	ret = ogg_page_read(&tags_page, opusfile);
	if (ret != OGG_SUCCESS) {
		fprintf(stderr, "Failed to read Ogg Page: %s\n", ogg_error);
		exit(1);
	}
	fprintf(stderr, "Page contains %d data bytes\n", tags_page.data_len);



	
	
	

	
	
	
	
	
	free(header_page.data);
	header_page.data = NULL;
	free(tags_page.data);
	tags_page.data = NULL;
	
}
