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

#include <config.h>

#include "sim-net-level.h"

enum 
{
  DESTROY,
  LAST_SIGNAL
};

struct _SimNetLevelPrivate {
  gchar     *name;
  gint       a;
  gint       c;
};

static gpointer parent_class = NULL;
static gint sim_net_level_signals[LAST_SIGNAL] = { 0 };

/* GType Functions */

static void 
sim_net_level_impl_dispose (GObject  *gobject)
{
  G_OBJECT_CLASS (parent_class)->dispose (gobject);
}

static void 
sim_net_level_impl_finalize (GObject  *gobject)
{
  SimNetLevel  *net_level = SIM_NET_LEVEL (gobject);

  if (net_level->_priv->name)
    g_free (net_level->_priv->name);

  g_free (net_level->_priv);

  G_OBJECT_CLASS (parent_class)->finalize (gobject);
}

static void
sim_net_level_class_init (SimNetLevelClass * class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  parent_class = g_type_class_peek_parent (class);

  object_class->dispose = sim_net_level_impl_dispose;
  object_class->finalize = sim_net_level_impl_finalize;
}

static void
sim_net_level_instance_init (SimNetLevel *net_level)
{
  net_level->_priv = g_new0 (SimNetLevelPrivate, 1);

  net_level->_priv->name = NULL;
  net_level->_priv->c = 1;
  net_level->_priv->a = 1;
}

/* Public Methods */

GType
sim_net_level_get_type (void)
{
  static GType object_type = 0;
 
  if (!object_type)
  {
    static const GTypeInfo type_info = {
              sizeof (SimNetLevelClass),
              NULL,
              NULL,
              (GClassInitFunc) sim_net_level_class_init,
              NULL,
              NULL,                       /* class data */
              sizeof (SimNetLevel),
              0,                          /* number of pre-allocs */
              (GInstanceInitFunc) sim_net_level_instance_init,
              NULL                        /* value table */
    };
    
    g_type_init ();
                                                                                                                             
    object_type = g_type_register_static (G_TYPE_OBJECT, "SimNetLevel", &type_info, 0);
  }
                                                                                                                             
  return object_type;
}

/*
 *
 *
 *
 *
 */
SimNetLevel*
sim_net_level_new (const gchar  *name,
		   gint          c,
		   gint          a)
{
  SimNetLevel *net_level = NULL;

  g_return_val_if_fail (name, NULL);
  g_return_val_if_fail (c > 0, NULL);
  g_return_val_if_fail (a > 0, NULL);

  net_level = SIM_NET_LEVEL (g_object_new (SIM_TYPE_NET_LEVEL, NULL));
  net_level->_priv->name = g_strdup (name);
  net_level->_priv->c = c;
  net_level->_priv->a = a;

  return net_level;
}

/*
 *
 *
 *
 */
SimNetLevel*
sim_net_level_new_from_dm (GdaDataModel  *dm,
			   gint           row)
{
  SimNetLevel  *net_level;
  GdaValue      *value;

  g_return_val_if_fail (dm, NULL);
  g_return_val_if_fail (GDA_IS_DATA_MODEL (dm), NULL);

  net_level = SIM_NET_LEVEL (g_object_new (SIM_TYPE_NET_LEVEL, NULL));

  value = (GdaValue *) gda_data_model_get_value_at (dm, 0, row);
  net_level->_priv->name = gda_value_stringify (value);

  value = (GdaValue *) gda_data_model_get_value_at (dm, 1, row);
  net_level->_priv->c = gda_value_get_integer (value);

  value = (GdaValue *) gda_data_model_get_value_at (dm, 2, row);
  net_level->_priv->a = gda_value_get_integer (value);

  return net_level;
}

/*
 *
 *
 *
 */
gchar*
sim_net_level_get_name (SimNetLevel     *net_level)
{
  g_return_val_if_fail (net_level, NULL);
  g_return_val_if_fail (SIM_IS_NET_LEVEL (net_level), NULL);

  return net_level->_priv->name;
}

/*
 *
 *
 *
 */
void
sim_net_level_set_name (SimNetLevel     *net_level,
			const gchar     *name)
{
  g_return_if_fail (net_level);
  g_return_if_fail (SIM_IS_NET_LEVEL (net_level));
  g_return_if_fail (name);

  if (net_level->_priv->name)
    g_free (net_level->_priv->name);

  net_level->_priv->name = g_strdup (name);
}

/*
 *
 *
 *
 */
gint
sim_net_level_get_c (SimNetLevel  *net_level)
{
  g_return_val_if_fail (net_level, 0);
  g_return_val_if_fail (SIM_IS_NET_LEVEL (net_level), 0);

  return net_level->_priv->c;
}

/*
 *
 *
 *
 */
void
sim_net_level_set_c (SimNetLevel  *net_level,
		     gint          c)
{
  g_return_if_fail (net_level);
  g_return_if_fail (SIM_IS_NET_LEVEL (net_level));
  g_return_if_fail (c > 0);

  net_level->_priv->c = c;
}

/*
 *
 *
 *
 */
void
sim_net_level_plus_c (SimNetLevel  *net_level,
		      gint          c)
{
  g_return_if_fail (net_level);
  g_return_if_fail (SIM_IS_NET_LEVEL (net_level));
  g_return_if_fail (c > 0);

  net_level->_priv->c += c;
}

/*
 *
 *
 *
 */
gint
sim_net_level_get_a (SimNetLevel  *net_level)
{
  g_return_val_if_fail (net_level, 0);
  g_return_val_if_fail (SIM_IS_NET_LEVEL (net_level), 0);

  return net_level->_priv->a;
}

/*
 *
 *
 *
 */
void
sim_net_level_set_a (SimNetLevel  *net_level,
		     gint          a)
{
  g_return_if_fail (net_level);
  g_return_if_fail (SIM_IS_NET_LEVEL (net_level));
  g_return_if_fail (a > 0);

  net_level->_priv->a = a;
}

/*
 *
 *
 *
 */
void
sim_net_level_plus_a (SimNetLevel  *net_level,
		      gint          a)
{
  g_return_if_fail (net_level);
  g_return_if_fail (SIM_IS_NET_LEVEL (net_level));
  g_return_if_fail (a > 0);
  
  net_level->_priv->a += a;
}

/*
 *
 *
 *
 */
void
sim_net_level_set_recovery (SimNetLevel  *net_level,
			    gint          recovery)
{
  g_return_if_fail (net_level);
  g_return_if_fail (SIM_IS_NET_LEVEL (net_level));
  g_return_if_fail (recovery > 0);

  if (net_level->_priv->c > recovery)
    net_level->_priv->c -= recovery;
  else
    net_level->_priv->c = 0;

  if (net_level->_priv->a > recovery)
    net_level->_priv->a -= recovery;
  else
    net_level->_priv->a = 0;
}

/*
 *
 *
 *
 */
gchar*
sim_net_level_get_insert_clause (SimNetLevel  *net_level)
{
  gchar *query;

  g_return_val_if_fail (net_level, NULL);
  g_return_val_if_fail (SIM_IS_NET_LEVEL (net_level), NULL);
  g_return_val_if_fail (net_level->_priv->name, NULL);

  query = g_strdup_printf ("INSERT INTO net_qualification VALUES ('%s', %d, %d)",
			   net_level->_priv->name,
			   net_level->_priv->c,
			   net_level->_priv->a);

  return query;
}

/*
 *
 *
 *
 */
gchar*
sim_net_level_get_update_clause (SimNetLevel  *net_level)
{
  gchar *query;

  g_return_val_if_fail (net_level, NULL);
  g_return_val_if_fail (SIM_IS_NET_LEVEL (net_level), NULL);
  g_return_val_if_fail (net_level->_priv->name, NULL);

  query = g_strdup_printf ("UPDATE net_qualification SET compromise = %d, attack = %d WHERE net_name = '%s'",
			   net_level->_priv->c,
			   net_level->_priv->a,
			   net_level->_priv->name);

  return query;
}

/*
 *
 *
 *
 */
gchar*
sim_net_level_get_delete_clause (SimNetLevel  *net_level)
{
  gchar *query;

  g_return_val_if_fail (net_level, NULL);
  g_return_val_if_fail (SIM_IS_NET_LEVEL (net_level), NULL);
  g_return_val_if_fail (net_level->_priv->name, NULL);

  query = g_strdup_printf ("DELETE FROM net_qualification WHERE net_name = '%s'",
			   net_level->_priv->name);

  return query;
}
