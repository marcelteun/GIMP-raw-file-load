/* Copyright (C) 2011 Marcel Tunnissen
 *
 * This file implements a GTK dialog for loading raw images used by
 * GIMP plugins that load raw images in a RGB & YUV format.
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

#include <stdio.h>
#include <libgimp/gimp.h>

#include "file-raw-load.h"
#include "file-raw-load-gtk.h"

#define IDX_RED		0
#define IDX_GREEN	1
#define IDX_BLUE	2

static gint32 create_new_layer(
    gint32              img,
    gint                position,
    const gchar*        layername,
    guint               width,
    guint               height,
    GimpImageBaseType   bas_type,
    GimpDrawable**      drawable,
    GimpPixelRgn*       pixel_rgn)
{
    gint32        layer;
    GimpImageType img_type = GIMP_RGB_IMAGE;

    /* TODO: add support for GIMP_RGBA_IMAGE in case of RGBA888 */
    switch (bas_type)
    {
	case GIMP_RGB:
	    img_type = GIMP_RGB_IMAGE;
	    break;
	case GIMP_GRAY:
	    img_type = GIMP_GRAY_IMAGE;
	    break;
	case GIMP_INDEXED:
	    img_type = GIMP_INDEXED_IMAGE;
	    break;
    }

    if (!layername) {
	layername = "background";
    }

    layer = gimp_layer_new(img, layername, width, height, img_type, 100,
							    GIMP_NORMAL_MODE);
    gimp_image_add_layer(img, layer, position);

    *drawable = gimp_drawable_get(layer);
    gimp_pixel_rgn_init(pixel_rgn, *drawable, 0, 0, (*drawable)->width,
					    (*drawable)->height, TRUE, FALSE);

    return layer;
}

static gint32 create_new_image(
	const gchar*        filename,
	const gchar*        layername,
	guint               width,
	guint               height,
	GimpImageBaseType   bas_type,
	//gdouble             xres,
	//gdouble             yres,
	gint32*             layer,
	GimpDrawable**      drawable,
	GimpPixelRgn*       pixel_rgn
) {
  gint32 img;

  img = gimp_image_new(width, height, bas_type);

  gimp_image_undo_disable(img);
  gimp_image_set_filename(img, filename);
  //gimp_image_set_resolution(img, xres, yres);

  *layer = create_new_layer(img, 0, layername, width, height, bas_type,
							drawable, pixel_rgn);
  return img;
}

static guchar read_rgb_pixel(FILE* file, enum pix_fmt fmt, guchar rgb[3]) {
/*
 * returns ERROR if an EOF occurred or if the fmt is not supported
 */
    guchar c;
    switch (fmt) {
    case RGB_565:
	/* read (least significant) part of GREEN and BLUE */
        if ((c = fgetc(file)) == EOF) return ERROR;
	rgb[IDX_GREEN] = (c & 0xe0) >> 3;
	rgb[IDX_BLUE]  = (c & 0x1f) << 3;
	/* read RED and (most significant) part of GREEN */
        if ((c = fgetc(file)) == EOF) return ERROR;
	rgb[IDX_RED]   = c & 0xf8;
	rgb[IDX_GREEN] = rgb[1] | (c & 0x07) << 5;
	return OK;
	break;
    default:
	return ERROR;
	break;
    }
}

static gint32 load_raw(
    char*            filename,
    FILE*            file,
    enum             pix_fmt fmt,
    struct raw_data* img_data,
    const gchar*     plugin_name
) {
    gint nr_chls;
    gint x, y;
    gint32 img;
    gint32 layer;
    GimpDrawable* drawable;
    GimpPixelRgn pixel_rgn;
    guchar *pix_row;

    guint width = img_data->size[0];
    guint height = img_data->size[1];
    if (width <= 0 || height <= 0) {
	return ERROR;
    }
    img = create_new_image(filename, NULL, width, height, GIMP_RGB, &layer,
						    &drawable, &pixel_rgn);
    nr_chls = gimp_drawable_bpp(drawable->drawable_id);
    pix_row = g_new(guchar, nr_chls * width);
    x = 0;
    y = 0;
    while (1) {
        guchar rgb[3];
	if (read_rgb_pixel(file, fmt, rgb) == ERROR)
	    return ERROR;
	pix_row[nr_chls * x]     = rgb[IDX_RED];
	pix_row[nr_chls * x + 1] = rgb[IDX_GREEN];
	pix_row[nr_chls * x + 2] = rgb[IDX_BLUE];
	x++;
	if (x == width) {
	    gimp_pixel_rgn_set_row(&pixel_rgn, pix_row, 0, y, width);
	    x = 0;
	    y++;
	}
	if (y == height) break;
    }
    if (fgetc(file) != EOF) {
	printf("%s: warning: file contains more data\n", plugin_name);
    }

    return img;
}

static gint32 open_raw(
    char*            filename,
    enum pix_fmt     fmt,
    struct raw_data* img_data,
    const gchar*     plugin_name
) {
    gint32 img;
    FILE *file;

    file = fopen(filename, "rb");
    if (!file)
    {
	show_message("can't open file for reading");
	return(ERROR);
    }
    img = load_raw(filename, file, fmt, img_data, plugin_name);
    fclose (file);

    return img;
}

/*
 * EXTERNALS
 */

void run(
    const gchar*      plugin_name,
    gint              nparams,
    const GimpParam*  param,
    gint*             nreturn_vals,
    GimpParam**       return_vals,
    enum pix_fmt      fmt,
    struct raw_data*  img_data
) {
    static GimpParam values[2];
    GimpRunMode run_mode;
    gint32 img = ERROR;

    if (!img_data) goto call_error;

    run_mode = param[0].data.d_int32;

    values[0].type = GIMP_PDB_STATUS;
    values[0].data.d_status = GIMP_PDB_SUCCESS;
    values[1].type = GIMP_PDB_IMAGE;
    values[1].data.d_image = ERROR;
    *return_vals = values;
    *nreturn_vals = 2;

    switch (run_mode)
    {
    case GIMP_RUN_INTERACTIVE:
	/*  Possibly retrieve data  */
	gimp_get_data(plugin_name, img_data);

	if (!load_dialog(img_data))
	    goto exec_error;
	break;

    case GIMP_RUN_NONINTERACTIVE:
	/* TODO */
	goto call_error;
	break;

    case GIMP_RUN_WITH_LAST_VALS:
	gimp_get_data(plugin_name, img_data);
	break;
    }

    img = open_raw(param[1].data.d_string, fmt, img_data, plugin_name);

    if (img == ERROR)
	goto exec_error;

    if (run_mode != GIMP_RUN_INTERACTIVE) {
	gimp_image_clean_all(img);
    } else {
	gimp_set_data(plugin_name, img_data, sizeof(struct raw_data));
    }
    values[0].data.d_status = GIMP_PDB_SUCCESS;
    values[1].data.d_image = img;
    return;

call_error:
    values[0].data.d_status = GIMP_PDB_EXECUTION_ERROR;
    return;

exec_error:
    values[0].data.d_status = GIMP_PDB_EXECUTION_ERROR;
    return;
}
