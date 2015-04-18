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

#include "sim-host.h"

enum
{
  DESTROY,
  LAST_SIGNAL
};

struct _SimHostPrivate {
  GInetAddr  *ia;
  gchar      *name;
  gint        asset;
};

static gpointer parent_class = NULL;
static gint sim_host_signals[LAST_SIGNAL] = { 0 };

/* GType Functions */

static void 
sim_host_impl_dispose (GObject  *gobject)
{
  G_OBJECT_CLASS (parent_class)->dispose (gobject);
}

static void 
sim_host_impl_finalize (GObject  *gobject)
{
  SimHost *host = SIM_HOST (gobject);

  gnet_inetaddr_unref (host->_priv->ia);
  g_free (host->_priv->name);

  G_OBJECT_CLASS (parent_class)->finalize (gobject);
}

static void
sim_host_class_init (SimHostClass * class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  parent_class = g_type_class_peek_parent (class);

  object_class->dispose = sim_host_impl_dispose;
  object_class->finalize = sim_host_impl_finalize;
}

static void
sim_host_instance_init (SimHost *host)
{
  host->_priv = g_new0 (SimHostPrivate, 1);

  host->_priv->ia = NULL;
  host->_priv->name = NULL;
  host->_priv->asset = 0;
}

/* Public Methods */

GType
sim_host_get_type (void)
{
  static GType object_type = 0;
 
  if (!object_type)
  {
    static const GTypeInfo type_info = {
              sizeof (SimHostClass),
              NULL,
              NULL,
              (GClassInitFunc) sim_host_class_init,
              NULL,
              NULL,                       /* class data */
              sizeof (SimHost),
              0,                          /* number of pre-allocs */
              (GInstanceInitFunc) sim_host_instance_init,
              NULL                        /* value table */
    };
    
    g_type_init ();
                                                                                                                             
    object_type = g_type_register_static (G_TYPE_OBJECT, "SimHost", &type_info, 0);
  }
                                                                                                                             
  return object_type;
}

/*
 *
 *
 *
 *
 */
SimHost*
sim_host_new (GInetAddr    *ia,
	      gchar        *name,
	      gint          asset)
{
  SimHost *host;

  g_return_val_if_fail (ia, NULL);
  g_return_val_if_fail (name, NULL);

  host = SIM_HOST (g_object_new (SIM_TYPE_HOST, NULL));
  host->_priv->ia = ia;
  host->_priv->name = name;
  host->_priv->asset = asset;

  return host;
}

/*
 *
 *
 *
 */
SimHost*
sim_host_new_from_dm (GdaDataModel  *dm,
		      gint           row)
{
  SimHost    *host;
  GInetAddr  *ia;
  GdaValue   *value;
  gchar      *ip;
  gchar      *name;
  gint        asset;

  g_return_val_if_fail (dm, NULL);
  g_return_val_if_fail (GDA_IS_DATA_MODEL (dm), NULL);

  value = (GdaValue *) gda_data_model_get_value_at (dm, 0, row);
  ip = gda_value_stringify (value);
  ia = gnet_inetaddr_new_nonblock (ip, 0);
  g_free (ip);
  
  value = (GdaValue *) gda_data_model_get_value_at (dm, 1, row);
  name = gda_value_stringify (value);
  
  value = (GdaValue *) gda_data_model_get_value_at (dm, 2, row);
  asset = gda_value_get_smallint (value);

  host = SIM_HOST (g_object_new (SIM_TYPE_HOST, NULL));
  host->_priv->ia = ia;
  host->_priv->name = name;
  host->_priv->asset = asset;

  return host;
}

/*
 *
 *
 *
 */
GInetAddr*
sim_host_get_ia (SimHost *host)
{
  g_return_val_if_fail (host, NULL);
  g_return_val_if_fail (SIM_IS_HOST (host), NULL);

  return host->_priv->ia;
}

/*
 *
 *
 *
 */
void
sim_host_set_ia (SimHost    *host,
		 GInetAddr  *ia)
{
  g_return_if_fail (host);
  g_return_if_fail (SIM_IS_HOST (host));
  g_return_if_fail (ia);

  host->_priv->ia = ia;
}

/*
 *
 *
 *
 */
gchar*
sim_host_get_name (SimHost  *host)
{
  g_return_val_if_fail (host, NULL);
  g_return_val_if_fail (SIM_IS_HOST (host), NULL);

  return host->_priv->name;
}

/*
 *
 *
 *
 */
void
sim_host_set_name (SimHost      *host,
		   gchar        *name)
{
  g_return_if_fail (host);
  g_return_if_fail (SIM_IS_HOST (host));
  g_return_if_fail (name);

  host->_priv->name = name;
}

/*
 *
 *
 *
 */
gint
sim_host_get_asset (SimHost  *host)
{
  g_return_val_if_fail (host, 0);
  g_return_val_if_fail (SIM_IS_HOST (host), 0);

  return host->_priv->asset;
}

/*
 *
 *
 *
 */
void
sim_host_set_asset (SimHost  *host,
		    gint      asset)
{
  g_return_if_fail (host);
  g_return_if_fail (SIM_IS_HOST (host));

  host->_priv->asset = asset;
}
