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


#include "os-sim.h"
#include "sim-organizer.h"
#include "sim-server.h"
#include "sim-host.h"
#include "sim-net.h"
#include "sim-plugin-sid.h"
#include "sim-policy.h"
#include "sim-rule.h"
#include "sim-directive-group.h"
#include "sim-directive.h"
#include "sim-host-level.h"
#include "sim-net-level.h"
#include "sim-connect.h"
#include <math.h>
#include <time.h>
#include <config.h>

extern SimMain  ossim;

enum 
{
  DESTROY,
  LAST_SIGNAL
};

struct _SimOrganizerPrivate {
  SimConfig	*config;
};

static gpointer parent_class = NULL;
static gint sim_container_signals[LAST_SIGNAL] = { 0 };


void
config_send_notify_email (SimConfig	*config,
			  SimEvent	*event);
void
insert_event_alarm (SimEvent	*event);

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
sim_organizer_new (SimConfig	*config)
{
  SimOrganizer *organizer = NULL;

  g_return_val_if_fail (config, NULL);
  g_return_val_if_fail (SIM_IS_CONFIG (config), NULL);

  organizer = SIM_ORGANIZER (g_object_new (SIM_TYPE_ORGANIZER, NULL));
  organizer->_priv->config = config;

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
  SimEvent *event = NULL;
  SimCommand *cmd = NULL;
  gchar    *query;
  gchar		*str;

  g_return_if_fail (organizer != NULL);
  g_return_if_fail (SIM_IS_ORGANIZER (organizer));

	GInetAddr *ia_zero = gnet_inetaddr_new_nonblock ("0.0.0.0", 0);

  while (TRUE) 
  {
    event =  sim_container_pop_event (ossim.container);//gets and remove the last event in queue 
		sim_server_debug_print_sessions (ossim.server);
      
    if (!event)
		{
		  continue;
		}

    if (event->type == SIM_EVENT_TYPE_NONE)
		{
	  	g_object_unref (event);
		  continue;
		}

/********debug******/
      str = sim_event_to_string (event); 
      g_message ("sim_organizer_run before calculations: %s", str);
      g_free (str);
/*********************/      
      
    if (!gnet_inetaddr_noport_equal(event->dst_ia, ia_zero))
			sim_organizer_correlation_plugin (organizer, event);  //Actualize priority and reliability. Also, event -> alarm. 
    sim_organizer_calificate (organizer, event);				//Actualice priority (if match with some policy), C&A, event->alarm
    sim_organizer_snort (organizer, event); 						//Insert the snort OR other event into DB
    sim_organizer_rrd (organizer, event);
    insert_event_alarm (event);													//Insert alarm & assign event->id 
    sim_organizer_correlation (organizer, event);

    str = sim_event_to_string (event); 
    g_message ("sim_organizer_run after calculations: %s", str);
    g_free (str);

    if (event->alarm) 
		{
			cmd = sim_command_new ();
			cmd->type = SIM_COMMAND_TYPE_EVENT;
			cmd->data.event.event = event;
			sim_server_push_session_command (ossim.server, SIM_SESSION_TYPE_RSERVER, cmd);
			g_object_unref (cmd);
	    sim_connect_send_alarm (organizer->_priv->config,event);
    }

    g_object_unref (event);
  }
}

/*
 *
 * This is usefull only if the event has the "Alarm" flag.
 * We also assign here an event->id (if it hasn't got anyone, like the first time the event arrives)
 *
 */
void
insert_event_alarm (SimEvent	*event)
{
  GdaDataModel	*dm;
  GdaValue	*value;
  guint		backlog_id = 0;
  gint		row;
  gchar		*query0;
  gchar		*query1;

  if (!event->alarm)
    return;

				
  if (!event->id)	
	{
    sim_container_db_insert_event_ul (ossim.container, ossim.dbossim, event); //the event (wooops, I mean, the Alarm) is new
		g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "insert_event_alarm: Inserting new event Into Alarms");
	}
  else
	{
    sim_container_db_update_event_ul (ossim.container, ossim.dbossim, event); //update the event (it depends on event id)
		g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "insert_event_alarm: Updating event Into Alarms. event->id: %d",event->id);
	}

  query0 = g_strdup_printf ("SELECT backlog_id, MAX(event_id) from backlog_event GROUP BY backlog_id HAVING MAX(event_id) = %d",  event->id); //One backlog_id can handle multiple event_id's. We choose the bigger event_id if it coincides with the event->id.
  dm = sim_database_execute_single_command (ossim.dbossim, query0);
  if (dm)
  {
    if (!gda_data_model_get_n_rows (dm)) //first event inserted as alarm
		{
		  if (event->backlog_id) 
	  	{
	      query1 = g_strdup_printf ("DELETE FROM alarm WHERE backlog_id = %lu", event->backlog_id);
	      sim_database_execute_no_query (ossim.dbossim, query1);
	      g_free (query1);
	    }

	  	query1 = sim_event_get_alarm_insert_clause (event);
		  sim_database_execute_no_query (ossim.dbossim, query1);
		  g_free (query1);
		}

    for (row = 0; row < gda_data_model_get_n_rows (dm); row++) //All the events (the alarms, in fact) enter here (except the first)
		{
		  value = (GdaValue *) gda_data_model_get_value_at (dm, 0, row);
	  	if (!gda_value_is_null (value))
	    	backlog_id = gda_value_get_bigint (value);
	 
		  query1 = g_strdup_printf ("DELETE FROM alarm WHERE backlog_id = %lu", backlog_id);
		  sim_database_execute_no_query (ossim.dbossim, query1);
	  	g_free (query1);
	  
		  event->backlog_id = backlog_id;
		  query1 = sim_event_get_alarm_insert_clause (event);
	  	sim_database_execute_no_query (ossim.dbossim, query1);
		  g_free (query1);

		}

    g_object_unref(dm);
  }
  else
    g_message ("ORGANIZER ALARM INSERT DATA MODEL ERROR");
			
  g_free (query0);

  query0 = g_strdup_printf ("SELECT COUNT(backlog_id) FROM alarm WHERE backlog_id != 0 GROUP BY event_id HAVING COUNT(backlog_id) > 2");
  g_free (query0);
}


/*
 * FIXME: This function isn't called from anywhere at this time.
 *
 *
 */
void
config_send_notify_email (SimConfig    *config,
			  SimEvent     *event)
{
  SimAlarmRiskType type = SIM_ALARM_RISK_TYPE_NONE;
  GList     *notifies;
  gint       risk;

  g_return_if_fail (config);
  g_return_if_fail (SIM_IS_CONFIG (config));

  if (!config->notifies)
    return;

  risk = rint (event->risk_a);
  type = sim_get_alarm_risk_from_risk (risk);

  notifies = config->notifies;
  while (notifies)
    {
      SimConfigNotify *notify = (SimConfigNotify *) notifies->data;

      GList *risks = notify->alarm_risks;
      while (risks)
	{
	  risk = GPOINTER_TO_INT (risks->data);

	  if (risk == SIM_ALARM_RISK_TYPE_ALL || risk == type)
	    {
	      gchar *tmpname;
	      gchar *cmd;
	      gchar *msg;
	      gint   fd;

	      tmpname = g_strdup ("/tmp/ossim-mail.XXXXXX");
	      fd = g_mkstemp (tmpname);
	      
	      msg = g_strdup_printf ("Subject: OSSIM ALARM RISK (%d)\n", risk);
	      write (fd, msg, strlen(msg));
	      g_free (msg);
	      write (fd, "\n", 1);

	      msg = sim_event_get_msg (event);
	      write (fd, msg, strlen(msg));
	      g_free (msg);

	      write (fd, ".\n", 2);
	      
	      cmd = g_strdup_printf ("%s %s < %s", config->notify_prog, 
				     notify->emails, tmpname);
	      system (cmd);
	      g_free (cmd);
	      
	      close (fd);
	      unlink(tmpname);
	      g_free (tmpname);
	    }

	  risks = risks->next;
	}
      notifies = notifies->next;
    }
}

/*
 *
 * Actualize the reliability and the priority of all the plugin_sids of the dst_ia from the Event.
 * Also, if the event has plugin_sids associated, the event is transformed into an Alarm.
 * This function has sense just with events with a defined dst.
 */
void
sim_organizer_correlation_plugin (SimOrganizer *organizer, 
																  SimEvent     *event)
{
  GList           *list;

  g_return_if_fail (organizer);
  g_return_if_fail (SIM_IS_ORGANIZER (organizer));
  g_return_if_fail (event);
  g_return_if_fail (SIM_IS_EVENT (event));

  if (!event->dst_ia) return;
  if (event->rserver) return;

  list = sim_container_db_host_get_plugin_sids_ul (ossim.container,
						   ossim.dbossim,
						   event->dst_ia,
						   event->plugin_id,
						   event->plugin_sid);

  if (!list) //if there aren't any plugin_sid associated with the dst_ia...
    return;

  // actualize the reliability and the priority of all the pluginsids of a specific dst_ia.
  while (list)
  {
    SimPluginSid *plugin_sid = (SimPluginSid *) list->data;
    gint  new_priority = sim_plugin_sid_get_priority (plugin_sid);

    event->priority = (new_priority > event->priority) ? new_priority : event->priority; //takes the greatest priority
    event->reliability += sim_plugin_sid_get_reliability (plugin_sid); //reliability of the plugin_sid is added to the event reliability

    list = list->next;
  }
  g_list_free (list);

  event->alarm = TRUE;

  if (event->priority > 5)
    event->priority = 5;
  if (event->reliability > 10)
    event->reliability = 10;
}

/*
 * 1.- Modifies the priority if the event belongs to a policy
 * 2.- Update everything's C and A
 * 3.- If Risk >= 2 then transform the event into an alarm
 * 4.- Tells if the event must be stored in DB or not (thanks to its policy)
 *
 */
void
sim_organizer_calificate (SimOrganizer *organizer, 
												  SimEvent     *event)
{
  SimHost         *host;
  SimNet          *net;
  SimPlugin       *plugin;
  SimPluginSid    *plugin_sid=NULL;
  SimPolicy       *policy;
  SimHostLevel    *host_level;
  SimNetLevel     *net_level;
  GList           *list;
  GList           *nets; //SimNet
  gint             threshold = 1;
  gint             asset_net = 1;

  SimPortProtocol *pp;

  gint             date = 0;
  gint             i;
  struct tm       *loctime;
  time_t           curtime;

  g_return_if_fail (organizer != NULL);
  g_return_if_fail (SIM_IS_ORGANIZER (organizer));
  g_return_if_fail (event != NULL);
  g_return_if_fail (SIM_IS_EVENT (event));

  if (event->rserver) return;

  /*
   * get current day and current hour
   * calculate date expresion to be able to compare dates
   *
   * for example, Fri 21h = ((5 - 1) * 24) + 21 = 117
	 *              Sat 14h = ((6 - 1) * 24) + 14 = 134
	 */
  curtime = time (NULL);
  loctime = localtime (&curtime);
  date = ((loctime->tm_wday - 1) * 24) + loctime->tm_hour;

  //get plugin and plugin-sid objects from the plugin_id and plugin_sid of the event
  plugin = sim_container_get_plugin_by_id (ossim.container, event->plugin_id);
  plugin_sid = sim_container_get_plugin_sid_by_pky (ossim.container, event->plugin_id, event->plugin_sid);
  if (!plugin_sid)
  {
    g_message ("sim_organizer_calificate: Error Plugin %d, PluginSid %d", event->plugin_id, event->plugin_sid);
    return;
  }

	//get the port/protocol used to obtain the policy that matches.
	pp = sim_port_protocol_new (event->dst_port, event->protocol);
	policy = (SimPolicy *) 
  sim_container_get_policy_match (ossim.container,  //Check if some policy applies, so we get the new priority
															    date,
															    event->src_ia, 
															    event->dst_ia,
															    pp,
																	event->sensor,
																	event->plugin_id,
																	event->plugin_sid);
  g_free (pp);
	  
  if (policy)
	{
		 gint aux;
     if ((aux = sim_policy_get_priority (policy)) != -1) //-1 means that it won't affect to the priority
	     event->priority = aux;
		 event->store_in_DB = sim_policy_get_store (policy);
     g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_organizer_calificate: Policy Match. new priority: %d", event->priority);
	   g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_organizer_calificate: Store. new stored: %d", event->store_in_DB);
   }
	 else
		 g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_organizer_calificate: Policy Doesn't match");
	
  event->plugin = plugin;
  event->pluginsid = plugin_sid;
	
	if (event->plugin_id == 0)
		event->store_in_DB = FALSE;

  // Get the reliability of the plugin sid. There are a reliability inside the directive, but the
	// reliability from the plugin is more important. So we usually try to get the plugin reliability
	// and assign it to the event.
  if ((event->reliability == 1) && (event->plugin_id != SIM_PLUGIN_ID_DIRECTIVE))
  {
	  gint aux;
	  if ( (aux = sim_plugin_sid_get_reliability (plugin_sid)) != -1)
		  event->reliability = aux;								
  }

  event->asset_src = 2; 
  event->asset_dst = 2;

	// if the priority from the event is 0 (extracted from the plugin_sid and stored in the event in sim_session_cmd_event()),
	// then the event won't update the C or A.
	// Also, if the destination IP is "0.0.0.0" (p0f, MAc events...) it won't increase the C or A level.
	// FIXME: plugin_id doesn't matters because it hasn't got priority by itsel, although it should!.
	if (event->priority != 0)
	{	
	
  if ((event->src_ia) && (gnet_inetaddr_get_canonical_name(event->src_ia))) //error checking
  {
    g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_organizer_calificate event->src_ia: -%s-",gnet_inetaddr_get_canonical_name(event->src_ia));
          
    /* Source Asset */
    host = sim_container_get_host_by_ia (ossim.container, event->src_ia);
    nets = sim_container_get_nets_has_ia (ossim.container, event->src_ia);

    if (host) //writes the event->asset_src choosing between host (if available) or net.
      event->asset_src = sim_host_get_asset (host);
    else if (nets)
    {
	    list = nets;
  	  while (list)
	    {
	      net = (SimNet *) list->data;
	      asset_net = sim_net_get_asset (net);
	      if (asset_net > event->asset_src)
					event->asset_src = asset_net;

	      list = list->next;
	    }
    }

		//check if the source could be an alarm. This our (errr) "famous" formula!
    event->risk_c = ((double) (event->priority * event->asset_src * event->reliability)) / 10;
    if (event->risk_c < 0)
			event->risk_c = 0;
    else 
		if (event->risk_c > 10)
			event->risk_c = 10;
      
    if (event->risk_c >= 2) 
			event->alarm = TRUE;

    /* Updates Host Level C */
    host_level = sim_container_get_host_level_by_ia (ossim.container, event->src_ia);
    if (host_level)
		{
	    sim_host_level_plus_c (host_level, (event->risk_c / 2.5)); /* Memory update */
  	  sim_container_db_update_host_level (ossim.container, ossim.dbossim, host_level); /* DB update */
	  }
    else
  	{
	    if (host_level = sim_host_level_new (event->src_ia, (event->risk_c / 2.5), 1)) /* Create new host_level */
			{
  		  sim_container_append_host_level (ossim.container, host_level); 							/* Memory addition */
		    sim_container_db_insert_host_level (ossim.container, ossim.dbossim, host_level); /* DB insert */
			}
  	}
      
    /* Update Net Levels C */
    list = nets;
    while (list)
  	{
		  net = (SimNet *) list->data;
	  
	  	net_level = sim_container_get_net_level_by_name (ossim.container, sim_net_get_name (net));
		  if (net_level)
	  	{
	    	sim_net_level_plus_c (net_level, (event->risk_c / 2.5)); /* Memory update */
		    sim_container_db_update_net_level (ossim.container, ossim.dbossim, net_level); /* DB update */
		  }
	  	else
		  {
		    net_level = sim_net_level_new (sim_net_get_name (net), (event->risk_c / 2.5), 1);
	  	  sim_container_append_net_level (ossim.container, net_level); /* Memory addition */
	    	sim_container_db_insert_net_level (ossim.container, ossim.dbossim, net_level); /* DB insert */
		  }

		  list = list->next;
		}
  	g_list_free (nets);
  }

	//if destination is "0.0.0.0", it will be very probably a MAC, a OS event, or something like that. And we shouldn't
	//update the C & A of destination because it doesn't exists.
	gchar *aux = gnet_inetaddr_get_canonical_name(event->dst_ia);  
	if ((event->dst_ia) && (aux) && (strcmp (aux, "0.0.0.0")))
  {
    g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_organizer_calificate event->dst_ia: %s",gnet_inetaddr_get_canonical_name(event->dst_ia));
          
    /* Destination Asset */
    host = (SimHost *) sim_container_get_host_by_ia (ossim.container, event->dst_ia);
    nets = sim_container_get_nets_has_ia (ossim.container, event->dst_ia);

    if (host)
			event->asset_dst = sim_host_get_asset (host);
    else 
		if (nets)
  	{
	  	list = nets;
		  while (list)
	    {
	      net = (SimNet *) list->data;
	      asset_net = sim_net_get_asset (net);
	      if (asset_net > event->asset_dst)
					event->asset_dst = asset_net;
	      list = list->next;
	    }
		}
      
    event->risk_a = ((double) (event->priority * event->asset_dst * event->reliability)) / 10;
    if (event->risk_a < 0)
			event->risk_a = 0;
    else
		if (event->risk_a > 10)
			event->risk_a = 10;
		
		if (event->risk_a >= 2) 
			event->alarm = TRUE;
    
    /* Threshold */
    threshold = sim_container_db_get_threshold (ossim.container, ossim.dbossim);

    /* Updates Host Level A */
    host_level = sim_container_get_host_level_by_ia (ossim.container, event->dst_ia);
    if (host_level)
		{
		  sim_host_level_plus_a (host_level, (event->risk_a / 2.5)); /* Memory update */
	  	sim_container_db_update_host_level (ossim.container, ossim.dbossim, host_level); /* DB update */
		}
    else
		{
	  	host_level = sim_host_level_new (event->dst_ia, 1, (event->risk_a / 2.5)); /* Create new host*/
		  sim_container_append_host_level (ossim.container, host_level); /* Memory addition */
		  sim_container_db_insert_host_level (ossim.container, ossim.dbossim, host_level); /* DB insert */
		}
      
    /* Update Net Levels A */
    list = nets;
    while (list)
		{
	  	net = (SimNet *) list->data;
	  
		  net_level = sim_container_get_net_level_by_name (ossim.container, sim_net_get_name (net));
		  if (net_level)
	    {
	      sim_net_level_plus_a (net_level, (event->risk_a / 2.5)); /* Memory update */
	      sim_container_db_update_net_level (ossim.container, ossim.dbossim, net_level); /* DB update */
	    }
	  	else
	    {
	      net_level = sim_net_level_new (sim_net_get_name (net), 1, (event->risk_a / 2.5));
	      sim_container_append_net_level (ossim.container, net_level); /* Memory addition */
	      sim_container_db_insert_net_level (ossim.container, ossim.dbossim, net_level); /* DB insert */
	    }
	  
	  	list = list->next;
		}
    g_list_free (nets);
    
  }

	} //end event->priority != 0
}

/*
 *
 *
 *
 */
void
sim_organizer_correlation (SimOrganizer  *organizer,
												   SimEvent      *event)
{
  SimConfig     *config;
  GList         *groups = NULL;
  GList         *lgs = NULL;
  GList         *list = NULL;
  GList					*removes = NULL;
  GList         *stickys = NULL;
  GList         *tmp = NULL;
  SimEvent      *new_event = NULL;
  gint           id;
  gboolean       found = FALSE;
  gboolean       inserted;
  GInetAddr     *ia = NULL;

  g_return_if_fail (organizer);
  g_return_if_fail (SIM_IS_ORGANIZER (organizer));
  g_return_if_fail (event);
  g_return_if_fail (SIM_IS_EVENT (event));

  config = organizer->_priv->config;

  /* Match Backlogs */
  g_mutex_lock (ossim.mutex_backlogs);
  list = sim_container_get_backlogs_ul (ossim.container); //1st time the server runs, this is empty
  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_organizer_correlation: BEGIN backlogs %d", g_list_length (list));
  while (list)
  {
    SimDirective *backlog = (SimDirective *) list->data;
    id = sim_directive_get_id (backlog);

    inserted = FALSE;

		//if this is true (we check it aginst the children), inside sim_directive_backlog_match_by_event
		//we go down one level
		if (sim_directive_backlog_match_by_event (backlog, event))	
		{
		  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_directive_backlog_match_by_event TRUE. event->id: %d, id: %d, backlog_id : %d",event->id, sim_directive_get_id(backlog), sim_directive_get_backlog_id(backlog));
				
		  GNode         *rule_node;
		  SimRule       *rule_root;
	  	SimRule       *rule_curr;

		  rule_node = sim_directive_get_curr_node (backlog);
		  rule_root = sim_directive_get_root_rule (backlog);
	  	rule_curr = sim_directive_get_curr_rule (backlog);

		  event->matched = TRUE;

		  /* Create New Event */
	  	new_event = sim_event_new ();
		  new_event->type = SIM_EVENT_TYPE_DETECTOR;
		  new_event->time = time (NULL);

	  	if (config->sensor.ip)
	    	new_event->sensor = g_strdup (config->sensor.ip);
			if (config->sensor.interface)
		    new_event->interface = g_strdup (config->sensor.interface);
	  
			new_event->plugin_id = SIM_PLUGIN_ID_DIRECTIVE;
			new_event->plugin_sid = sim_directive_get_id (backlog);
			
			if ((ia = sim_rule_get_src_ia (rule_root))) 
				new_event->src_ia = gnet_inetaddr_clone (ia);
			if ((ia = sim_rule_get_dst_ia (rule_root))) 
				new_event->dst_ia = gnet_inetaddr_clone (ia);
			new_event->src_port = sim_rule_get_src_port (rule_root);
			new_event->dst_port = sim_rule_get_dst_port (rule_root);
			new_event->protocol = sim_rule_get_protocol (rule_root);
			new_event->data = sim_directive_backlog_to_string (backlog);
			if ((ia = sim_rule_get_sensor (rule_root))) 
				new_event->sensor = gnet_inetaddr_get_canonical_name (ia);

			new_event->alarm = FALSE;
			new_event->level = event->level;

			event->backlog_id = sim_directive_get_backlog_id (backlog);	//as the event generated belongs to the directive, the event must know
																																	//which is the backlog_id of that directive.
			new_event->backlog_id = event->backlog_id;

			/* Rule reliability */
			if (sim_rule_get_rel_abs (rule_curr))
				new_event->reliability = sim_rule_get_reliability (rule_curr);
			else
				new_event->reliability = sim_rule_get_reliability_relative (rule_node);

			/* Directive Priority */
			new_event->priority = sim_directive_get_priority (backlog);

			if (!event->id)
				sim_container_db_insert_event_ul (ossim.container, ossim.dbossim, event);

			sim_container_db_insert_event_ul (ossim.container, ossim.dbossim, new_event);

			sim_container_push_event (ossim.container, new_event);

			sim_container_db_update_backlog_ul (ossim.container, ossim.dbossim, backlog);
			sim_container_db_insert_backlog_event_ul (ossim.container, ossim.dbossim, backlog, event);
			sim_container_db_insert_backlog_event_ul (ossim.container, ossim.dbossim, backlog, new_event);

			inserted = TRUE;

		  /* Children Rules with type MONITOR */
		  if (!G_NODE_IS_LEAF (rule_node)) //if this is not the last node (i.e., if it has some children...)
	    {
	      GNode *children = rule_node->children;
	      while (children)
				{
				  SimRule *rule = children->data;
		  
				  if (rule->type == SIM_RULE_TYPE_MONITOR)
				  {
						g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_organizer_correlation: Monitor rule");
			    	SimCommand *cmd = sim_command_new_from_rule (rule);
				    sim_server_push_session_plugin_command (ossim.server, 
																							      SIM_SESSION_TYPE_SENSOR, 
																							      sim_rule_get_plugin_id (rule),
																							      cmd);
	    		  g_object_unref (cmd);
			    }
		  
				  children = children->next;
				}
			} 
			else	//if the rule is not the last node, append the backlog (a directive with all the rules) to remove later.	
						//Here is where the directive is stored to be destroyed later. As we have reached the last node, it has no sense
						//that we continue checking events against it.
			{
			  removes = g_list_append (removes, backlog);
			}
		}
		else
		  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_directive_backlog_match_by_event FALSE. event->id: %d, id: %d, backlog_id: %d",event->id, sim_directive_get_id(backlog), sim_directive_get_backlog_id(backlog));

		if ((event->match) && (!inserted))	//When the ocurrence is > 1 in the directive, the first call to 
																				//sim_directive_backlog_match_by_event (above) will fail, and the event won't be 
																				//inserted. So we have to insert it here.
		{
		  if (!event->id)
	  	  sim_container_db_insert_event_ul (ossim.container, ossim.dbossim, event);

		  event->backlog_id = sim_directive_get_backlog_id (backlog);
		  sim_container_db_insert_backlog_event_ul (ossim.container, ossim.dbossim, backlog, event);
		}

    if (event->sticky)
			stickys = g_list_append (stickys, GINT_TO_POINTER (id));

    event->matched = FALSE;
    event->match = FALSE;

    list = list->next;
  }

  list = removes;
  while (list)
  {
    SimDirective *backlog = (SimDirective *) list->data;
    sim_container_remove_backlog_ul (ossim.container, backlog);
    
    g_object_unref (backlog);
    list = list->next;
  }
  g_list_free (removes);
  g_mutex_unlock (ossim.mutex_backlogs);


  /* Match Directives */
  g_mutex_lock (ossim.mutex_directives);
  list = sim_container_get_directives_ul (ossim.container);
  while (list)
  {
    SimDirective *directive = (SimDirective *) list->data;
    id = sim_directive_get_id (directive);

    found = FALSE;
    lgs = groups; //FIXME: ??? here groups is _always_ null...
    while (lgs)
		{
		  SimDirectiveGroup *group = (SimDirectiveGroup *) lgs->data;

	  	if ((sim_directive_group_get_sticky (group)) && (sim_directive_has_group (directive, group)))
	    {
	      found = TRUE;
	      break;
	    }

		  lgs = lgs->next;
		}

    if (found)
		{
		  list = list->next;
	  	break;
		}

    found = FALSE;
    tmp = stickys;	//first time server runs this is null.
    while (tmp) 
		{
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
		
		g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_organizer_correlation: MIDDLE backlogs %d", g_list_length (sim_container_get_backlogs_ul (ossim.container)));
		//The directive hasn't match yet, so we try to test if it match with the event itself. (for example, the 1st time)
    if (sim_directive_match_by_event (directive, event)) 
		{
		  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_directive_match_by_event TRUE. event->id: %d, id: %d, directive_id: %d",event->id, sim_directive_get_id(directive), sim_directive_get_backlog_id(directive));
	
	  	SimDirective *backlog;
			SimRule      *rule_root;
	  	GNode        *node_root;

		  GTime          time_last = time (NULL);	//gets the actual time so we can update the rule

	  	if (sim_directive_get_groups (directive))
		    groups = g_list_concat (groups, g_list_copy (sim_directive_get_groups (directive)));

			/* Create a backlog from directive */
			backlog = sim_directive_clone (directive);
			/* Gets the root node from backlog */
			node_root = sim_directive_get_curr_node (backlog);
			// Gets the root rule from backlog. Rule_root is the data field in node_root.
			rule_root = sim_directive_get_curr_rule (backlog);

			sim_rule_set_time_last (rule_root, time_last);
			// Set the event data to the rule_root. This will copy some fields from event (src_ip, port..)  into the directive (into the backlog)
			sim_rule_set_event_data (rule_root, event);
			
			event->matched = TRUE;

			if (!event->id)
			{
				sim_container_db_insert_event_ul (ossim.container, ossim.dbossim, event);
				g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_organizer_correlation: insert event !event->id");
			}
			else
				g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_organizer_correlation: event->id: %d", event->id);

			if (!G_NODE_IS_LEAF (node_root))	//if the node has some children...
	    {
	      GNode *children = node_root->children;
	      while (children)								
				{
				  SimRule *rule = children->data;

				  sim_rule_set_time_last (rule, time_last);	// Actualice time in all the children
					sim_directive_set_rule_vars (backlog, children);	//this can be done only in children nodes, not in the root one.
																														//Store in the children the data from the node level specified
																														//in children.

				  if (rule->type == SIM_RULE_TYPE_MONITOR)
			    {
			      SimCommand *cmd = sim_command_new_from_rule (rule);
		  			g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "MONITOR rule");
						sim_server_debug_print_sessions (ossim.server);
								
			      sim_server_push_session_plugin_command (ossim.server,									//FIXME: make this non-blocking 
																							      SIM_SESSION_TYPE_SENSOR, 
																							      sim_rule_get_plugin_id (rule),
																							      cmd);
			      g_object_unref (cmd);
			    }

			  	children = children->next;
				}

	      sim_container_append_backlog (ossim.container, backlog); //this is where the SimDirective gets inserted into backlog
	      sim_container_db_insert_backlog_ul (ossim.container, ossim.dbossim, backlog);
	      sim_container_db_insert_backlog_event_ul (ossim.container, ossim.dbossim, backlog, event);
	      event->backlog_id = sim_directive_get_backlog_id (backlog);
	    } 
		  else
	    {
	      sim_directive_set_matched (backlog, TRUE);	//As this hasn't got any children we know that the directive has match. 
				//Now we need to create a new event, fill it with data from the directive wich made the event match.
				new_event = sim_event_new ();
	      new_event->type = SIM_EVENT_TYPE_DETECTOR;
	      new_event->alarm = FALSE;
	      new_event->time = time (NULL);
	      new_event->backlog_id = sim_directive_get_backlog_id (backlog);
	  
/*
	      if (config->sensor.ip)
					new_event->sensor = g_strdup (config->sensor.ip);
	      if (config->sensor.interface)
					new_event->interface = g_strdup (config->sensor.interface);
*/
				new_event->sensor = gnet_inetaddr_get_canonical_name (sim_rule_get_sensor (rule_root));
	      if (config->sensor.interface)											//FIXME: I think this is wrong
					new_event->interface = g_strdup (config->sensor.interface);

	      new_event->plugin_id = SIM_PLUGIN_ID_DIRECTIVE;
	      new_event->plugin_sid = sim_directive_get_id (backlog);

	      if ((ia = sim_rule_get_src_ia (rule_root))) new_event->src_ia = gnet_inetaddr_clone (ia);
	      if ((ia = sim_rule_get_dst_ia (rule_root))) new_event->dst_ia = gnet_inetaddr_clone (ia);
	      new_event->src_port = sim_rule_get_src_port (rule_root);
	      new_event->dst_port = sim_rule_get_dst_port (rule_root);
	      new_event->protocol = sim_rule_get_protocol (rule_root);
	      new_event->data = sim_directive_backlog_to_string (backlog);
	      
	      /* Rule reliability */
	      if (sim_rule_get_rel_abs (rule_root))
					new_event->reliability = sim_rule_get_reliability (rule_root);
	      else
					new_event->reliability = sim_rule_get_reliability_relative (node_root);

	      /* Directive Priority */
	      new_event->priority = sim_directive_get_priority (backlog);

	      sim_container_db_insert_event_ul (ossim.container, ossim.dbossim, new_event);

	      sim_container_push_event (ossim.container, new_event);

	      sim_container_db_insert_backlog_ul (ossim.container, ossim.dbossim, backlog);
	      sim_container_db_insert_backlog_event_ul (ossim.container, ossim.dbossim, backlog, event);
	      sim_container_db_insert_backlog_event_ul (ossim.container, ossim.dbossim, backlog, new_event);
	      event->backlog_id = sim_directive_get_backlog_id (backlog);

	      g_object_unref (backlog);
	    }
		}
		else
		  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_directive_match_by_event FALSE. event->id: %d, directive: %d",event->id, sim_directive_get_id(directive));

    event->matched = FALSE;
    event->match = FALSE;

    list = list->next;
  }
  g_mutex_unlock (ossim.mutex_directives);

  g_list_free (stickys);
  g_list_free (groups);
  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_organizer_correlation: END backlogs %d", g_list_length (sim_container_get_backlogs_ul (ossim.container)));
}

/*
 *
 * 
 *
 *
 */
void
sim_organizer_snort_ossim_event_insert (SimDatabase  *db_snort,
																				SimEvent     *event,
																				gint          sid,
																				gulong        cid)
{
  gchar         *insert;
  gint           c;
  gint           a;

  g_return_if_fail (db_snort);
  g_return_if_fail (SIM_IS_DATABASE (db_snort));
  g_return_if_fail (event);
  g_return_if_fail (SIM_IS_EVENT (event));
  g_return_if_fail (sid > 0);
  g_return_if_fail (cid > 0);

  event->snort_sid = sid;
  event->snort_cid = cid;

  c = rint (event->risk_c); 
  a = rint (event->risk_a);

  /* insert OSSIM Event */
  insert = g_strdup_printf ("INSERT INTO ossim_event (sid, cid, type, priority, reliability, asset_src, asset_dst, risk_c, risk_a) "
			    "VALUES (%u, %u, %u, %u, %u, %u, %u, %u, %u)", sid, cid,
			    (event->alarm) ? 2 : 1,
			    event->priority, event->reliability,
			    event->asset_src, event->asset_dst, c, a);

  sim_database_execute_no_query (db_snort, insert);
  g_free (insert);
}

/*
 * This is called from sim_organizer_snort. It can be called with a snort event(plugin_name will be NULL)
 * or another event (plugin_name will be something like: "arp_watch: New Mac".
 *
 *
 */
gint
sim_organizer_snort_sensor_get_sid (SimDatabase  *db_snort,
																    gchar        *hostname, //sensor name
																    gchar        *interface,
																    gchar        *plugin_name)
{
  GdaDataModel  *dm;
  GdaValue      *value;
  guint         sid = 0;
  gchar         *query;
  gchar         *insert;
  gint           row;
  
  g_return_val_if_fail (db_snort, 0);
  g_return_val_if_fail (SIM_IS_DATABASE (db_snort), 0);
  g_return_val_if_fail (hostname, 0);
  g_return_val_if_fail (interface, 0);

  /* SID */
  if (plugin_name)
    query = g_strdup_printf ("SELECT sid FROM sensor WHERE hostname = '%s-%s' AND interface = '%s'", hostname, plugin_name, interface);
  else
    query = g_strdup_printf ("SELECT sid FROM sensor WHERE hostname = '%s' AND interface = '%s'", hostname, interface);

  dm = sim_database_execute_single_command (db_snort, query);
  if (dm)
  {
    if (gda_data_model_get_n_rows (dm))
		{
		  value = (GdaValue *) gda_data_model_get_value_at (dm, 0, 0);
      //FIXME: I don't know why, but in some systems (tested under MACOS X), although the database has a structure
      //with an unsigned integer,GDA thinks that it's an integer, and the call to "gda_value_get_uinteger (value)"
			//fails. This direct access to data type ensures that we get what we want.
	  	sid = value->value.v_uinteger;
	  	g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_organizer_snort_sensor_get_sid sid1: %d. Query: %s",sid, query);
		}
    else //if it's the first time that kind of event is saw
		{
	  	if (plugin_name)
	    	insert = g_strdup_printf ("INSERT INTO sensor (hostname, interface, encoding, last_cid) "
				      "VALUES ('%s-%s', '%s', 2, 0)", hostname, plugin_name, interface);
			else
	    	insert = g_strdup_printf ("INSERT INTO sensor (hostname, interface, detail, encoding, last_cid) "
				      "VALUES ('%s', '%s', 1, 0, 0)", hostname, interface);

		  sim_database_execute_no_query (db_snort, insert);

	  	g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_organizer_snort_sensor_get_sid sid2: %d. Query: %s",sid, insert);
	  	sid = sim_organizer_snort_sensor_get_sid (db_snort, hostname, interface, plugin_name);
	  	g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_organizer_snort_sensor_get_sid sid3: %d. Query: %s",sid, insert);
		  g_free (insert);
		}

    g_object_unref(dm);
  }
  else
    g_message ("SENSOR SID DATA MODEL ERROR");
		
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
  guint          last_cid = 0;
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
      	last_cid = value->value.v_uinteger;
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
 * This calls to sim_organizer_snort_ossim_event_insert, so the events are stored in the snort DB
 *
 */
void
sim_organizer_snort_event_insert (SimDatabase  *db_snort,
																  SimEvent     *event,
																  gint          sid,
																  gulong        cid)
{
  gchar timestamp[TIMEBUF_SIZE];
  gchar *query;

  g_return_if_fail (db_snort);
  g_return_if_fail (SIM_IS_DATABASE (db_snort));
  g_return_if_fail (event);
  g_return_if_fail (SIM_IS_EVENT (event));
  g_return_if_fail (sid > 0);
  g_return_if_fail (cid > 0);

  strftime (timestamp, TIMEBUF_SIZE, "%Y-%m-%d %H:%M:%S", localtime ((time_t *) &event->time));

  query = g_strdup_printf ("INSERT INTO event (sid, cid, timestamp) "
			   "VALUES (%u, %u, '%s')",
			   sid, cid, timestamp);
	g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_organizer_snort_event_insert: query1: %s",query);
		
  sim_database_execute_no_query (db_snort, query);
  g_free (query);

  query = g_strdup_printf ("INSERT INTO iphdr (sid, cid, ip_src, ip_dst, ip_proto) "
			   "VALUES (%u, %u, %lu, %lu, %d)",
			   sid, cid,
			   (event->src_ia) ? sim_inetaddr_ntohl (event->src_ia) : -1,
			   (event->dst_ia) ? sim_inetaddr_ntohl (event->dst_ia) : -1,
			   event->protocol);
	g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_organizer_snort_event_insert: query2: %s",query);
  sim_database_execute_no_query (db_snort, query);
  g_free (query);

  switch (event->protocol)
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
			       sid, cid, event->src_port, event->dst_port);
      sim_database_execute_no_query (db_snort, query);
      g_free (query);
      break;
    case SIM_PROTOCOL_TYPE_UDP:
      query = g_strdup_printf ("INSERT INTO udphdr (sid, cid, udp_sport, udp_dport) "
			       "VALUES (%u, %u, %d, %d)",
			       sid, cid, event->src_port, event->dst_port);
      sim_database_execute_no_query (db_snort, query);
      g_free (query);
      break;
    default:
      break;
    }

  if (event->data)
    {
      query = g_strdup_printf ("INSERT INTO data (sid, cid, data_payload) "
			       "VALUES (%u, %u, '%s')",
			       sid, cid, event->data);
      sim_database_execute_no_query (db_snort, query);
      g_free (query);
    }

  sim_organizer_snort_ossim_event_insert (db_snort, event, sid, cid);
}

/*
 *
 * Inserts an event in the snort DB in the table ossim_event. 
 *
 */
void
sim_organizer_snort_event_get_cid_from_event (SimDatabase  *db_snort,
					      SimEvent     *event,
					      gint          sid)
{
  GdaDataModel  *dm;
  GdaValue      *value;
  gchar          timestamp[TIMEBUF_SIZE];
  GString       *select;
  GString       *where;
  gint           row;
  guint         cid;
  gchar         *src_ip;
  gchar         *dst_ip;

  g_return_if_fail (db_snort);
  g_return_if_fail (SIM_IS_DATABASE (db_snort));
  g_return_if_fail (event);
  g_return_if_fail (SIM_IS_EVENT (event));
  g_return_if_fail (sid > 0);

  strftime (timestamp, TIMEBUF_SIZE, "%Y-%m-%d %H:%M:%S", localtime ((time_t *) &event->time));

  select = g_string_new ("SELECT event.cid FROM event LEFT JOIN iphdr ON (event.sid = iphdr.sid AND event.cid = iphdr.cid)");
  where = g_string_new (" WHERE");

  g_string_append_printf (where, " event.sid = %u", sid);
  g_string_append_printf (where, " AND event.timestamp = '%s'", timestamp);

  if (event->src_ia)
    g_string_append_printf (where, " AND ip_src = %lu", sim_inetaddr_ntohl (event->src_ia));
  if (event->dst_ia)
    g_string_append_printf (where, " AND ip_dst = %lu", sim_inetaddr_ntohl (event->dst_ia));
  
  g_string_append_printf (where, " AND ip_proto = %d", event->protocol);

  switch (event->protocol)
    {
    case SIM_PROTOCOL_TYPE_ICMP:
      break;
    case SIM_PROTOCOL_TYPE_TCP:
      g_string_append (select, " LEFT JOIN tcphdr ON (event.sid = tcphdr.sid AND event.cid = tcphdr.cid)");

      if (event->src_port)
	g_string_append_printf (where, " AND tcp_sport = %d", event->src_port);
      if (event->dst_port)
	g_string_append_printf (where, " AND tcp_dport = %d", event->dst_port);
      break;
    case SIM_PROTOCOL_TYPE_UDP:
      g_string_append (select, " LEFT JOIN udphdr ON (event.sid = udphdr.sid AND event.cid = udphdr.cid)");

      if (event->src_port)
	g_string_append_printf (where, " AND udp_sport = %d ", event->src_port);
      if (event->dst_port)
	g_string_append_printf (where, " AND udp_dport = %d ", event->dst_port);
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
	  cid = value->value.v_uinteger;
	  sim_organizer_snort_ossim_event_insert (db_snort, event, sid, cid);
	}

      g_object_unref(dm);
    }
  else
    {
      g_message ("EVENTS ID DATA MODEL ERROR");
    }

  g_string_free (select, TRUE);
}

/*
 *
 *
 *
 */
void
sim_organizer_snort (SimOrganizer	*organizer,
								     SimEvent		*event)
{
  SimPlugin	*plugin;
  SimPluginSid	*plugin_sid;
  gint64	 last_id;
  gchar		*query;
  gint		 sid;
  gulong	 cid;
  GList		*events = NULL;
  GList		*list = NULL;

  g_return_if_fail (organizer);
  g_return_if_fail (SIM_IS_ORGANIZER (organizer));
  g_return_if_fail (event);
  g_return_if_fail (SIM_IS_EVENT (event));
  g_return_if_fail (event->sensor);
  g_return_if_fail (event->interface);

  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_organizer_snort Start: event->sid: %d ; event->cid: %lu",event->snort_sid,event->snort_cid);
  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_organizer_snort event->sensor: %s ; event->interface: %s",event->sensor, event->interface);
  // if there are snort_sid (wich snort is running) and snort_cid (number of
  // event inside snort) inside the event received, insert it directly in the snort DB.
  if (event->snort_sid && event->snort_cid && event->store_in_DB) 
  {
    sim_organizer_snort_ossim_event_insert (ossim.dbsnort,
																			      event,
																			      event->snort_sid,
																			      event->snort_cid);
    return;
  }

  plugin_sid = sim_container_get_plugin_sid_by_pky (ossim.container, event->plugin_id, event->plugin_sid);
  if (!plugin_sid)
  {
    g_message ("sim_organizer_snort: Error Plugin %d, PlugginSid %d", event->plugin_id, event->plugin_sid);
    return;
  }
  sim_plugin_sid_print_internal_data (plugin_sid);
  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_organizer_snort: event->store_in_DB: %d", event->store_in_DB);
										
  /* Events SNORT */
  if ((event->plugin_id >= 1001) && (event->plugin_id < 1500) && event->store_in_DB)
  {
      sid = sim_organizer_snort_sensor_get_sid (ossim.dbsnort,
																								event->sensor,
																								event->interface,
																								NULL);
		
      sim_organizer_snort_event_get_cid_from_event (ossim.dbsnort,	//get's the CID and insert into ossim_event	
						    event, sid);
  }
  else /* Others Events */
  if (event->store_in_DB)
    {
      plugin = sim_container_get_plugin_by_id (ossim.container, event->plugin_id);
			
      sid = sim_organizer_snort_sensor_get_sid (ossim.dbsnort,
																								event->sensor,
																								event->interface,
																								sim_plugin_get_name (plugin));
      cid = sim_organizer_snort_event_get_max_cid (ossim.dbsnort,
																								   sid);

			g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_organizer_snort: (non-snort events) sid: %d, max_cid. %d", sid, cid);
		
			//host_mac_ host_os, host_service and host_id events are inserted here too.
			//The "right" way of doing this is using the sim_container_get_plugin_id_by_name() function,
			//but this function is executed with all events, so we need as much speed as possible.
			//We're going to check it directly against the plugin_id.
			//If we change the plugin_id's in the database, we'll have to modify here too.
			gint i=0;
			gchar timestamp[TIMEBUF_SIZE];
			gboolean ok=FALSE;
			switch (event->plugin_id)
			{
				case 1512: //arpwatch
								  strftime (timestamp, TIMEBUF_SIZE, "%Y-%m-%d %H:%M:%S", localtime ((time_t *) &event->time));

									if ((event->plugin_sid == EVENT_NEW) || (event->plugin_sid == EVENT_CHANGE))
										sim_container_db_insert_host_mac_ul (ossim.container,
														    	                        ossim.dbossim,
																						              event->src_ia,
																													timestamp,
							                        	                  event->data_storage[0], //new mac
							                        	                  event->data_storage[1], //vendor
																													event->interface,
																												  event->sensor);
									g_strfreev (event->data_storage);
									break;
									
				case 1511: //P0f, OS event
									strftime (timestamp, TIMEBUF_SIZE, "%Y-%m-%d %H:%M:%S", localtime ((time_t *) &event->time));
								  if ((event->plugin_sid == EVENT_NEW) || (event->plugin_sid == EVENT_CHANGE))
									{
 								    sim_container_db_insert_host_os_ul (ossim.container,
                								                         ossim.dbossim,
								                                         event->src_ia,
                								                         timestamp,
                                								         event->sensor,
								                                         event->interface,
                								                         event->data_storage[0]); //OS
									}									
									g_strfreev (event->data_storage);
									break;
	
				case 1516: //pads, service event
									strftime (timestamp, TIMEBUF_SIZE, "%Y-%m-%d %H:%M:%S", localtime ((time_t *) &event->time));
								  if ((event->plugin_sid == EVENT_NEW) || (event->plugin_sid == EVENT_CHANGE))
						        sim_container_db_insert_host_service_ul (ossim.container,
    																						             	ossim.dbossim,
																															event->src_ia,
                 																							timestamp,
																															atoi (event->data_storage[0]) , //port
																															atoi (event->data_storage[1]) , //protocol
																															event->sensor,
																															event->interface,
																															event->data_storage[2], //service
																															event->data_storage[3]); //application
	
									g_strfreev (event->data_storage);
									break;
				case 4001: //prelude, HIDS event
									if (event->data_storage)
									{
										strftime (timestamp, TIMEBUF_SIZE, "%Y-%m-%d %H:%M:%S", localtime ((time_t *) &event->time));
										sim_container_db_insert_host_ids_event_ul (ossim.container,
																																ossim.dbossim,
																																event->src_ia,
																																timestamp,
																								                event->data_storage[0], //hostname
																								                event->data_storage[1], //event_type
																								                event->data_storage[2], //target
																								                event->data_storage[3], //what
																								                event->data_storage[4], //extra_data
																								                event->sensor,
																								                event->plugin_sid);	
										g_strfreev (event->data_storage);
									}
									else
										g_message("sim_organizer_snort: Error: data from HIDS event incomplete. Maybe someone is testing this app?...");
			
							break;
				default:
									event->data = g_strdup(event->log);
									sim_organizer_snort_event_insert (ossim.dbsnort, event, sid, ++cid);
							break;

			}
			
			
							 
    }
}

/*
 * 
 *
 * Insert rrd anomalies into separate tables
 */
void
sim_organizer_rrd (SimOrganizer  *organizer,
							     SimEvent      *event)
{
  SimPluginSid  *plugin_sid;
  gchar         *insert;
  gchar         *name;
  gchar         *plugin_sid_name;
  gchar timestamp[TIMEBUF_SIZE];

  g_return_if_fail (organizer);
  g_return_if_fail (SIM_IS_ORGANIZER (organizer));
  g_return_if_fail (event);
  g_return_if_fail (SIM_IS_EVENT (event));
  g_return_if_fail (event->sensor);
  g_return_if_fail (event->interface);

  if (event->plugin_id != 1508 ) // Return if not rrd_anomaly
    return;

  plugin_sid = sim_container_get_plugin_sid_by_pky (ossim.container, event->plugin_id, event->plugin_sid);
  if (!plugin_sid)
  {
    g_message ("sim_organizer_rrd: Error Plugin %d, PlugginSid %d", event->plugin_id, event->plugin_sid);
    return;
  }

  strftime (timestamp, TIMEBUF_SIZE, "%Y-%m-%d %H:%M:%S", localtime ((time_t *) &event->time));
  
  name = gnet_inetaddr_get_canonical_name(event->src_ia);
	if (name)
	{
	  plugin_sid_name = sim_plugin_sid_get_name(plugin_sid);
  	insert = g_strdup_printf("INSERT INTO rrd_anomalies(ip, what, anomaly_time, range) VALUES ('%s', '%s', '%s', '0')", name, plugin_sid_name, timestamp);
	  sim_database_execute_no_query (ossim.dbossim, insert);
  	g_free(insert);
	  g_free(name);
	}
}

// vim: set tabstop=2:
