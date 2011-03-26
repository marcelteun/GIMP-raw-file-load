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

#include <libgimp/gimp.h>

static gint32 M_rgb2yuv[3][3] = {
    {66, 129, 25},
    {-38, -74, 112},
    {112, -94, -18},
};
static gint32 O_rgb2yuv[3] = {16, 128, 128};
static gint32 M_yuv2rgb[3][3] = {
    {298, 0, 409},
    {298, -100, -208},
    {298, 516, 0},
};
static gint32 O_yuv2rgb[3] = {-223, 136, -277};
static void convert(gint32 M[3][3], gint32 O[3], guchar in[3], guchar out[3])
{
    int i, j;
    for (i = 0; i < 3; i++) {
        gint32 ch_out = 0;
        for (j = 0; j < 3; j++) {
            ch_out += M[i][j] * in[j];
	}
        ch_out /= 256;
        ch_out += O[i];
        /* clamp */
        if (ch_out > 255) {
            ch_out = 0xff;
        } else if (ch_out < 0) {
            ch_out = 0;
        }
        out[i] = 0xff & ch_out;
    }
}
void convert_rgb2yuv(guchar rgb[3], guchar yuv[3])
{
    convert(M_rgb2yuv, O_rgb2yuv, rgb, yuv);
}
void convert_yuv2rgb(guchar yuv[3], guchar rgb[3])
{
    convert(M_yuv2rgb, O_yuv2rgb, yuv, rgb);
}

