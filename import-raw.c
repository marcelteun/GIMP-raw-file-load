#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libgimp/gimp.h>

#include "gtk/gtk.h"

static void query (void);
static void run(
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
  .query_proc = query,
  .run_proc = run
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

#define PLUGIN_VERSION "0.1"
#define PLUGIN_NAME "raw_file_rgb565_load"
#define ERROR	-1

struct raw_data {
    guint width;
    guint height;
};
static struct raw_data Image_input_data = {
    .width = 640,
    .height = 480,
};

static gint32 open_raw(char* filename);
static gint32 load_raw(char* filename, FILE* file);
static gint32 create_new_image(
	const gchar        *filename,
	const gchar        *layername,
	guint               width,
	guint               height,
	GimpImageBaseType   bas_type,
	//gdouble             xres,
	//gdouble             yres,
	gint32             *layer,
	GimpDrawable      **drawable,
	GimpPixelRgn       *pixel_rgn
);
static void show_message(char *msg);

static void query (void)
{
    gimp_install_procedure(
	PLUGIN_NAME,
	"raw file RGB565 load",
	"Load raw file in RGB 556 format",
	"Marcel Tunnissen",
	"Copyright Marcel Tunnissen",
	PLUGIN_VERSION,
	"<Load>/RGB565",
	NULL,
	GIMP_PLUGIN,
	nr_args, nr_results, args, results
    );

    gimp_register_load_handler(PLUGIN_NAME, "raw, 565", "");
}

static GimpRunMode l_run_mode;

static void run (
    const gchar*      name,
    gint              nparams,
    const GimpParam*  param,
    gint*             nreturn_vals,
    GimpParam**       return_vals
) {
    static GimpParam values[2];
    GimpRunMode run_mode;
    GimpPDBStatusType status = GIMP_PDB_SUCCESS;
    gint32 img = ERROR;

    l_run_mode = run_mode = param[0].data.d_int32;

    values[0].type = GIMP_PDB_STATUS;
    values[0].data.d_status = GIMP_PDB_SUCCESS;
    *return_vals = values;
    *nreturn_vals = 1;

    if (strcmp(name, PLUGIN_NAME) == 0) {
	*nreturn_vals = 2;
	values[1].type = GIMP_PDB_IMAGE;
	values[1].data.d_image = ERROR;

	switch (run_mode)
	{
	case GIMP_RUN_INTERACTIVE:
	    /*  Possibly retrieve data  */
	    gimp_get_data(PLUGIN_NAME, &Image_input_data);

	    // TODO if (!load_dialog ()) return;
	    break;

        case GIMP_RUN_NONINTERACTIVE:
	    /* TODO */
	    status = GIMP_PDB_CALLING_ERROR;
	    break;

	case GIMP_RUN_WITH_LAST_VALS:
	    gimp_get_data(PLUGIN_NAME, &Image_input_data);
	    break;
	}
    }
    if (status == GIMP_PDB_SUCCESS) {
	img = open_raw(param[1].data.d_string);

	status = (img != ERROR) ? GIMP_PDB_SUCCESS : GIMP_PDB_EXECUTION_ERROR;

	if (status == GIMP_PDB_SUCCESS) {
	    if (run_mode != GIMP_RUN_INTERACTIVE) {
		/* Clean image */
		gimp_image_clean_all(img);
	    } else {
		/* Save new values */
		gimp_set_data(PLUGIN_NAME, &Image_input_data,
						   sizeof (struct raw_data));
	    }
	}
    }
    values[0].data.d_status = status;
    values[1].data.d_image = img;
}


static gint32 open_raw(char* filename)
{
    gint32 img;
    FILE *file;

    file = fopen(filename, "rb");
    if (!file)
    {
	show_message("can't open file for reading");
	return(ERROR);
    }
    img = load_raw(filename, file);
    fclose (file);

    return (img);
}

static gint32 load_raw(char* filename, FILE* file) {
    gint32 img;
    gint32 layer;
    GimpDrawable* drawable;
    GimpPixelRgn pixel_rgn;

    guint width = Image_input_data.width;
    guint height = Image_input_data.height;

    img = create_new_image(filename, NULL, width, height, GIMP_RGB, &layer,
							&drawable, &pixel_rgn);
    /*
    tile_height = gimp_tile_height ();
    dst = (unsigned char *)g_malloc (width * tile_height * 3);
    if (!dst) return ERROR;
    */
}

static gint32 create_new_layer(
    gint32              img,
    gint                position,
    const gchar        *layername,
    guint               width,
    guint               height,
    GimpImageBaseType   bas_type,
    GimpDrawable      **drawable,
    GimpPixelRgn       *pixel_rgn)
{
    gint32        layer;
    GimpImageType img_type = GIMP_RGB_IMAGE;

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
	const gchar        *filename,
	const gchar        *layername,
	guint               width,
	guint               height,
	GimpImageBaseType   bas_type,
	//gdouble             xres,
	//gdouble             yres,
	gint32             *layer,
	GimpDrawable      **drawable,
	GimpPixelRgn       *pixel_rgn
) {
  gint32 img;

  img = gimp_image_new (width, height, bas_type);

  gimp_image_undo_disable(img);
  gimp_image_set_filename(img, filename);
  //gimp_image_set_resolution(img, xres, yres);

  *layer = create_new_layer(img, 0, layername, width, height, bas_type,
							drawable, pixel_rgn);
  return img;
}

static void show_message(char *msg)
{
    /*
    if (l_run_mode == GIMP_RUN_INTERACTIVE)
	gtk_message_box(msg);
    else
    */
	fprintf (stderr, "Import-raw: %s\n", msg);
}
