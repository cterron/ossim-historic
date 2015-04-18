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

#include "sim-host-level.h"
 
#include <math.h>

enum 
{
  DESTROY,
  LAST_SIGNAL
};

struct _SimHostLevelPrivate {
  GInetAddr  *ia;
  gdouble     a;
  gdouble     c;
};

static gpointer parent_class = NULL;
static gint sim_host_level_signals[LAST_SIGNAL] = { 0 };

/* GType Functions */

static void 
sim_host_level_impl_dispose (GObject  *gobject)
{
  G_OBJECT_CLASS (parent_class)->dispose (gobject);
}

static void 
sim_host_level_impl_finalize (GObject  *gobject)
{
  SimHostLevel *host_level = SIM_HOST_LEVEL (gobject);

  if (host_level->_priv->ia)
    gnet_inetaddr_unref (host_level->_priv->ia);

  g_free (host_level->_priv);

  G_OBJECT_CLASS (parent_class)->finalize (gobject);
}

static void
sim_host_level_class_init (SimHostLevelClass * class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  parent_class = g_type_class_peek_parent (class);

  object_class->dispose = sim_host_level_impl_dispose;
  object_class->finalize = sim_host_level_impl_finalize;
}

static void
sim_host_level_instance_init (SimHostLevel *host_level)
{
  host_level->_priv = g_new0 (SimHostLevelPrivate, 1);

  host_level->_priv->ia = NULL;
  host_level->_priv->c = 1;
  host_level->_priv->a = 1;
}

/* Public Methods */
GType
sim_host_level_get_type (void)
{
  static GType object_type = 0;
 
  if (!object_type)
  {
    static const GTypeInfo type_info = {
              sizeof (SimHostLevelClass),
              NULL,
              NULL,
              (GClassInitFunc) sim_host_level_class_init,
              NULL,
              NULL,                       /* class data */
              sizeof (SimHostLevel),
              0,                          /* number of pre-allocs */
              (GInstanceInitFunc) sim_host_level_instance_init,
              NULL                        /* value table */
    };
    
    g_type_init ();
                                                                                                                             
    object_type = g_type_register_static (G_TYPE_OBJECT, "SimHostLevel", &type_info, 0);
  }
                                                                                                                             
  return object_type;
}

/*
 *
 *
 *
 */
SimHostLevel*
sim_host_level_new (const GInetAddr     *ia,
		    gint           c,
		    gint           a)
{
  SimHostLevel *host_level = NULL;

  g_return_val_if_fail (ia, NULL);

  if (c < 1) c = 1;
  if (a < 1) a = 1;

  host_level = SIM_HOST_LEVEL (g_object_new (SIM_TYPE_HOST_LEVEL, NULL));
  host_level->_priv->ia = gnet_inetaddr_clone (ia);
  host_level->_priv->c = c;
  host_level->_priv->a = a;  

  return host_level;
}

/*
 *
 *
 *
 */
SimHostLevel*
sim_host_level_new_from_dm (GdaDataModel  *dm,
			    gint           row)
{
  SimHostLevel  *host_level;
  GdaValue      *value;

  g_return_val_if_fail (dm, NULL);
  g_return_val_if_fail (GDA_IS_DATA_MODEL (dm), NULL);

  host_level = SIM_HOST_LEVEL (g_object_new (SIM_TYPE_HOST_LEVEL, NULL));

  value = (GdaValue *) gda_data_model_get_value_at (dm, 0, row);
  host_level->_priv->ia = gnet_inetaddr_new_nonblock (gda_value_get_string (value), 0);

  value = (GdaValue *) gda_data_model_get_value_at (dm, 1, row);
  host_level->_priv->c = gda_value_get_integer (value);

  value = (GdaValue *) gda_data_model_get_value_at (dm, 2, row);
  host_level->_priv->a = gda_value_get_integer (value);

  return host_level;
}

/*
 *
 *
 *
 */
GInetAddr*
sim_host_level_get_ia (SimHostLevel  *host_level)
{
  g_return_val_if_fail (host_level, NULL);
  g_return_val_if_fail (SIM_IS_HOST_LEVEL (host_level), NULL);

  return host_level->_priv->ia;
}

/*
 *
 *
 *
 */
void
sim_host_level_set_ia (SimHostLevel  *host_level,
		       const GInetAddr     *ia)
{
  g_return_if_fail (host_level);
  g_return_if_fail (SIM_IS_HOST_LEVEL (host_level));
  g_return_if_fail (ia);

  if (host_level->_priv->ia)
    gnet_inetaddr_unref (host_level->_priv->ia);

  host_level->_priv->ia = gnet_inetaddr_clone (ia);
}

/*
 *
 *
 *
 */
gdouble
sim_host_level_get_c (SimHostLevel  *host_level)
{
  g_return_val_if_fail (host_level, 0);
  g_return_val_if_fail (SIM_IS_HOST_LEVEL (host_level), 0);

  return host_level->_priv->c;
}

/*
 *
 *
 *
 */
void
sim_host_level_set_c (SimHostLevel  *host_level,
		      gdouble        c)
{
  g_return_if_fail (host_level);
  g_return_if_fail (SIM_IS_HOST_LEVEL (host_level));

  if (c < 1) c = 1;

  host_level->_priv->c = c;
}

/*
 *
 *
 *
 */
void
sim_host_level_plus_c (SimHostLevel  *host_level,
		       gdouble        c)
{
  g_return_if_fail (host_level);
  g_return_if_fail (SIM_IS_HOST_LEVEL (host_level));

  host_level->_priv->c += c;
}

/*
 *
 *
 *
 */
gdouble
sim_host_level_get_a (SimHostLevel  *host_level)
{
  g_return_val_if_fail (host_level, 0);
  g_return_val_if_fail (SIM_IS_HOST_LEVEL (host_level), 0);

  return host_level->_priv->a;
}

/*
 *
 *
 *
 */
void
sim_host_level_set_a (SimHostLevel  *host_level,
		      gdouble        a)
{
  g_return_if_fail (host_level);
  g_return_if_fail (SIM_IS_HOST_LEVEL (host_level));

  if (a < 1) a = 1;

  host_level->_priv->a = a;
}

/*
 *
 *
 *
 */
void
sim_host_level_plus_a (SimHostLevel  *host_level,
		       gdouble        a)
{
  g_return_if_fail (host_level);
  g_return_if_fail (SIM_IS_HOST_LEVEL (host_level));

  host_level->_priv->a += a;
}

/*
 *
 *
 *
 */
void
sim_host_level_set_recovery (SimHostLevel  *host_level,
			     gint           recovery)
{
  g_return_if_fail (host_level);
  g_return_if_fail (SIM_IS_HOST_LEVEL (host_level));
  g_return_if_fail (recovery >= 0);

  if (host_level->_priv->c > recovery)
    host_level->_priv->c -= recovery;
  else
    host_level->_priv->c = 0;

  if (host_level->_priv->a > recovery)
    host_level->_priv->a -= recovery;
  else
    host_level->_priv->a = 0;
}

/*
 *
 *
 *
 */
gchar*
sim_host_level_get_insert_clause (SimHostLevel  *host_level)
{
  gchar *query;
  gchar *name;
  gint   c = 0;
  gint   a = 0;

  g_return_val_if_fail (host_level, NULL);
  g_return_val_if_fail (SIM_IS_HOST_LEVEL (host_level), NULL);
  g_return_val_if_fail (host_level->_priv->ia, NULL);

  c = rint (host_level->_priv->c);
  a = rint (host_level->_priv->a);

  name = gnet_inetaddr_get_canonical_name (host_level->_priv->ia);
  query = g_strdup_printf ("INSERT INTO host_qualification VALUES ('%s', %d, %d)",
			   name, c, a);

  g_free (name);

  return query;
}

/*
 *
 *
 *
 */
gchar*
sim_host_level_get_update_clause (SimHostLevel  *host_level)
{
  gchar *query;
  gchar *name;
  gint   c = 0;
  gint   a = 0;

  g_return_val_if_fail (host_level, NULL);
  g_return_val_if_fail (SIM_IS_HOST_LEVEL (host_level), NULL);
  g_return_val_if_fail (host_level->_priv->ia, NULL);

  c = rint (host_level->_priv->c);
  a = rint (host_level->_priv->a);

  name = gnet_inetaddr_get_canonical_name (host_level->_priv->ia);
  query = g_strdup_printf ("UPDATE host_qualification SET compromise = %d, attack = %d WHERE host_ip = '%s'",
			   c, a, name);

  g_free (name);

  return query;
}

/*
 *
 *
 *
 */
gchar*
sim_host_level_get_delete_clause (SimHostLevel  *host_level)
{
  gchar *query;
  gchar *name;

  g_return_val_if_fail (host_level, NULL);
  g_return_val_if_fail (SIM_IS_HOST_LEVEL (host_level), NULL);
  g_return_val_if_fail (host_level->_priv->ia, NULL);

  name = gnet_inetaddr_get_canonical_name (host_level->_priv->ia);
  query = g_strdup_printf ("DELETE FROM host_qualification WHERE host_ip = '%s'", name);
  g_free (name);

  return query;
}
