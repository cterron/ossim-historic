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

#include "sim-container.h"
#include "sim-xml-directive.h"

enum
{
  DESTROY,
  LAST_SIGNAL
};

struct _SimContainerPrivate {
  GList        *categories;
  GList        *classifications;;
  GList        *plugins;
  GList        *plugin_sids;
  GList        *sensors;
  GList        *hosts;
  GList        *nets;
  GList        *policies;
  GList        *directives;

  GList        *host_levels;
  GList        *net_levels;
  GList        *backlogs;

  GQueue       *alerts;

  GCond        *cond_alerts;
  GMutex       *mutex_alerts;
};

static gpointer parent_class = NULL;
static gint sim_container_signals[LAST_SIGNAL] = { 0 };

/* GType Functions */

static void 
sim_container_impl_dispose (GObject  *gobject)
{
  G_OBJECT_CLASS (parent_class)->dispose (gobject);
}

static void
sim_container_impl_finalize (GObject  *gobject)
{
  SimContainer  *container = SIM_CONTAINER (gobject);

  sim_container_free_categories_ul (container);
  sim_container_free_classifications_ul (container);
  sim_container_free_plugins_ul (container);
  sim_container_free_plugin_sids_ul (container);
  sim_container_free_sensors_ul (container);
  sim_container_free_hosts_ul (container);
  sim_container_free_nets_ul (container);
  sim_container_free_policies_ul (container);
  sim_container_free_directives_ul (container);
  sim_container_free_host_levels_ul (container);
  sim_container_free_net_levels_ul (container);
  sim_container_free_backlogs_ul (container);
  sim_container_free_alerts (container);

  g_cond_free (container->_priv->cond_alerts);
  g_mutex_free (container->_priv->mutex_alerts);

  g_free (container->_priv);

  G_OBJECT_CLASS (parent_class)->finalize (gobject);
}

static void
sim_container_class_init (SimContainerClass * class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  parent_class = g_type_class_ref (G_TYPE_OBJECT);

  object_class->dispose = sim_container_impl_dispose;
  object_class->finalize = sim_container_impl_finalize;
}

static void
sim_container_instance_init (SimContainer *container)
{
  container->_priv = g_new0 (SimContainerPrivate, 1);

  container->_priv->categories = NULL;
  container->_priv->classifications = NULL;
  container->_priv->plugins = NULL;
  container->_priv->plugin_sids = NULL;
  container->_priv->sensors = NULL;
  container->_priv->hosts = NULL;
  container->_priv->nets = NULL;

  container->_priv->policies = NULL;
  container->_priv->directives = NULL;

  container->_priv->host_levels = NULL;
  container->_priv->net_levels = NULL;
  container->_priv->backlogs = NULL;

  container->_priv->alerts = g_queue_new ();

  /* Mutex Alerts Init */
  container->_priv->cond_alerts = g_cond_new ();
  container->_priv->mutex_alerts = g_mutex_new ();
}

/* Public Methods */

GType
sim_container_get_type (void)
{
  static GType object_type = 0;
 
  if (!object_type)
  {
    static const GTypeInfo type_info = {
              sizeof (SimContainerClass),
              NULL,
              NULL,
              (GClassInitFunc) sim_container_class_init,
              NULL,
              NULL,                       /* class data */
              sizeof (SimContainer),
              0,                          /* number of pre-allocs */
              (GInstanceInitFunc) sim_container_instance_init,
              NULL                        /* value table */
    };
    
    g_type_init ();
                                                                                                                             
    object_type = g_type_register_static (G_TYPE_OBJECT, "SimContainer", &type_info, 0);
  }

  return object_type;
}

/*
 *
 *
 *
 *
 */
SimContainer*
sim_container_new (SimConfig  *config)
{
  SimDatabase  *database;
  SimContainer *container = NULL;
  SimConfigDS  *ds;

  g_return_val_if_fail (config, NULL);
  g_return_val_if_fail (SIM_IS_CONFIG (config), NULL);

  ds = sim_config_get_ds_by_name (config, SIM_DS_OSSIM);
  database = sim_database_new (ds);

  container = SIM_CONTAINER (g_object_new (SIM_TYPE_CONTAINER, NULL));
  sim_container_db_delete_plugin_sid_directive_ul (container, database);
  sim_container_db_delete_backlogs_ul (container, database);
  sim_container_db_load_categories (container, database);
  sim_container_db_load_classifications (container, database);
  sim_container_db_load_plugins (container, database);
  sim_container_db_load_plugin_sids (container, database);
  sim_container_db_load_sensors (container, database);
  sim_container_db_load_hosts (container, database);
  sim_container_db_load_nets (container, database);
  sim_container_db_load_policies (container, database);
  sim_container_db_load_host_levels (container, database);
  sim_container_db_load_net_levels (container, database);
  
  if ((config->directive.filename) && (g_file_test (config->directive.filename, G_FILE_TEST_EXISTS)))
    sim_container_load_directives_from_file (container, database, config->directive.filename);
  
  g_object_unref (database);

  return container;
}


/*
 *
 *
 *
 *
 */
gchar*
sim_container_db_get_host_os_ul (SimContainer  *container,
				 SimDatabase   *database,
				 GInetAddr     *ia)
{
  GdaDataModel  *dm;
  GdaValue      *value;
  gchar         *query;
  gchar         *os = NULL;
  gint           row;

  g_return_if_fail (container != NULL);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database != NULL);
  g_return_if_fail (SIM_IS_DATABASE (database));

  query = g_strdup_printf ("SELECT os FROM host_os WHERE ip = %lu",
			   sim_inetaddr_ntohl (ia));
  dm = sim_database_execute_single_command (database, query);
  if (dm)
    {
      for (row = 0; row < gda_data_model_get_n_rows (dm); row++)
	{
	  value = (GdaValue *) gda_data_model_get_value_at (dm, 0, row);
	  if (!gda_value_is_null (value))
	    os = gda_value_stringify (value);
	}
      
      g_object_unref(dm);
    }
  else
    {
      g_message ("HOST OS DATA MODEL ERROR");
    }
  g_free (query);

  return os;
}

/*
 *
 *
 *
 *
 */
void
sim_container_db_insert_host_os_ul (SimContainer  *container,
				    SimDatabase   *database,
				    GInetAddr     *ia,
				    gchar         *date,
				    gchar         *os)
{
  gchar         *query;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));
  g_return_if_fail (ia);
  g_return_if_fail (date);
  g_return_if_fail (os);

  query = g_strdup_printf ("INSERT INTO host_os (ip, date, os) VALUES (%lu, '%s', '%s')",
			   sim_inetaddr_ntohl (ia), date, os);

  sim_database_execute_no_query (database, query);

  g_free (query);
}

/*
 *
 *
 *
 *
 */
void
sim_container_db_update_host_os_ul (SimContainer  *container,
				    SimDatabase   *database,
				    GInetAddr     *ia,
				    gchar         *date,
				    gchar         *curr_os,
				    gchar         *prev_os)
{
  gchar         *query;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));
  g_return_if_fail (ia);
  g_return_if_fail (date);
  g_return_if_fail (curr_os);
  g_return_if_fail (prev_os);

  query = g_strdup_printf ("UPDATE host_os SET date='%s', os='%s', previous='%s' WHERE ip = %lu",
			   date, curr_os, prev_os, sim_inetaddr_ntohl (ia));

  sim_database_execute_no_query (database, query);

  g_free (query);
}

/*
 *
 *
 *
 *
 */
gchar*
sim_container_db_get_host_mac_ul (SimContainer  *container,
				 SimDatabase   *database,
				 GInetAddr     *ia)
{
  GdaDataModel  *dm;
  GdaValue      *value;
  gchar         *query;
  gchar         *mac = NULL;
  gint           row;

  g_return_if_fail (container != NULL);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database != NULL);
  g_return_if_fail (SIM_IS_DATABASE (database));

  query = g_strdup_printf ("SELECT mac FROM host_mac WHERE ip = %lu",
			   sim_inetaddr_ntohl (ia));
  dm = sim_database_execute_single_command (database, query);
  if (dm)
    {
      for (row = 0; row < gda_data_model_get_n_rows (dm); row++)
	{
	  value = (GdaValue *) gda_data_model_get_value_at (dm, 0, row);
	  if (!gda_value_is_null (value))
	    mac = gda_value_stringify (value);
	}
      
      g_object_unref(dm);
    }
  else
    {
      g_message ("HOST OS DATA MODEL ERROR");
    }
  g_free (query);

  return mac;
}

/*
 *
 *
 *
 *
 */
gchar*
sim_container_db_get_host_mac_vendor_ul (SimContainer  *container,
					 SimDatabase   *database,
					 GInetAddr     *ia)
{
  GdaDataModel  *dm;
  GdaValue      *value;
  gchar         *query;
  gchar         *vendor = NULL;
  gint           row;

  g_return_if_fail (container != NULL);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database != NULL);
  g_return_if_fail (SIM_IS_DATABASE (database));

  query = g_strdup_printf ("SELECT vendor FROM host_mac WHERE ip = %lu",
			   sim_inetaddr_ntohl (ia));
  dm = sim_database_execute_single_command (database, query);
  if (dm)
    {
      for (row = 0; row < gda_data_model_get_n_rows (dm); row++)
	{
	  value = (GdaValue *) gda_data_model_get_value_at (dm, 0, row);
	  if (!gda_value_is_null (value))
	    vendor = gda_value_stringify (value);
	}
      
      g_object_unref(dm);
    }
  else
    {
      g_message ("HOST OS DATA MODEL ERROR");
    }
  g_free (query);

  return vendor;
}

/*
 *
 *
 *
 *
 */
void
sim_container_db_insert_host_mac_ul (SimContainer  *container,
				     SimDatabase   *database,
				     GInetAddr     *ia,
				     gchar         *date,
				     gchar         *mac,
				     gchar         *vendor)
{
  gchar         *query;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));
  g_return_if_fail (ia);
  g_return_if_fail (date);
  g_return_if_fail (mac);

  query = g_strdup_printf ("INSERT INTO host_mac (ip, date, mac, vendor) VALUES (%lu, '%s', '%s', '%s')",
			   sim_inetaddr_ntohl (ia), date, mac, (vendor) ? vendor : "");

  sim_database_execute_no_query (database, query);

  g_free (query);
}

/*
 *
 *
 *
 *
 */
void
sim_container_db_update_host_mac_ul (SimContainer  *container,
				     SimDatabase   *database,
				     GInetAddr     *ia,
				     gchar         *date,
				     gchar         *curr_mac,
				     gchar         *prev_mac,
				     gchar         *vendor)
{
  gchar         *query;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));
  g_return_if_fail (ia);
  g_return_if_fail (date);
  g_return_if_fail (curr_mac);
  g_return_if_fail (prev_mac);

  query = g_strdup_printf ("UPDATE host_mac SET date='%s', mac='%s', previous='%s', vendor='%s' WHERE ip = %lu",
			   date, curr_mac, prev_mac, (vendor) ? vendor : "", sim_inetaddr_ntohl (ia));

  sim_database_execute_no_query (database, query);

  g_free (query);
}


/*
 *
 *
 *
 *
 */
void
sim_container_db_delete_backlogs_ul (SimContainer  *container,
				     SimDatabase   *database)
{
  gchar         *query = "DELETE FROM backlog";

  g_return_if_fail (container != NULL);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database != NULL);
  g_return_if_fail (SIM_IS_DATABASE (database));
  
  sim_database_execute_no_query (database, query);
}

/*
 *
 *
 *
 *
 */
void
sim_container_db_delete_plugin_sid_directive_ul (SimContainer  *container,
						 SimDatabase   *database)
{
  gchar         *query = "DELETE FROM plugin_sid WHERE plugin_id = 1505";
  
  g_return_if_fail (container != NULL);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database != NULL);
  g_return_if_fail (SIM_IS_DATABASE (database));
  
  sim_database_execute_no_query (database, query);
}

/*
 *
 *
 *
 *
 */
GList*
sim_container_db_host_get_plugin_sids_ul (SimContainer  *container,
					 SimDatabase   *database,
					 GInetAddr     *ia,
					 gint           plugin_id,
					 gint           plugin_sid)
{
  GdaDataModel  *dm;
  GdaValue      *value;
  gchar         *query;
  gint           row;
  GList         *list = NULL;
  gint           reference_id;
  gint           reference_sid;

  g_return_val_if_fail (container, 0);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), 0);
  g_return_val_if_fail (database, 0);
  g_return_val_if_fail (SIM_IS_DATABASE (database), 0);
  g_return_val_if_fail (ia, 0);
  g_return_val_if_fail (plugin_id > 0, 0);
  g_return_val_if_fail (plugin_sid > 0, 0);

  query = g_strdup_printf ("SELECT reference_id, reference_sid "
			   "FROM host_plugin_sid INNER JOIN plugin_reference "
			   "ON (host_plugin_sid.plugin_id = plugin_reference.reference_id "
			   "AND host_plugin_sid.plugin_sid = plugin_reference.reference_sid) "
			   "WHERE host_ip = %u "
			   "AND plugin_reference.plugin_id = %d "
			   "AND plugin_reference.plugin_sid = %d",
			   sim_inetaddr_ntohl (ia), plugin_id, plugin_sid);

  dm = sim_database_execute_single_command (database, query);
  if (dm)
    {
      for (row = 0; row < gda_data_model_get_n_rows (dm); row++)
	{
	  SimPluginSid *sid;

	  value = (GdaValue *) gda_data_model_get_value_at (dm, 0, row);
	  reference_id = gda_value_get_integer (value);
	  value = (GdaValue *) gda_data_model_get_value_at (dm, 1, row);
	  reference_sid = gda_value_get_integer (value);

	  sid = sim_container_get_plugin_sid_by_pky (container,
						     reference_id,
						     reference_sid);

	  if (sid)
	    list = g_list_append (list, sid);
	}
      
      g_object_unref(dm);
    }
  else
    {
      g_message ("HOST PLUGIN SID DATA MODEL ERROR");
    }
  g_free (query);

  return list;
}


/*
 *
 *
 *
 *
 */
gint
sim_container_db_get_recovery_ul (SimContainer  *container,
				  SimDatabase   *database)
{
  GdaDataModel  *dm;
  GdaValue      *value;
  gchar         *query = "SELECT recovery FROM conf";
  gint           row;
  gint           recovery = 1;

  g_return_val_if_fail (container != NULL, 0);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), 0);
  g_return_val_if_fail (database != NULL, 0);
  g_return_val_if_fail (SIM_IS_DATABASE (database), 0);
  
  dm = sim_database_execute_single_command (database, query);
  if (dm)
    {
      for (row = 0; row < gda_data_model_get_n_rows (dm); row++)
	{
	  /* Recovery */
	  value = (GdaValue *) gda_data_model_get_value_at (dm, 0, row);
	  recovery = gda_value_get_integer (value);
	}
      
      g_object_unref(dm);
    }
  else
    {
      g_message ("RECOVERY DATA MODEL ERROR");
    }

  return recovery;
}

/*
 *
 *
 *
 *
 */
gint
sim_container_db_get_recovery (SimContainer  *container,
			       SimDatabase   *database)
{
  gint   recovery;

  g_return_val_if_fail (container != NULL, 0);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), 0);
  g_return_val_if_fail (database != NULL, 0);
  g_return_val_if_fail (SIM_IS_DATABASE (database), 0);

  G_LOCK (s_mutex_config);
  recovery = sim_container_db_get_recovery_ul (container, database);
  G_UNLOCK (s_mutex_config);

  return recovery;
}

/*
 *
 *
 *
 *
 */
gint
sim_container_db_get_threshold_ul (SimContainer  *container,
				  SimDatabase   *database)
{
  GdaDataModel  *dm;
  GdaValue      *value;
  gchar         *query = "SELECT threshold FROM conf";
  gint           row;
  gint           threshold = 1;

  g_return_val_if_fail (container != NULL, 0);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), 0);
  g_return_val_if_fail (database != NULL, 0);
  g_return_val_if_fail (SIM_IS_DATABASE (database), 0);
  
  dm = sim_database_execute_single_command (database, query);
  if (dm)
    {
      for (row = 0; row < gda_data_model_get_n_rows (dm); row++)
	{
	  /* Threshold */
	  value = (GdaValue *) gda_data_model_get_value_at (dm, 0, row);
	  threshold = gda_value_get_integer (value);
	}
      
      g_object_unref(dm);
    }
  else
    {
      g_message ("THRESHOLD DATA MODEL ERROR");
    }

  return threshold;
}

/*
 *
 *
 *
 *
 */
gint
sim_container_db_get_threshold (SimContainer  *container,
				SimDatabase   *database)
{
  gint   threshold;

  g_return_val_if_fail (container != NULL, 0);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), 0);
  g_return_val_if_fail (database != NULL, 0);
  g_return_val_if_fail (SIM_IS_DATABASE (database), 0);

  G_LOCK (s_mutex_config);
  threshold = sim_container_db_get_threshold_ul (container, database);
  G_UNLOCK (s_mutex_config);

  return threshold;
}

/*
 *
 *
 *
 *
 */
gint
sim_container_db_get_max_plugin_sid_ul (SimContainer  *container,
					SimDatabase   *database,
					gint           plugin_id)
{
  GdaDataModel  *dm;
  GdaValue      *value;
  gchar         *query;
  gint           row;
  gint           max_sid = 0;

  g_return_val_if_fail (container != NULL, 0);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), 0);
  g_return_val_if_fail (database != NULL, 0);
  g_return_val_if_fail (SIM_IS_DATABASE (database), 0);
  g_return_val_if_fail (plugin_id > 0, 0);  

  query = g_strdup_printf ("SELECT max(sid) FROM plugin_sid WHERE plugin_id = %d", plugin_id);

  dm = sim_database_execute_single_command (database, query);
  if (dm)
    {
      for (row = 0; row < gda_data_model_get_n_rows (dm); row++)
	{
	  /* Max Sid */
	  value = (GdaValue *) gda_data_model_get_value_at (dm, 0, row);
	  if (!gda_value_is_null (value))
	    max_sid = gda_value_get_integer (value);
	}
      
      g_object_unref(dm);
    }
  else
    {
      g_message ("MAX PLUGIN SID DATA MODEL ERROR");
    }


  g_free (query);

  return max_sid;
}

/*
 *
 *
 *
 *
 */
gint
sim_container_db_get_max_plugin_sid (SimContainer  *container,
				     SimDatabase   *database,
				     gint           plugin_id)
{
  gint   max_sid;

  g_return_val_if_fail (container != NULL, 0);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), 0);
  g_return_val_if_fail (database != NULL, 0);
  g_return_val_if_fail (SIM_IS_DATABASE (database), 0);
  g_return_val_if_fail (plugin_id > 0, 0);  

  G_LOCK (s_mutex_plugin_sids);
  max_sid = sim_container_db_get_max_plugin_sid_ul (container, database, plugin_id);
  G_UNLOCK (s_mutex_plugin_sids);

  return max_sid;
}


/*
 *
 *
 *
 *
 */
void
sim_container_db_load_categories_ul (SimContainer  *container,
				SimDatabase   *database)
{
  SimCategory       *category;
  GdaDataModel  *dm;
  gint           row;
  gchar         *query = "SELECT id, name FROM category";

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));

  dm = sim_database_execute_single_command (database, query);
  if (dm)
    {
      for (row = 0; row < gda_data_model_get_n_rows (dm); row++)
	{
	  category  = sim_category_new_from_dm (dm, row);
	  container->_priv->categories = g_list_append (container->_priv->categories, category);
	}

      g_object_unref(dm);
    }
  else
    {
      g_message ("CATEGORIES DATA MODEL ERROR");
    }
}

/*
 *
 *
 *
 *
 */
void
sim_container_append_category_ul (SimContainer  *container,
			      SimCategory       *category)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (category);
  g_return_if_fail (SIM_IS_CATEGORY (category));

  container->_priv->categories = g_list_append (container->_priv->categories, category);
}

/*
 *
 *
 *
 *
 */
void
sim_container_remove_category_ul (SimContainer  *container,
			      SimCategory       *category)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (category);
  g_return_if_fail (SIM_IS_CATEGORY (category));

  container->_priv->categories = g_list_remove (container->_priv->categories, category);
}

/*
 *
 *
 *
 *
 */
GList*
sim_container_get_categories_ul (SimContainer  *container)
{
  GList *list;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);

  list = g_list_copy (container->_priv->categories);

  return list;
}

/*
 *
 *
 *
 *
 */
void
sim_container_set_categories_ul (SimContainer  *container,
			    GList         *categories)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (categories);

  container->_priv->categories = g_list_concat (container->_priv->categories, categories);
}

/*
 *
 *
 *
 *
 */
void
sim_container_free_categories_ul (SimContainer  *container)
{
  GList *list;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));

  list = container->_priv->categories;
  while (list)
    {
      SimCategory *category = (SimCategory *) list->data;
      g_object_unref (category);

      list = list->next;
    }
  g_list_free (container->_priv->categories);
  container->_priv->categories = NULL;
}

/*
 *
 *
 *
 *
 */
SimCategory*
sim_container_get_category_by_id_ul (SimContainer  *container,
				   gint           id)
{
  SimCategory   *category;
  GList     *list;
  gboolean   found = FALSE;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);
  g_return_val_if_fail (id > 0, NULL);

  list = container->_priv->categories;
  while (list)
    {
      category = (SimCategory *) list->data;

      if (sim_category_get_id (category) == id)
	{
	  found = TRUE;
	  break;
	}

      list = list->next;
    }

  if (!found)
    return NULL;

  return category;
}

/*
 *
 *
 *
 *
 */
SimCategory*
sim_container_get_category_by_name_ul (SimContainer  *container,
				       const gchar   *name)
{
  SimCategory   *category;
  GList         *list;
  gboolean       found = FALSE;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);
  g_return_val_if_fail (name, NULL);

  list = container->_priv->categories;
  while (list)
    {
      category = (SimCategory *) list->data;

      if (!g_ascii_strcasecmp (sim_category_get_name (category), name))
	{
	  found = TRUE;
	  break;
	}

      list = list->next;
    }

  if (!found)
    return NULL;

  return category;
}

/*
 *
 *
 *
 *
 */
void
sim_container_db_load_categories (SimContainer  *container,
			     SimDatabase   *database)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));

  G_LOCK (s_mutex_categories);
  sim_container_db_load_categories_ul (container, database);
  G_UNLOCK (s_mutex_categories);
}

/*
 *
 *
 *
 *
 */
void
sim_container_append_category (SimContainer  *container,
			   SimCategory       *category)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (category);
  g_return_if_fail (SIM_IS_CATEGORY (category));

  G_LOCK (s_mutex_categories);
  sim_container_append_category_ul (container, category);
  G_UNLOCK (s_mutex_categories);
}

/*
 *
 *
 *
 *
 */
void
sim_container_remove_category (SimContainer  *container,
			   SimCategory       *category)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (category);
  g_return_if_fail (SIM_IS_CATEGORY (category));

  G_LOCK (s_mutex_categories);
  sim_container_remove_category_ul (container, category);
  G_UNLOCK (s_mutex_categories);
}

/*
 *
 *
 *
 *
 */
GList*
sim_container_get_categories (SimContainer  *container)
{
  GList *list;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);

  G_LOCK (s_mutex_categories);
  list = sim_container_get_categories_ul (container);
  G_UNLOCK (s_mutex_categories);

  return list;
}

/*
 *
 *
 *
 *
 */
void
sim_container_set_categories (SimContainer  *container,
			 GList         *categories)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (categories);

  G_LOCK (s_mutex_categories);
  sim_container_set_categories_ul (container, categories);
  G_UNLOCK (s_mutex_categories);
}

/*
 *
 *
 *
 *
 */
void
sim_container_free_categories (SimContainer  *container)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));

  G_LOCK (s_mutex_categories);
  sim_container_free_categories_ul (container);
  G_UNLOCK (s_mutex_categories);
}

/*
 *
 *
 *
 *
 */
SimCategory*
sim_container_get_category_by_id (SimContainer  *container,
				  gint           id)
{
  SimCategory   *category;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);
  g_return_val_if_fail (id > 0, NULL);

  G_LOCK (s_mutex_categories);
  category = sim_container_get_category_by_id_ul (container, id);
  G_UNLOCK (s_mutex_categories);

  return category;
}

/*
 *
 *
 *
 *
 */
SimCategory*
sim_container_get_category_by_name (SimContainer  *container,
				    const gchar   *name)
{
  SimCategory   *category;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);
  g_return_val_if_fail (name, NULL);

  G_LOCK (s_mutex_categories);
  category = sim_container_get_category_by_name_ul (container, name);
  G_UNLOCK (s_mutex_categories);

  return category;
}

/*
 *
 *
 *
 *
 */
void
sim_container_db_load_classifications_ul (SimContainer  *container,
				SimDatabase   *database)
{
  SimClassification       *classification;
  GdaDataModel  *dm;
  gint           row;
  gchar         *query = "SELECT id, name, description, priority FROM classification";

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));

  dm = sim_database_execute_single_command (database, query);
  if (dm)
    {
      for (row = 0; row < gda_data_model_get_n_rows (dm); row++)
	{
	  classification  = sim_classification_new_from_dm (dm, row);
	  container->_priv->classifications = g_list_append (container->_priv->classifications, classification);
	}

      g_object_unref(dm);
    }
  else
    {
      g_message ("CLASSIFICATIONS DATA MODEL ERROR");
    }
}

/*
 *
 *
 *
 *
 */
void
sim_container_append_classification_ul (SimContainer  *container,
			      SimClassification       *classification)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (classification);
  g_return_if_fail (SIM_IS_CLASSIFICATION (classification));

  container->_priv->classifications = g_list_append (container->_priv->classifications, classification);
}

/*
 *
 *
 *
 *
 */
void
sim_container_remove_classification_ul (SimContainer  *container,
			      SimClassification       *classification)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (classification);
  g_return_if_fail (SIM_IS_CLASSIFICATION (classification));

  container->_priv->classifications = g_list_remove (container->_priv->classifications, classification);
}

/*
 *
 *
 *
 *
 */
GList*
sim_container_get_classifications_ul (SimContainer  *container)
{
  GList *list;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);

  list = g_list_copy (container->_priv->classifications);

  return list;
}

/*
 *
 *
 *
 *
 */
void
sim_container_set_classifications_ul (SimContainer  *container,
			    GList         *classifications)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (classifications);

  container->_priv->classifications = g_list_concat (container->_priv->classifications, classifications);
}

/*
 *
 *
 *
 *
 */
void
sim_container_free_classifications_ul (SimContainer  *container)
{
  GList *list;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));

  list = container->_priv->classifications;
  while (list)
    {
      SimClassification *classification = (SimClassification *) list->data;
      g_object_unref (classification);

      list = list->next;
    }
  g_list_free (container->_priv->classifications);
  container->_priv->classifications = NULL;
}

/*
 *
 *
 *
 *
 */
SimClassification*
sim_container_get_classification_by_id_ul (SimContainer  *container,
				   gint           id)
{
  SimClassification   *classification;
  GList     *list;
  gboolean   found = FALSE;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);
  g_return_val_if_fail (id > 0, NULL);

  list = container->_priv->classifications;
  while (list)
    {
      classification = (SimClassification *) list->data;

      if (sim_classification_get_id (classification) == id)
	{
	  found = TRUE;
	  break;
	}

      list = list->next;
    }

  if (!found)
    return NULL;

  return classification;
}

/*
 *
 *
 *
 *
 */
SimClassification*
sim_container_get_classification_by_name_ul (SimContainer  *container,
					     const gchar   *name)
{
  SimClassification   *classification;
  GList     *list;
  gboolean   found = FALSE;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);
  g_return_val_if_fail (name, NULL);

  list = container->_priv->classifications;
  while (list)
    {
      classification = (SimClassification *) list->data;

      if (!g_ascii_strcasecmp (sim_classification_get_name (classification), name))
	{
	  found = TRUE;
	  break;
	}

      list = list->next;
    }

  if (!found)
    return NULL;

  return classification;
}


/*
 *
 *
 *
 *
 */
void
sim_container_db_load_classifications (SimContainer  *container,
			     SimDatabase   *database)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));

  G_LOCK (s_mutex_classifications);
  sim_container_db_load_classifications_ul (container, database);
  G_UNLOCK (s_mutex_classifications);
}

/*
 *
 *
 *
 *
 */
void
sim_container_append_classification (SimContainer  *container,
			   SimClassification       *classification)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (classification);
  g_return_if_fail (SIM_IS_CLASSIFICATION (classification));

  G_LOCK (s_mutex_classifications);
  sim_container_append_classification_ul (container, classification);
  G_UNLOCK (s_mutex_classifications);
}

/*
 *
 *
 *
 *
 */
void
sim_container_remove_classification (SimContainer  *container,
			   SimClassification       *classification)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (classification);
  g_return_if_fail (SIM_IS_CLASSIFICATION (classification));

  G_LOCK (s_mutex_classifications);
  sim_container_remove_classification_ul (container, classification);
  G_UNLOCK (s_mutex_classifications);
}

/*
 *
 *
 *
 *
 */
GList*
sim_container_get_classifications (SimContainer  *container)
{
  GList *list;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);

  G_LOCK (s_mutex_classifications);
  list = sim_container_get_classifications_ul (container);
  G_UNLOCK (s_mutex_classifications);

  return list;
}

/*
 *
 *
 *
 *
 */
void
sim_container_set_classifications (SimContainer  *container,
			 GList         *classifications)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (classifications);

  G_LOCK (s_mutex_classifications);
  sim_container_set_classifications_ul (container, classifications);
  G_UNLOCK (s_mutex_classifications);
}

/*
 *
 *
 *
 *
 */
void
sim_container_free_classifications (SimContainer  *container)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));

  G_LOCK (s_mutex_classifications);
  sim_container_free_classifications_ul (container);
  G_UNLOCK (s_mutex_classifications);
}

/*
 *
 *
 *
 *
 */
SimClassification*
sim_container_get_classification_by_id (SimContainer  *container,
				gint           id)
{
  SimClassification   *classification;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);
  g_return_val_if_fail (id > 0, NULL);

  G_LOCK (s_mutex_classifications);
  classification = sim_container_get_classification_by_id_ul (container, id);
  G_UNLOCK (s_mutex_classifications);

  return classification;
}

/*
 *
 *
 *
 *
 */
SimClassification*
sim_container_get_classification_by_name (SimContainer  *container,
					const gchar   *name)
{
  SimClassification   *classification;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);
  g_return_val_if_fail (name, NULL);

  G_LOCK (s_mutex_classifications);
  classification = sim_container_get_classification_by_name_ul (container, name);
  G_UNLOCK (s_mutex_classifications);

  return classification;
}

/*
 *
 *
 *
 *
 */
void
sim_container_db_load_plugins_ul (SimContainer  *container,
				SimDatabase   *database)
{
  SimPlugin       *plugin;
  GdaDataModel  *dm;
  gint           row;
  gchar         *query = "SELECT id, type, name, description FROM plugin";

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));

  dm = sim_database_execute_single_command (database, query);
  if (dm)
    {
      for (row = 0; row < gda_data_model_get_n_rows (dm); row++)
	{
	  plugin  = sim_plugin_new_from_dm (dm, row);
	  container->_priv->plugins = g_list_append (container->_priv->plugins, plugin);
	}

      g_object_unref(dm);
    }
  else
    {
      g_message ("PLUGINS DATA MODEL ERROR");
    }
}

/*
 *
 *
 *
 *
 */
void
sim_container_append_plugin_ul (SimContainer  *container,
			      SimPlugin       *plugin)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (plugin);
  g_return_if_fail (SIM_IS_PLUGIN (plugin));

  container->_priv->plugins = g_list_append (container->_priv->plugins, plugin);
}

/*
 *
 *
 *
 *
 */
void
sim_container_remove_plugin_ul (SimContainer  *container,
			      SimPlugin       *plugin)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (plugin);
  g_return_if_fail (SIM_IS_PLUGIN (plugin));

  container->_priv->plugins = g_list_remove (container->_priv->plugins, plugin);
}

/*
 *
 *
 *
 *
 */
GList*
sim_container_get_plugins_ul (SimContainer  *container)
{
  GList *list;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);

  list = g_list_copy (container->_priv->plugins);

  return list;
}

/*
 *
 *
 *
 *
 */
void
sim_container_set_plugins_ul (SimContainer  *container,
			    GList         *plugins)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (plugins);

  container->_priv->plugins = g_list_concat (container->_priv->plugins, plugins);
}

/*
 *
 *
 *
 *
 */
void
sim_container_free_plugins_ul (SimContainer  *container)
{
  GList *list;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));

  list = container->_priv->plugins;
  while (list)
    {
      SimPlugin *plugin = (SimPlugin *) list->data;
      g_object_unref (plugin);

      list = list->next;
    }
  g_list_free (container->_priv->plugins);
  container->_priv->plugins = NULL;
}

/*
 *
 *
 *
 *
 */
SimPlugin*
sim_container_get_plugin_by_id_ul (SimContainer  *container,
				   gint           id)
{
  SimPlugin   *plugin;
  GList     *list;
  gboolean   found = FALSE;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);
  g_return_val_if_fail (id > 0, NULL);

  list = container->_priv->plugins;
  while (list)
    {
      plugin = (SimPlugin *) list->data;

      if (sim_plugin_get_id (plugin) == id)
	{
	  found = TRUE;
	  break;
	}

      list = list->next;
    }

  if (!found)
    return NULL;

  return plugin;
}

/*
 *
 *
 *
 *
 */
void
sim_container_db_load_plugins (SimContainer  *container,
			     SimDatabase   *database)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));

  G_LOCK (s_mutex_plugins);
  sim_container_db_load_plugins_ul (container, database);
  G_UNLOCK (s_mutex_plugins);
}

/*
 *
 *
 *
 *
 */
void
sim_container_append_plugin (SimContainer  *container,
			   SimPlugin       *plugin)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (plugin);
  g_return_if_fail (SIM_IS_PLUGIN (plugin));

  G_LOCK (s_mutex_plugins);
  sim_container_append_plugin_ul (container, plugin);
  G_UNLOCK (s_mutex_plugins);
}

/*
 *
 *
 *
 *
 */
void
sim_container_remove_plugin (SimContainer  *container,
			   SimPlugin       *plugin)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (plugin);
  g_return_if_fail (SIM_IS_PLUGIN (plugin));

  G_LOCK (s_mutex_plugins);
  sim_container_remove_plugin_ul (container, plugin);
  G_UNLOCK (s_mutex_plugins);
}

/*
 *
 *
 *
 *
 */
GList*
sim_container_get_plugins (SimContainer  *container)
{
  GList *list;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);

  G_LOCK (s_mutex_plugins);
  list = sim_container_get_plugins_ul (container);
  G_UNLOCK (s_mutex_plugins);

  return list;
}

/*
 *
 *
 *
 *
 */
void
sim_container_set_plugins (SimContainer  *container,
			   GList         *plugins)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (plugins);

  G_LOCK (s_mutex_plugins);
  sim_container_set_plugins_ul (container, plugins);
  G_UNLOCK (s_mutex_plugins);
}

/*
 *
 *
 *
 *
 */
void
sim_container_free_plugins (SimContainer  *container)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));

  G_LOCK (s_mutex_plugins);
  sim_container_free_plugins_ul (container);
  G_UNLOCK (s_mutex_plugins);
}

/*
 *
 *
 *
 *
 */
SimPlugin*
sim_container_get_plugin_by_id (SimContainer  *container,
				gint           id)
{
  SimPlugin   *plugin;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);
  g_return_val_if_fail (id > 0, NULL);

  G_LOCK (s_mutex_plugins);
  plugin = sim_container_get_plugin_by_id_ul (container, id);
  G_UNLOCK (s_mutex_plugins);

  return plugin;
}

/*
 *
 *
 *
 *
 */
void
sim_container_db_load_plugin_sids_ul (SimContainer  *container,
				      SimDatabase   *database)
{
  SimPluginSid  *plugin_sid;
  GdaDataModel  *dm;
  gint           row;
  gchar         *query = "SELECT plugin_id, sid, category_id, class_id, reliability, priority, name FROM plugin_sid";

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));

  dm = sim_database_execute_single_command (database, query);
  if (dm)
    {
      for (row = 0; row < gda_data_model_get_n_rows (dm); row++)
	{
	  plugin_sid  = sim_plugin_sid_new_from_dm (dm, row);
	  container->_priv->plugin_sids = g_list_append (container->_priv->plugin_sids, plugin_sid);
	}

      g_object_unref(dm);
    }
  else
    {
      g_message ("PLUGINS DATA MODEL ERROR");
    }
}

/*
 *
 *
 *
 *
 */
void
sim_container_append_plugin_sid_ul (SimContainer  *container,
				    SimPluginSid  *plugin_sid)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (plugin_sid);
  g_return_if_fail (SIM_IS_PLUGIN_SID (plugin_sid));

  container->_priv->plugin_sids = g_list_append (container->_priv->plugin_sids, plugin_sid);
}

/*
 *
 *
 *
 *
 */
void
sim_container_remove_plugin_sid_ul (SimContainer  *container,
				    SimPluginSid  *plugin_sid)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (plugin_sid);
  g_return_if_fail (SIM_IS_PLUGIN_SID (plugin_sid));

  container->_priv->plugin_sids = g_list_remove (container->_priv->plugin_sids, plugin_sid);
}

/*
 *
 *
 *
 *
 */
GList*
sim_container_get_plugin_sids_ul (SimContainer  *container)
{
  GList *list;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);

  list = g_list_copy (container->_priv->plugin_sids);

  return list;
}

/*
 *
 *
 *
 *
 */
void
sim_container_set_plugin_sids_ul (SimContainer  *container,
				  GList         *plugin_sids)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (plugin_sids);

  container->_priv->plugin_sids = g_list_concat (container->_priv->plugin_sids, plugin_sids);
}

/*
 *
 *
 *
 *
 */
void
sim_container_free_plugin_sids_ul (SimContainer  *container)
{
  GList *list;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));

  list = container->_priv->plugin_sids;
  while (list)
    {
      SimPluginSid *plugin_sid = (SimPluginSid *) list->data;
      g_object_unref (plugin_sid);

      list = list->next;
    }
  g_list_free (container->_priv->plugin_sids);
  container->_priv->plugin_sids = NULL;
}

/*
 *
 *
 *
 *
 */
SimPluginSid*
sim_container_get_plugin_sid_by_pky_ul (SimContainer  *container,
					 gint           plugin_id,
					 gint           sid)
{
  SimPluginSid   *plugin_sid;
  GList     *list;
  gboolean   found = FALSE;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);
  g_return_val_if_fail (plugin_id > 0, NULL);
  g_return_val_if_fail (sid > 0, NULL);

  list = container->_priv->plugin_sids;
  while (list)
    {
      plugin_sid = (SimPluginSid *) list->data;

      if ((sim_plugin_sid_get_plugin_id (plugin_sid) == plugin_id) && 
	  (sim_plugin_sid_get_sid (plugin_sid) == sid))
	{
	  found = TRUE;
	  break;
	}

      list = list->next;
    }

  if (!found)
    return NULL;

  return plugin_sid;
}

/*
 *
 *
 *
 *
 */
SimPluginSid*
sim_container_get_plugin_sid_by_name_ul (SimContainer  *container,
					 gint           plugin_id,
					 const gchar   *name)
{
  SimPluginSid   *plugin_sid;
  GList     *list;
  gboolean   found = FALSE;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);
  g_return_val_if_fail (plugin_id > 0, NULL);
  g_return_val_if_fail (name, NULL);

  list = container->_priv->plugin_sids;
  while (list)
    {
      plugin_sid = (SimPluginSid *) list->data;

      if ((sim_plugin_sid_get_plugin_id (plugin_sid) == plugin_id) && 
	  (!strcmp (name, sim_plugin_sid_get_name (plugin_sid))))
	{
	  found = TRUE;
	  break;
	}

      list = list->next;
    }

  if (!found)
    return NULL;

  return plugin_sid;
}

/*
 *
 *
 *
 *
 */
void
sim_container_db_load_plugin_sids (SimContainer  *container,
				   SimDatabase   *database)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));

  G_LOCK (s_mutex_plugin_sids);
  sim_container_db_load_plugin_sids_ul (container, database);
  G_UNLOCK (s_mutex_plugin_sids);
}

/*
 *
 *
 *
 *
 */
void
sim_container_append_plugin_sid (SimContainer  *container,
				 SimPluginSid  *plugin_sid)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (plugin_sid);
  g_return_if_fail (SIM_IS_PLUGIN_SID (plugin_sid));

  G_LOCK (s_mutex_plugin_sids);
  sim_container_append_plugin_sid_ul (container, plugin_sid);
  G_UNLOCK (s_mutex_plugin_sids);
}

/*
 *
 *
 *
 *
 */
void
sim_container_remove_plugin_sid (SimContainer  *container,
				 SimPluginSid  *plugin_sid)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (plugin_sid);
  g_return_if_fail (SIM_IS_PLUGIN_SID (plugin_sid));

  G_LOCK (s_mutex_plugin_sids);
  sim_container_remove_plugin_sid_ul (container, plugin_sid);
  G_UNLOCK (s_mutex_plugin_sids);
}

/*
 *
 *
 *
 *
 */
GList*
sim_container_get_plugin_sids (SimContainer  *container)
{
  GList *list;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);

  G_LOCK (s_mutex_plugin_sids);
  list = sim_container_get_plugin_sids_ul (container);
  G_UNLOCK (s_mutex_plugin_sids);

  return list;
}

/*
 *
 *
 *
 *
 */
void
sim_container_set_plugin_sids (SimContainer  *container,
			       GList         *plugin_sids)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (plugin_sids);

  G_LOCK (s_mutex_plugin_sids);
  sim_container_set_plugin_sids_ul (container, plugin_sids);
  G_UNLOCK (s_mutex_plugin_sids);
}

/*
 *
 *
 *
 *
 */
void
sim_container_free_plugin_sids (SimContainer  *container)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));

  G_LOCK (s_mutex_plugin_sids);
  sim_container_free_plugin_sids_ul (container);
  G_UNLOCK (s_mutex_plugin_sids);
}

/*
 *
 *
 *
 *
 */
SimPluginSid*
sim_container_get_plugin_sid_by_pky (SimContainer  *container,
				      gint           plugin_id,
				      gint            sid)
{
  SimPluginSid   *plugin_sid;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);
  g_return_val_if_fail (plugin_id > 0, NULL);
  g_return_val_if_fail (sid > 0, NULL);

  G_LOCK (s_mutex_plugin_sids);
  plugin_sid = sim_container_get_plugin_sid_by_pky_ul (container, plugin_id, sid);
  G_UNLOCK (s_mutex_plugin_sids);

  return plugin_sid;
}

/*
 *
 *
 *
 *
 */
SimPluginSid*
sim_container_get_plugin_sid_by_name (SimContainer  *container,
				      gint           plugin_id,
				      const gchar   *name)
{
  SimPluginSid   *plugin_sid;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);
  g_return_val_if_fail (plugin_id > 0, NULL);
  g_return_val_if_fail (name, NULL);

  G_LOCK (s_mutex_plugin_sids);
  plugin_sid = sim_container_get_plugin_sid_by_name_ul (container, plugin_id, name);
  G_UNLOCK (s_mutex_plugin_sids);

  return plugin_sid;
}

/*
 *
 *
 *
 *
 */
void
sim_container_db_load_sensors_ul (SimContainer  *container,
				  SimDatabase   *database)
{
  SimSensor     *sensor;
  GdaDataModel  *dm;
  gint           row;
  gchar         *query = "SELECT name, ip, port, connect FROM sensor";

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));

  dm = sim_database_execute_single_command (database, query);
  if (dm)
    {
      for (row = 0; row < gda_data_model_get_n_rows (dm); row++)
	{
	  sensor  = sim_sensor_new_from_dm (dm, row);
	  container->_priv->sensors = g_list_append (container->_priv->sensors, sensor);
	}

      g_object_unref(dm);
    }
  else
    {
      g_message ("SENSORS DATA MODEL ERROR");
    }
}

/*
 *
 *
 *
 *
 */
void
sim_container_append_sensor_ul (SimContainer  *container,
			      SimSensor       *sensor)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (sensor);
  g_return_if_fail (SIM_IS_SENSOR (sensor));

  container->_priv->sensors = g_list_append (container->_priv->sensors, sensor);
}

/*
 *
 *
 *
 *
 */
void
sim_container_remove_sensor_ul (SimContainer  *container,
			      SimSensor       *sensor)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (sensor);
  g_return_if_fail (SIM_IS_SENSOR (sensor));

  container->_priv->sensors = g_list_remove (container->_priv->sensors, sensor);
}

/*
 *
 *
 *
 *
 */
GList*
sim_container_get_sensors_ul (SimContainer  *container)
{
  GList *list;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);

  list = g_list_copy (container->_priv->sensors);

  return list;
}

/*
 *
 *
 *
 *
 */
void
sim_container_set_sensors_ul (SimContainer  *container,
			    GList         *sensors)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (sensors);

  container->_priv->sensors = g_list_concat (container->_priv->sensors, sensors);
}

/*
 *
 *
 *
 *
 */
void
sim_container_free_sensors_ul (SimContainer  *container)
{
  GList *list;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));

  list = container->_priv->sensors;
  while (list)
    {
      SimSensor *sensor = (SimSensor *) list->data;
      g_object_unref (sensor);

      list = list->next;
    }
  g_list_free (container->_priv->sensors);
  container->_priv->sensors = NULL;
}


/*
 *
 *
 *
 *
 */
SimSensor*
sim_container_get_sensor_by_name_ul (SimContainer  *container,
				     gchar         *name)
{
  SimSensor   *sensor;
  GList     *list;
  gboolean   found = FALSE;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);
  g_return_val_if_fail (name, NULL);

  list = container->_priv->sensors;
  while (list)
    {
      sensor = (SimSensor *) list->data;

      if (!g_ascii_strcasecmp (sim_sensor_get_name (sensor), name))
	{
	  found = TRUE;
	  break;
	}

      list = list->next;
    }

  if (!found)
    return NULL;

  return sensor;
}

/*
 *
 *
 *
 *
 */
void
sim_container_db_load_sensors (SimContainer  *container,
			     SimDatabase   *database)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));

  G_LOCK (s_mutex_sensors);
  sim_container_db_load_sensors_ul (container, database);
  G_UNLOCK (s_mutex_sensors);
}

/*
 *
 *
 *
 *
 */
void
sim_container_append_sensor (SimContainer  *container,
			   SimSensor       *sensor)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (sensor);
  g_return_if_fail (SIM_IS_SENSOR (sensor));

  G_LOCK (s_mutex_sensors);
  sim_container_append_sensor_ul (container, sensor);
  G_UNLOCK (s_mutex_sensors);
}

/*
 *
 *
 *
 *
 */
void
sim_container_remove_sensor (SimContainer  *container,
			   SimSensor       *sensor)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (sensor);
  g_return_if_fail (SIM_IS_SENSOR (sensor));

  G_LOCK (s_mutex_sensors);
  sim_container_remove_sensor_ul (container, sensor);
  G_UNLOCK (s_mutex_sensors);
}

/*
 *
 *
 *
 *
 */
GList*
sim_container_get_sensors (SimContainer  *container)
{
  GList *list;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);

  G_LOCK (s_mutex_sensors);
  list = sim_container_get_sensors_ul (container);
  G_UNLOCK (s_mutex_sensors);

  return list;
}

/*
 *
 *
 *
 *
 */
void
sim_container_set_sensors (SimContainer  *container,
			 GList         *sensors)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (sensors);

  G_LOCK (s_mutex_sensors);
  sim_container_set_sensors_ul (container, sensors);
  G_UNLOCK (s_mutex_sensors);
}

/*
 *
 *
 *
 *
 */
void
sim_container_free_sensors (SimContainer  *container)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));

  G_LOCK (s_mutex_sensors);
  sim_container_free_sensors_ul (container);
  G_UNLOCK (s_mutex_sensors);
}

/*
 *
 *
 *
 *
 */
SimSensor*
sim_container_get_sensor_by_name (SimContainer  *container,
				  gchar         *name)
{
  SimSensor   *sensor;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);
  g_return_val_if_fail (name, NULL);

  G_LOCK (s_mutex_sensors);
  sensor = sim_container_get_sensor_by_name_ul (container, name);
  G_UNLOCK (s_mutex_sensors);

  return sensor;
}

/*
 *
 *
 *
 *
 */
void
sim_container_db_load_hosts_ul (SimContainer  *container,
				SimDatabase   *database)
{
  SimHost       *host;
  GdaDataModel  *dm;
  gint           row;
  gchar         *query = "SELECT * FROM host";

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));

  dm = sim_database_execute_single_command (database, query);
  if (dm)
    {
      for (row = 0; row < gda_data_model_get_n_rows (dm); row++)
	{
	  host  = sim_host_new_from_dm (dm, row);
	  container->_priv->hosts = g_list_append (container->_priv->hosts, host);
	}

      g_object_unref(dm);
    }
  else
    {
      g_message ("HOSTS DATA MODEL ERROR");
    }
}

/*
 *
 *
 *
 *
 */
void
sim_container_append_host_ul (SimContainer  *container,
			      SimHost       *host)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (host);
  g_return_if_fail (SIM_IS_HOST (host));

  container->_priv->hosts = g_list_append (container->_priv->hosts, host);
}

/*
 *
 *
 *
 *
 */
void
sim_container_remove_host_ul (SimContainer  *container,
			      SimHost       *host)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (host);
  g_return_if_fail (SIM_IS_HOST (host));

  container->_priv->hosts = g_list_remove (container->_priv->hosts, host);
}

/*
 *
 *
 *
 *
 */
GList*
sim_container_get_hosts_ul (SimContainer  *container)
{
  GList *list;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);

  list = g_list_copy (container->_priv->hosts);

  return list;
}

/*
 *
 *
 *
 *
 */
void
sim_container_set_hosts_ul (SimContainer  *container,
			    GList         *hosts)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (hosts);

  container->_priv->hosts = g_list_concat (container->_priv->hosts, hosts);
}

/*
 *
 *
 *
 *
 */
void
sim_container_free_hosts_ul (SimContainer  *container)
{
  GList *list;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));

  list = container->_priv->hosts;
  while (list)
    {
      SimHost *host = (SimHost *) list->data;
      g_object_unref (host);

      list = list->next;
    }
  g_list_free (container->_priv->hosts);
  container->_priv->hosts = NULL;
}

/*
 *
 *
 *
 *
 */
SimHost*
sim_container_get_host_by_ia_ul (SimContainer  *container,
				 GInetAddr     *ia)
{
  SimHost   *host;
  GList     *list;
  gboolean   found = FALSE;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);
  g_return_val_if_fail (ia, NULL);

  list = container->_priv->hosts;
  while (list)
    {
      host = (SimHost *) list->data;

      if (gnet_inetaddr_noport_equal (sim_host_get_ia (host), ia))
	{
	  found = TRUE;
	  break;
	}

      list = list->next;
    }

  if (!found)
    return NULL;

  return host;
}

/*
 *
 *
 *
 *
 */
void
sim_container_db_load_hosts (SimContainer  *container,
			     SimDatabase   *database)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));

  G_LOCK (s_mutex_hosts);
  sim_container_db_load_hosts_ul (container, database);
  G_UNLOCK (s_mutex_hosts);
}

/*
 *
 *
 *
 *
 */
void
sim_container_append_host (SimContainer  *container,
			   SimHost       *host)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (host);
  g_return_if_fail (SIM_IS_HOST (host));

  G_LOCK (s_mutex_hosts);
  sim_container_append_host_ul (container, host);
  G_UNLOCK (s_mutex_hosts);
}

/*
 *
 *
 *
 *
 */
void
sim_container_remove_host (SimContainer  *container,
			   SimHost       *host)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (host);
  g_return_if_fail (SIM_IS_HOST (host));

  G_LOCK (s_mutex_hosts);
  sim_container_remove_host_ul (container, host);
  G_UNLOCK (s_mutex_hosts);
}

/*
 *
 *
 *
 *
 */
GList*
sim_container_get_hosts (SimContainer  *container)
{
  GList *list;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);

  G_LOCK (s_mutex_hosts);
  list = sim_container_get_hosts_ul (container);
  G_UNLOCK (s_mutex_hosts);

  return list;
}

/*
 *
 *
 *
 *
 */
void
sim_container_set_hosts (SimContainer  *container,
			 GList         *hosts)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (hosts);

  G_LOCK (s_mutex_hosts);
  sim_container_set_hosts_ul (container, hosts);
  G_UNLOCK (s_mutex_hosts);
}

/*
 *
 *
 *
 *
 */
void
sim_container_free_hosts (SimContainer  *container)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));

  G_LOCK (s_mutex_hosts);
  sim_container_free_hosts_ul (container);
  G_UNLOCK (s_mutex_hosts);
}

/*
 *
 *
 *
 *
 */
SimHost*
sim_container_get_host_by_ia (SimContainer  *container,
			      GInetAddr     *ia)
{
  SimHost   *host;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);
  g_return_val_if_fail (ia, NULL);

  G_LOCK (s_mutex_hosts);
  host = sim_container_get_host_by_ia_ul (container, ia);
  G_UNLOCK (s_mutex_hosts);

  return host;
}

/*
 *
 *
 *
 *
 */
void
sim_container_db_load_nets_ul (SimContainer  *container,
			       SimDatabase   *database)
{
  SimNet        *net;
  GdaDataModel  *dm;
  GdaDataModel  *dm2;
  GdaValue      *value;
  GInetAddr     *ia;
  gint           row;
  gint           row2;
  gchar         *query = "SELECT * FROM net";
  gchar         *query2;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));

  dm = sim_database_execute_single_command (database, query);
  if (dm)
    {
      for (row = 0; row < gda_data_model_get_n_rows (dm); row++)
	{
	  net  = sim_net_new_from_dm (dm, row);

	  query2 = g_strdup_printf ("SELECT host_ip FROM net_host_reference WHERE net_name = '%s'",
				    sim_net_get_name (net));

	  dm2 = sim_database_execute_single_command (database, query2);
	  if (dm2)
	    {
	      for (row2 = 0; row2 < gda_data_model_get_n_rows (dm2); row2++)
		{
		  gchar *ip;

		  value = (GdaValue *) gda_data_model_get_value_at (dm2, 0, row2);
		  ip = gda_value_stringify (value);

		  ia = gnet_inetaddr_new_nonblock (ip, 0);
		  sim_net_append_ia (net, ia);

		  g_free (ip);
		}
	      g_object_unref(dm2);
	    }
	  else
	    {
	      g_message ("NET HOST REFERENCES DATA MODEL ERROR");
	    }

	  g_free (query2);

	  container->_priv->nets = g_list_append (container->_priv->nets, net);
	}
      
      g_object_unref(dm);
    }
  else
    {
      g_message ("NETS DATA MODEL ERROR");
    }
}

/*
 *
 *
 *
 *
 */
void
sim_container_append_net_ul (SimContainer  *container,
			     SimNet        *net)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (net);
  g_return_if_fail (SIM_IS_NET (net));

  container->_priv->nets = g_list_append (container->_priv->nets, net);
}

/*
 *
 *
 *
 *
 */
void
sim_container_remove_net_ul (SimContainer  *container,
			     SimNet        *net)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (net);
  g_return_if_fail (SIM_IS_NET (net));

  container->_priv->nets = g_list_remove (container->_priv->nets, net);
}

/*
 *
 *
 *
 *
 */
GList*
sim_container_get_nets_ul (SimContainer  *container)
{
  GList *list;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);

  list = g_list_copy (container->_priv->nets);

  return list;
}

/*
 *
 *
 *
 *
 */
void
sim_container_set_nets_ul (SimContainer  *container,
			   GList         *nets)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (nets);

  container->_priv->nets = g_list_concat (container->_priv->nets, nets);
}

/*
 *
 *
 *
 *
 */
void
sim_container_free_nets_ul (SimContainer  *container)
{
  GList *list;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));

  list = container->_priv->nets;
  while (list)
    {
      SimNet *net = (SimNet *) list->data;
      g_object_unref (net);

      list = list->next;
    }
  g_list_free (container->_priv->nets);
  container->_priv->nets = NULL; 
}

/*
 *
 *
 *
 *
 */
GList*
sim_container_get_nets_has_ia_ul (SimContainer  *container,
				  GInetAddr     *ia)
{
  GList *list;
  GList *nets = NULL;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);
  g_return_val_if_fail (ia, NULL);

  list = container->_priv->nets;
  while (list)
    {
      SimNet *net = (SimNet *) list->data;

      if (sim_net_has_ia (net, ia))
	{
	  nets = g_list_append (nets, net);
	}

      list = list->next;
    }

  return nets;
}

/*
 *
 *
 *
 *
 */
SimNet*
sim_container_get_net_by_name_ul (SimContainer  *container,
				  const gchar   *name)
{
  SimNet    *net;
  GList     *list;
  gboolean   found = FALSE;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);
  g_return_val_if_fail (name, NULL);

  list = container->_priv->nets;
  while (list)
    {
      net = (SimNet *) list->data;

      if (!strcmp (sim_net_get_name (net), name))
	{
	  found = TRUE;
	  break;
	}

      list = list->next;
    }

  if (!found)
    return NULL;

  return net;
}

/*
 *
 *
 *
 *
 */
void
sim_container_db_load_nets (SimContainer  *container,
			    SimDatabase   *database)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));

  G_LOCK (s_mutex_nets);
  sim_container_db_load_nets_ul (container, database);
  G_UNLOCK (s_mutex_nets);
}

/*
 *
 *
 *
 *
 */
void
sim_container_append_net (SimContainer  *container,
			  SimNet        *net)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (net);
  g_return_if_fail (SIM_IS_NET (net));

  G_LOCK (s_mutex_nets);
  sim_container_append_net_ul (container, net);
  G_UNLOCK (s_mutex_nets);
}

/*
 *
 *
 *
 *
 */
void
sim_container_remove_net (SimContainer  *container,
			  SimNet        *net)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (net);
  g_return_if_fail (SIM_IS_NET (net));

  G_LOCK (s_mutex_nets);
  sim_container_remove_net_ul (container, net);
  G_UNLOCK (s_mutex_nets);
}

/*
 *
 *
 *
 *
 */
GList*
sim_container_get_nets (SimContainer  *container)
{
  GList *list;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);

  G_LOCK (s_mutex_nets);
  list = sim_container_get_nets_ul (container);
  G_UNLOCK (s_mutex_nets);

  return list;
}

/*
 *
 *
 *
 *
 */
void
sim_container_set_nets (SimContainer  *container,
			GList         *nets)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (nets);

  G_LOCK (s_mutex_nets);
  sim_container_set_nets_ul (container, nets);
  G_UNLOCK (s_mutex_nets);
}

/*
 *
 *
 *
 *
 */
void
sim_container_free_nets (SimContainer  *container)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));

  G_LOCK (s_mutex_nets);
  sim_container_free_nets_ul (container);
  G_UNLOCK (s_mutex_nets);
}

/*
 *
 *
 *
 *
 */
GList*
sim_container_get_nets_has_ia (SimContainer  *container,
			       GInetAddr     *ia)
{
  GList *list;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);
  g_return_val_if_fail (ia, NULL);

  G_LOCK (s_mutex_nets);
  list = sim_container_get_nets_has_ia_ul (container, ia);
  G_UNLOCK (s_mutex_nets);

  return list;
}

/*
 *
 *
 *
 *
 */
SimNet*
sim_container_get_net_by_name (SimContainer  *container,
			       const gchar   *name)
{
  SimNet    *net;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);
  g_return_val_if_fail (name, NULL);

  G_LOCK (s_mutex_nets);
  net = sim_container_get_net_by_name_ul (container, name);
  G_UNLOCK (s_mutex_nets);

  return net;
}

/*
 *
 *
 *
 *
 */
void
sim_container_db_load_policies_ul (SimContainer  *container,
				   SimDatabase   *database)
{
  SimPolicy     *policy;
  GdaDataModel  *dm;
  GdaDataModel  *dm2;
  GdaValue      *value;
  GInetAddr     *ia;
  gint           row;
  gint           row2;
  gchar         *query = "SELECT policy.id, policy.priority, policy.descr, policy_time.begin_hour, policy_time.end_hour, policy_time.begin_day, policy_time.end_day FROM policy, policy_time WHERE policy.id = policy_time.policy_id;";
  gchar         *query2;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));

  dm = sim_database_execute_single_command (database, query);
  if (dm)
    {
      for (row = 0; row < gda_data_model_get_n_rows (dm); row++)
	{
	  policy  = sim_policy_new_from_dm (dm, row);

	  /* Host Source Inet Address */
	  query2 = g_strdup_printf ("SELECT host_ip FROM  policy_host_reference WHERE policy_id = %d AND direction = 'source'",
				    sim_policy_get_id (policy));
	  dm2 = sim_database_execute_single_command (database, query2);
	  if (dm2)
	    {
	      for (row2 = 0; row2 < gda_data_model_get_n_rows (dm2); row2++)
		{
		  gchar *src_ip;

		  value = (GdaValue *) gda_data_model_get_value_at (dm2, 0, row2);
		  src_ip = gda_value_stringify (value);

		  if (!g_ascii_strncasecmp (src_ip, SIM_IN_ADDR_ANY_CONST, 3))
		    ia = gnet_inetaddr_new_nonblock (SIM_IN_ADDR_ANY_IP_STR, 0);
		  else
		    ia = gnet_inetaddr_new_nonblock (src_ip, 0);

		  sim_policy_append_src_ia (policy, ia);

		  g_free (src_ip);
		}
	      g_object_unref(dm2);
	    }
	  else
	    {
	      g_message ("POLICY HOST SOURCE REFERENCES DATA MODEL ERROR");
	    }
	  g_free (query2);

	  /* Host Destination Inet Address */
	  query2 = g_strdup_printf ("SELECT host_ip FROM  policy_host_reference WHERE policy_id = %d AND direction = 'dest'",
				    sim_policy_get_id (policy));
	  dm2 = sim_database_execute_single_command (database, query2);
	  if (dm2)
	    {
	      for (row2 = 0; row2 < gda_data_model_get_n_rows (dm2); row2++)
		{
		  gchar *dst_ip;

		  value = (GdaValue *) gda_data_model_get_value_at (dm2, 0, row2);
		  dst_ip = gda_value_stringify (value);

		  if (!g_ascii_strncasecmp (dst_ip, SIM_IN_ADDR_ANY_CONST, 3))
		    ia = gnet_inetaddr_new_nonblock (SIM_IN_ADDR_ANY_IP_STR, 0);
		  else
		    ia = gnet_inetaddr_new_nonblock (dst_ip, 0);

		  sim_policy_append_dst_ia (policy, ia);

		  g_free (dst_ip);
		}
	      g_object_unref(dm2);
	    }
	  else
	    {
	      g_message ("POLICY HOST DEST REFERENCES DATA MODEL ERROR");
	    }
	  g_free (query2);


	  /* Net Source Inet Address */
	  query2 = g_strdup_printf ("SELECT host_ip FROM policy_net_reference, net_host_reference WHERE policy_net_reference.net_name = net_host_reference.net_name AND policy_net_reference.direction = 'source' AND policy_id = %d",
				    sim_policy_get_id (policy));
	  dm2 = sim_database_execute_single_command (database, query2);
	  if (dm2)
	    {
	      for (row2 = 0; row2 < gda_data_model_get_n_rows (dm2); row2++)
		{
		  gchar *src_ip;

		  value = (GdaValue *) gda_data_model_get_value_at (dm2, 0, row2);
		  src_ip = gda_value_stringify (value);

		  if (!g_ascii_strncasecmp (src_ip, SIM_IN_ADDR_ANY_CONST, 3))
		    ia = gnet_inetaddr_new_nonblock (SIM_IN_ADDR_ANY_IP_STR, 0);
		  else
		    ia = gnet_inetaddr_new_nonblock (src_ip, 0);

		  sim_policy_append_src_ia (policy, ia);

		  g_free (src_ip);
		}
	      g_object_unref(dm2);
	    }
	  else
	    {
	      g_message ("POLICY NET SOURCE REFERENCES DATA MODEL ERROR");
	    }
	  g_free (query2);


	  /* Net Destination Inet Address */
	  query2 = g_strdup_printf ("SELECT host_ip FROM policy_net_reference, net_host_reference WHERE policy_net_reference.net_name = net_host_reference.net_name AND policy_net_reference.direction = 'dest' AND policy_id = %d",
				    sim_policy_get_id (policy));
	  dm2 = sim_database_execute_single_command (database, query2);
	  if (dm2)
	    {
	      for (row2 = 0; row2 < gda_data_model_get_n_rows (dm2); row2++)
		{
		  gchar *dst_ip;

		  value = (GdaValue *) gda_data_model_get_value_at (dm2, 0, row2);
		  dst_ip = gda_value_stringify (value);

		  if (!g_ascii_strncasecmp (dst_ip, SIM_IN_ADDR_ANY_CONST, 3))
		    ia = gnet_inetaddr_new_nonblock (SIM_IN_ADDR_ANY_IP_STR, 0);
		  else
		    ia = gnet_inetaddr_new_nonblock (dst_ip, 0);

		  sim_policy_append_dst_ia (policy, ia);

		  g_free (dst_ip);
		}
	      g_object_unref(dm2);
	    }
	  else
	    {
	      g_message ("POLICY NET DEST REFERENCES DATA MODEL ERROR");
	    }
	  g_free (query2);

	  /* Ports */
	  query2 = g_strdup_printf ("SELECT port_number, protocol_name  FROM policy_port_reference, port_group_reference WHERE policy_port_reference.port_group_name = port_group_reference.port_group_name AND policy_port_reference.policy_id = %d",
				    sim_policy_get_id (policy));
	  dm2 = sim_database_execute_single_command (database, query2);
	  if (dm2)
	    {
	      for (row2 = 0; row2 < gda_data_model_get_n_rows (dm2); row2++)
		{
		  SimPortProtocol  *pp;
		  SimProtocolType   proto_type;
		  gint              port_num;
		  gchar            *proto_name;

		  value = (GdaValue *) gda_data_model_get_value_at (dm2, 0, row2);
		  port_num = gda_value_get_integer (value);
		  value = (GdaValue *) gda_data_model_get_value_at (dm2, 1, row2);
		  proto_name = gda_value_stringify (value);

		  proto_type = sim_protocol_get_type_from_str (proto_name);

		  pp = sim_port_protocol_new (port_num, proto_type);

		  sim_policy_append_port (policy, pp);
		  g_free (proto_name);
		}
	      g_object_unref(dm2);
	    }
	  else
	    {
	      g_message ("POLICY CATEGORY REFERENCES DATA MODEL ERROR");
	    }
	  g_free (query2);

	  /* Categories */
	  query2 = g_strdup_printf ("SELECT sig_name FROM policy_sig_reference, signature_group_reference WHERE policy_sig_reference.sig_group_name = signature_group_reference.sig_group_name AND policy_sig_reference.policy_id = %d",
				    sim_policy_get_id (policy));
	  dm2 = sim_database_execute_single_command (database, query2);
	  if (dm2)
	    {
	      for (row2 = 0; row2 < gda_data_model_get_n_rows (dm2); row2++)
		{
		  gchar *category;

		  value = (GdaValue *) gda_data_model_get_value_at (dm2, 0, row2);
		  category = gda_value_stringify (value);

		  sim_policy_append_category (policy, category);
		}
	      g_object_unref(dm2);
	    }
	  else
	    {
	      g_message ("POLICY CATEGORY REFERENCES DATA MODEL ERROR");
	    }
	  g_free (query2);

	  container->_priv->policies = g_list_append (container->_priv->policies, policy);
	}
      g_object_unref(dm);
    }
  else
    {
      g_message ("POLICY DATA MODEL ERROR");
    }
}

/*
 *
 *
 *
 *
 */
void
sim_container_append_policy_ul (SimContainer  *container,
				SimPolicy       *policy)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (policy);
  g_return_if_fail (SIM_IS_POLICY (policy));

  container->_priv->policies = g_list_append (container->_priv->policies, policy);
}

/*
 *
 *
 *
 *
 */
void
sim_container_remove_policy_ul (SimContainer  *container,
				SimPolicy       *policy)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (policy);
  g_return_if_fail (SIM_IS_POLICY (policy));

  container->_priv->policies = g_list_remove (container->_priv->policies, policy);
}

/*
 *
 *
 *
 *
 */
GList*
sim_container_get_policies_ul (SimContainer  *container)
{
  GList *list;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);

  list = g_list_copy (container->_priv->policies);

  return list;
}

/*
 *
 *
 *
 *
 */
void
sim_container_set_policies_ul (SimContainer  *container,
			       GList         *policies)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (policies);

  container->_priv->policies = g_list_concat (container->_priv->policies, policies);
}

/*
 *
 *
 *
 *
 */
void
sim_container_free_policies_ul (SimContainer  *container)
{
  GList *list;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));

  list = container->_priv->policies;
  while (list)
    {
      SimPolicy *policy = (SimPolicy *) list->data;
      g_object_unref (policy);

      list = list->next;
    }
  g_list_free (container->_priv->policies);
  container->_priv->policies = NULL;
}

/*
 *
 *
 *
 */
SimPolicy*
sim_container_get_policy_match_ul (SimContainer     *container,
				   gint              date,
				   GInetAddr        *src_ip,
				   GInetAddr        *dst_ip,
				   SimPortProtocol  *port,
				   const gchar      *category)
{
  SimPolicy  *policy;
  GList      *list;
  gboolean    found = FALSE;

  g_return_val_if_fail (container != NULL, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);
  g_return_val_if_fail (src_ip != NULL, NULL);
  g_return_val_if_fail (dst_ip != NULL, NULL);
  g_return_val_if_fail (port != NULL, NULL);
  g_return_val_if_fail (category != NULL, NULL);

  list = container->_priv->policies;
  while (list)
    {
      policy = (SimPolicy *) list->data;

      if (sim_policy_match (policy, date, src_ip, dst_ip, port, category))
	{
	  found = TRUE;
	  break;
	}

      list = list->next;
    }

  if (!found)
    return FALSE;

  return policy;
}

/*
 *
 *
 *
 *
 */
void
sim_container_db_load_policies (SimContainer  *container,
				SimDatabase   *database)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));

  G_LOCK (s_mutex_policies);
  sim_container_db_load_policies_ul (container, database);
  G_UNLOCK (s_mutex_policies);
}

/*
 *
 *
 *
 *
 */
void
sim_container_append_policy (SimContainer  *container,
			   SimPolicy       *policy)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (policy);
  g_return_if_fail (SIM_IS_POLICY (policy));

  G_LOCK (s_mutex_policies);
  sim_container_append_policy_ul (container, policy);
  G_UNLOCK (s_mutex_policies);
}

/*
 *
 *
 *
 *
 */
void
sim_container_remove_policy (SimContainer  *container,
			   SimPolicy       *policy)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (policy);
  g_return_if_fail (SIM_IS_POLICY (policy));

  G_LOCK (s_mutex_policies);
  sim_container_remove_policy_ul (container, policy);
  G_UNLOCK (s_mutex_policies);
}

/*
 *
 *
 *
 *
 */
GList*
sim_container_get_policies (SimContainer  *container)
{
  GList *list;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);

  G_LOCK (s_mutex_policies);
  list = sim_container_get_policies_ul (container);
  G_UNLOCK (s_mutex_policies);

  return list;
}

/*
 *
 *
 *
 *
 */
void
sim_container_set_policies (SimContainer  *container,
			    GList         *policies)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (policies);

  G_LOCK (s_mutex_policies);
  sim_container_set_policies_ul (container, policies);
  G_UNLOCK (s_mutex_policies);
}

/*
 *
 *
 *
 *
 */
void
sim_container_free_policies (SimContainer  *container)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));

  G_LOCK (s_mutex_policies);
  sim_container_free_policies_ul (container);
  G_UNLOCK (s_mutex_policies);
}

/*
 *
 *
 *
 */
SimPolicy*
sim_container_get_policy_match (SimContainer     *container,
				gint              date,
				GInetAddr        *src_ip,
				GInetAddr        *dst_ip,
				SimPortProtocol  *port,
				const gchar      *category)
{
  SimPolicy  *policy;

  g_return_val_if_fail (container != NULL, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);
  g_return_val_if_fail (src_ip != NULL, NULL);
  g_return_val_if_fail (dst_ip != NULL, NULL);
  g_return_val_if_fail (port != NULL, NULL);
  g_return_val_if_fail (category != NULL, NULL);

  G_LOCK (s_mutex_policies);
  policy = sim_container_get_policy_match_ul (container, date, src_ip, dst_ip, port, category);
  G_UNLOCK (s_mutex_policies);

  return policy;
}

/*
 *
 *
 *
 *
 */
void
sim_container_load_directives_from_file_ul (SimContainer  *container,
					    SimDatabase   *db_ossim,
					    const gchar   *filename)
{
  SimXmlDirective *xml_directive;
  GList           *list = NULL;
  gint             max_sid = 0;
  SimPluginSid    *plugin_sid;
  gchar           *query;
  
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (filename);

  xml_directive = sim_xml_directive_new_from_file (container, filename);
  container->_priv->directives = sim_xml_directive_get_directives (xml_directive);

  max_sid = sim_container_db_get_max_plugin_sid (container, db_ossim,
						 SIM_PLUGIN_ID_DIRECTIVE);

  list = container->_priv->directives;
  while (list)
    {
      SimDirective *directive = (SimDirective *) list->data;

      plugin_sid = sim_container_get_plugin_sid_by_name (container, 
							 SIM_PLUGIN_ID_DIRECTIVE,
							 sim_directive_get_name (directive));

      if (!plugin_sid)
	{
	  plugin_sid = sim_plugin_sid_new_from_data (SIM_PLUGIN_ID_DIRECTIVE,
						     sim_directive_get_id (directive),
						     0,
						     0,
						     1,
						     sim_directive_get_priority (directive),
						     sim_directive_get_name (directive));
	  sim_container_append_plugin_sid (container, plugin_sid);
	  
	  query = sim_plugin_sid_get_insert_clause (plugin_sid);
	  g_message (query);
	  sim_database_execute_no_query (db_ossim, query); 
	  g_free (query);
	}

      list = list->next;
    }

  g_object_unref (xml_directive);
}

/*
 *
 *
 *
 *
 */
void
sim_container_append_directive_ul (SimContainer  *container,
				   SimDirective  *directive)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (directive);
  g_return_if_fail (SIM_IS_DIRECTIVE (directive));
  
  container->_priv->directives = g_list_append (container->_priv->directives, directive);
}

/*
 *
 *
 *
 *
 */
void
sim_container_remove_directive_ul (SimContainer  *container,
				   SimDirective  *directive)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (directive);
  g_return_if_fail (SIM_IS_DIRECTIVE (directive));
  
  container->_priv->directives = g_list_remove (container->_priv->directives, directive);
}

/*
 *
 *
 *
 *
 */
GList*
sim_container_get_directives_ul (SimContainer  *container)
{
  GList *list;
  
  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);

  list = g_list_copy (container->_priv->directives);
  
  return list;
}

/*
 *
 *
 *
 *
 */
void
sim_container_set_directives_ul (SimContainer  *container,
				 GList         *directives)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (directives);

  container->_priv->directives = g_list_concat (container->_priv->directives, directives);
}

/*
 *
 *
 *
 *
 */
void
sim_container_free_directives_ul (SimContainer  *container)
{
  GList *list;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));

  list = container->_priv->directives;
  while (list)
    {
      SimDirective *directive = (SimDirective *) list->data;
      g_object_unref (directive);

      list = list->next;
    }
  g_list_free (container->_priv->directives);
  container->_priv->directives = NULL;
}

/*
 *
 *
 *
 *
 */
void
sim_container_load_directives_from_file (SimContainer  *container,
					 SimDatabase   *db_ossim,
					 const gchar   *filename)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (filename);

  G_LOCK (s_mutex_directives);
  sim_container_load_directives_from_file_ul (container, db_ossim, filename);
  G_UNLOCK (s_mutex_directives);
}

/*
 *
 *
 *
 *
 */
void
sim_container_append_directive (SimContainer  *container,
				SimDirective  *directive)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (directive);
  g_return_if_fail (SIM_IS_DIRECTIVE (directive));

  G_LOCK (s_mutex_directives);
  sim_container_append_directive_ul (container, directive);
  G_UNLOCK (s_mutex_directives);
}

/*
 *
 *
 *
 *
 */
void
sim_container_remove_directive (SimContainer  *container,
				SimDirective  *directive)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (directive);
  g_return_if_fail (SIM_IS_DIRECTIVE (directive));

  G_LOCK (s_mutex_directives);
  sim_container_remove_directive_ul (container, directive);
  G_UNLOCK (s_mutex_directives);
}

/*
 *
 *
 *
 *
 */
GList*
sim_container_get_directives (SimContainer  *container)
{
  GList *list;
  
  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);

  G_LOCK (s_mutex_directives);
  list = sim_container_get_directives_ul (container);
  G_UNLOCK (s_mutex_directives);
  
  return list;
}

/*
 *
 *
 *
 *
 */
void
sim_container_set_directives (SimContainer  *container,
			      GList         *directives)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (directives);

  G_LOCK (s_mutex_directives);
  sim_container_set_directives_ul (container, directives);
  G_UNLOCK (s_mutex_directives);
}

/*
 *
 *
 *
 *
 */
void
sim_container_free_directives (SimContainer  *container)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));

  G_LOCK (s_mutex_directives);
  sim_container_free_directives_ul (container);
  G_UNLOCK (s_mutex_directives);
}

/*
 *
 *
 *
 */
void
sim_container_db_load_host_levels_ul (SimContainer  *container,
				      SimDatabase   *database)
{
  SimHostLevel  *host_level;
  GdaDataModel  *dm;
  gint           row;
  gchar         *query = "SELECT * FROM host_qualification";

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));

  dm = sim_database_execute_single_command (database, query);
  if (dm)
    {
      for (row = 0; row < gda_data_model_get_n_rows (dm); row++)
	{
	  host_level  = sim_host_level_new_from_dm (dm, row);

	  container->_priv->host_levels = g_list_append (container->_priv->host_levels, host_level);
	}

      g_object_unref(dm);
    }
  else
    {
      g_message ("HOST LEVELS DATA MODEL ERROR");
    }
}

/*
 *
 *
 *
 *
 */
void
sim_container_db_insert_host_level_ul (SimContainer  *container,
				       SimDatabase   *database,
				       SimHostLevel  *host_level)
{
  gchar *query;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));
  g_return_if_fail (host_level);
  g_return_if_fail (SIM_IS_HOST_LEVEL (host_level));

  query = sim_host_level_get_insert_clause (host_level);
  sim_database_execute_no_query (database, query);
  g_free (query);
}

/*
 *
 *
 *
 *
 */
void 
sim_container_db_update_host_level_ul (SimContainer  *container,
				       SimDatabase   *database,
				       SimHostLevel  *host_level)
{
  gchar *query;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));
  g_return_if_fail (host_level);
  g_return_if_fail (SIM_IS_HOST_LEVEL (host_level));

  query = sim_host_level_get_update_clause (host_level);
  sim_database_execute_no_query (database, query);
  g_free (query);
}

/*
 *
 *
 *
 *
 */
void 
sim_container_db_delete_host_level_ul (SimContainer  *container,
				       SimDatabase   *database,
				       SimHostLevel  *host_level)
{
  gchar *query;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));
  g_return_if_fail (host_level);
  g_return_if_fail (SIM_IS_HOST_LEVEL (host_level));

  query = sim_host_level_get_delete_clause (host_level);
  sim_database_execute_no_query (database, query);
  g_free (query);
}

/*
 *
 *
 *
 *
 */
void
sim_container_append_host_level_ul (SimContainer  *container,
				    SimHostLevel  *host_level)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (host_level);
  g_return_if_fail (SIM_IS_HOST_LEVEL (host_level));

  container->_priv->host_levels = g_list_append (container->_priv->host_levels, host_level);
}

/*
 *
 *
 *
 *
 */
void
sim_container_remove_host_level_ul (SimContainer  *container,
				    SimHostLevel  *host_level)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (host_level);
  g_return_if_fail (SIM_IS_HOST_LEVEL (host_level));

  container->_priv->host_levels = g_list_remove (container->_priv->host_levels, host_level);
}

/*
 *
 *
 *
 *
 */
GList*
sim_container_get_host_levels_ul (SimContainer  *container)
{
  GList *list;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);

  list = g_list_copy (container->_priv->host_levels);

  return list;
}

/*
 *
 *
 *
 *
 */
void
sim_container_set_host_levels_ul (SimContainer  *container,
				  GList         *host_levels)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (host_levels);

  container->_priv->host_levels = g_list_concat (container->_priv->host_levels, host_levels);
}

/*
 *
 *
 *
 *
 */
void
sim_container_free_host_levels_ul (SimContainer  *container)
{
  GList *list;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));

  list = container->_priv->host_levels;
  while (list)
    {
      SimHostLevel *host_level = (SimHostLevel *) list->data;
      g_object_unref (host_level);

      list = list->next;
    }
  g_list_free (container->_priv->host_levels);
  container->_priv->host_levels = NULL;
}

/*
 *
 *
 *
 *
 */
SimHostLevel*
sim_container_get_host_level_by_ia_ul (SimContainer  *container,
				       GInetAddr     *ia)
{
  SimHostLevel  *host_level;
  GList         *list;
  gboolean       found = FALSE;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);
  g_return_val_if_fail (ia, NULL);

  list = container->_priv->host_levels;
  while (list)
    {
      host_level = (SimHostLevel *) list->data;

      if (gnet_inetaddr_noport_equal (sim_host_level_get_ia (host_level), ia))
	{
	  found = TRUE;
	  break;
	}

      list = list->next;
    }

  if (!found)
    return NULL;

  return host_level;
}

/*
 *
 *
 *
 */
void
sim_container_set_host_levels_recovery_ul (SimContainer  *container,
					   SimDatabase   *database,
					   gint           recovery)
{
  GList           *list;
  GList           *removes = NULL;
  gint             c;
  gint             a;

  g_return_if_fail (container != NULL);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (recovery >= 0);

  list = container->_priv->host_levels;
  while (list)
    {
      SimHostLevel *host_level = (SimHostLevel *) list->data;

      sim_host_level_set_recovery (host_level, recovery); /* Update Memory */

      c = sim_host_level_get_c (host_level);
      a = sim_host_level_get_a (host_level);

      if (c == 0 && a == 0)
	{
	  gchar *query = sim_host_level_get_delete_clause (host_level);
	  sim_database_execute_no_query (database, query);
	  g_free (query);

	  removes = g_list_append (removes, host_level);
	}
      else
	{
	  gchar *query = sim_host_level_get_update_clause (host_level);
	  sim_database_execute_no_query (database, query);
	  g_free (query);
	}

      list = list->next;
    }

  while (removes)
    {
      SimHostLevel *host_level = (SimHostLevel *) removes->data;

      container->_priv->host_levels = g_list_remove_all (container->_priv->host_levels, host_level);
      g_object_unref (host_level);

      removes = removes->next;
    }
  g_list_free (removes);
}

/*
 *
 *
 *
 */
void
sim_container_db_load_host_levels (SimContainer  *container,
				   SimDatabase   *database)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));

  G_LOCK (s_mutex_host_levels);
  sim_container_db_load_host_levels_ul (container, database);
  G_UNLOCK (s_mutex_host_levels);
}

/*
 *
 *
 *
 *
 */
void
sim_container_db_insert_host_level (SimContainer  *container,
				    SimDatabase   *database,
				    SimHostLevel  *host_level)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));
  g_return_if_fail (host_level);
  g_return_if_fail (SIM_IS_HOST_LEVEL (host_level));

  G_LOCK (s_mutex_host_levels);
  sim_container_db_insert_host_level_ul (container, database, host_level);
  G_UNLOCK (s_mutex_host_levels);
}

/*
 *
 *
 *
 *
 */
void 
sim_container_db_update_host_level (SimContainer  *container,
				    SimDatabase   *database,
				    SimHostLevel  *host_level)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));
  g_return_if_fail (host_level);
  g_return_if_fail (SIM_IS_HOST_LEVEL (host_level));

  G_LOCK (s_mutex_host_levels);
  sim_container_db_update_host_level_ul (container, database, host_level);
  G_UNLOCK (s_mutex_host_levels);
}

/*
 *
 *
 *
 *
 */
void 
sim_container_db_delete_host_level (SimContainer  *container,
				    SimDatabase   *database,
				    SimHostLevel  *host_level)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));
  g_return_if_fail (host_level);
  g_return_if_fail (SIM_IS_HOST_LEVEL (host_level));

  G_LOCK (s_mutex_host_levels);
  sim_container_db_delete_host_level_ul (container, database, host_level);
  G_UNLOCK (s_mutex_host_levels);
}

/*
 *
 *
 *
 *
 */
void
sim_container_append_host_level (SimContainer  *container,
				 SimHostLevel  *host_level)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (host_level);
  g_return_if_fail (SIM_IS_HOST_LEVEL (host_level));

  G_LOCK (s_mutex_host_levels);
  sim_container_append_host_level_ul (container, host_level);
  G_UNLOCK (s_mutex_host_levels);
}

/*
 *
 *
 *
 *
 */
void
sim_container_remove_host_level (SimContainer  *container,
				 SimHostLevel  *host_level)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (host_level);
  g_return_if_fail (SIM_IS_HOST_LEVEL (host_level));

  G_LOCK (s_mutex_host_levels);
  sim_container_remove_host_level_ul (container, host_level);
  G_UNLOCK (s_mutex_host_levels);
}

/*
 *
 *
 *
 *
 */
GList*
sim_container_get_host_levels (SimContainer  *container)
{
  GList *list;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);

  G_LOCK (s_mutex_host_levels);
  list = sim_container_get_host_levels_ul (container);
  G_UNLOCK (s_mutex_host_levels);

  return list;
}

/*
 *
 *
 *
 *
 */
void
sim_container_set_host_levels (SimContainer  *container,
			       GList         *host_levels)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (host_levels);

  G_LOCK (s_mutex_host_levels);
  sim_container_set_host_levels_ul (container, host_levels);
  G_UNLOCK (s_mutex_host_levels);
}

/*
 *
 *
 *
 *
 */
void
sim_container_free_host_levels (SimContainer  *container)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));

  G_LOCK (s_mutex_host_levels);
  sim_container_free_host_levels_ul (container);
  G_UNLOCK (s_mutex_host_levels);
}

/*
 *
 *
 *
 *
 */
SimHostLevel*
sim_container_get_host_level_by_ia (SimContainer  *container,
				    GInetAddr     *ia)
{
  SimHostLevel  *host_level;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);
  g_return_val_if_fail (ia, NULL);

  G_LOCK (s_mutex_host_levels);
  host_level = sim_container_get_host_level_by_ia_ul (container, ia);
  G_UNLOCK (s_mutex_host_levels);

  return host_level;
}

/*
 *
 *
 *
 */
void
sim_container_set_host_levels_recovery (SimContainer  *container,
					SimDatabase   *database,
					gint           recovery)
{
  g_return_if_fail (container != NULL);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (recovery >= 0);

  G_LOCK (s_mutex_host_levels);
  sim_container_set_host_levels_recovery_ul (container, database, recovery);
  G_UNLOCK (s_mutex_host_levels);
}


/*
 *
 *
 *
 */
void
sim_container_db_load_net_levels_ul (SimContainer  *container,
				     SimDatabase   *database)
{
  SimNetLevel   *net_level;
  GdaDataModel  *dm;
  gint           row;
  gchar         *query = "SELECT * FROM net_qualification";

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));

  dm = sim_database_execute_single_command (database, query);
  if (dm)
    {
      for (row = 0; row < gda_data_model_get_n_rows (dm); row++)
	{
	  net_level  = sim_net_level_new_from_dm (dm, row);

	  container->_priv->net_levels = g_list_append (container->_priv->net_levels, net_level);
	}

      g_object_unref(dm);
    }
  else
    {
      g_message ("NET LEVELS DATA MODEL ERROR");
    }
}

/*
 *
 *
 *
 *
 */
void
sim_container_db_insert_net_level_ul (SimContainer  *container,
				      SimDatabase   *database,
				      SimNetLevel   *net_level)
{
  gchar *query;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));
  g_return_if_fail (net_level);
  g_return_if_fail (SIM_IS_NET_LEVEL (net_level));

  query = sim_net_level_get_insert_clause (net_level);
  sim_database_execute_no_query (database, query);
  g_free (query);
}

/*
 *
 *
 *
 *
 */
void 
sim_container_db_update_net_level_ul (SimContainer  *container,
				      SimDatabase   *database,
				      SimNetLevel   *net_level)
{
  gchar *query;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));
  g_return_if_fail (net_level);
  g_return_if_fail (SIM_IS_NET_LEVEL (net_level));

  query = sim_net_level_get_update_clause (net_level);
  sim_database_execute_no_query (database, query);
  g_free (query);
}

/*
 *
 *
 *
 *
 */
void 
sim_container_db_delete_net_level_ul (SimContainer  *container,
				      SimDatabase   *database,
				      SimNetLevel   *net_level)
{
  gchar *query;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));
  g_return_if_fail (net_level);
  g_return_if_fail (SIM_IS_NET_LEVEL (net_level));

  query = sim_net_level_get_delete_clause (net_level);
  sim_database_execute_no_query (database, query);
  g_free (query);
}

/*
 *
 *
 *
 *
 */
void
sim_container_append_net_level_ul (SimContainer  *container,
				   SimNetLevel   *net_level)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (net_level);
  g_return_if_fail (SIM_IS_NET_LEVEL (net_level));

  container->_priv->net_levels = g_list_append (container->_priv->net_levels, net_level);
}

/*
 *
 *
 *
 *
 */
void
sim_container_remove_net_level_ul (SimContainer  *container,
				   SimNetLevel   *net_level)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (net_level);
  g_return_if_fail (SIM_IS_NET_LEVEL (net_level));

  container->_priv->net_levels = g_list_remove (container->_priv->net_levels, net_level);
}

/*
 *
 *
 *
 *
 */
GList*
sim_container_get_net_levels_ul (SimContainer  *container)
{
  GList *list;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);

  list = g_list_copy (container->_priv->net_levels);

  return list;
}

/*
 *
 *
 *
 *
 */
void
sim_container_set_net_levels_ul (SimContainer  *container,
				 GList         *net_levels)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (net_levels);

  container->_priv->net_levels = g_list_concat (container->_priv->net_levels, net_levels);
}

/*
 *
 *
 *
 *
 */
void
sim_container_free_net_levels_ul (SimContainer  *container)
{
  GList *list;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));

  list = container->_priv->net_levels;
  while (list)
    {
      SimNetLevel *net_level = (SimNetLevel *) list->data;
      g_object_unref (net_level);

      list = list->next;
    }
  g_list_free (container->_priv->net_levels);
  container->_priv->net_levels = NULL;
}

/*
 *
 *
 *
 *
 */
SimNetLevel*
sim_container_get_net_level_by_name_ul (SimContainer  *container,
					const gchar   *name)
{
  SimNetLevel  *net_level;
  GList        *list;
  gboolean      found = FALSE;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);
  g_return_val_if_fail (name, NULL);

  list = container->_priv->net_levels;
  while (list)
    {
      net_level = (SimNetLevel *) list->data;

      if (!strcmp (sim_net_level_get_name (net_level), name))
	{
	  found = TRUE;
	  break;
	}

      list = list->next;
    }

  if (!found)
    return NULL;

  return net_level;
}

/*
 *
 *
 *
 */
void
sim_container_set_net_levels_recovery_ul (SimContainer  *container,
					  SimDatabase   *database,
					  gint           recovery)
{
  GList           *list;
  GList           *removes = NULL;
  gint             c;
  gint             a;

  g_return_if_fail (container != NULL);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (recovery >= 0);

  list = container->_priv->net_levels;
  while (list)
    {
      SimNetLevel *net_level = (SimNetLevel *) list->data;

      sim_net_level_set_recovery (net_level, recovery); /* Update Memory */

      c = sim_net_level_get_c (net_level);
      a = sim_net_level_get_a (net_level);

      if (c == 0 && a == 0)
	{
	  gchar *query = sim_net_level_get_update_clause (net_level);
	  sim_database_execute_no_query (database, query);
	  g_free (query);

	  /* Fix this in the PostgreSQL version */
	  //container->_priv->net_levels = g_list_remove (container->_priv->net_levels, net_level); /* Delete Container List */
	  //sim_container_db_delete_net_level (container, database, net_level); /* Delete DB */
	}
      else
	{
	  gchar *query = sim_net_level_get_update_clause (net_level);
	  sim_database_execute_no_query (database, query);
	  g_free (query);
	  //sim_container_db_update_net_level (container, database, net_level); /* Update DB */
	}

      list = list->next;
    }

  while (removes)
    {
      SimNetLevel *net_level = (SimNetLevel *) removes->data;

      container->_priv->net_levels = g_list_remove_all (container->_priv->net_levels, net_level);
      g_object_unref (net_level);

      removes = removes->next;
    }
  g_list_free (removes);
}

/*
 *
 *
 *
 */
void
sim_container_db_load_net_levels (SimContainer  *container,
				  SimDatabase   *database)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));

  G_LOCK (s_mutex_net_levels);
  sim_container_db_load_net_levels_ul (container, database);
  G_UNLOCK (s_mutex_net_levels);
}

/*
 *
 *
 *
 *
 */
void
sim_container_db_insert_net_level (SimContainer  *container,
				   SimDatabase   *database,
				   SimNetLevel   *net_level)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));
  g_return_if_fail (net_level);
  g_return_if_fail (SIM_IS_NET_LEVEL (net_level));

  G_LOCK (s_mutex_net_levels);
  sim_container_db_insert_net_level_ul (container, database, net_level);
  G_UNLOCK (s_mutex_net_levels);
}

/*
 *
 *
 *
 *
 */
void 
sim_container_db_update_net_level (SimContainer  *container,
				   SimDatabase   *database,
				   SimNetLevel   *net_level)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));
  g_return_if_fail (net_level);
  g_return_if_fail (SIM_IS_NET_LEVEL (net_level));

  G_LOCK (s_mutex_net_levels);
  sim_container_db_update_net_level_ul (container, database, net_level);
  G_UNLOCK (s_mutex_net_levels);
}

/*
 *
 *
 *
 *
 */
void 
sim_container_db_delete_net_level (SimContainer  *container,
				   SimDatabase   *database,
				   SimNetLevel   *net_level)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));
  g_return_if_fail (net_level);
  g_return_if_fail (SIM_IS_NET_LEVEL (net_level));

  G_LOCK (s_mutex_net_levels);
  sim_container_db_delete_net_level_ul (container, database, net_level);
  G_UNLOCK (s_mutex_net_levels);
}

/*
 *
 *
 *
 *
 */
void
sim_container_append_net_level (SimContainer  *container,
				SimNetLevel   *net_level)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (net_level);
  g_return_if_fail (SIM_IS_NET_LEVEL (net_level));

  G_LOCK (s_mutex_net_levels);
  sim_container_append_net_level_ul (container, net_level);
  G_UNLOCK (s_mutex_net_levels);
}

/*
 *
 *
 *
 *
 */
void
sim_container_remove_net_level (SimContainer  *container,
				SimNetLevel   *net_level)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (net_level);
  g_return_if_fail (SIM_IS_NET_LEVEL (net_level));

  G_LOCK (s_mutex_net_levels);
  sim_container_remove_net_level_ul (container, net_level);
  G_UNLOCK (s_mutex_net_levels);
}

/*
 *
 *
 *
 *
 */
GList*
sim_container_get_net_levels (SimContainer  *container)
{
  GList *list;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);

  G_LOCK (s_mutex_net_levels);
  list = sim_container_get_net_levels_ul (container);
  G_UNLOCK (s_mutex_net_levels);

  return list;
}

/*
 *
 *
 *
 *
 */
void
sim_container_set_net_levels (SimContainer  *container,
			      GList         *net_levels)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (net_levels);

  G_LOCK (s_mutex_net_levels);
  sim_container_set_net_levels_ul (container, net_levels);
  G_UNLOCK (s_mutex_net_levels);
}

/*
 *
 *
 *
 *
 */
void
sim_container_free_net_levels (SimContainer  *container)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));

  G_LOCK (s_mutex_net_levels);
  sim_container_free_net_levels_ul (container);
  G_UNLOCK (s_mutex_net_levels);
}

/*
 *
 *
 *
 *
 */
SimNetLevel*
sim_container_get_net_level_by_name (SimContainer  *container,
				     const gchar   *name)
{
  SimNetLevel  *net_level;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);
  g_return_val_if_fail (name, NULL);

  G_LOCK (s_mutex_net_levels);
  net_level = sim_container_get_net_level_by_name_ul (container, name);
  G_UNLOCK (s_mutex_net_levels);

  return net_level;
}

/*
 *
 *
 *
 */
void
sim_container_set_net_levels_recovery (SimContainer  *container,
				       SimDatabase   *database,
				       gint           recovery)
{
  g_return_if_fail (container != NULL);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (recovery >= 0);

  G_LOCK (s_mutex_net_levels);
  sim_container_set_net_levels_recovery_ul (container, database, recovery);
  G_UNLOCK (s_mutex_net_levels);
}

/*
 *
 *
 *
 *
 */
void
sim_container_db_insert_backlog_ul (SimContainer  *container,
				    SimDatabase   *database,
				    SimDirective  *backlog)
{
  gchar *query;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));
  g_return_if_fail (backlog);
  g_return_if_fail (SIM_IS_DIRECTIVE (backlog));

  query = sim_directive_backlog_get_insert_clause (backlog);
  sim_database_execute_no_query (database, query);
  g_free (query);
}

/*
 *
 *
 *
 *
 */
void 
sim_container_db_update_backlog_ul (SimContainer  *container,
				    SimDatabase   *database,
				    SimDirective   *backlog)
{
  gchar *query;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));
  g_return_if_fail (backlog);
  g_return_if_fail (SIM_IS_DIRECTIVE (backlog));

  query = sim_directive_backlog_get_update_clause (backlog);
  g_message (query);
  sim_database_execute_no_query (database, query);
  g_free (query);
}

/*
 *
 *
 *
 *
 */
void 
sim_container_db_delete_backlog_ul (SimContainer  *container,
				    SimDatabase   *database,
				    SimDirective   *backlog)
{
  gchar *query;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));
  g_return_if_fail (backlog);
  g_return_if_fail (SIM_IS_DIRECTIVE (backlog));

  query = sim_directive_backlog_get_delete_clause (backlog);
  sim_database_execute_no_query (database, query);
  g_free (query);
}

/*
 *
 *
 *
 *
 */
void
sim_container_append_backlog_ul (SimContainer  *container,
				 SimDirective  *backlog)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (backlog);
  g_return_if_fail (SIM_IS_DIRECTIVE (backlog));
  
  container->_priv->backlogs = g_list_append (container->_priv->backlogs, backlog);
}

/*
 *
 *
 *
 *
 */
void
sim_container_remove_backlog_ul (SimContainer  *container,
				 SimDirective  *backlog)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (backlog);
  g_return_if_fail (SIM_IS_DIRECTIVE (backlog));
  
  container->_priv->backlogs = g_list_remove (container->_priv->backlogs, backlog);
}

/*
 *
 *
 *
 *
 */
GList*
sim_container_get_backlogs_ul (SimContainer  *container)
{
  GList *list;
  
  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);

  list = g_list_copy (container->_priv->backlogs);
  
  return list;
}

/*
 *
 *
 *
 *
 */
void
sim_container_set_backlogs_ul (SimContainer  *container,
			       GList         *backlogs)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (backlogs);

  container->_priv->backlogs = g_list_concat (container->_priv->backlogs, backlogs);
}

/*
 *
 *
 *
 *
 */
void
sim_container_free_backlogs_ul (SimContainer  *container)
{
  GList *list;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));

  list = container->_priv->backlogs;
  while (list)
    {
      SimDirective *backlog = (SimDirective *) list->data;
      g_object_unref (backlog);

      list = list->next;
    }
  g_list_free (container->_priv->backlogs);
  container->_priv->backlogs = NULL;
}

/*
 *
 *
 *
 *
 */
void
sim_container_db_insert_backlog (SimContainer  *container,
				 SimDatabase   *database,
				 SimDirective  *backlog)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));
  g_return_if_fail (backlog);
  g_return_if_fail (SIM_IS_DIRECTIVE (backlog));

  G_LOCK (s_mutex_backlogs);
  sim_container_db_insert_backlog_ul (container, database, backlog);
  G_UNLOCK (s_mutex_backlogs);
}

/*
 *
 *
 *
 *
 */
void 
sim_container_db_update_backlog (SimContainer  *container,
				 SimDatabase   *database,
				 SimDirective   *backlog)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));
  g_return_if_fail (backlog);
  g_return_if_fail (SIM_IS_DIRECTIVE (backlog));

  G_LOCK (s_mutex_backlogs);
  sim_container_db_update_backlog_ul (container, database, backlog);
  G_UNLOCK (s_mutex_backlogs);
}

/*
 *
 *
 *
 *
 */
void 
sim_container_db_delete_backlog (SimContainer  *container,
				 SimDatabase   *database,
				 SimDirective   *backlog)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));
  g_return_if_fail (backlog);
  g_return_if_fail (SIM_IS_DIRECTIVE (backlog));

  G_LOCK (s_mutex_backlogs);
  sim_container_db_delete_backlog_ul (container, database, backlog);
  G_UNLOCK (s_mutex_backlogs);
}

/*
 *
 *
 *
 *
 */
void
sim_container_append_backlog (SimContainer  *container,
			      SimDirective  *backlog)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (backlog);
  g_return_if_fail (SIM_IS_DIRECTIVE (backlog));
  
  G_LOCK (s_mutex_backlogs);
  sim_container_append_backlog_ul (container, backlog);
  G_UNLOCK (s_mutex_backlogs);
}

/*
 *
 *
 *
 *
 */
void
sim_container_remove_backlog (SimContainer  *container,
			      SimDirective  *backlog)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (backlog);
  g_return_if_fail (SIM_IS_DIRECTIVE (backlog));
  
  G_LOCK (s_mutex_backlogs);
  sim_container_remove_backlog_ul (container, backlog);
  G_UNLOCK (s_mutex_backlogs);
}

/*
 *
 *
 *
 *
 */
GList*
sim_container_get_backlogs (SimContainer  *container)
{
  GList *list;
  
  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);

  G_LOCK (s_mutex_backlogs);
  list = sim_container_get_backlogs_ul (container);
  G_UNLOCK (s_mutex_backlogs);
  
  return list;
}

/*
 *
 *
 *
 *
 */
void
sim_container_set_backlogs (SimContainer  *container,
			    GList         *backlogs)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (backlogs);

  G_LOCK (s_mutex_backlogs);
  sim_container_set_backlogs_ul (container, backlogs);
  G_UNLOCK (s_mutex_backlogs);
}

/*
 *
 *
 *
 *
 */
void
sim_container_free_backlogs (SimContainer  *container)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));

  G_LOCK (s_mutex_backlogs);
  sim_container_free_backlogs_ul (container);
  G_UNLOCK (s_mutex_backlogs);
}

/*
 *
 *
 *
 *
 */
void
sim_container_push_alert (SimContainer  *container,
			    SimAlert    *alert)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (alert);
  g_return_if_fail (SIM_IS_ALERT (alert));

  g_mutex_lock (container->_priv->mutex_alerts);
  g_queue_push_head (container->_priv->alerts, alert);
  g_cond_signal (container->_priv->cond_alerts);
  g_mutex_unlock (container->_priv->mutex_alerts);
}

/*
 *
 *
 *
 *
 */
SimAlert*
sim_container_pop_alert (SimContainer  *container)
{
  SimAlert *alert;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);

  g_mutex_lock (container->_priv->mutex_alerts);

  while (!g_queue_peek_tail (container->_priv->alerts))
    g_cond_wait (container->_priv->cond_alerts, container->_priv->mutex_alerts);

  alert = (SimAlert *) g_queue_pop_tail (container->_priv->alerts);
  if (!g_queue_peek_tail (container->_priv->alerts))
    {
      g_cond_free (container->_priv->cond_alerts);
      container->_priv->cond_alerts = g_cond_new ();
    }
  g_mutex_unlock (container->_priv->mutex_alerts);

  return alert;
}

/*
 *
 *
 *
 *
 */
void
sim_container_free_alerts (SimContainer  *container)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));

  g_mutex_lock (container->_priv->mutex_alerts);
  while (!g_queue_is_empty (container->_priv->alerts))
    {
      SimAlert *alert = (SimAlert *) g_queue_pop_head (container->_priv->alerts);
      g_object_unref (alert);
    }
  g_queue_free (container->_priv->alerts);
  container->_priv->alerts = g_queue_new ();
  g_mutex_unlock (container->_priv->mutex_alerts);
}

/*
 *
 *
 *
 *
 */
gboolean
sim_container_is_empty_alerts (SimContainer  *container)
{
  gboolean empty;

  g_return_val_if_fail (container, TRUE);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), TRUE);

  g_mutex_lock (container->_priv->mutex_alerts);
  empty = g_queue_is_empty (container->_priv->alerts);
  g_mutex_unlock (container->_priv->mutex_alerts);

  return empty;
}

/*
 *
 *
 *
 *
 */
gint
sim_container_length_alerts (SimContainer  *container)
{
  gint length;

  g_return_val_if_fail (container, 0);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), 0);

  g_mutex_lock (container->_priv->mutex_alerts);
  length = container->_priv->alerts->length;
  g_mutex_unlock (container->_priv->mutex_alerts);

  return length;
}
