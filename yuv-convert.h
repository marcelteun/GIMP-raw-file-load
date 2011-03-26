
/* Copyright (C) 2011 Marcel Tunnissen
 *
 * This file implements RGB / YUV conversion.
 *
 * License: GNU Public License version 2
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not,
 * check at http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 * or write to the Free Software Foundation,
 */

#ifndef __YUV_CONVERT_H
#define __YUV_CONVERT_H

#include <libgimp/gimp.h>

void convert_rgb2yuv(guchar rgb[3], guchar yuv[3]);
void convert_yuv2rgb(guchar yuv[3], guchar rgb[3]);

#endif /* __YUV_CONVERT_H */
