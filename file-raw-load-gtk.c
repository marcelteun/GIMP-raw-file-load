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

#include <libgimp/gimp.h>
#include <gtk/gtk.h>
#include <stdio.h>

#include "file-raw-load.h"
#include "file-raw-load-gtk.h"

struct load_dlg_data {
    GtkWidget*       id;
    GtkWidget*       size[SIZE_DIM];
    struct raw_data* img_data;
    gint             result_ok;
};

static void load_on_close(GtkWidget *widget, gpointer data)
{
    gtk_main_quit();
}

static void load_on_ok(GtkWidget *widget, gpointer data)
{
    struct load_dlg_data* dlg_data;
    int i;

    if (data == NULL) {
	gtk_main_quit();
	return;
    }
    dlg_data = (struct load_dlg_data *) data;

    for (i = 0; i < SIZE_DIM; i++) {
	gint s_i;
	if (sscanf(
		gtk_entry_get_text(GTK_ENTRY(dlg_data->size[i])), "%d", &s_i
	    ) > 0
	) {
	    guint s_ui;
	    if (s_i < 0) {
		s_ui = 0;
	    } else {
		s_ui = s_i;
	    }
	    dlg_data->img_data->size[i] = s_ui;
	}
    }

    gtk_widget_destroy(GTK_WIDGET(dlg_data->id));
    dlg_data->result_ok = TRUE;

    return;
}


static void load_on_cancel(GtkWidget *widget, gpointer data)
{
    struct load_dlg_data* dlg_data;

    gtk_main_quit();
    if (data == NULL) {
	return;
    }
    dlg_data = (struct load_dlg_data *) data;
    dlg_data->result_ok = FALSE;
    return;
}

/*
 * EXTERNALS
 */

void show_message(gchar *msg)
{
    fprintf (stderr, "Import-raw: %s\n", msg);
}

gint load_dialog(struct raw_data* img_data)
{
    int i;
    GtkWidget* dlg;
    GtkWidget* wgt;
    gchar** argv = g_new(gchar*, 1);
    gint argc = 1;
    struct load_dlg_data dlg_data;
    static char *labs[] = {"Width", "Height"};

    if (!img_data) return ERROR;

    dlg_data.img_data = img_data;

    argv[0] = g_strdup("load");
    gtk_init (&argc, &argv);
    gtk_rc_parse(gimp_gtkrc());
    gdk_set_use_xshm(TRUE);

    dlg_data.id = gtk_dialog_new();
    dlg = dlg_data.id;

    gtk_window_set_title(GTK_WINDOW (dlg), "Load raw image");
    gtk_window_position(GTK_WINDOW (dlg), GTK_WIN_POS_MOUSE);
    gtk_signal_connect(GTK_OBJECT (dlg), "destroy",
				  (GtkSignalFunc) load_on_close, NULL);

    /* OK button */
    wgt = gtk_button_new_with_label("OK");
    GTK_WIDGET_SET_FLAGS(wgt, GTK_CAN_DEFAULT);
    gtk_signal_connect(GTK_OBJECT (wgt), "clicked",
				      (GtkSignalFunc) load_on_ok, &dlg_data);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dlg)->action_area), wgt,
							      TRUE, TRUE, 0);
    gtk_widget_grab_default(wgt);
    gtk_widget_show(wgt);

    /* Cancel button */
    wgt = gtk_button_new_with_label("Cancel");
    GTK_WIDGET_SET_FLAGS(wgt, GTK_CAN_DEFAULT);
    gtk_signal_connect(GTK_OBJECT (wgt), "clicked",
				  (GtkSignalFunc) load_on_cancel, &dlg_data);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dlg)->action_area), wgt,
							      TRUE, TRUE, 0);
    gtk_widget_grab_default(wgt);
    gtk_widget_show(wgt);

    /* Width and Height */
    wgt = gtk_table_new(2, SIZE_DIM, FALSE);
    gtk_table_set_row_spacings(GTK_TABLE(wgt), 3);
    gtk_table_set_col_spacings(GTK_TABLE(wgt), 3);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dlg)->vbox), wgt, TRUE, TRUE, 0);
    gtk_widget_show(wgt);

    for (i = 0; i < SIZE_DIM; i++) {
	char init_size[16];

	GtkWidget *label = gtk_label_new(labs[i]);
	gtk_misc_set_alignment(GTK_MISC(label), 0.0, 0.5);
	gtk_table_attach(GTK_TABLE(wgt), label, 0, 1, i, i+1, GTK_FILL,
							    GTK_FILL, 0, 0);
	gtk_widget_show(label);

	dlg_data.size[i] = gtk_entry_new();
	gtk_widget_set_usize(dlg_data.size[i], 35, 0);
	sprintf(init_size, "%d", (int) dlg_data.img_data->size[i]);
	gtk_entry_set_text(GTK_ENTRY(dlg_data.size[i]), init_size);
	gtk_table_attach(GTK_TABLE(wgt), dlg_data.size[i], 1, 2, i, i+1,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
	gtk_widget_show(dlg_data.size[i]);
    }

    gtk_widget_show(dlg);
    gtk_main();
    gdk_flush();

    return dlg_data.result_ok;
}
