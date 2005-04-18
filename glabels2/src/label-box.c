/*
 *  (GLABELS) Label and Business Card Creation program for GNOME
 *
 *  label_box.c:  GLabels label box object
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

#include "label-box.h"

#include <glib/gmem.h>
#include <glib/gstrfuncs.h>
#include <glib/gmessages.h>

#include "debug.h"

/*========================================================*/
/* Private types.                                         */
/*========================================================*/

struct _glLabelBoxPrivate {
	gdouble          line_width;
	glColorNode      *line_color_node;
	glColorNode      *fill_color_node;
};

/*========================================================*/
/* Private globals.                                       */
/*========================================================*/

static GObjectClass *parent_class = NULL;

static guint instance = 0;

/*========================================================*/
/* Private function prototypes.                           */
/*========================================================*/

static void    gl_label_box_class_init    (glLabelBoxClass *klass);
static void    gl_label_box_instance_init (glLabelBox      *lbox);
static void    gl_label_box_finalize      (GObject         *object);

static void    copy                       (glLabelObject   *dst_object,
					   glLabelObject   *src_object);

static void    set_fill_color             (glLabelObject   *object,
					   glColorNode     *fill_color_node);

static void    set_line_color             (glLabelObject   *object,
					   glColorNode     *line_color_node);

static void    set_line_width             (glLabelObject   *object,
					   gdouble          line_width);

static glColorNode*   get_fill_color      (glLabelObject   *object);

static glColorNode*   get_line_color      (glLabelObject   *object);

static gdouble get_line_width             (glLabelObject   *object);



/*****************************************************************************/
/* Boilerplate object stuff.                                                 */
/*****************************************************************************/
GType
gl_label_box_get_type (void)
{
	static GType type = 0;

	if (!type) {
		static const GTypeInfo info = {
			sizeof (glLabelBoxClass),
			NULL,
			NULL,
			(GClassInitFunc) gl_label_box_class_init,
			NULL,
			NULL,
			sizeof (glLabelBox),
			0,
			(GInstanceInitFunc) gl_label_box_instance_init,
			NULL
		};

		type = g_type_register_static (GL_TYPE_LABEL_OBJECT,
					       "glLabelBox", &info, 0);
	}

	return type;
}

static void
gl_label_box_class_init (glLabelBoxClass *klass)
{
	GObjectClass       *object_class       = (GObjectClass *) klass;
	glLabelObjectClass *label_object_class = (glLabelObjectClass *) klass;

	parent_class = g_type_class_peek_parent (klass);

	label_object_class->copy           = copy;
	label_object_class->set_fill_color = set_fill_color;
	label_object_class->set_line_color = set_line_color;
	label_object_class->set_line_width = set_line_width;
	label_object_class->get_fill_color = get_fill_color;
	label_object_class->get_line_color = get_line_color;
	label_object_class->get_line_width = get_line_width;

	object_class->finalize = gl_label_box_finalize;
}

static void
gl_label_box_instance_init (glLabelBox *lbox)
{
	lbox->private = g_new0 (glLabelBoxPrivate, 1);
	lbox->private->line_color_node = gl_color_node_new_default ();
	lbox->private->fill_color_node = gl_color_node_new_default ();
}

static void
gl_label_box_finalize (GObject *object)
{
	glLabelBox *lbox;

	g_return_if_fail (object && GL_IS_LABEL_BOX (object));

	lbox = GL_LABEL_BOX (object);

	gl_color_node_free (&(lbox->private->fill_color_node));
	gl_color_node_free (&(lbox->private->line_color_node));
	g_free (lbox->private);

	G_OBJECT_CLASS (parent_class)->finalize (object);
}

/*****************************************************************************/
/* NEW label "box" object.                                                   */
/*****************************************************************************/
GObject *
gl_label_box_new (glLabel *label)
{
	glLabelBox *lbox;

	lbox = g_object_new (gl_label_box_get_type(), NULL);

	gl_label_object_set_parent (GL_LABEL_OBJECT(lbox), label);

	return G_OBJECT (lbox);
}

/*****************************************************************************/
/* Copy object contents.                                                     */
/*****************************************************************************/
static void
copy (glLabelObject *dst_object,
      glLabelObject *src_object)
{
	glLabelBox  *lbox     = (glLabelBox *)src_object;
	glLabelBox  *new_lbox = (glLabelBox *)dst_object;
	gdouble      line_width;
	glColorNode *line_color_node;
	glColorNode *fill_color_node;

	gl_debug (DEBUG_LABEL, "START");

	g_return_if_fail (lbox && GL_IS_LABEL_BOX (lbox));
	g_return_if_fail (new_lbox && GL_IS_LABEL_BOX (new_lbox));

	line_width = get_line_width (src_object);
	line_color_node = get_line_color (src_object);
	fill_color_node = get_fill_color (src_object);

	set_line_width (dst_object, line_width);
	set_line_color (dst_object, line_color_node);
	set_fill_color (dst_object, fill_color_node);
	
	gl_color_node_free (&line_color_node);
	gl_color_node_free (&fill_color_node);

	gl_debug (DEBUG_LABEL, "END");
}


/*---------------------------------------------------------------------------*/
/* PRIVATE.  Set fill color method.                                          */
/*---------------------------------------------------------------------------*/
static void
set_fill_color (glLabelObject *object,
		glColorNode   *fill_color_node)
{
	glLabelBox *lbox = (glLabelBox *)object;
	
	gl_debug (DEBUG_LABEL, "START");

	g_return_if_fail (lbox && GL_IS_LABEL_BOX (lbox));

	if (!gl_color_node_equal (lbox->private->fill_color_node, fill_color_node)) {

		gl_color_node_free (&(lbox->private->fill_color_node));
		lbox->private->fill_color_node = gl_color_node_dup (fill_color_node);

		gl_label_object_emit_changed (GL_LABEL_OBJECT(lbox));
	}
	gl_debug (DEBUG_LABEL, "END");
}

/*---------------------------------------------------------------------------*/
/* PRIVATE.  Set line color method.                                          */
/*---------------------------------------------------------------------------*/
static void
set_line_color (glLabelObject *object,
		glColorNode   *line_color_node)
{
	glLabelBox *lbox = (glLabelBox *)object;

	g_return_if_fail (lbox && GL_IS_LABEL_BOX (lbox));

	if ( !gl_color_node_equal (lbox->private->line_color_node, line_color_node )) {
		gl_color_node_free (&(lbox->private->line_color_node));
		lbox->private->line_color_node = gl_color_node_dup (line_color_node);
		gl_label_object_emit_changed (GL_LABEL_OBJECT(lbox));
	}
}

/*---------------------------------------------------------------------------*/
/* PRIVATE.  Set line width method.                                          */
/*---------------------------------------------------------------------------*/
static void
set_line_width (glLabelObject *object,
		gdouble        line_width)
{
	glLabelBox *lbox = (glLabelBox *)object;

	g_return_if_fail (lbox && GL_IS_LABEL_BOX (lbox));

	if ( lbox->private->line_width != line_width ) {
		lbox->private->line_width = line_width;
		gl_label_object_emit_changed (GL_LABEL_OBJECT(lbox));
	}
}


/*---------------------------------------------------------------------------*/
/* PRIVATE.  Get fill color method.                                          */
/*---------------------------------------------------------------------------*/
static gdouble
get_line_width (glLabelObject *object)
{
	glLabelBox *lbox = (glLabelBox *)object;

	g_return_val_if_fail (lbox && GL_IS_LABEL_BOX (lbox), 0.0);

	return lbox->private->line_width;
}

/*---------------------------------------------------------------------------*/
/* PRIVATE.  Get line color method.                                          */
/*---------------------------------------------------------------------------*/
static glColorNode*
get_line_color (glLabelObject *object)
{
	glLabelBox *lbox = (glLabelBox *)object;

	g_return_val_if_fail (lbox && GL_IS_LABEL_BOX (lbox), 0);

	return gl_color_node_dup (lbox->private->line_color_node);
}

/*---------------------------------------------------------------------------*/
/* PRIVATE.  Get line width method.                                          */
/*---------------------------------------------------------------------------*/
static glColorNode*
get_fill_color (glLabelObject *object)
{
	glLabelBox *lbox = (glLabelBox *)object;

	g_return_val_if_fail (lbox && GL_IS_LABEL_BOX (lbox), 0);

	return gl_color_node_dup (lbox->private->fill_color_node);
}
