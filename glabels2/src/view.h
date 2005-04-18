/*
 *  (GLABELS) Label and Business Card Creation program for GNOME
 *
 *  view.h:  GLabels View module header file
 *
 *  Copyright (C) 2001-2002  Jim Evins <evins@snaught.com>.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

#ifndef __VIEW_H__
#define __VIEW_H__

#include <gtk/gtkvbox.h>
#include <gtk/gtkenums.h>
#include <libgnomecanvas/libgnomecanvas.h>
#include <libgnomeprint/gnome-font.h>

#include "label-object.h"

typedef enum {
	GL_VIEW_STATE_ARROW,
	GL_VIEW_STATE_OBJECT_CREATE
} glViewState;

#define GL_TYPE_VIEW            (gl_view_get_type ())
#define GL_VIEW(obj)            (GTK_CHECK_CAST((obj), GL_TYPE_VIEW, glView ))
#define GL_VIEW_CLASS(klass)    (GTK_CHECK_CLASS_CAST ((klass), GL_TYPE_VIEW, glViewClass))
#define GL_IS_VIEW(obj)         (GTK_CHECK_TYPE ((obj), GL_TYPE_VIEW))
#define GL_IS_VIEW_CLASS(klass) (GTK_CHECK_CLASS_TYPE ((klass), GL_TYPE_VIEW))

typedef struct _glView      glView;
typedef struct _glViewClass glViewClass;

#include "view-object.h"
#include "color.h"

struct _glView {
	GtkVBox           parent_widget;

	glLabel          *label;

	GtkWidget        *canvas;
	gdouble           zoom;
	gboolean          zoom_to_fit_flag;
	gdouble           home_scale;

	GnomeCanvasGroup *bg_group;              /* Background layer */
	GnomeCanvasGroup *grid_group;            /* Grid layer */
	GnomeCanvasGroup *markup_group;          /* Markup layer */
	GnomeCanvasGroup *label_group;           /* Label layer (user objects) */
	GnomeCanvasGroup *fg_group;              /* Foreground layer */
	GnomeCanvasGroup *highlight_group;       /* Highlight layer */

	gdouble           grid_spacing;

	glViewState       state;
	glLabelObjectType create_type;

	GList             *object_list;           /* glViewObjects */
	GList             *selected_object_list;  /* glViewObjects */

	/* Clipboard selection stuff */
	gint              have_selection;
	glLabel           *selection_data;
	GtkWidget         *invisible;

	/* Menus */
	GList             *atomic_selection_items;
	GList             *multi_selection_items;
	GtkWidget         *selection_menu;
	GtkWidget         *empty_selection_menu;

	/* Merge Properties Dialog */
	GtkWidget         *merge_props_dialog;

	/* Print Dialog */
	GtkWidget         *print_dialog;

	/* Default object text properties */
	gchar             *default_font_family;
	gdouble            default_font_size;
	GnomeFontWeight    default_font_weight;
	gboolean           default_font_italic_flag;
	guint              default_text_color;
	GtkJustification   default_text_alignment;
	gdouble            default_text_line_spacing;

	/* Default object line properties */
	gdouble            default_line_width;
	guint              default_line_color;
	
	/* Default object fill properties */
	guint             default_fill_color;
};

struct _glViewClass {
	GtkVBoxClass      parent_class;

	/* Selection changed signal */
	void (*selection_changed) (glView   *view,
				   gpointer  user_data);

	/* Signals to support a status bar */
	void (*zoom_changed)      (glView   *view,
				   gdouble   zoom,
				   gpointer  user_data);
	void (*pointer_moved)     (glView   *view,
				   gdouble   x,
				   gdouble   y,
				   gpointer  user_data);
	void (*pointer_exit)      (glView   *view,
				   gpointer  user_data);
	void (*mode_changed)      (glView   *view,
				   gpointer  user_data);
};

G_BEGIN_DECLS

GType      gl_view_get_type                (void) G_GNUC_CONST;

GtkWidget *gl_view_new                     (glLabel           *label);


void       gl_view_show_grid               (glView            *view);

void       gl_view_hide_grid               (glView            *view);

void       gl_view_set_grid_spacing        (glView            *view,
					    gdouble            spacing);

void       gl_view_show_markup             (glView            *view);

void       gl_view_hide_markup             (glView            *view);

void       gl_view_arrow_mode              (glView            *view);

void       gl_view_object_create_mode      (glView            *view,
					    glLabelObjectType  type);


void       gl_view_select_object           (glView            *view,
					    glViewObject      *view_object);

void       gl_view_unselect_object         (glView            *view,
					    glViewObject      *view_object);

void       gl_view_select_all              (glView            *view);

void       gl_view_unselect_all            (glView            *view);

void       gl_view_select_region           (glView            *view,
					    gdouble            x1,
					    gdouble            y1,
					    gdouble            x2,
					    gdouble            y2);

gboolean   gl_view_is_object_selected      (glView            *view,
					    glViewObject      *view_object);

gboolean   gl_view_is_selection_empty      (glView            *view);

gboolean   gl_view_is_selection_atomic     (glView            *view);

void       gl_view_delete_selection        (glView            *view);

GtkWidget *gl_view_get_editor              (glView            *view);

void       gl_view_raise_selection         (glView            *view);

void       gl_view_lower_selection         (glView            *view);

void       gl_view_rotate_selection        (glView            *view,
					    gdouble            theta_degs);

void       gl_view_rotate_selection_left   (glView            *view);

void       gl_view_rotate_selection_right  (glView            *view);

void       gl_view_flip_selection_horiz    (glView            *view);

void       gl_view_flip_selection_vert     (glView            *view);

void       gl_view_align_selection_left    (glView            *view);

void       gl_view_align_selection_right   (glView            *view);

void       gl_view_align_selection_hcenter (glView            *view);

void       gl_view_align_selection_top     (glView            *view);

void       gl_view_align_selection_bottom  (glView            *view);

void       gl_view_align_selection_vcenter (glView            *view);

void       gl_view_center_selection_horiz  (glView            *view);

void       gl_view_center_selection_vert   (glView            *view);

void       gl_view_move_selection          (glView            *view,
					    gdouble            dx,
					    gdouble            dy);

gboolean   gl_view_can_selection_text             (glView           *view);

void       gl_view_set_selection_font_family      (glView           *view,
						   const gchar      *font_family);

void       gl_view_set_selection_font_size        (glView           *view,
						   gdouble           font_size);

void       gl_view_set_selection_font_weight      (glView           *view,
						   GnomeFontWeight   font_weight);

void       gl_view_set_selection_text_line_spacing (glView           *view,
						   gdouble           text_line_spacing);

void       gl_view_set_selection_font_italic_flag (glView           *view,
						   gboolean          font_italic_flag);

void       gl_view_set_selection_text_alignment   (glView           *view,
						   GtkJustification  text_alignment);

void       gl_view_set_selection_text_color       (glView           *view,
						   glColorNode      *text_color_node);

gboolean   gl_view_can_selection_fill             (glView           *view);

void       gl_view_set_selection_fill_color       (glView           *view,
						   glColorNode      *fill_color);

gboolean   gl_view_can_selection_line_color       (glView           *view);

void       gl_view_set_selection_line_color       (glView           *view,
						   glColorNode      *line_color_node);

gboolean   gl_view_can_selection_line_width       (glView           *view);

void       gl_view_set_selection_line_width       (glView           *view,
						   gdouble           line_width);


void       gl_view_cut                     (glView            *view);

void       gl_view_copy                    (glView            *view);

void       gl_view_paste                   (glView            *view);


void       gl_view_zoom_in                 (glView            *view);

void       gl_view_zoom_out                (glView            *view);

void       gl_view_zoom_to_fit             (glView            *view);

void       gl_view_set_zoom                (glView            *view,
					    gdouble            zoom);

gdouble    gl_view_get_zoom                (glView            *view);

gboolean   gl_view_is_zoom_max             (glView            *view);

gboolean   gl_view_is_zoom_min             (glView            *view);


void       gl_view_popup_menu              (glView            *view,
					    GdkEvent          *event);

void       gl_view_edit_merge_props        (glView            *view);


void       gl_view_set_default_font_family      (glView            *view,
						 const gchar       *font_family);

void       gl_view_set_default_font_size        (glView            *view,
						 gdouble            font_size);

void       gl_view_set_default_font_weight      (glView            *view,
						 GnomeFontWeight    font_weight);

void       gl_view_set_default_font_italic_flag (glView            *view,
					         gboolean           font_italic_flag);

void       gl_view_set_default_text_color       (glView            *view,
						 guint              text_color);

void       gl_view_set_default_text_alignment   (glView            *view,
						 GtkJustification   text_alignment);

void       gl_view_set_default_line_width       (glView            *view,
						 gdouble            line_width);

void       gl_view_set_default_line_color       (glView            *view,
						 guint              line_color);

void       gl_view_set_default_fill_color       (glView            *view,
						 guint              fill_color);
void       gl_view_set_default_text_line_spacing (glView            *view,
						 gdouble            text_line_spacing);



gchar           *gl_view_get_default_font_family      (glView            *view);

gdouble          gl_view_get_default_font_size        (glView            *view);

GnomeFontWeight  gl_view_get_default_font_weight      (glView            *view);

gboolean         gl_view_get_default_font_italic_flag (glView            *view);

guint            gl_view_get_default_text_color       (glView            *view);

GtkJustification gl_view_get_default_text_alignment   (glView            *view);

gdouble          gl_view_get_default_text_line_spacing (glView            *view);

gdouble          gl_view_get_default_line_width       (glView            *view);

guint            gl_view_get_default_line_color       (glView            *view);

guint            gl_view_get_default_fill_color       (glView            *view);



G_END_DECLS

#endif
