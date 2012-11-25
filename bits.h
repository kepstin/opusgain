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

#ifndef BITS_H
#define BITS_H

static inline uint16_t read_le16(const uint8_t *in) {
	return ((uint16_t) in[0]) |
	       (((uint16_t) in[1]) << 8);
}
static inline uint32_t read_le32(const uint8_t *in) {
	return ((uint32_t) in[0]) |
	       (((uint32_t) in[1]) << 8) |
	       (((uint32_t) in[2]) << 16) |
	       (((uint32_t) in[3]) << 24);
}
static inline uint64_t read_le64(const uint8_t *in) {
	return ((uint64_t) in[0]) |
	       (((uint64_t) in[1]) << 8) |
	       (((uint64_t) in[2]) << 16) |
	       (((uint64_t) in[3]) << 24) |
	       (((uint64_t) in[4]) << 32) |
	       (((uint64_t) in[5]) << 40) |
	       (((uint64_t) in[6]) << 48) |
	       (((uint64_t) in[7]) << 56);
}

static inline void write_le32(uint8_t *out, uint32_t in) {
	out[0] = in & 0xff;
	out[1] = (in & 0xff00) >> 8;
	out[2] = (in & 0xff0000) >> 16;
	out[3] = (in & 0xff000000) >> 24;
}

#endif
