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


#include "sim-plugin-state.h"
#include <config.h>

enum
{
  DESTROY,
  LAST_SIGNAL
};

struct _SimPluginStatePrivate {
  SimPlugin   *plugin;

  gint         plugin_id;
  gint         state;
  gboolean     enabled;
};

static gpointer parent_class = NULL;
static gint sim_server_signals[LAST_SIGNAL] = { 0 };

/* GType Functions */

static void 
sim_plugin_state_impl_dispose (GObject  *gobject)
{
  G_OBJECT_CLASS (parent_class)->dispose (gobject);
}

static void 
sim_plugin_state_impl_finalize (GObject  *gobject)
{
  SimPluginState *plugin = SIM_PLUGIN_STATE (gobject);

  G_OBJECT_CLASS (parent_class)->finalize (gobject);
}

static void
sim_plugin_state_class_init (SimPluginStateClass * class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  parent_class = g_type_class_ref (G_TYPE_OBJECT);

  object_class->dispose = sim_plugin_state_impl_dispose;
  object_class->finalize = sim_plugin_state_impl_finalize;
}

static void
sim_plugin_state_instance_init (SimPluginState *plugin)
{
  plugin->_priv = g_new0 (SimPluginStatePrivate, 1);

  plugin->_priv->plugin_id = 0;
  plugin->_priv->plugin = NULL;
  plugin->_priv->state = 0;
  plugin->_priv->enabled = FALSE;
}

/* Public Methods */

GType
sim_plugin_state_get_type (void)
{
  static GType object_type = 0;
 
  if (!object_type)
  {
    static const GTypeInfo type_info = {
              sizeof (SimPluginStateClass),
              NULL,
              NULL,
              (GClassInitFunc) sim_plugin_state_class_init,
              NULL,
              NULL,                       /* class data */
              sizeof (SimPluginState),
              0,                          /* number of pre-allocs */
              (GInstanceInitFunc) sim_plugin_state_instance_init,
              NULL                        /* value table */
    };
    
    g_type_init ();
                                                                                                                             
    object_type = g_type_register_static (G_TYPE_OBJECT, "SimPluginState", &type_info, 0);
  }

  return object_type;
}

/*
 *
 *
 *
 *
 */
SimPluginState*
sim_plugin_state_new (void)
{
  SimPluginState *plugin_state = NULL;

  plugin_state = SIM_PLUGIN_STATE (g_object_new (SIM_TYPE_PLUGIN_STATE, NULL));

  return plugin_state;
}

/*
 *
 *
 *
 *
 */
SimPluginState*
sim_plugin_state_new_from_data (SimPlugin    *plugin,
				gint          plugin_id,
				gint          state,
				gboolean      enabled)
{
  SimPluginState *plugin_state = NULL;

  g_return_val_if_fail (plugin, NULL);
  g_return_val_if_fail (SIM_IS_PLUGIN (plugin), NULL);
  g_return_if_fail (state >= 0);

  plugin_state = SIM_PLUGIN_STATE (g_object_new (SIM_TYPE_PLUGIN_STATE, NULL));
  plugin_state->_priv->plugin = plugin;
  plugin_state->_priv->plugin_id = plugin_id;
  plugin_state->_priv->state = state;
  plugin_state->_priv->enabled = enabled;

  return plugin_state;
}

/*
 *
 *
 *
 *
 */
SimPlugin*
sim_plugin_state_get_plugin (SimPluginState   *plugin_state)
{
  g_return_val_if_fail (plugin_state, NULL);
  g_return_val_if_fail (SIM_IS_PLUGIN_STATE (plugin_state), NULL);

  return plugin_state->_priv->plugin;
}

/*
 *
 *
 *
 *
 */
void
sim_plugin_state_set_plugin (SimPluginState   *plugin_state,
			     SimPlugin        *plugin)
{
  g_return_if_fail (plugin_state);
  g_return_if_fail (SIM_IS_PLUGIN_STATE (plugin_state));
  g_return_if_fail (plugin);
  g_return_if_fail (SIM_IS_PLUGIN (plugin));

  plugin_state->_priv->plugin = plugin;
}

/*
 *
 *
 *
 *
 */
gint
sim_plugin_state_get_plugin_id (SimPluginState   *plugin_state)
{
  g_return_val_if_fail (plugin_state, 0);
  g_return_val_if_fail (SIM_IS_PLUGIN_STATE (plugin_state), 0);

  return plugin_state->_priv->plugin_id;
}

/*
 *
 *
 *
 *
 */
void
sim_plugin_state_set_plugin_id (SimPluginState   *plugin_state,
				gint              plugin_id)
{
  g_return_if_fail (plugin_state);
  g_return_if_fail (SIM_IS_PLUGIN_STATE (plugin_state));

  plugin_state->_priv->plugin_id = plugin_id;
}

/*
 *
 *
 *
 *
 */
gint
sim_plugin_state_get_state (SimPluginState   *plugin_state)
{
  g_return_val_if_fail (plugin_state, 0);
  g_return_val_if_fail (SIM_IS_PLUGIN_STATE (plugin_state), 0);

  return plugin_state->_priv->state;
}

/*
 *
 *
 *
 *
 */
void
sim_plugin_state_set_state (SimPluginState   *plugin_state,
			    gint              state)
{
  g_return_if_fail (plugin_state);
  g_return_if_fail (SIM_IS_PLUGIN_STATE (plugin_state));
  g_return_if_fail (state >= 0);

  plugin_state->_priv->state = state;
}

/*
 *
 *
 *
 *
 */
gboolean
sim_plugin_state_get_enabled (SimPluginState   *plugin_state)
{
  g_return_val_if_fail (plugin_state, FALSE);
  g_return_val_if_fail (SIM_IS_PLUGIN_STATE (plugin_state), FALSE);

  return plugin_state->_priv->enabled;
}

/*
 *
 *
 *
 *
 */
void
sim_plugin_state_set_enabled (SimPluginState   *plugin_state,
			      gboolean          enabled)
{
  g_return_if_fail (plugin_state);
  g_return_if_fail (SIM_IS_PLUGIN_STATE (plugin_state));

  plugin_state->_priv->enabled = enabled;
}

// vim: set tabstop=2:
