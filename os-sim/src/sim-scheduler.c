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
#include "sim-database.h"
#include "sim-config.h"
#include "sim-directive.h"
#include "sim-command.h"
#include "sim-server.h"

extern SimMain  ossim;

G_LOCK_EXTERN (s_mutex_backlogs);

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
  SimDatabase    *db_ossim;
  SimDatabase    *db_snort;
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

  scheduler->_priv->db_ossim = NULL;
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
  SimConfigDS  *ds;

  g_return_val_if_fail (config, NULL);
  g_return_val_if_fail (SIM_IS_CONFIG (config), NULL);

  scheduler = SIM_SCHEDULER (g_object_new (SIM_TYPE_SCHEDULER, NULL));
  scheduler->_priv->config = config;

  ds = sim_config_get_ds_by_name (config, SIM_DS_OSSIM);
  scheduler->_priv->db_ossim = sim_database_new (ds);

  ds = sim_config_get_ds_by_name (config, SIM_DS_SNORT);
  scheduler->_priv->db_snort = sim_database_new (ds);

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
  SimDatabase   *db_ossim;
  SimConfig     *config;
  gint           recovery;
  GTimeVal       curr_time;

  g_return_if_fail (scheduler != NULL);
  g_return_if_fail (SIM_IS_SCHEDULER (scheduler));

  db_ossim = scheduler->_priv->db_ossim;
  config = scheduler->_priv->config;

  g_get_current_time (&curr_time);

  if (curr_time.tv_sec < (last + timer))
    return;

  last = curr_time.tv_sec;

  timer = config->scheduler.interval;

  recovery = sim_container_db_get_recovery (ossim.container, db_ossim);
  sim_container_set_host_levels_recovery (ossim.container, db_ossim, recovery);
  sim_container_set_net_levels_recovery (ossim.container, db_ossim, recovery);
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
  SimDatabase   *db_ossim;
  GList         *list;

  g_return_if_fail (scheduler);
  g_return_if_fail (SIM_IS_SCHEDULER (scheduler));

  db_ossim = scheduler->_priv->db_ossim;
  config = scheduler->_priv->config;

  G_LOCK (s_mutex_backlogs);
  list = sim_container_get_backlogs_ul (ossim.container);
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
	      
	      sim_container_db_update_backlog_ul (ossim.container, db_ossim, backlog);
	      
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
		  sim_container_remove_backlog_ul (ossim.container, backlog);
		  if (sim_directive_get_rule_level (backlog) <= 1)
		    sim_container_db_delete_backlog_ul (ossim.container, db_ossim, backlog);

		  g_object_unref (backlog);
		}
	      
	      list = list->next;
	      continue;
	    }

	  sim_container_remove_backlog_ul (ossim.container, backlog);
	  if (sim_directive_get_rule_level (backlog) <= 1)
	      sim_container_db_delete_backlog_ul (ossim.container, db_ossim, backlog);

	  g_object_unref (backlog);
	}

      list = list->next;
    }
  g_list_free (list);
  G_UNLOCK (s_mutex_backlogs);
}
