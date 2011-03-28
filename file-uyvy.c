/* Copyright (C) 2011 Marcel Tunnissen
 *
 * This file implements a GIMP plugin for load raw images in a YUV 422 format
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

#include <string.h>

#include <libgimp/gimp.h>

#include "file-raw-load.h"

static void query_yuv(void);
static void run_yuv(
    const gchar*      name,
    gint              nparams,
    const GimpParam*  param,
    gint*             nreturn_vals,
    GimpParam**       return_vals
);

GimpPlugInInfo PLUG_IN_INFO =
{
  .init_proc = NULL,
  .quit_proc = NULL,
  .query_proc = query_yuv,
  .run_proc = run_yuv
};

MAIN()

static GimpParamDef args[] =
{
    {GIMP_PDB_INT32, "run-mode", "Interactive, non-interactive"},
    {GIMP_PDB_STRING, "filename", "The name of the file to load"},
    {GIMP_PDB_STRING, "raw_filename", "The name of the file to load"},
};
static int nr_args = G_N_ELEMENTS(args);

static GimpParamDef results[] =
{
    {GIMP_PDB_IMAGE, "image", "Loaded image" },
};
static int nr_results = G_N_ELEMENTS(results);

#define PLUGIN_VERSION "2011-03-26"
#define PLUGIN_NAME "raw_file_yuv_422_load"

static struct raw_data Image_input_data = {
    .size = {640, 480},
};

static void query_yuv(void)
{
    gimp_install_procedure(
	PLUGIN_NAME,
	"raw file YUV-422 load",
	"Load raw file in YUV 422 format",
	"Marcel Tunnissen",
	"Copyright Marcel Tunnissen",
	PLUGIN_VERSION,
	"<Load>/YUV422",
	NULL,
	GIMP_PLUGIN,
	nr_args, nr_results, args, results
    );

    gimp_register_load_handler(PLUGIN_NAME, "UYVY, YUYV, YUV-422, 422", "");
}

static void run_yuv(
    const gchar*      name,
    gint              nparams,
    const GimpParam*  param,
    gint*             nreturn_vals,
    GimpParam**       return_vals
) {
    if (strcmp(name, PLUGIN_NAME) == 0) {
	run(name, nparams, param, nreturn_vals, return_vals, UYVY_422,
							    &Image_input_data);
    }
}
