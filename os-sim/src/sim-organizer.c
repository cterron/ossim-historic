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

#include "sim-organizer.h"
#include "sim-server.h"
#include "sim-host.h"
#include "sim-net.h"
#include "sim-category.h"
#include "sim-plugin-sid.h"
#include "sim-policy.h"
#include "sim-rule.h"
#include "sim-directive.h"
#include "sim-host-level.h"
#include "sim-net-level.h"

#include <math.h>
#include <time.h>

extern SimContainer  *sim_ctn;
extern SimServer     *sim_svr;

enum 
{
  DESTROY,
  LAST_SIGNAL
};

typedef struct {
  gulong     ip;
  gchar     *current;
  gchar     *previous;
  gchar     *vendor;
  gchar     *date;
} SimHostMacOs;

struct _SimOrganizerPrivate {
  SimConfig     *config;
  SimDatabase   *db_ossim;
  SimDatabase   *db_snort;
};

static gpointer parent_class = NULL;
static gint sim_container_signals[LAST_SIGNAL] = { 0 };

G_LOCK_EXTERN (s_mutex_directives);
G_LOCK_EXTERN (s_mutex_backlogs);

/* GType Functions */

static void 
sim_organizer_impl_dispose (GObject  *gobject)
{
  G_OBJECT_CLASS (parent_class)->dispose (gobject);
}

static void 
sim_organizer_impl_finalize (GObject  *gobject)
{
  SimOrganizer *organizer = SIM_ORGANIZER (gobject);

  g_free (organizer->_priv);

  G_OBJECT_CLASS (parent_class)->finalize (gobject);
}

static void
sim_organizer_class_init (SimOrganizerClass * class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  parent_class = g_type_class_peek_parent (class);

  object_class->dispose = sim_organizer_impl_dispose;
  object_class->finalize = sim_organizer_impl_finalize;
}

static void
sim_organizer_instance_init (SimOrganizer *organizer)
{
  organizer->_priv = g_new0 (SimOrganizerPrivate, 1);

  organizer->_priv->config = NULL;
  organizer->_priv->db_ossim = NULL;
  organizer->_priv->db_snort = NULL;
}

/* Public Methods */

GType
sim_organizer_get_type (void)
{
  static GType object_type = 0;
 
  if (!object_type)
  {
    static const GTypeInfo type_info = {
              sizeof (SimOrganizerClass),
              NULL,
              NULL,
              (GClassInitFunc) sim_organizer_class_init,
              NULL,
              NULL,                       /* class data */
              sizeof (SimOrganizer),
              0,                          /* number of pre-allocs */
              (GInstanceInitFunc) sim_organizer_instance_init,
              NULL                        /* value table */
    };
    
    g_type_init ();
                                                                                                                             
    object_type = g_type_register_static (G_TYPE_OBJECT, "SimOrganizer", &type_info, 0);
  }
                                                                                                                             
  return object_type;
}

/*
 *
 *
 *
 *
 */
SimOrganizer*
sim_organizer_new (SimConfig  *config)
{
  SimOrganizer *organizer = NULL;
  SimConfigDS  *ds;

  g_return_val_if_fail (config, NULL);
  g_return_val_if_fail (SIM_IS_CONFIG (config), NULL);


  organizer = SIM_ORGANIZER (g_object_new (SIM_TYPE_ORGANIZER, NULL));
  organizer->_priv->config = config;

  ds = sim_config_get_ds_by_name (config, SIM_DS_OSSIM);
  organizer->_priv->db_ossim = sim_database_new (ds);

  ds = sim_config_get_ds_by_name (config, SIM_DS_SNORT);
  organizer->_priv->db_snort = sim_database_new (ds);

  return organizer;
}

/*
 *
 *
 *
 */
void
sim_organizer_run (SimOrganizer *organizer)
{
  SimAlert *alert = NULL;
  gchar    *query;

  g_return_if_fail (organizer != NULL);
  g_return_if_fail (SIM_IS_ORGANIZER (organizer));

  while (TRUE) 
    {
      alert =  sim_container_pop_alert (sim_ctn);
      
      if (!alert)
	{
	  continue;
	}

      if (alert->type == SIM_ALERT_TYPE_NONE)
	{
	  g_object_unref (alert);
	  continue;
	}

      switch (alert->type)
	{
	case SIM_ALERT_TYPE_DETECTOR:
	  sim_organizer_mac_os_change (organizer, alert);
	  sim_organizer_correlation_plugin (organizer, alert);
	  sim_organizer_calificate (organizer, alert);
	  sim_organizer_correlation (organizer, alert);
	  break;
	case SIM_ALERT_TYPE_MONITOR:
	  sim_organizer_correlation_plugin (organizer, alert);
	  sim_organizer_calificate (organizer, alert);
	  sim_organizer_correlation (organizer, alert);
	  break;
	default:
	  break;
	}

      sim_organizer_snort (organizer, alert);

      query = sim_alert_get_ossim_insert_clause (alert);
      if (query)
	{
	  sim_database_execute_no_query (organizer->_priv->db_ossim, query);
	  g_free (query);
	}

      g_object_unref (alert);
    }
}

/*
 *
 *
 *
 *
 */
static SimHostMacOs*
host_mac_has_host (SimDatabase     *db_ossim,
		   GInetAddr       *ia)
{
  GdaDataModel  *dm;
  GdaValue      *value;
  gchar         *query;
  gint           row;
  SimHostMacOs  *data = NULL;

  g_return_val_if_fail (db_ossim, NULL);
  g_return_val_if_fail (SIM_IS_DATABASE (db_ossim), NULL);
  g_return_val_if_fail (ia, NULL);

  query = g_strdup_printf ("SELECT ip, mac, previous FROM host_mac WHERE ip = %lu",
			   sim_inetaddr_ntohl (ia));

  dm = sim_database_execute_single_command (db_ossim, query);
  if (dm)
    {
      for (row = 0; row < gda_data_model_get_n_rows (dm); row++)
	{
	  data = g_new0 (SimHostMacOs, 1);
	  data->ip = sim_inetaddr_ntohl (ia);

	  value = (GdaValue *) gda_data_model_get_value_at (dm, 1, row);
	  if (!gda_value_is_null (value))
	    data->current = gda_value_stringify (value);

	  value = (GdaValue *) gda_data_model_get_value_at (dm, 2, row);
	  if (!gda_value_is_null (value))
	    data->previous = gda_value_stringify (value);
	  
	}
      
      g_object_unref(dm);
    }
  else
    {
      g_message ("HOST MAC DATA MODEL ERROR");
    }

  g_message (query);
  g_free (query);

  return data;
}

/*
 *
 *
 *
 *
 */
static SimHostMacOs*
host_os_has_host (SimDatabase     *db_ossim,
		  GInetAddr       *ia)
{
  GdaDataModel  *dm;
  GdaValue      *value;
  gchar         *query;
  gint           row;
  SimHostMacOs  *data = NULL;

  g_return_val_if_fail (db_ossim, NULL);
  g_return_val_if_fail (SIM_IS_DATABASE (db_ossim), NULL);
  g_return_val_if_fail (ia, NULL);

  query = g_strdup_printf ("SELECT ip, os, previous FROM host_os WHERE ip = %lu",
			   sim_inetaddr_ntohl (ia));
  dm = sim_database_execute_single_command (db_ossim, query);
  if (dm)
    {
      for (row = 0; row < gda_data_model_get_n_rows (dm); row++)
	{
	  data = g_new0 (SimHostMacOs, 1);
	  data->ip = sim_inetaddr_ntohl (ia);

	  value = (GdaValue *) gda_data_model_get_value_at (dm, 1, row);
	  if (!gda_value_is_null (value))
	    data->current = gda_value_stringify (value);

	  value = (GdaValue *) gda_data_model_get_value_at (dm, 2, row);
	  if (!gda_value_is_null (value))
	    data->previous = gda_value_stringify (value);
	  
	}
      
      g_object_unref(dm);
    }
  else
    {
      g_message ("HOST OS DATA MODEL ERROR");
    }
  g_message (query);
  g_free (query);

  return data;
}

/*
 *
 *
 *
 *
 */
static void
host_mac_db_insert (SimDatabase     *db_ossim,
		    SimHostMacOs    *data)
{
  gchar         *query;

  g_return_if_fail (db_ossim);
  g_return_if_fail (SIM_IS_DATABASE (db_ossim));
  g_return_if_fail (data);

  query = g_strdup_printf ("INSERT INTO host_mac (ip, mac, previous, vendor, date)"
			   " VALUES (%lu, '%s', '%s', '%s', '%s')",
			   data->ip,
			   data->current,
			   data->previous,
			   data->vendor,
			   data->date);
  sim_database_execute_no_query (db_ossim, query);

  g_message (query);
  g_free (query);
}

/*
 *
 *
 *
 *
 */
static void
host_os_db_insert (SimDatabase     *db_ossim,
		   SimHostMacOs    *data)
{
  gchar         *query;

  g_return_if_fail (db_ossim);
  g_return_if_fail (SIM_IS_DATABASE (db_ossim));
  g_return_if_fail (data);

  query = g_strdup_printf ("INSERT INTO host_os (ip, os, previous, date)"
			   " VALUES (%lu, '%s', '%s', '%s')",
			   data->ip,
			   data->current,
			   data->previous,
			   data->date);
  sim_database_execute_no_query (db_ossim, query);

  g_message (query);
  g_free (query);
}

/*
 *
 *
 *
 *
 */
static void
host_mac_db_update (SimDatabase     *db_ossim,
		    SimHostMacOs    *data)
{
  gchar         *query;

  g_return_if_fail (db_ossim);
  g_return_if_fail (SIM_IS_DATABASE (db_ossim));
  g_return_if_fail (data);

  query = g_strdup_printf ("UPDATE host_mac SET mac='%s', previous='%s', vendor='%s', date='%s'"
			   " WHERE ip = %lu",
			   data->current,
			   data->previous,
			   data->vendor,
			   data->date,
			   data->ip);
  sim_database_execute_no_query (db_ossim, query);

  g_message (query);
  g_free (query);
}

/*
 *
 *
 *
 *
 */
static void
host_os_db_update (SimDatabase     *db_ossim,
		   SimHostMacOs    *data)
{
  gchar         *query;

  g_return_if_fail (db_ossim);
  g_return_if_fail (SIM_IS_DATABASE (db_ossim));
  g_return_if_fail (data);

  query = g_strdup_printf ("UPDATE host_os SET os='%s', previous='%s', date='%s'"
			   " WHERE ip = %lu",
			   data->current,
			   data->previous,
			   data->date,
			   data->ip);
  sim_database_execute_no_query (db_ossim, query);

  g_message (query);
  g_free (query);
}


/*
 *
 *
 *
 *
 */
void
sim_organizer_mac_os_change (SimOrganizer  *organizer, 
			     SimAlert      *alert)
{
  SimDatabase     *db_ossim;
  SimHostMacOs    *data = NULL;
  gchar          **values;
  gchar            timestamp[32];

  g_return_if_fail (organizer);
  g_return_if_fail (SIM_IS_ORGANIZER (organizer));
  g_return_if_fail (alert);
  g_return_if_fail (SIM_IS_ALERT (alert));
  g_return_if_fail (alert->src_ia);

  if (!alert->data)
    return;

  if (alert->plugin_id != 1511 && alert->plugin_id != 1512)
    return;

  db_ossim = organizer->_priv->db_ossim;
  strftime (timestamp, 32, "%Y-%m-%d %H:%M:%S", localtime ((time_t *) &alert->time));

  switch (alert->plugin_id)
    {
    case 1511:
      data = host_os_has_host (db_ossim, alert->src_ia);
      if (data)
	{
	  if (g_ascii_strcasecmp (data->current, alert->data))
	    {
	      g_free (data->previous);
	      data->previous = g_strdup (data->current);

	      g_free (data->current);
	      data->current = g_strdup (alert->data);

	      data->date = g_strdup (timestamp);

	      host_os_db_update (db_ossim, data);
	    }
	}
      else
	{
	  data = g_new0 (SimHostMacOs, 1);
	  data->ip = sim_inetaddr_ntohl (alert->src_ia);
	  data->current = g_strdup (alert->data);
	  data->previous = g_strdup (alert->data);
	  data->date = g_strdup (timestamp);

	  host_os_db_insert (db_ossim, data);
	}
      break;

    case 1512:
      values = g_strsplit (alert->data, "|", 0);

      data = host_mac_has_host (db_ossim, alert->src_ia);
      if (data)
	{
	  if (g_ascii_strcasecmp (data->current, values[0]))
	    {
	      g_free (data->previous);
	      data->previous = g_strdup (data->current);

	      g_free (data->current);
	      data->current = g_strdup (values[0]);

	      g_free (data->vendor);
	      data->vendor = g_strdup (values[1]);

	      data->date = g_strdup (timestamp);

	      host_mac_db_update (db_ossim, data);
	    }
	}
      else
	{
	  data = g_new0 (SimHostMacOs, 1);
	  data->ip = sim_inetaddr_ntohl (alert->src_ia);
	  data->current = g_strdup (values[0]);
	  data->previous = g_strdup (values[0]);
	  data->vendor = g_strdup (values[1]);
	  data->date = g_strdup (timestamp);

	  host_mac_db_insert (db_ossim, data);
	}

      g_strfreev (values);
      break;

    default:
      break;
    }
  
  /* Data free */
  if (data)
    {
      g_free (data->current);
      g_free (data->previous);
      g_free (data->vendor);
      g_free (data->date);
    }
}


/*
 *
 *
 *
 *
 *
 */
void
sim_organizer_correlation_plugin (SimOrganizer *organizer, 
				  SimAlert     *alert)
{
  SimDatabase     *db_ossim;
  GList           *list;

  g_return_if_fail (organizer);
  g_return_if_fail (SIM_IS_ORGANIZER (organizer));
  g_return_if_fail (alert);
  g_return_if_fail (SIM_IS_ALERT (alert));
  g_return_if_fail (alert->dst_ia);

  db_ossim = organizer->_priv->db_ossim;

  list = sim_container_db_host_get_plugin_sids_ul (sim_ctn,
						   db_ossim,
						   alert->dst_ia,
						   alert->plugin_id,
						   alert->plugin_sid);

  if (!list)
    return;

  while (list)
    {
      SimPluginSid *plugin_sid = (SimPluginSid *) list->data;

      alert->priority += sim_plugin_sid_get_priority (plugin_sid);
      alert->reliability += sim_plugin_sid_get_reliability (plugin_sid);

      list = list->next;
    }

  alert->alarm = TRUE;

  if (alert->priority > 5)
    alert->priority = 5;
  if (alert->reliability > 10)
    alert->reliability = 10;
}

/*
 *
 *
 *
 *
 *
 */
void
sim_organizer_calificate (SimOrganizer *organizer, 
			  SimAlert     *alert)
{
  SimDatabase     *db_ossim;
  SimHost         *host;
  SimNet          *net;
  SimCategory     *category = NULL;
  SimPluginSid    *plugin_sid;
  SimPolicy       *policy;
  SimHostLevel    *host_level;
  SimNetLevel     *net_level;
  GList           *list;
  GList           *nets;
  gint             threshold = 1;

  SimPortProtocol *pp;

  gint             date = 0;
  gint             i;
  struct tm       *loctime;
  time_t           curtime;

  g_return_if_fail (organizer != NULL);
  g_return_if_fail (SIM_IS_ORGANIZER (organizer));
  g_return_if_fail (alert != NULL);
  g_return_if_fail (SIM_IS_ALERT (alert));

  db_ossim = organizer->_priv->db_ossim;

  /*
   * get current day and current hour
   * calculate date expresion to be able to compare dates
   *
   * for example, Fri 21h = ((5 - 1) * 7) + 21 = 49
   *              Sat 14h = ((6 - 1) * 7) + 14 = 56
   */
  curtime = time (NULL);
  loctime = localtime (&curtime);
  date = ((loctime->tm_wday - 1) * 7) + loctime->tm_hour;

  plugin_sid = sim_container_get_plugin_sid_by_pky (sim_ctn, alert->plugin_id, alert->plugin_sid);
  if ((plugin_sid) && (sim_plugin_sid_get_category_id (plugin_sid) > 0))
    {
      category = sim_container_get_category_by_id (sim_ctn, sim_plugin_sid_get_category_id (plugin_sid));
      if (category)
	{
	  pp = sim_port_protocol_new (alert->dst_port, alert->protocol);
	  policy = (SimPolicy *) 
	    sim_container_get_policy_match (sim_ctn,
					    date,
					    alert->src_ia,
					    alert->dst_ia,
					    pp,
					    sim_category_get_name (category));
	  g_free (pp);
	  
	  if (policy)
	    alert->priority = sim_policy_get_priority (policy);
	}
      else
	g_message ("Category not found: %d, %d", alert->plugin_id, alert->plugin_sid);
    }

  /* Get the reliability of thr plugin sid */
  if ((alert->reliability == 1) && (alert->plugin_id != SIM_PLUGIN_ID_DIRECTIVE))
    {
      alert->reliability = sim_plugin_sid_get_reliability (plugin_sid);
    }

  alert->asset_src = 1;
  alert->asset_dst = 1;
      
  if (alert->src_ia)
    {
      /* Source Asset */
      host = (SimHost *) sim_container_get_host_by_ia (sim_ctn, alert->src_ia);
      if (host)
	alert->asset_src = sim_host_get_asset (host);
      
      alert->risk_c = ((double) (alert->priority * alert->asset_src * alert->reliability)) / 25;
      if (alert->risk_c < 0)
	alert->risk_c = 0;
      else if (alert->risk_c > 10)
	alert->risk_c = 10;


      /* Updates Host Level C */
      host_level = sim_container_get_host_level_by_ia (sim_ctn, alert->src_ia);
      if (host_level)
	{
	  sim_host_level_plus_c (host_level, alert->risk_c); /* Memory update */
	  sim_container_db_update_host_level (sim_ctn, db_ossim, host_level); /* DB update */
	}
      else
	{
	  host_level = sim_host_level_new (alert->src_ia, alert->risk_c, 1); /* Create new host_level */
	  sim_container_append_host_level (sim_ctn, host_level); /* Memory addition */
	  sim_container_db_insert_host_level (sim_ctn, db_ossim, host_level); /* DB insert */
	}
      
      /* Update Net Levels C */
      nets = sim_container_get_nets_has_ia (sim_ctn, alert->src_ia);
      list = nets;
      while (list)
	{
	  net = (SimNet *) list->data;
	  
	  net_level = sim_container_get_net_level_by_name (sim_ctn, sim_net_get_name (net));
	  if (net_level)
	    {
	      sim_net_level_plus_c (net_level, alert->risk_c); /* Memory update */
	      sim_container_db_update_net_level (sim_ctn, db_ossim, net_level); /* DB update */
	    }
	  else
	    {
	      net_level = sim_net_level_new (sim_net_get_name (net), alert->risk_c, 1);
	      sim_container_append_net_level (sim_ctn, net_level); /* Memory addition */
	      sim_container_db_insert_net_level (sim_ctn, db_ossim, net_level); /* DB insert */
	    }
	  
	  list = list->next;
	}
      g_list_free (nets);
    }

  if (alert->dst_ia)
    {

      /* Destination Asset */
      host = (SimHost *) sim_container_get_host_by_ia (sim_ctn, alert->dst_ia);
      if (host)
	alert->asset_dst = sim_host_get_asset (host);
      
      alert->risk_a = ((double) (alert->priority * alert->asset_dst * alert->reliability)) / 25;
      if (alert->risk_a < 0)
	alert->risk_a = 0;
      else if (alert->risk_a > 10)
	alert->risk_a = 10;

      /* Threshold */
      threshold = sim_container_db_get_threshold (sim_ctn, db_ossim);
      if (alert->risk_a > 2) {
	alert->alarm = TRUE;
      }

      /* Updates Host Level A */
      host_level = sim_container_get_host_level_by_ia (sim_ctn, alert->dst_ia);
      if (host_level)
	{
	  sim_host_level_plus_a (host_level, alert->risk_a); /* Memory update */
	  sim_container_db_update_host_level (sim_ctn, db_ossim, host_level); /* DB update */
	}
      else
	{
	  host_level = sim_host_level_new (alert->dst_ia, 1, alert->risk_a); /* Create new host*/
	  sim_container_append_host_level (sim_ctn, host_level); /* Memory addition */
	  sim_container_db_insert_host_level (sim_ctn, db_ossim, host_level); /* DB insert */
	}
      
      /* Update Net Levels A */
      nets = sim_container_get_nets_has_ia (sim_ctn, alert->dst_ia);
      list = nets;
      while (list)
	{
	  net = (SimNet *) list->data;
	  
	  net_level = sim_container_get_net_level_by_name (sim_ctn, sim_net_get_name (net));
	  if (net_level)
	    {
	      sim_net_level_plus_a (net_level, alert->risk_a); /* Memory update */
	      sim_container_db_update_net_level (sim_ctn, db_ossim, net_level); /* DB update */
	    }
	  else
	    {
	      net_level = sim_net_level_new (sim_net_get_name (net), 1, alert->risk_a);
	      sim_container_append_net_level (sim_ctn, net_level); /* Memory addition */
	      sim_container_db_insert_net_level (sim_ctn, db_ossim, net_level); /* DB insert */
	    }
	  
	  list = list->next;
	}
      g_list_free (nets);
      
      /* Attack Responses */
      /* Updates Host Level C */
      if ((category) && (sim_category_get_id (category) == 101))
	{
	  host_level = sim_container_get_host_level_by_ia (sim_ctn, alert->dst_ia);
	  if (host_level)
	    {
	      sim_host_level_plus_c (host_level, alert->risk_c); /* Memory update */
	      sim_container_db_update_host_level (sim_ctn, db_ossim, host_level); /* DB update */
	    }
	  else
	    {
	      host_level = sim_host_level_new (alert->dst_ia, alert->risk_c, 1); /* Create new host*/
	      sim_container_append_host_level (sim_ctn, host_level); /* Memory addition */
	      sim_container_db_insert_host_level (sim_ctn, db_ossim, host_level); /* DB insert */
	    }
	  
	  /* Update Net Levels C */
	  nets = sim_container_get_nets_has_ia (sim_ctn, alert->dst_ia);
	  list = nets;
	  while (list)
	    {
	      net = (SimNet *) list->data;
	      
	      net_level = sim_container_get_net_level_by_name (sim_ctn, sim_net_get_name (net));
	      if (net_level)
		{
		  sim_net_level_plus_c (net_level, alert->risk_c); /* Memory update */
		  sim_container_db_update_net_level (sim_ctn, db_ossim, net_level); /* DB update */
		}
	      else
		{
		  net_level = sim_net_level_new (sim_net_get_name (net), alert->risk_c, 1);
		  sim_container_append_net_level (sim_ctn, net_level); /* Memory addition */
		  sim_container_db_insert_net_level (sim_ctn, db_ossim, net_level); /* DB insert */
		}
	      
	      list = list->next;
	    }
	  g_list_free (nets);
	}
    }
}

/*
 *
 *
 *
 */
void
sim_organizer_correlation (SimOrganizer  *organizer,
			   SimAlert      *alert)
{
  SimDatabase   *db_ossim;
  GList         *list;
  GList         *stickys = NULL;
  GList         *tmp = NULL;
  SimAlert      *new_alert = NULL;
  gint           id;
  gboolean       found = FALSE;

  g_return_if_fail (organizer);
  g_return_if_fail (SIM_IS_ORGANIZER (organizer));
  g_return_if_fail (alert);
  g_return_if_fail (SIM_IS_ALERT (alert));

  db_ossim = organizer->_priv->db_ossim;

  /* Match Backlogs */
  G_LOCK (s_mutex_backlogs);
  list = sim_container_get_backlogs_ul (sim_ctn);
  while (list)
    {
      SimDirective *backlog = (SimDirective *) list->data;

      if (sim_directive_backlog_match_by_alert (backlog, alert))
	{
	  GNode         *rule_node;
	  SimRule       *rule_curr;

	  if (sim_directive_get_matched (backlog))
	    sim_container_remove_backlog_ul (sim_ctn, backlog);

	  id = sim_directive_get_id (backlog);
	  rule_node = sim_directive_get_curr_node (backlog);
	  rule_curr = sim_directive_get_curr_rule (backlog);

	  if (sim_rule_get_sticky (rule_curr))
	    stickys = g_list_append (stickys, GINT_TO_POINTER (id));

	  /* Create New Alert */
	  new_alert = sim_alert_clone(alert);
	  new_alert->type = SIM_ALERT_TYPE_DETECTOR;
	  new_alert->alarm = FALSE;
	  new_alert->plugin_id = SIM_PLUGIN_ID_DIRECTIVE;
	  new_alert->plugin_sid = sim_directive_get_id (backlog);
	  
	  /* Rule reliability */
	  if (sim_rule_get_rel_abs (rule_curr))
	    new_alert->reliability = sim_rule_get_reliability (rule_curr);
	  else
	    new_alert->reliability = sim_rule_get_reliability_relative (rule_node);
	  
	  /* Directive Priority */
	  new_alert->priority = sim_directive_get_priority (backlog);

	  sim_container_push_alert (sim_ctn, new_alert);

	  sim_container_db_update_backlog_ul (sim_ctn, db_ossim, backlog);

	  /* Children Rules with type MONITOR */
	  if (!G_NODE_IS_LEAF (rule_node))
	    {
	      GNode *children = rule_node->children;
	      while (children)
		{
		  SimRule *rule = children->data;
		  
		  if (rule->type == SIM_RULE_TYPE_MONITOR)
		    {
		      SimCommand *cmd = sim_command_new_from_rule (rule);
		      sim_server_push_session_plugin_command (sim_svr, 
							      SIM_SESSION_TYPE_SENSOR, 
							      sim_rule_get_plugin_id (rule),
							      cmd);
		    }
		  
		  children = children->next;
		}
	    } 
	  else
	    {
	      g_object_unref (backlog);
	    }
	}
      list = list->next;
    }
  g_list_free (list);
  G_UNLOCK (s_mutex_backlogs);

  /* Match Directives */
  G_LOCK (s_mutex_directives);
  list = sim_container_get_directives_ul (sim_ctn);
  while (list)
    {
      SimDirective *directive = (SimDirective *) list->data;
      id = sim_directive_get_id (directive);

      found = FALSE;
      tmp = stickys;
      while (tmp) {
	gint cmp = GPOINTER_TO_INT (tmp->data);

	if (cmp == id)
	  {
	    found = TRUE;
	    break;
	  }

	tmp = tmp->next;
      }

      if (found)
	{
	  list = list->next;
	  break;
	}

      if (sim_directive_match_by_alert (directive, alert))
	{
	  SimDirective *backlog;
	  SimRule      *rule_root;
	  GNode        *node_root;

	  GTime          time_last = time (NULL);

	  /* Create a backlog from directive */
	  backlog = sim_directive_clone (directive);
	  /* Gets the root node from backlog */
	  node_root = sim_directive_get_curr_node (backlog);
	  /* Gets the root rule from backlog */
	  rule_root = sim_directive_get_curr_rule (backlog);

	  /* Set the alert data to the rule_root */
	  sim_rule_set_alert_data (rule_root, alert);
	  
	  if (!G_NODE_IS_LEAF (node_root))
	    {
	      GNode *children = node_root->children;
	      while (children)
		{
		  SimRule *rule = children->data;

		  sim_rule_set_time_last (rule, time_last);
		  sim_directive_set_rule_vars (backlog, children);

		  if (rule->type == SIM_RULE_TYPE_MONITOR)
		    {
		      SimCommand *cmd = sim_command_new_from_rule (rule);
		      sim_server_push_session_plugin_command (sim_svr, 
							      SIM_SESSION_TYPE_SENSOR, 
							      sim_rule_get_plugin_id (rule),
							      cmd);
		    }

		  children = children->next;
		}

	      sim_container_append_backlog (sim_ctn, backlog);
	      sim_container_db_insert_backlog_ul (sim_ctn, db_ossim, backlog);
	    } 
	  else
	    {
	      sim_directive_set_matched (backlog, TRUE);
	      sim_container_db_insert_backlog_ul (sim_ctn, db_ossim, backlog);

	      new_alert = sim_alert_clone (alert);
	      new_alert->type = SIM_ALERT_TYPE_DETECTOR;
	      new_alert->alarm = FALSE;
	      new_alert->plugin_id = SIM_PLUGIN_ID_DIRECTIVE;
	      new_alert->plugin_sid = sim_directive_get_id (backlog);

	      /* Rule reliability */
	      if (sim_rule_get_rel_abs (rule_root))
		new_alert->reliability = sim_rule_get_reliability (rule_root);
	      else
		new_alert->reliability = sim_rule_get_reliability_relative (node_root);
	      
	      /* Directive Priority */
	      new_alert->priority = sim_directive_get_priority (backlog);

	      sim_container_push_alert (sim_ctn, new_alert);

	      g_object_unref (backlog);
	    }
	}
      list = list->next;
    }
  g_list_free (list);
  G_UNLOCK (s_mutex_directives);

  g_list_free (stickys);
}

/*
 *
 *
 *
 *
 */
void
sim_organizer_snort_ossim_event_insert (SimDatabase  *db_snort,
					SimAlert     *alert,
					gint          sid,
					gint          cid)
{
  gchar         *insert;
  gint           c;
  gint           a;

  g_return_if_fail (db_snort);
  g_return_if_fail (SIM_IS_DATABASE (db_snort));
  g_return_if_fail (alert);
  g_return_if_fail (SIM_IS_ALERT (alert));
  g_return_if_fail (sid > 0);
  g_return_if_fail (cid > 0);

  c = rint (alert->risk_c);
  a = rint (alert->risk_a);

  /* insert OSSIM Alert */
  insert = g_strdup_printf ("INSERT INTO ossim_event (sid, cid, type, priority, reliability, asset_src, asset_dst, risk_c, risk_a) "
			    "VALUES (%u, %u, %u, %u, %u, %u, %u, %u, %u)", sid, cid,
			    (alert->alarm) ? 2 : 1,
			    alert->priority, alert->reliability,
			    alert->asset_src, alert->asset_dst, c, a);

  sim_database_execute_no_query (db_snort, insert);
  g_free (insert);
}

/*
 *
 *
 *
 */
gint
sim_organizer_snort_sersor_get_sid (SimDatabase  *db_snort,
				    gchar        *hostname,
				    gchar        *interface,
				    gchar        *plugin_name)
{
  GdaDataModel  *dm;
  GdaValue      *value;
  gint64         sid;
  gchar         *query;
  gchar         *insert;
  gint           row;
  
  g_return_val_if_fail (db_snort, 0);
  g_return_val_if_fail (SIM_IS_DATABASE (db_snort), 0);
  g_return_val_if_fail (hostname, 0);
  g_return_val_if_fail (interface, 0);

  /* SID */
  if (plugin_name)
    query = g_strdup_printf ("SELECT sid FROM sensor WHERE hostname = '%s-%s' AND interface = '%s'", 
			   hostname, plugin_name, interface);
  else
    query = g_strdup_printf ("SELECT sid FROM sensor WHERE hostname = '%s' AND interface = '%s'", 
			   hostname, interface);

  dm = sim_database_execute_single_command (db_snort, query);
  if (dm)
    {
      if (gda_data_model_get_n_rows (dm))
	{
	  value = (GdaValue *) gda_data_model_get_value_at (dm, 0, 0);
	  sid = gda_value_get_integer (value);
	}
      else
	{
	  if (plugin_name)
	    insert = g_strdup_printf ("INSERT INTO sensor (hostname, interface, last_cid) "
				      "VALUES ('%s-%s', '%s', 0)", hostname, plugin_name, interface);
	  else
	    insert = g_strdup_printf ("INSERT INTO sensor (hostname, interface, last_cid) "
				      "VALUES ('%s', '%s', 0)", hostname, interface);
	  
	  sim_database_execute_no_query (db_snort, insert);
	  g_free (insert);

	  sid = sim_organizer_snort_sersor_get_sid (db_snort, hostname, interface, plugin_name);
	}

      g_object_unref(dm);
    }
  else
    {
      g_message ("SENSOR SID DATA MODEL ERROR");
    }
  g_free (query);

  return sid;
}

/*
 *
 *
 *
 *
 */
