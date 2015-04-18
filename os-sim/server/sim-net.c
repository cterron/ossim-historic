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
 
#include "sim-net.h"

enum
{
  DESTROY,
  LAST_SIGNAL
};

struct _SimNetPrivate {
  gchar           *name;
  gchar           *ips;
  gint             asset;

  GList           *ias;
};

static gpointer parent_class = NULL;
static gint sim_net_signals[LAST_SIGNAL] = { 0 };

/* GType Functions */

static void 
sim_net_impl_dispose (GObject  *gobject)
{
  G_OBJECT_CLASS (parent_class)->dispose (gobject);
}

static void 
sim_net_impl_finalize (GObject  *gobject)
{
  SimNet *net = SIM_NET (gobject);

  g_free (net->_priv->name);
  g_free (net->_priv->ips);

  G_OBJECT_CLASS (parent_class)->finalize (gobject);
}

static void
sim_net_class_init (SimNetClass * class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  parent_class = g_type_class_peek_parent (class);

  object_class->dispose = sim_net_impl_dispose;
  object_class->finalize = sim_net_impl_finalize;
}

static void
sim_net_instance_init (SimNet *net)
{
  net->_priv = g_new0 (SimNetPrivate, 1);

  net->_priv->name = NULL;
  net->_priv->ips = NULL;
  net->_priv->asset = 0;

  net->_priv->ias = NULL;
}

/* Public Methods */

GType
sim_net_get_type (void)
{
  static GType object_type = 0;
 
  if (!object_type)
  {
    static const GTypeInfo type_info = {
              sizeof (SimNetClass),
              NULL,
              NULL,
              (GClassInitFunc) sim_net_class_init,
              NULL,
              NULL,                       /* class data */
              sizeof (SimNet),
              0,                          /* number of pre-allocs */
              (GInstanceInitFunc) sim_net_instance_init,
              NULL                        /* value table */
    };
    
    g_type_init ();
                                                                                                                             
    object_type = g_type_register_static (G_TYPE_OBJECT, "SimNet", &type_info, 0);
  }
                                                                                                                             
  return object_type;
}

/*
 *
 *
 *
 *
 */
SimNet*
sim_net_new (gchar   *name,
	     gchar   *ips,
	     gint     asset)
{
  SimNet *net = NULL;

  g_return_val_if_fail (name, NULL);
  g_return_val_if_fail (ips, NULL);

  net = SIM_NET (g_object_new (SIM_TYPE_NET, NULL));
  net->_priv->name = name;
  net->_priv->ips = ips;
  net->_priv->asset = asset;

  return net;
}

/*
 *
 *
 *
 */
SimNet*
sim_net_new_from_dm (GdaDataModel  *dm,
		     gint           row)
{
  SimNet     *net;
  GdaValue   *value;
  gchar      *ips;
  gchar      *name;
  gint        asset;

  g_return_val_if_fail (dm, NULL);
  g_return_val_if_fail (GDA_IS_DATA_MODEL (dm), NULL);

  value = (GdaValue *) gda_data_model_get_value_at (dm, 0, row);
  name = gda_value_stringify (value);

  value = (GdaValue *) gda_data_model_get_value_at (dm, 1, row);
  ips = gda_value_stringify (value);
  
  value = (GdaValue *) gda_data_model_get_value_at (dm, 2, row);
  asset = gda_value_get_integer (value);

  net = SIM_NET (g_object_new (SIM_TYPE_NET, NULL));
  net->_priv->name = name;
  net->_priv->ips = ips;
  net->_priv->asset = asset;

  return net;
}

/*
 *
 *
 *
 */
gchar*
sim_net_get_name (SimNet  *net)
{
  g_return_val_if_fail (net, NULL);
  g_return_val_if_fail (SIM_IS_NET (net), NULL);

  return net->_priv->name;
}

/*
 *
 *
 *
 */
void
sim_net_set_name (SimNet      *net,
		  gchar       *name)
{
  g_return_if_fail (net);
  g_return_if_fail (SIM_IS_NET (net));
  g_return_if_fail (name);

  net->_priv->name = name;
}

/*
 *
 *
 *
 */
gint
sim_net_get_asset (SimNet  *net)
{
  g_return_val_if_fail (net, 0);
  g_return_val_if_fail (SIM_IS_NET (net), 0);

  return net->_priv->asset;
}

/*
 *
 *
 *
 */
void
sim_net_set_asset (SimNet  *net,
		   gint     asset)
{
  g_return_if_fail (net);
  g_return_if_fail (SIM_IS_NET (net));

  net->_priv->asset = asset;
}

/*
 *
 *
 *
 */
void
sim_net_append_ia (SimNet     *net,
		  GInetAddr  *ia)
{
  g_return_if_fail (net);
  g_return_if_fail (SIM_IS_NET (net));
  g_return_if_fail (ia);
  
  net->_priv->ias = g_list_append (net->_priv->ias, ia);
}

/*
 *
 *
 *
 */
void
sim_net_remove_ia (SimNet   *net,
		   GInetAddr  *ia)
{
  g_return_if_fail (net);
  g_return_if_fail (SIM_IS_NET (net));
  g_return_if_fail (ia);

  net->_priv->ias = g_list_remove (net->_priv->ias, ia);
}

/*
 *
 *
 *
 */
GList*
sim_net_get_ias (SimNet        *net)
{
  g_return_val_if_fail (net, NULL);
  g_return_val_if_fail (SIM_IS_NET (net), NULL);

  return net->_priv->ias;
}

/*
 *
 *
 *
 */
gboolean
sim_net_has_ia (SimNet     *net,
		GInetAddr  *ia)
{ 
  GList  *list;

  g_return_if_fail (net);
  g_return_if_fail (SIM_IS_NET (net));
  g_return_if_fail (ia);

  list = net->_priv->ias;
  while (list)
    {
      GInetAddr *cmp = (GInetAddr *) list->data;

      if (gnet_inetaddr_noport_equal (cmp, ia))
	{
	  return TRUE;
	}

      list = list->next;
    }

  return FALSE;
}
