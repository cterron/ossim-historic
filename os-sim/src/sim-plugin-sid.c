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


#include "sim-plugin-sid.h"
#include <config.h>

enum
{
  DESTROY,
  LAST_SIGNAL
};

struct _SimPluginSidPrivate {
  gint     plugin_id;
  gint     sid;
  gint     reliability;
  gint     priority;
  gchar   *name;
};

static gpointer parent_class = NULL;
static gint sim_plugin_sid_signals[LAST_SIGNAL] = { 0 };

/* GType Functions */

static void 
sim_plugin_sid_impl_dispose (GObject  *gobject)
{
  G_OBJECT_CLASS (parent_class)->dispose (gobject);
}

static void 
sim_plugin_sid_impl_finalize (GObject  *gobject)
{
  SimPluginSid *plugin = SIM_PLUGIN_SID (gobject);

  if (plugin->_priv->name)
    g_free (plugin->_priv->name);

  g_free (plugin->_priv);

  G_OBJECT_CLASS (parent_class)->finalize (gobject);
}

static void
sim_plugin_sid_class_init (SimPluginSidClass * class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  parent_class = g_type_class_ref (G_TYPE_OBJECT);

  object_class->dispose = sim_plugin_sid_impl_dispose;
  object_class->finalize = sim_plugin_sid_impl_finalize;
}

static void
sim_plugin_sid_instance_init (SimPluginSid *plugin)
{
  plugin->_priv = g_new0 (SimPluginSidPrivate, 1);

  plugin->_priv->plugin_id = 0;
  plugin->_priv->sid = 0;
  plugin->_priv->reliability = 1;
  plugin->_priv->priority = 1;
  plugin->_priv->name = NULL;
}

/* Public Methods */

GType
sim_plugin_sid_get_type (void)
{
  static GType object_type = 0;
 
  if (!object_type)
  {
    static const GTypeInfo type_info = {
              sizeof (SimPluginSidClass),
              NULL,
              NULL,
              (GClassInitFunc) sim_plugin_sid_class_init,
              NULL,
              NULL,                       /* class data */
              sizeof (SimPluginSid),
              0,                          /* number of pre-allocs */
              (GInstanceInitFunc) sim_plugin_sid_instance_init,
              NULL                        /* value table */
    };
    
    g_type_init ();
                                                                                                                             
    object_type = g_type_register_static (G_TYPE_OBJECT, "SimPluginSid", &type_info, 0);
  }

  return object_type;
}

/*
 *
 *
 *
 *
 */
SimPluginSid*
sim_plugin_sid_new (void)
{
  SimPluginSid *plugin_sid = NULL;

  plugin_sid = SIM_PLUGIN_SID (g_object_new (SIM_TYPE_PLUGIN_SID, NULL));

  return plugin_sid;
}

/*
 *
 *
 *
 *
 */
SimPluginSid*
sim_plugin_sid_new_from_data (gint          plugin_id,
												      gint          sid,
												      gint          reliability,
												      gint          priority,
												      const gchar  *name)
{
  SimPluginSid *plugin_sid = NULL;

  plugin_sid = SIM_PLUGIN_SID (g_object_new (SIM_TYPE_PLUGIN_SID, NULL));
  plugin_sid->_priv->plugin_id = plugin_id;
  plugin_sid->_priv->sid = sid;
  plugin_sid->_priv->reliability = reliability;
  plugin_sid->_priv->priority = priority;
  plugin_sid->_priv->name = g_strdup (name);  

  return plugin_sid;
}

/*
 *
 *
 *
 */
SimPluginSid*
sim_plugin_sid_new_from_dm (GdaDataModel  *dm,
												    gint           row)
{
  SimPluginSid  *plugin_sid;
  GdaValue      *value;

  g_return_val_if_fail (dm, NULL);
  g_return_val_if_fail (GDA_IS_DATA_MODEL (dm), NULL);

  plugin_sid = SIM_PLUGIN_SID (g_object_new (SIM_TYPE_PLUGIN_SID, NULL));

  value = (GdaValue *) gda_data_model_get_value_at (dm, 0, row);
  if (!gda_value_is_null (value))
    plugin_sid->_priv->plugin_id = gda_value_get_integer (value);

  value = (GdaValue *) gda_data_model_get_value_at (dm, 1, row);
  if (!gda_value_is_null (value))
    plugin_sid->_priv->sid = gda_value_get_integer (value);
  
  value = (GdaValue *) gda_data_model_get_value_at (dm, 2, row);
  if (!gda_value_is_null (value))
    plugin_sid->_priv->reliability = gda_value_get_integer (value);
  
  value = (GdaValue *) gda_data_model_get_value_at (dm, 3, row);
  if (!gda_value_is_null (value))
    plugin_sid->_priv->priority = gda_value_get_integer (value);
  
  value = (GdaValue *) gda_data_model_get_value_at (dm, 4, row);
  if (!gda_value_is_null (value))
    plugin_sid->_priv->name = gda_value_stringify (value);

  return plugin_sid;
}

/*
 *
 * Returns the plugin id wich is the "owner" of the plugin_sid given.
 *
 *
 */
gint
sim_plugin_sid_get_plugin_id (SimPluginSid  *plugin_sid)
{
  g_return_val_if_fail (plugin_sid, 0);
  g_return_val_if_fail (SIM_IS_PLUGIN_SID (plugin_sid), 0);

  return plugin_sid->_priv->plugin_id;
}

/*
 *
 *
 *
 *
 */
void
sim_plugin_sid_set_plugin_id (SimPluginSid  *plugin_sid,
			      gint           plugin_id)
{
  g_return_if_fail (plugin_sid);
  g_return_if_fail (SIM_IS_PLUGIN_SID (plugin_sid));
  g_return_if_fail (plugin_id > 0);

  plugin_sid->_priv->plugin_id = plugin_id;
}

/*
 *
 * gets the sid from the object plugin_sid
 *
 *
 */
gint
sim_plugin_sid_get_sid (SimPluginSid  *plugin_sid)
{
  g_return_val_if_fail (plugin_sid, 0);
  g_return_val_if_fail (SIM_IS_PLUGIN_SID (plugin_sid), 0);

  return plugin_sid->_priv->sid;
}

/*
 *
 *
 *
 *
 */
void
sim_plugin_sid_set_sid (SimPluginSid  *plugin_sid,
			gint           sid)
{
  g_return_if_fail (plugin_sid);
  g_return_if_fail (SIM_IS_PLUGIN_SID (plugin_sid));
  g_return_if_fail (sid > 0);

  plugin_sid->_priv->sid = sid;
}

/*
 *
 *
 *
 *
 */
gint
sim_plugin_sid_get_reliability (SimPluginSid  *plugin_sid)
{
  g_return_val_if_fail (plugin_sid, -1);
  g_return_val_if_fail (SIM_IS_PLUGIN_SID (plugin_sid), -1);

  return plugin_sid->_priv->reliability;
}

/*
 *
 *
 *
 *
 */
void
sim_plugin_sid_set_reliability (SimPluginSid  *plugin_sid,
			      gint           reliability)
{
  g_return_if_fail (plugin_sid);
  g_return_if_fail (SIM_IS_PLUGIN_SID (plugin_sid));
  g_return_if_fail (reliability > 0);

  plugin_sid->_priv->reliability = reliability;
}

/*
 *
 *
 *
 *
 */
gint
sim_plugin_sid_get_priority (SimPluginSid  *plugin_sid)
{
  g_return_val_if_fail (plugin_sid, -1);
  g_return_val_if_fail (SIM_IS_PLUGIN_SID (plugin_sid), -1);

  return plugin_sid->_priv->priority;
}

/*
 *
 *
 *
 *
 */
void
sim_plugin_sid_set_priority (SimPluginSid  *plugin_sid,
			      gint           priority)
{
  g_return_if_fail (plugin_sid);
  g_return_if_fail (SIM_IS_PLUGIN_SID (plugin_sid));
  g_return_if_fail (priority > 0);

  plugin_sid->_priv->priority = priority;
}


/*
 *
 *
 *
 *
 */
gchar*
sim_plugin_sid_get_name (SimPluginSid  *plugin_sid)
{
  g_return_val_if_fail (plugin_sid, NULL);
  g_return_val_if_fail (SIM_IS_PLUGIN_SID (plugin_sid), NULL);

  return plugin_sid->_priv->name;
}

/*
 *
 *
 *
 *
 */
void
sim_plugin_sid_set_name (SimPluginSid  *plugin_sid,
			 gchar         *name)
{
  g_return_if_fail (plugin_sid);
  g_return_if_fail (SIM_IS_PLUGIN_SID (plugin_sid));
  g_return_if_fail (name);

  if (plugin_sid->_priv->name)
    g_free (plugin_sid->_priv->name);

  plugin_sid->_priv->name = name;
}

/*
 *
 *
 *
 *
 */
gchar*
sim_plugin_sid_get_insert_clause (SimPluginSid  *plugin_sid)
{
  GString  *insert;
  GString  *values;

  g_return_val_if_fail (plugin_sid, NULL);
  g_return_val_if_fail (SIM_IS_PLUGIN_SID (plugin_sid), NULL);
  g_return_val_if_fail (plugin_sid->_priv->plugin_id > 0, NULL);
  g_return_val_if_fail (plugin_sid->_priv->sid > 0, NULL);
  g_return_val_if_fail (plugin_sid->_priv->name, NULL);

  insert = g_string_new ("INSERT INTO plugin_sid (");
  values = g_string_new (" VALUES (");

  g_string_append (insert, "plugin_id");
  g_string_append_printf (values, "%d", plugin_sid->_priv->plugin_id);

  g_string_append (insert, ", sid");
  g_string_append_printf (values, ", %d", plugin_sid->_priv->sid);

  if (plugin_sid->_priv->reliability > 0)
    {
      g_string_append (insert, ", reliability");
      g_string_append_printf (values, ", %d", plugin_sid->_priv->reliability);
    }

  if (plugin_sid->_priv->priority > 0)
    {
      g_string_append (insert, ", priority");
      g_string_append_printf (values, ", %d", plugin_sid->_priv->priority);
    }

  g_string_append (insert, ", name)");
  g_string_append_printf (values, ", '%s')", plugin_sid->_priv->name);

  g_string_append (insert, values->str);

  g_string_free (values, TRUE);

  return g_string_free (insert, FALSE);
}

/*
 *
 * Debug function
 *
 *
 */
void
sim_plugin_sid_print_internal_data (SimPluginSid  *plugin_sid)
{

  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_plugin_sid_print_internal_data:");

  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Name: %s", plugin_sid->_priv->name);
  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "plugin_id: %d", plugin_sid->_priv->plugin_id);
  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sid: %d", plugin_sid->_priv->sid);
  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "reliability: %d", plugin_sid->_priv->reliability);
  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "priority: %d", plugin_sid->_priv->priority);



}

// vim: set tabstop=2:
