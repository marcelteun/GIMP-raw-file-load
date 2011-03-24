/* Copyright (C) 2011 Marcel Tunnissen
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

#ifndef __FILE_RAW_LOAD_H
#define __FILE_RAW_LOAD_H

#define ERROR	-1
#define OK	0

enum pix_fmt {
	RGB_565 = 0,
	RGBA_5551,
	RGBA_4444,
	RGB_888,
	RGBX_8888,
	RGBA_8888,
	YCbCr_422,
	nr_of_pix_fmts,
};

#define SIZE_DIM 2
struct raw_data {
    guint size[SIZE_DIM];
};

void run(
    const gchar*      plugin_name,
    gint              nparams,
    const GimpParam*  param,
    gint*             nreturn_vals,
    GimpParam**       return_vals,
    enum pix_fmt      fmt,
    struct raw_data*  img_data
);

#endif
