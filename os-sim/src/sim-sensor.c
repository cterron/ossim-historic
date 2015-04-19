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


#include "sim-sensor.h"
#include <config.h>

enum
{
  DESTROY,
  LAST_SIGNAL
};

struct _SimSensorPrivate {
  gchar       *name;

  GInetAddr   *ia;
  gint         port;

  gboolean     connect;
  gboolean     compress;
  gboolean     ssl;

  GHashTable  *plugins; //SimPlugin

	event_kind	event_number;
	
};

static gpointer parent_class = NULL;
static gint sim_inet_signals[LAST_SIGNAL] = { 0 };

/* GType Functions */

static void 
sim_sensor_impl_dispose (GObject  *gobject)
{
  G_OBJECT_CLASS (parent_class)->dispose (gobject);
}

static void 
sim_sensor_impl_finalize (GObject  *gobject)
{
  SimSensor *sensor = SIM_SENSOR (gobject);
  GList    *list;

  if (sensor->_priv->ia)
    gnet_inetaddr_unref (sensor->_priv->ia);

  list = sim_sensor_get_plugins (sensor);
  while (list)
    {
      SimPlugin *plugin = (SimPlugin *) list->data;
      g_object_unref (plugin);
      list = list->next;
    }
  g_list_free (list);
  g_hash_table_destroy (sensor->_priv->plugins);

  g_free (sensor->_priv);

  G_OBJECT_CLASS (parent_class)->finalize (gobject);
}

static void
sim_sensor_class_init (SimSensorClass * class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  parent_class = g_type_class_ref (G_TYPE_OBJECT);

  object_class->dispose = sim_sensor_impl_dispose;
  object_class->finalize = sim_sensor_impl_finalize;
}

static void
sim_sensor_instance_init (SimSensor *sensor)
{
  sensor->_priv = g_new0 (SimSensorPrivate, 1);

  sensor->_priv->name = NULL;

  sensor->_priv->ia = NULL;
  sensor->_priv->port = 0;

  sensor->_priv->connect = FALSE;
  sensor->_priv->compress = FALSE;
  sensor->_priv->ssl = FALSE;

  sensor->_priv->plugins = g_hash_table_new (NULL, NULL);

	sensor->_priv->event_number.events = 0;
	sensor->_priv->event_number.host_os_events = 0;
	sensor->_priv->event_number.host_mac_events = 0;
	sensor->_priv->event_number.host_ids_events = 0;
	sensor->_priv->event_number.host_service_events = 0;

}

/* Public Methods */

GType
sim_sensor_get_type (void)
{
  static GType object_type = 0;
 
  if (!object_type)
  {
    static const GTypeInfo type_info = {
              sizeof (SimSensorClass),
              NULL,
              NULL,
              (GClassInitFunc) sim_sensor_class_init,
              NULL,
              NULL,                       /* class data */
              sizeof (SimSensor),
              0,                          /* number of pre-allocs */
              (GInstanceInitFunc) sim_sensor_instance_init,
              NULL                        /* value table */
    };
    
    g_type_init ();
                                                                                                                             
    object_type = g_type_register_static (G_TYPE_OBJECT, "SimSensor", &type_info, 0);
  }

  return object_type;
}

/*
 *
 *
 *
 *
 */
SimSensor*
sim_sensor_new (void)
{
  SimSensor *sensor = NULL;

  sensor = SIM_SENSOR (g_object_new (SIM_TYPE_SENSOR, NULL));

  return sensor;
}

/*
 * We can choose between create a sensor with or without ip defined.
 *
 */
SimSensor*
sim_sensor_new_from_hostname (gchar *sensor_ip)
{
  SimSensor *sensor = NULL;

  sensor = SIM_SENSOR (g_object_new (SIM_TYPE_SENSOR, NULL));
	
	if (sensor->_priv->ia = gnet_inetaddr_new_nonblock (sensor_ip, 0))
		return sensor;
	else
		return NULL;
}


/*
 *
 *
 *
 */
SimSensor*
sim_sensor_new_from_dm (GdaDataModel  *dm,
		      gint           row)
{
  SimSensor    *sensor;
  GdaValue     *value;

  g_return_val_if_fail (dm, NULL);
  g_return_val_if_fail (GDA_IS_DATA_MODEL (dm), NULL);

  sensor = SIM_SENSOR (g_object_new (SIM_TYPE_SENSOR, NULL));

  value = (GdaValue *) gda_data_model_get_value_at (dm, 0, row);
  sensor->_priv->name = gda_value_stringify (value);
  
  value = (GdaValue *) gda_data_model_get_value_at (dm, 1, row);
  sensor->_priv->ia = gnet_inetaddr_new_nonblock (gda_value_get_string (value), 0);
  
  value = (GdaValue *) gda_data_model_get_value_at (dm, 2, row);
  sensor->_priv->port = gda_value_get_integer (value);

  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_sensor_new_from_dm: %s", sensor->_priv->name);
  sim_sensor_debug_print(sensor);

  return sensor;
}

/*
 *
 *
 *
 *
 */
SimSensor*
sim_sensor_clone (SimSensor *sensor)
{
  SimSensor *new_sensor;
  
  g_return_val_if_fail (sensor != NULL, NULL);
  g_return_val_if_fail (SIM_IS_SENSOR (sensor), NULL);

  new_sensor = SIM_SENSOR (g_object_new (SIM_TYPE_SENSOR, NULL));
  new_sensor->_priv->name = g_strdup (sensor->_priv->name);
  new_sensor->_priv->ia = (sensor->_priv->ia) ? gnet_inetaddr_clone (sensor->_priv->ia) : NULL;
  new_sensor->_priv->port = sensor->_priv->port;
  new_sensor->_priv->connect = sensor->_priv->connect;
  new_sensor->_priv->compress = sensor->_priv->compress;
  new_sensor->_priv->ssl = sensor->_priv->ssl;

  new_sensor->_priv->plugins = NULL;

  return new_sensor;
}

/*
 *
 *
 *
 *
 */
gchar*
sim_sensor_get_name (SimSensor  *sensor)
{
  g_return_val_if_fail (sensor, NULL);
  g_return_val_if_fail (SIM_IS_SENSOR (sensor), NULL);

  return sensor->_priv->name;
}

/*
 *
 *
 *
 *
 */
void
sim_sensor_set_name (SimSensor  *sensor,
		     gchar      *name)
{
  g_return_if_fail (sensor);
  g_return_if_fail (SIM_IS_SENSOR (sensor));
  g_return_if_fail (name);

  if (sensor->_priv->name)
    g_free (sensor->_priv->name);

  sensor->_priv->name = g_strdup (name);
}

/*
 *
 *
 *
 *
 */
GInetAddr*
sim_sensor_get_ia (SimSensor  *sensor)
{
  g_return_val_if_fail (sensor, NULL);
  g_return_val_if_fail (SIM_IS_SENSOR (sensor), NULL);

  return sensor->_priv->ia;
}

/*
 *
 *
 *
 *
 */
void
sim_sensor_set_ia (SimSensor   *sensor,
		  GInetAddr  *ia)
{
  g_return_if_fail (sensor);
  g_return_if_fail (SIM_IS_SENSOR (sensor));
  g_return_if_fail (ia);

  if (sensor->_priv->ia)
    gnet_inetaddr_unref (sensor->_priv->ia);

  sensor->_priv->ia = ia;
}

/*
 *
 *
 *
 *
 */
gint
sim_sensor_get_port (SimSensor  *sensor)
{
  g_return_val_if_fail (sensor, 0);
  g_return_val_if_fail (SIM_IS_SENSOR (sensor), 0);

  return sensor->_priv->port;
}

/*
 *
 *
 *
 *
 */
void
sim_sensor_set_port (SimSensor  *sensor,
		    gint        port)
{
  g_return_if_fail (sensor);
  g_return_if_fail (SIM_IS_SENSOR (sensor));
  g_return_if_fail (port > 0);

  sensor->_priv->port = port;
}

/*
 *
 *
 *
 *
 */
gboolean
sim_sensor_is_connect (SimSensor  *sensor)
{
  g_return_val_if_fail (sensor, FALSE);
  g_return_val_if_fail (SIM_IS_SENSOR (sensor), FALSE);

  return sensor->_priv->connect;
}

/*
 *
 *
 *
 *
 */
void
sim_sensor_set_connect (SimSensor  *sensor,
		       gboolean   connect)
{
  g_return_if_fail (sensor);
  g_return_if_fail (SIM_IS_SENSOR (sensor));

  sensor->_priv->connect = connect;
}

/*
 *
 *
 *
 *
 */
gboolean
sim_sensor_is_compress (SimSensor  *sensor)
{
  g_return_val_if_fail (sensor, FALSE);
  g_return_val_if_fail (SIM_IS_SENSOR (sensor), FALSE);

  return sensor->_priv->compress;
}

/*
 *
 *
 *
 *
 */
void
sim_sensor_set_compress (SimSensor  *sensor,
			gboolean   compress)
{
  g_return_if_fail (sensor);
  g_return_if_fail (SIM_IS_SENSOR (sensor));

  sensor->_priv->compress = compress;
}

/*
 *
 *
 *
 *
 */
gboolean
sim_sensor_is_ssl (SimSensor  *sensor)
{
  g_return_val_if_fail (sensor, FALSE);
  g_return_val_if_fail (SIM_IS_SENSOR (sensor), FALSE);

  return sensor->_priv->ssl;
}

/*
 *
 *
 *
 *
 */
void
sim_sensor_set_ssl (SimSensor  *sensor,
		   gboolean   ssl)
{
  g_return_if_fail (sensor);
  g_return_if_fail (SIM_IS_SENSOR (sensor));

  sensor->_priv->ssl = ssl;
}

/*
 *
 *
 *
 *
 */
void
sim_sensor_insert_plugin (SimSensor    *sensor,
			 SimPlugin   *plugin)
{
  SimPlugin   *tmp;
  gint         key;

  g_return_if_fail (sensor);
  g_return_if_fail (SIM_IS_SENSOR (sensor));
  g_return_if_fail (plugin);
  g_return_if_fail (SIM_IS_PLUGIN (plugin));
  g_return_if_fail (sim_plugin_get_id (plugin) > 0);

  key = sim_plugin_get_id (plugin);
  if ((tmp = g_hash_table_lookup (sensor->_priv->plugins, GINT_TO_POINTER (key))))
    {
      g_object_unref (tmp);
      g_hash_table_replace (sensor->_priv->plugins, GINT_TO_POINTER (key), plugin);
    }
  else
    {
      g_hash_table_insert (sensor->_priv->plugins, GINT_TO_POINTER (key), plugin);
    }
}

/*
 *
 *
 *
 *
 */
void
sim_sensor_remove_plugin (SimSensor    *sensor,
			 SimPlugin   *plugin)
{
  gint         key;

  g_return_if_fail (sensor);
  g_return_if_fail (SIM_IS_SENSOR (sensor));
  g_return_if_fail (plugin);
  g_return_if_fail (SIM_IS_PLUGIN (plugin));
  g_return_if_fail (sim_plugin_get_id (plugin) > 0);

  key = sim_plugin_get_id (plugin);
  g_hash_table_remove (sensor->_priv->plugins, GINT_TO_POINTER (key));
}

/*
 *
 *
 *
 *
 */
SimPlugin*
sim_sensor_get_plugin_by_id (SimSensor    *sensor,
			    gint         id)
{
  g_return_val_if_fail (sensor, NULL);
  g_return_val_if_fail (SIM_IS_SENSOR (sensor), NULL);
  g_return_val_if_fail (id > 0, NULL);
  
  return (SimPlugin *) g_hash_table_lookup (sensor->_priv->plugins, GINT_TO_POINTER (id));
}

/*
 *
 *
 *
 *
 */
static void
append_plugin_to_list (gpointer key, gpointer value, gpointer user_data)
{
  GList **list = (GList **) user_data;
  
  *list = g_list_append (*list, value);
}

/*
 *
 *
 *
 *
 */
GList*
sim_sensor_get_plugins (SimSensor    *sensor)
{
  GList *list = NULL;

  g_return_val_if_fail (sensor, NULL);
  g_return_val_if_fail (SIM_IS_SENSOR (sensor), NULL);
  g_return_val_if_fail (sensor->_priv->plugins, NULL);

  g_hash_table_foreach (sensor->_priv->plugins, (GHFunc) append_plugin_to_list, &list);

  return list;
}

/*
 *
 *
 *
 *
 */
gboolean
sim_sensor_has_plugin_by_type (SimSensor       *sensor,
			      SimPluginType   type)
{
  GList     *list;
  gboolean   found = FALSE;

  g_return_val_if_fail (sensor, FALSE);
  g_return_val_if_fail (SIM_IS_SENSOR (sensor), FALSE);

  list = sim_sensor_get_plugins (sensor);
  while (list)
    {
      SimPlugin *plugin = (SimPlugin *) list->data;
      
      if (plugin->type == type)
	{
	  found = TRUE;
	  break;
	}

      list = list->next;
    }
  g_list_free (list);

  return found;
}

/*
 * Functions used to store in the sensor how many events occur each 5 minutes thanks 
 * to sim_container_set_sensor_event_number(). Inline to try to spped this a bit.
 */
inline void
sim_sensor_add_number_events                (SimSensor  *sensor)
{
	sensor->_priv->event_number.events++;
}

inline void
sim_sensor_add_number_host_os_events        (SimSensor  *sensor)
{
	sensor->_priv->event_number.host_os_events++;
}

inline void
sim_sensor_add_number_host_mac_events       (SimSensor  *sensor)
{
	sensor->_priv->event_number.host_mac_events++;
}

inline void
sim_sensor_add_number_host_service_events   (SimSensor  *sensor)
{
	sensor->_priv->event_number.host_service_events++;
}

inline void
sim_sensor_add_number_host_ids_events       (SimSensor  *sensor)
{
	sensor->_priv->event_number.host_ids_events++;
}

event_kind
sim_sensor_get_events_number (SimSensor	*sensor)
{
	return sensor->_priv->event_number;	
}

void
sim_sensor_reset_events_number(SimSensor	*sensor)
{
	sensor->_priv->event_number.events = 0;
	sensor->_priv->event_number.host_mac_events = 0;
	sensor->_priv->event_number.host_os_events = 0;
	sensor->_priv->event_number.host_service_events = 0;
	sensor->_priv->event_number.host_ids_events = 0;
}

void
sim_sensor_debug_print	(SimSensor *sensor)
{
	GInetAddr *ia = sim_sensor_get_ia (sensor);
  gchar *ip = gnet_inetaddr_get_canonical_name (ia);

	gchar *aux = g_strdup_printf("%s|%s|%d",  sim_sensor_get_name (sensor),
                                            ip,
                                            sim_sensor_get_port (sensor));

  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_sensor_debug_print: %s", aux);

	g_free (aux);
  g_free (ip);

}

// vim: set tabstop=2:
