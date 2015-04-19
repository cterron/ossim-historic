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

#include <unistd.h>

#include "os-sim.h"
#include "sim-scheduler.h"
#include "sim-container.h"
#include "sim-config.h"
#include "sim-directive.h"
#include "sim-command.h"
#include "sim-server.h"
#include <config.h>

extern SimMain  ossim;

enum 
{
  DESTROY,
  LAST_SIGNAL
};

/* FIXME: this struct is not used anywhere*/
struct SimSchedulerTask {
  gint     id;
  gchar   *name;
  gint     timer;
};
/**/


struct _SimSchedulerPrivate {
  SimConfig      *config;

  gint            timer;

  GList          *tasks;
};

static gpointer parent_class = NULL;
static gint sim_container_signals[LAST_SIGNAL] = { 0 };

static GTime       last = 0;
static GTime       timer = 0;

//the following used in sim_scheduler_task_store_event_number_at_5min()
static GTime       event_last = 0;
static GTime       event_timer = 300; //FIXME: A variable in config may be more friendly than this...

/* GType Functions */

static void 
sim_scheduler_impl_dispose (GObject  *gobject)
{
  G_OBJECT_CLASS (parent_class)->dispose (gobject);
}

static void 
sim_scheduler_impl_finalize (GObject  *gobject)
{
  SimScheduler *sch = SIM_SCHEDULER (gobject);

  g_free (sch->_priv);

  G_OBJECT_CLASS (parent_class)->finalize (gobject);
}

static void
sim_scheduler_class_init (SimSchedulerClass * class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  parent_class = g_type_class_peek_parent (class);

  object_class->dispose = sim_scheduler_impl_dispose;
  object_class->finalize = sim_scheduler_impl_finalize;
}

static void
sim_scheduler_instance_init (SimScheduler *scheduler)
{
  scheduler->_priv = g_new0 (SimSchedulerPrivate, 1);

  scheduler->_priv->config = NULL;

  scheduler->_priv->timer = 30;

  scheduler->_priv->tasks = NULL;
}

/* Public Methods */

GType
sim_scheduler_get_type (void)
{
  static GType object_type = 0;
 
  if (!object_type)
  {
    static const GTypeInfo type_info = {
              sizeof (SimSchedulerClass),
              NULL,
              NULL,
              (GClassInitFunc) sim_scheduler_class_init,
              NULL,
              NULL,                       /* class data */
              sizeof (SimScheduler),
              0,                          /* number of pre-allocs */
              (GInstanceInitFunc) sim_scheduler_instance_init,
              NULL                        /* value table */
    };
    
    g_type_init ();
                                                                                                                             
    object_type = g_type_register_static (G_TYPE_OBJECT, "SimScheduler", &type_info, 0);
  }
                                                                                                                             
  return object_type;
}

/*
 *
 *
 *
 *
 */
SimScheduler*
sim_scheduler_new (SimConfig    *config)
{
  SimScheduler *scheduler = NULL;

  g_return_val_if_fail (config, NULL);
  g_return_val_if_fail (SIM_IS_CONFIG (config), NULL);

  scheduler = SIM_SCHEDULER (g_object_new (SIM_TYPE_SCHEDULER, NULL));
  scheduler->_priv->config = config;

  return scheduler;
}

/*
 * Recover the host and net levels of C and A
 * 
 */
void
sim_scheduler_task_calculate (SimScheduler  *scheduler,
			      gpointer       data)
{
  gint           recovery;
  
  recovery = sim_container_db_get_recovery (ossim.container, ossim.dbossim);
  sim_container_set_host_levels_recovery (ossim.container, ossim.dbossim, recovery);
  sim_container_set_net_levels_recovery (ossim.container, ossim.dbossim, recovery);
}

/*
 *
 *
 *
 */
void
sim_scheduler_task_correlation (SimScheduler  *scheduler,
				gpointer       data)
{
  g_return_if_fail (scheduler != NULL);
  g_return_if_fail (SIM_IS_SCHEDULER (scheduler));

  sim_scheduler_backlogs_time_out (scheduler);
}

/*
 *
 *
 *
 *
 *
 */
static gpointer
sim_scheduler_session (gpointer data)
{
  SimSession  *session = (SimSession *) data;
  SimCommand  *command = NULL;
                             
  g_return_val_if_fail (session, NULL);
  g_return_val_if_fail (SIM_IS_SESSION (session), NULL);

  g_message ("New session (Scheduler)");

  command = sim_command_new ();
  command->id = 1;
  command->type = SIM_COMMAND_TYPE_CONNECT;
  command->data.connect.type = SIM_SESSION_TYPE_SERVER;
  
  sim_session_write (session, command);

  sim_session_read (session);

  g_message ("Remove Session (Scheduler)");
  sim_server_remove_session (ossim.server, session);

  g_object_unref (session);
     
  return NULL;
}

/*
 * Although this function is executed each second or so, only
 * do its job (executing other functions) each "interval" seconds approximately.
 *
 */
void
sim_scheduler_task_execute_at_interval (SimScheduler  *scheduler,
				                   	            gpointer       data)
{
  SimConfig     *config;
  GTimeVal       curr_time;

  g_return_if_fail (scheduler != NULL);
  g_return_if_fail (SIM_IS_SCHEDULER (scheduler));

  g_get_current_time (&curr_time);

  if (curr_time.tv_sec < (last + timer))
    return;

  last = curr_time.tv_sec;
  config = scheduler->_priv->config;

  timer = config->scheduler.interval; //interval is 15 by default in config.xml

	//Functions to execute:
	sim_scheduler_task_calculate(scheduler, data);//do the net and host level recovering
  sim_scheduler_task_GDAErrorHandling(); 	//do a GDA check to test if everything goes fine.

}

/*
 * Although this function is executed each second or so, only
 * do its job (store how much events of each kind has arrived to thr server)
 * each 5 minutes
 */
void
sim_scheduler_task_store_event_number_at_5min (SimScheduler  *scheduler)
{
  GTimeVal       curr_time;

  g_return_if_fail (scheduler != NULL);
  g_return_if_fail (SIM_IS_SCHEDULER (scheduler));

  g_get_current_time (&curr_time);

  if (curr_time.tv_sec < (event_last + event_timer))
    return;

  event_last = curr_time.tv_sec;

  //storing events:
	G_LOCK (s_mutex_sensors);
	
	GList *list;

  list = sim_container_get_sensors_ul(ossim.container);
  SimSensor *sensor;
  while (list)
  {
    sensor = (SimSensor *) list->data;
	  sim_container_db_update_sensor_events_number (ossim.container, ossim.dbossim, sensor); //store in DB!
		sim_sensor_reset_events_number (sensor); //reset memory 
		
		list = list->next;			
	}   


	G_UNLOCK (s_mutex_sensors);


}



/*
 * this function go through the last gda errors and print it.
 * may be this is not the best place to put this function, but...
 * its called from sim_scheduler_task_calculate() (each 15 seconds)
 */
void
sim_scheduler_task_GDAErrorHandling (void)
{
  GList		*list = NULL;
  GList		*node;
  GdaError	*error;
  GdaConnection *conn;

  conn = (GdaConnection *) sim_database_get_conn(ossim.dbossim);

  list = (GList *) gda_connection_get_errors (conn);
      
//  if (!list)
//    g_log (g_log_domain, g_log_level_debug, "gda ok");

  for (node = g_list_first (list); node != NULL; node = g_list_next (node))
  {
    error = (GdaError *) node->data;

    g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "error in gdaconnection:");
    if (error)
			g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "error no: %d \ndesc: %s \nsource: %s \nsqlstate: %s", gda_error_get_number (error), gda_error_get_description (error), gda_error_get_source (error), gda_error_get_sqlstate (error));
  }
  gda_connection_clear_error_list(conn); 
}


/*
 *
 *
 *
 */
void
sim_scheduler_task_rservers (SimScheduler  *scheduler,
												     gpointer       data)
{
  GThread  *thread;
  GList    *list;

  g_return_if_fail (scheduler);
  g_return_if_fail (SIM_IS_SCHEDULER (scheduler));

  list = ossim.config->rservers;

/***mientras no usemos rservers...**/
	
  if (list != NULL)
    g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_scheduler_task_rservers: there are some rservers");

/*****/
  while (list)
    {
      SimConfigRServer *rserver = (SimConfigRServer*) list->data;

      if (!sim_server_get_session_by_ia (ossim.server, SIM_SESSION_TYPE_RSERVER, rserver->ia))
	{
	  GTcpSocket *socket = gnet_tcp_socket_connect (rserver->ip, rserver->port);

	  if (socket)
	    {
	      SimSession *session = sim_session_new (G_OBJECT (ossim.server), ossim.config, socket);
	      session->type = SIM_SESSION_TYPE_RSERVER;
	      sim_server_append_session (ossim.server, session);
	      g_message ("sim_scheduler_task_rservers %s\n", rserver->name);
	      /* session thread */
	      thread = g_thread_create(sim_scheduler_session, session, FALSE, NULL);
	    }
	  else
	    {
	      g_message ("sim_scheduler_task_rservers not connection %s %s\n", rserver->name, rserver->ip);
	    }
	}
      list = list->next;
    }
}

/*
 * main scheduler loop wich decides what should run in a specific moment
 *
 */
void
sim_scheduler_run (SimScheduler *scheduler)
{
  GTimeVal      curr_time;

  g_return_if_fail (scheduler != NULL);
  g_return_if_fail (SIM_IS_SCHEDULER (scheduler));

  g_get_current_time (&curr_time);

  if (!last)
    last = curr_time.tv_sec;

	if (!event_last)
		event_last = curr_time.tv_sec;

	
  while (TRUE)
  {
    sleep (1);
    sim_scheduler_task_correlation (scheduler, NULL); //removes backlog entries when needed
    sim_scheduler_task_execute_at_interval (scheduler, NULL);//execute some tasks in the time interval defined in config.xml
    sim_scheduler_task_store_event_number_at_5min (scheduler); //stores the event number each 5 minutes (I know, this is a bad style, I'm sorry)
    sim_scheduler_task_rservers (scheduler, NULL); //Not in use by default. 
  }
}

/*
 *
 *
 *
 *
 */
void
sim_scheduler_backlogs_time_out (SimScheduler  *scheduler)
{
  SimConfig     *config;
  GList         *list;
  GList					*removes = NULL; //here will be append all the events so we can delete it in the second "while".

  g_return_if_fail (scheduler);
  g_return_if_fail (SIM_IS_SCHEDULER (scheduler));

  config = scheduler->_priv->config;

  g_mutex_lock (ossim.mutex_backlogs);
  list = sim_container_get_backlogs_ul (ossim.container);
  if (list)
    g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_scheduler_backlogs_time_out: backlogs %d", g_list_length (list));
  else
    g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_scheduler_backlogs_time_out: list is NULL");
  
  while (list)
  {
    SimDirective *backlog = (SimDirective *) list->data;

    if (sim_directive_is_time_out (backlog)) //the directive has ended. Its 'timeouted'
		{
		  /* Rule NOT */
		  if (sim_directive_backlog_match_by_not (backlog))
	    {
	      SimEvent     *new_event;
	      SimRule      *rule_root;
	      GNode        *rule_node;

	      rule_root = sim_directive_get_root_rule (backlog);
	      rule_node = sim_directive_get_curr_node (backlog);
	      
	      /* Create New Event */
	      new_event = sim_event_new ();
	      new_event->type = SIM_EVENT_TYPE_DETECTOR;
	      new_event->alarm = FALSE;
	      if (config->sensor.ip)
					new_event->sensor = g_strdup (config->sensor.ip);
	      if (config->sensor.interface)
					new_event->interface = g_strdup (config->sensor.interface);

	      new_event->plugin_id = SIM_PLUGIN_ID_DIRECTIVE;
	      new_event->plugin_sid = sim_directive_get_id (backlog);

	      if (sim_rule_get_src_ia (rule_root))
					new_event->src_ia = gnet_inetaddr_clone (sim_rule_get_src_ia (rule_root));
	      if (sim_rule_get_dst_ia (rule_root))
					new_event->dst_ia = gnet_inetaddr_clone (sim_rule_get_dst_ia (rule_root));
	      new_event->src_port = sim_rule_get_src_port (rule_root);
	      new_event->dst_port = sim_rule_get_dst_port (rule_root);
	      new_event->protocol = sim_rule_get_protocol (rule_root);
	      new_event->condition = sim_rule_get_condition (rule_root);
	      if (sim_rule_get_value (rule_root))
					new_event->value = g_strdup (sim_rule_get_value (rule_root));

	      new_event->data = sim_directive_backlog_to_string (backlog);

	      /* Rule reliability */
	      if (sim_rule_get_rel_abs (rule_root))
					new_event->reliability = sim_rule_get_reliability (rule_root);
	      else
					new_event->reliability = sim_rule_get_reliability_relative (rule_node);

	      /* Directive Priority */
	      new_event->priority = sim_directive_get_priority (backlog);

	      sim_container_push_event (ossim.container, new_event);
	      
	      sim_container_db_update_backlog_ul (ossim.container, ossim.dbossim, backlog);
	      
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
						  sim_server_push_session_plugin_command (ossim.server, 
																										  SIM_SESSION_TYPE_SENSOR, 
																										  sim_rule_get_plugin_id (rule),
																										  cmd);
						  g_object_unref (cmd);
											
						}
		      
		    	  children = children->next;
		  	  }
				} 
		    else
				{
		  		removes = g_list_append (removes, backlog);
				}
							 
	  	  list = list->next;
		    continue;
		  }
	  	removes = g_list_append (removes, backlog);
							
		}
	  list = list->next;
  }

  list = removes;
  while (list)
  {
    SimDirective *backlog = (SimDirective *) list->data;
    sim_container_remove_backlog_ul (ossim.container, backlog);
    sim_container_db_delete_backlog_ul (ossim.container, ossim.dbossim, backlog);
    g_object_unref (backlog);
    list = list->next;
  }

	if (removes)
		g_list_free (removes);

  g_mutex_unlock (ossim.mutex_backlogs);
}
// vim: set tabstop=2:
