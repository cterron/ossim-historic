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
#include <config.h>

#include "os-sim.h"
#include "sim-scheduler.h"
#include "sim-container.h"
#include "sim-config.h"
#include "sim-directive.h"
#include "sim-command.h"
#include "sim-server.h"

extern SimMain  ossim;

enum 
{
  DESTROY,
  LAST_SIGNAL
};

struct SimSchedulerTask {
  gint     id;
  gchar   *name;
  gint     timer;
};

struct _SimSchedulerPrivate {
  SimConfig      *config;

  gint            timer;

  GList          *tasks;
};

static gpointer parent_class = NULL;
static gint sim_container_signals[LAST_SIGNAL] = { 0 };

static GTime       last = 0;
static GTime       timer = 0;


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
 *
 *
 *
 */
void
sim_scheduler_task_calculate (SimScheduler  *scheduler,
			      gpointer       data)
{
  SimConfig	*config;
  gint		 recovery;
  GTimeVal	 curr_time;

  g_return_if_fail (scheduler != NULL);
  g_return_if_fail (SIM_IS_SCHEDULER (scheduler));

  config = scheduler->_priv->config;

  g_get_current_time (&curr_time);

  if (curr_time.tv_sec < (last + timer))
    return;

  last = curr_time.tv_sec;

  timer = config->scheduler.interval;

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

  g_message ("New session");

  command = sim_command_new ();
  command->id = 1;
  command->type = SIM_COMMAND_TYPE_CONNECT;
  command->data.connect.type = SIM_SESSION_TYPE_SERVER;
  
  sim_session_write (session, command);

  sim_session_read (session);

  g_message ("Remove Session");
  sim_server_remove_session (ossim.server, session);

  g_object_unref (session);
     
  return NULL;
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
	      /* Session Thread */
	      thread = g_thread_create(sim_scheduler_session, session, TRUE, NULL);
	    }
	  else
	    {
	      g_message ("sim_scheduler_task_rservers NOT CONNECTION %s %s\n", rserver->name, rserver->ip);
	    }
	}
      list = list->next;
    }
}

/*
 *
 *
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

  while (TRUE)
  {
    sleep (1);
    sim_scheduler_task_calculate (scheduler, NULL);
    sim_scheduler_task_correlation (scheduler, NULL);
    sim_scheduler_task_rservers (scheduler, NULL);
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
  GList		*removes = NULL;

  g_return_if_fail (scheduler);
  g_return_if_fail (SIM_IS_SCHEDULER (scheduler));

  config = scheduler->_priv->config;

  g_mutex_lock (ossim.mutex_backlogs);
  list = sim_container_get_backlogs_ul (ossim.container);
  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_scheduler_backlogs_time_out: backlogs %d", g_list_length (list));
  while (list)
    {
      SimDirective *backlog = (SimDirective *) list->data;

      if (sim_directive_is_time_out (backlog))
	{
	  /* Rule NOT */
	  if (sim_directive_backlog_match_by_not (backlog))
	    {
	      SimAlert     *new_alert;
	      SimRule      *rule_root;
	      GNode        *rule_node;

	      rule_root = sim_directive_get_root_rule (backlog);
	      rule_node = sim_directive_get_curr_node (backlog);
	      
	      /* Create New Alert */
	      new_alert = sim_alert_new ();
	      new_alert->type = SIM_ALERT_TYPE_DETECTOR;
	      new_alert->alarm = FALSE;
	      if (config->sensor.ip)
		new_alert->sensor = g_strdup (config->sensor.ip);
	      if (config->sensor.interface)
		new_alert->interface = g_strdup (config->sensor.interface);

	      new_alert->plugin_id = SIM_PLUGIN_ID_DIRECTIVE;
	      new_alert->plugin_sid = sim_directive_get_id (backlog);

	      if (sim_rule_get_src_ia (rule_root))
		new_alert->src_ia = gnet_inetaddr_clone (sim_rule_get_src_ia (rule_root));
	      if (sim_rule_get_dst_ia (rule_root))
		new_alert->dst_ia = gnet_inetaddr_clone (sim_rule_get_dst_ia (rule_root));
	      new_alert->src_port = sim_rule_get_src_port (rule_root);
	      new_alert->dst_port = sim_rule_get_dst_port (rule_root);
	      new_alert->protocol = sim_rule_get_protocol (rule_root);
	      new_alert->condition = sim_rule_get_condition (rule_root);
	      if (sim_rule_get_value (rule_root))
		new_alert->value = g_strdup (sim_rule_get_value (rule_root));

	      new_alert->data = sim_directive_backlog_to_string (backlog);

	      /* Rule reliability */
	      if (sim_rule_get_rel_abs (rule_root))
		new_alert->reliability = sim_rule_get_reliability (rule_root);
	      else
		new_alert->reliability = sim_rule_get_reliability_relative (rule_node);

	      /* Directive Priority */
	      new_alert->priority = sim_directive_get_priority (backlog);

	      sim_container_push_alert (ossim.container, new_alert);
	      
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
  g_list_free (removes);

  g_mutex_unlock (ossim.mutex_backlogs);
}
