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

static const uint8_t OGGOPUS_HEAD_MAGIC[] = {
	0x4f, 0x70, 0x75, 0x73, 0x48, 0x65, 0x61, 0x64
};

static const uint8_t OGGOPUS_TAGS_MAGIC[] = {
	0x4f, 0x70, 0x75, 0x73, 0x54, 0x61, 0x67, 0x73
}
