#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libgimp/gimp.h>
#include <gtk/gtk.h>

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
static gint load_dialog(void);

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

static void run(
    const gchar*      name,
    gint              nparams,
    const GimpParam*  param,
    gint*             nreturn_vals,
    GimpParam**       return_vals
) {
    static GimpParam values[2];
    GimpRunMode run_mode;
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

	    if (!load_dialog())
		goto exec_error;
	    break;

        case GIMP_RUN_NONINTERACTIVE:
	    /* TODO */
	    goto call_error;
	    break;

	case GIMP_RUN_WITH_LAST_VALS:
	    gimp_get_data(PLUGIN_NAME, &Image_input_data);
	    break;
	}
    }

    img = open_raw(param[1].data.d_string);

    if (img == ERROR)
	goto exec_error;

    if (run_mode != GIMP_RUN_INTERACTIVE) {
	gimp_image_clean_all(img);
    } else {
	gimp_set_data(PLUGIN_NAME, &Image_input_data, sizeof(struct raw_data));
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

    return img;
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

/*
 * ===========================================================================
 * GTK part
 * ===========================================================================
 */

gint Dialog_result_ok;

static void show_message(char *msg)
{
    /*
    if (l_run_mode == GIMP_RUN_INTERACTIVE)
	gtk_message_box(msg);
    else
    */
	fprintf (stderr, "Import-raw: %s\n", msg);
}

static void load_on_close(GtkWidget *widget, gpointer data)
{
    gtk_main_quit();
}

static void load_on_ok(GtkWidget *widget, gpointer data)
{
    /* TODO */
    //gtk_widget_destroy(GTK_WIDGET(vals->dialog));

    gtk_main_quit();
    return;
}


static void load_on_cancel(GtkWidget *widget, gpointer data)
{
    Dialog_result_ok = FALSE;
    gtk_main_quit();
    return;
}

static gint load_dialog(void)
{
    GtkWidget *dlg;
    GtkWidget *button;
    gchar **argv = g_new(gchar*, 1);
    gint argc = 1;

    argv[0] = g_strdup("load");
    gtk_init (&argc, &argv);
    gtk_rc_parse(gimp_gtkrc());
    gdk_set_use_xshm(TRUE);

    dlg = gtk_dialog_new();
    Dialog_result_ok = TRUE;

    gtk_window_set_title(GTK_WINDOW (dlg), "Load raw image");
    gtk_window_position(GTK_WINDOW (dlg), GTK_WIN_POS_MOUSE);
    gtk_signal_connect(GTK_OBJECT (dlg), "destroy",
				  (GtkSignalFunc) load_on_close, NULL);

    /* OK button */
    button = gtk_button_new_with_label("OK");
    GTK_WIDGET_SET_FLAGS(button, GTK_CAN_DEFAULT);
    gtk_signal_connect(GTK_OBJECT (button), "clicked",
				      (GtkSignalFunc) load_on_ok, NULL);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dlg)->action_area), button,
							      TRUE, TRUE, 0);
    gtk_widget_grab_default(button);
    gtk_widget_show(button);

    /* Cancel button */
    button = gtk_button_new_with_label("Cancel");
    GTK_WIDGET_SET_FLAGS(button, GTK_CAN_DEFAULT);
    gtk_signal_connect(GTK_OBJECT (button), "clicked",
				      (GtkSignalFunc) load_on_cancel, NULL);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dlg)->action_area), button,
							      TRUE, TRUE, 0);
    gtk_widget_grab_default(button);
    gtk_widget_show(button);

    gtk_widget_show(dlg);
    gtk_main();
    gdk_flush();

    return Dialog_result_ok;
}
