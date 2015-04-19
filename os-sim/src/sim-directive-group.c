/* Copyright (c) 2003 ossim.net
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission
 *    from the author.
 *
 * 4. Products derived from this software may not be called "Os-sim" nor
 *    may "Os-sim" appear in their names without specific prior written
 *    permission from the author.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
 * THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include "sim-directive-group.h"
#include <config.h>

enum 
{
  DESTROY,
  LAST_SIGNAL
};

struct _SimDirectiveGroupPrivate {
  gchar		*name;
  gboolean	sticky;

  GList		*ids;
};

static gpointer parent_class = NULL;
static gint sim_directive_group_signals[LAST_SIGNAL] = { 0 };

/* GType Functions */

static void 
sim_directive_group_impl_dispose (GObject	*gobject)
{
  G_OBJECT_CLASS (parent_class)->dispose (gobject);
}

static void 
sim_directive_group_impl_finalize (GObject	*gobject)
{
  SimDirectiveGroup *directive_group = SIM_DIRECTIVE_GROUP (gobject);

  if (directive_group->_priv->ids)
    g_list_free (directive_group->_priv->ids);

  g_free (directive_group->_priv);

  G_OBJECT_CLASS (parent_class)->finalize (gobject);
}

static void
sim_directive_group_class_init (SimDirectiveGroupClass	*class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  parent_class = g_type_class_peek_parent (class);

  object_class->dispose = sim_directive_group_impl_dispose;
  object_class->finalize = sim_directive_group_impl_finalize;
}

static void
sim_directive_group_instance_init (SimDirectiveGroup	*directive_group)
{
  directive_group->_priv = g_new0 (SimDirectiveGroupPrivate, 1);

  directive_group->_priv->name = NULL;
  directive_group->_priv->sticky = FALSE;
  directive_group->_priv->ids = NULL;
}

/* Public Methods */
GType
sim_directive_group_get_gtype (void)
{
  static GType object_type = 0;
 
  if (!object_type)
  {
    static const GTypeInfo type_info = {
              sizeof (SimDirectiveGroupClass),
              NULL,
              NULL,
              (GClassInitFunc) sim_directive_group_class_init,
              NULL,
              NULL,                       /* class data */
              sizeof (SimDirectiveGroup),
              0,                          /* number of pre-allocs */
              (GInstanceInitFunc) sim_directive_group_instance_init,
              NULL                        /* value table */
    };
    
    g_type_init ();
                                                                                                                             
    object_type = g_type_register_static (G_TYPE_OBJECT, "SimDirectiveGroup", &type_info, 0);
  }
                                                                                                                             
  return object_type;
}

/*
 *
 *
 *
 */
SimDirectiveGroup*
sim_directive_group_new (void)
{
  SimDirectiveGroup *directive_group;

  directive_group = SIM_DIRECTIVE_GROUP (g_object_new (SIM_TYPE_DIRECTIVE_GROUP, NULL));

  return directive_group;
}


/*
 *
 *
 *
 */
gchar*
sim_directive_group_get_name (SimDirectiveGroup	*dg)
{
  g_return_val_if_fail (dg, NULL);
  g_return_val_if_fail (SIM_IS_DIRECTIVE_GROUP (dg), NULL);

  return dg->_priv->name;
}

/*
 *
 *
 *
 */
void
sim_directive_group_set_name (SimDirectiveGroup	*dg,
			      gchar		*name)
{
  g_return_if_fail (dg);
  g_return_if_fail (SIM_IS_DIRECTIVE_GROUP (dg));
  g_return_if_fail (name);

  if (dg->_priv->name)
    g_free (dg->_priv->name);

  dg->_priv->name = name;
}

/*
 *
 *
 *
 */
gboolean
sim_directive_group_get_sticky (SimDirectiveGroup	*dg)
{
  g_return_val_if_fail (dg, FALSE);
  g_return_val_if_fail (SIM_IS_DIRECTIVE_GROUP (dg), FALSE);

  return dg->_priv->sticky;
}

/*
 *
 *
 *
 */
void
sim_directive_group_set_sticky (SimDirectiveGroup	*dg,
				gboolean		sticky)
{
  g_return_if_fail (dg);
  g_return_if_fail (SIM_IS_DIRECTIVE_GROUP (dg));

  dg->_priv->sticky = sticky;
}

/*
 *
 *
 *
 */
void
sim_directive_group_append_id (SimDirectiveGroup	*dg,
			       gint			id)
{
  g_return_if_fail (dg);
  g_return_if_fail (SIM_IS_DIRECTIVE_GROUP (dg));

  dg->_priv->ids = g_list_append (dg->_priv->ids, GINT_TO_POINTER (id));
}

/*
 *
 *
 *
 */
void
sim_directive_group_remove_id (SimDirectiveGroup	*dg,
			       gint			id)
{
  GList	*list;

  g_return_if_fail (dg);
  g_return_if_fail (SIM_IS_DIRECTIVE_GROUP (dg));

  list = dg->_priv->ids;
  while (list)
    {
      gint cmp = GPOINTER_TO_INT (list->data);

      if (cmp == id)
	{
	  dg->_priv->ids = g_list_remove (dg->_priv->ids, GINT_TO_POINTER (cmp));
	  return;
	}

      list = list->next;
    }
}

/*
 *
 *
 *
 */
GList*
sim_directive_group_get_ids (SimDirectiveGroup	*dg)
{
  g_return_val_if_fail (dg, NULL);
  g_return_val_if_fail (SIM_IS_DIRECTIVE_GROUP (dg), NULL);

  return dg->_priv->ids;
}

// vim: set tabstop=2:

