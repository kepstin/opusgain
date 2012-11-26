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

#ifndef OGGOPUS_H
#define OGGOPUS_H

typedef struct ogg_page ogg_page;

/**
 * oggopus_recognize:
 * @page: An Ogg Opus page containing a possible OggOpus stream first packet
 *
 * Check if the Ogg page is the start of an OggOpus stream
 *
 * Returns: 0 if the stream appears to be OggOpus
 */
int oggopus_recognize(const ogg_page *page);

#endif