gint
sim_organizer_snort_event_get_max_cid (SimDatabase  *db_snort,
					 gint          sid)
{
  GdaDataModel  *dm;
  GdaValue      *value;
  gint           last_cid = 0;
  gchar         *query;
  gint           row;

  g_return_val_if_fail (db_snort, 0);
  g_return_val_if_fail (SIM_IS_DATABASE (db_snort), 0);
  g_return_val_if_fail (sid > 0, 0);

  /* CID */
  query = g_strdup_printf ("SELECT max(cid) FROM event WHERE sid = %d", sid);
  dm = sim_database_execute_single_command (db_snort, query);
  if (dm)
    {
      for (row = 0; row < gda_data_model_get_n_rows (dm); row++)
	{
	  value = (GdaValue *) gda_data_model_get_value_at (dm, 0, row);
	  if (!gda_value_is_null (value))
	    last_cid = gda_value_get_integer (value);
	}
      
      g_object_unref(dm);
    }
  else
    {
      g_message ("LAST CID DATA MODEL ERROR");
    }
  g_free (query);

  return last_cid;
}

/*
 *
 *
 *
 *
 */
void
sim_organizer_snort_sensor_update_last_cid (SimDatabase  *db_snort,
					    gint          sid,
					    gint          cid)
{
  gchar         *update;

  g_return_if_fail (db_snort);
  g_return_if_fail (SIM_IS_DATABASE (db_snort));
  g_return_if_fail (sid > 0);
  g_return_if_fail (cid > 0);

  /* insert OSSIM Alert */
  update = g_strdup_printf ("UPDATE sensor SET last_cid = %u WHERE sid = %u ",
			    cid, sid);

  sim_database_execute_no_query (db_snort, update);
  g_free (update);
}

/*
 *
 *
 *
 *
 */
gint
sim_organizer_snort_signature_get_id (SimDatabase  *db_snort,
				      gchar        *name)
{
  GdaDataModel  *dm;
  GdaValue      *value;
  gint           sig_id;
  gchar         *query;
  gchar         *insert;
  gint           row;

  g_return_val_if_fail (db_snort, 0);
  g_return_val_if_fail (SIM_IS_DATABASE (db_snort), 0);
  g_return_val_if_fail (name, 0);

  /* SID */
  query = g_strdup_printf ("SELECT sig_id FROM signature WHERE sig_name = '%s'", name);
  dm = sim_database_execute_single_command (db_snort, query);
  if (dm)
    {
      if (gda_data_model_get_n_rows (dm))
	{
	  value = (GdaValue *) gda_data_model_get_value_at (dm, 0, 0);
	  sig_id = gda_value_get_integer (value);
	}
      else
	{
	  insert = g_strdup_printf ("INSERT INTO signature (sig_name, sig_class_id) "
				    "VALUES ('%s', 0)", name);
	  sim_database_execute_no_query (db_snort, insert);
	  g_free (insert);

	  sig_id = sim_organizer_snort_signature_get_id (db_snort, name);
	}
      
      g_object_unref(dm);
    }
  else
    {
      g_message ("SIG ID DATA MODEL ERROR");
    }
  g_free (query);

  return sig_id;  
}

/*
 *
 *
 *
 */
void
sim_organizer_snort_event_insert (SimDatabase  *db_snort,
				  SimAlert     *alert,
				  gint          sid,
				  gint          cid,
				  gint          sig_id)
{
  gchar timestamp[TIMEBUF_SIZE];
  gchar *query;

  g_return_if_fail (db_snort);
  g_return_if_fail (SIM_IS_DATABASE (db_snort));
  g_return_if_fail (alert);
  g_return_if_fail (SIM_IS_ALERT (alert));
  g_return_if_fail (sid > 0);
  g_return_if_fail (cid > 0);
  g_return_if_fail (sig_id > 0);

  strftime (timestamp, TIMEBUF_SIZE, "%Y-%m-%d %H:%M:%S", localtime ((time_t *) &alert->time));

  query = g_strdup_printf ("INSERT INTO event (sid, cid, signature, timestamp) "
			   "VALUES (%u, %u, %u, '%s')",
			   sid, cid, sig_id, timestamp);
  sim_database_execute_no_query (db_snort, query);
  g_free (query);

  query = g_strdup_printf ("INSERT INTO iphdr (sid, cid, ip_src, ip_dst, ip_proto) "
			   "VALUES (%u, %u, %lu, %lu, %d)",
			   sid, cid,
			   (alert->src_ia) ? sim_inetaddr_ntohl (alert->src_ia) : -1,
			   (alert->dst_ia) ? sim_inetaddr_ntohl (alert->dst_ia) : -1,
			   alert->protocol);
  sim_database_execute_no_query (db_snort, query);
  g_free (query);

  switch (alert->protocol)
    {
    case SIM_PROTOCOL_TYPE_ICMP:
      query = g_strdup_printf ("INSERT INTO icmphdr (sid, cid, icmp_type, icmp_code) "
			       "VALUES (%u, %u, 8, 0)",
			       sid, cid);
      sim_database_execute_no_query (db_snort, query);
      g_free (query);
      break;
    case SIM_PROTOCOL_TYPE_TCP:
      query = g_strdup_printf ("INSERT INTO tcphdr (sid, cid, tcp_sport, tcp_dport, tcp_flags) "
			       "VALUES (%u, %u, %d, %d, 24)",
			       sid, cid, alert->src_port, alert->dst_port);
      sim_database_execute_no_query (db_snort, query);
      g_free (query);
      break;
    case SIM_PROTOCOL_TYPE_UDP:
      query = g_strdup_printf ("INSERT INTO udphdr (sid, cid, udp_sport, udp_dport) "
			       "VALUES (%u, %u, %d, %d)",
			       sid, cid, alert->src_port, alert->dst_port);
      sim_database_execute_no_query (db_snort, query);
      g_free (query);
      break;
    default:
      break;
    }

  sim_organizer_snort_ossim_event_insert (db_snort, alert, sid, cid);
}

/*
 *
 *
 *
 */
void
sim_organizer_snort_event_get_cid_from_alert (SimDatabase  *db_snort,
					      SimAlert     *alert,
					      gint          sid,
					      gint          sig_id)
{
  GdaDataModel  *dm;
  GdaValue      *value;
  gchar          timestamp[TIMEBUF_SIZE];
  GString       *select;
  GString       *where;
  gint           row;
  gint           cid;

  g_return_if_fail (db_snort);
  g_return_if_fail (SIM_IS_DATABASE (db_snort));
  g_return_if_fail (alert);
  g_return_if_fail (SIM_IS_ALERT (alert));
  g_return_if_fail (sid > 0);
  g_return_if_fail (sig_id > 0);

  strftime (timestamp, TIMEBUF_SIZE, "%Y-%m-%d %H:%M:%S", localtime ((time_t *) &alert->time));

  select = g_string_new ("SELECT event.cid FROM event LEFT JOIN iphdr ON (event.sid = iphdr.sid AND event.cid = iphdr.cid)");
  where = g_string_new (" WHERE");

  g_string_append_printf (where, " event.sid = %u", sid);
  g_string_append_printf (where, " AND event.signature = %u", sig_id);
  g_string_append_printf (where, " AND event.timestamp = '%s'", timestamp);

  if (alert->src_ia)
    g_string_append_printf (where, " AND ip_src = %lu", sim_inetaddr_ntohl (alert->src_ia));
  if (alert->dst_ia)
    g_string_append_printf (where, " AND ip_dst = %lu", sim_inetaddr_ntohl (alert->dst_ia));
  
  g_string_append_printf (where, " AND ip_proto = %d", alert->protocol);

  switch (alert->protocol)
    {
    case SIM_PROTOCOL_TYPE_ICMP:
      break;
    case SIM_PROTOCOL_TYPE_TCP:
      g_string_append (select, " LEFT JOIN tcphdr ON (event.sid = tcphdr.sid AND event.cid = tcphdr.cid)");

      if (alert->src_port)
	g_string_append_printf (where, " AND tcp_sport = %d", alert->src_port);
      if (alert->dst_port)
	g_string_append_printf (where, " AND tcp_dport = %d", alert->dst_port);
      break;
    case SIM_PROTOCOL_TYPE_UDP:
      g_string_append (select, " LEFT JOIN udphdr ON (event.sid = udphdr.sid AND event.cid = udphdr.cid)");

      if (alert->src_port)
	g_string_append_printf (where, " AND udp_sport = %d ", alert->src_port);
      if (alert->dst_port)
	g_string_append_printf (where, " AND udp_dport = %d ", alert->dst_port);
      break;
    default:
      break;
    }

  g_string_append (select, where->str);

  g_string_free (where, TRUE);

  dm = sim_database_execute_single_command (db_snort, select->str);
  if (dm)
    {
      for (row = 0; row < gda_data_model_get_n_rows (dm); row++)
	{
	  value = (GdaValue *) gda_data_model_get_value_at (dm, 0, row);
	  cid = gda_value_get_integer (value);
	 
	  sim_organizer_snort_ossim_event_insert (db_snort, alert, sid, cid);
	}
      
      g_object_unref(dm);
    }
  else
    {
      g_message ("ALERTS ID DATA MODEL ERROR");
    }

  g_string_free (select, TRUE);
}

/*
 *
 *
 *
 *
 *
 */
void
sim_organizer_snort (SimOrganizer  *organizer,
		     SimAlert      *alert)
{
  SimPlugin     *plugin;
  SimPluginSid  *plugin_sid;
  gint64         last_id;
  gchar         *query;
  gint           sid;
  gint           cid;
  gint           sig_id;
  GList         *alerts = NULL;
  GList         *list = NULL;

  g_return_if_fail (organizer);
  g_return_if_fail (SIM_IS_ORGANIZER (organizer));
  g_return_if_fail (alert);
  g_return_if_fail (SIM_IS_ALERT (alert));
  g_return_if_fail (alert->sensor);
  g_return_if_fail (alert->interface);

  plugin_sid = sim_container_get_plugin_sid_by_pky (sim_ctn, alert->plugin_id, alert->plugin_sid);
  if (!plugin_sid)
    {
      g_message ("sim_organizer_snort: Error Plugin %d, PlugginSid %d", alert->plugin_id, alert->plugin_sid);
      return;
    }
  sig_id = sim_organizer_snort_signature_get_id (organizer->_priv->db_snort,
						 sim_plugin_sid_get_name (plugin_sid));

  /* Alerts SNORT */
  if ((alert->plugin_id >= 1001) && (alert->plugin_id < 1500))
    {
      sid = sim_organizer_snort_sersor_get_sid (organizer->_priv->db_snort,
						alert->sensor,
						alert->interface,
						NULL);
      sim_organizer_snort_event_get_cid_from_alert (organizer->_priv->db_snort,
						    alert, sid, sig_id);
    }
  else /* Others Alerts */
    {
      plugin = sim_container_get_plugin_by_id (sim_ctn, alert->plugin_id);
      sid = sim_organizer_snort_sersor_get_sid (organizer->_priv->db_snort,
						alert->sensor,
						alert->interface,
						sim_plugin_get_name (plugin));
      cid = sim_organizer_snort_event_get_max_cid (organizer->_priv->db_snort,
						     sid);
      sim_organizer_snort_event_insert (organizer->_priv->db_snort,
					alert, sid, ++cid, sig_id);
    }
}


