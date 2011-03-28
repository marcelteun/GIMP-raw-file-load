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

#ifndef __FILE_RAW_LOAD_GTK_H
#define __FILE_RAW_LOAD_GTK_H

#include <libgimp/gimp.h>
#include "file-raw-load.h"

void show_message(gchar *msg);
gint load_dialog(struct raw_data* img_data, gchar* check_button_label);

#endif
