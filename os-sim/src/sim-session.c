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

#include <gnet.h>
#include <config.h>

#include "os-sim.h"
#include "sim-session.h"
#include "sim-rule.h"
#include "sim-directive.h"
#include "sim-plugin-sid.h"
#include "sim-container.h"
#include "sim-sensor.h"
#include "sim-command.h"
#include "sim-server.h"
#include "sim-plugin-state.h"

extern SimMain    ossim;

enum 
{
  DESTROY,
  LAST_SIGNAL
};

struct _SimSessionPrivate {
  GTcpSocket	*socket;

  SimServer	*server;
  SimConfig	*config;

  SimSensor	*sensor;
  GList		*plugins;
  GList		*plugin_states;

  GIOChannel	*io;

  GInetAddr	*ia;
  gint		seq;
  gboolean	close;
  gboolean	connect;
};

static gpointer parent_class = NULL;
static gint sim_server_signals[LAST_SIGNAL] = { 0 };

/* GType Functions */

static void 
sim_session_impl_dispose (GObject  *gobject)
{
  G_OBJECT_CLASS (parent_class)->dispose (gobject);
}

static void 
sim_session_impl_finalize (GObject  *gobject)
{
  SimSession *session = SIM_SESSION (gobject);

  if (session->_priv->socket)
    gnet_tcp_socket_delete (session->_priv->socket);

  if (session->_priv->ia)
    gnet_inetaddr_unref (session->_priv->ia);

  g_free (session->_priv);

  g_message ("Session: REMOVED");

  G_OBJECT_CLASS (parent_class)->finalize (gobject);
}

static void
sim_session_class_init (SimSessionClass * class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  parent_class = g_type_class_ref (G_TYPE_OBJECT);

  object_class->dispose = sim_session_impl_dispose;
  object_class->finalize = sim_session_impl_finalize;
}

static void
sim_session_instance_init (SimSession *session)
{
  session->_priv = g_new0 (SimSessionPrivate, 1);

  session->type = SIM_SESSION_TYPE_NONE;

  session->_priv->socket = NULL;

  session->_priv->config = NULL;
  session->_priv->server = NULL;

  session->_priv->sensor = NULL;
  session->_priv->plugins = NULL;

  session->_priv->plugin_states = NULL;

  session->_priv->io = NULL;

  session->_priv->ia = NULL;

  session->_priv->seq = 0;

  session->_priv->connect = TRUE;
  session->_priv->connect = FALSE;
}

/* Public Methods */

GType
sim_session_get_type (void)
{
  static GType object_type = 0;
 
  if (!object_type)
  {
    static const GTypeInfo type_info = {
              sizeof (SimSessionClass),
              NULL,
              NULL,
              (GClassInitFunc) sim_session_class_init,
              NULL,
              NULL,                       /* class data */
              sizeof (SimSession),
              0,                          /* number of pre-allocs */
              (GInstanceInitFunc) sim_session_instance_init,
              NULL                        /* value table */
    };
    
    g_type_init ();
                                                                                                                             
    object_type = g_type_register_static (G_TYPE_OBJECT, "SimSession", &type_info, 0);
  }

  return object_type;
}

/*
 *
 *
 *
 *
 */
SimSession*
sim_session_new (GObject       *object,
		 SimConfig     *config,
		 GTcpSocket    *socket)
{
  SimServer    *server = (SimServer *) object;
  SimSession   *session;

  g_return_val_if_fail (server, NULL);
  g_return_val_if_fail (SIM_IS_SERVER (server), NULL);
  g_return_val_if_fail (config, NULL);
  g_return_val_if_fail (SIM_IS_CONFIG (config), NULL);
  g_return_val_if_fail (socket, NULL);

  session = SIM_SESSION (g_object_new (SIM_TYPE_SESSION, NULL));
  session->_priv->config = config;
  session->_priv->server = server;
  session->_priv->socket = socket;

  session->_priv->ia = gnet_tcp_socket_get_remote_inetaddr (socket);
  if (gnet_inetaddr_is_loopback (session->_priv->ia))
    {
      gnet_inetaddr_unref (session->_priv->ia);
      session->_priv->ia = gnet_inetaddr_get_host_addr ();
    }

  g_message ("SESSION: %s", gnet_inetaddr_get_canonical_name (session->_priv->ia));

  return session;
}

/*
 *
 *
 *
 */
static void
sim_session_cmd_connect (SimSession  *session,
			 SimCommand  *command)
{
  SimCommand  *cmd;
  SimSensor   *sensor = NULL;

  g_return_if_fail (session != NULL);
  g_return_if_fail (SIM_IS_SESSION (session));
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));
  
  if (command->data.connect.type)
    {
      sensor = sim_container_get_sensor_by_ia (ossim.container, session->_priv->ia);

      session->_priv->sensor = sensor;
      session->type = SIM_SESSION_TYPE_SENSOR;
    }

  cmd = sim_command_new_from_type (SIM_COMMAND_TYPE_OK);
  cmd->id = command->id;

  sim_session_write (session, cmd);
  g_object_unref (cmd);

  session->_priv->connect = TRUE;
}

/*
 *
 *
 *
 */
static void
sim_session_cmd_session_append_plugin (SimSession  *session,
				       SimCommand  *command)
{
  SimCommand      *cmd;
  SimPlugin       *plugin = NULL;
  SimPluginState  *plugin_state;

  g_return_if_fail (session != NULL);
  g_return_if_fail (SIM_IS_SESSION (session));
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));

  session->type = SIM_SESSION_TYPE_SENSOR;

  plugin = sim_container_get_plugin_by_id (ossim.container, command->data.session_append_plugin.id);
  if (plugin)
    {
      plugin_state = sim_plugin_state_new_from_data (plugin,
						     command->data.session_append_plugin.id,
						     command->data.session_append_plugin.state,
						     command->data.session_append_plugin.enabled);

      session->_priv->plugin_states = g_list_append (session->_priv->plugin_states, plugin_state);

      cmd = sim_command_new_from_type (SIM_COMMAND_TYPE_OK);
      cmd->id = command->id;

      sim_session_write (session, cmd);
      g_object_unref (cmd);
  
      /* Directives with root rule type MONITOR */
      if (plugin->type == SIM_PLUGIN_TYPE_MONITOR)
	{      
	  GList *directives = NULL;
	  g_mutex_lock (ossim.mutex_directives);
	  directives = sim_container_get_directives_ul (ossim.container);
	  while (directives)
	    {
	      SimDirective *directive = (SimDirective *) directives->data;
	      SimRule *rule = sim_directive_get_root_rule (directive);

	      if (sim_rule_get_plugin_id (rule) == command->data.session_append_plugin.id)
		{
		  cmd = sim_command_new_from_rule (rule);
		  sim_session_write (session, cmd);
		  g_object_unref (cmd);
		}

	      directives = directives->next;
	    }
	  g_list_free (directives);
	  g_mutex_unlock (ossim.mutex_directives);
	}
    }
  else
    {
      cmd = sim_command_new_from_type (SIM_COMMAND_TYPE_ERROR);
      cmd->id = command->id;

      sim_session_write (session, cmd);
      g_object_unref (cmd);
    }
}

/*
 *
 *
 *
 */
static void
sim_session_cmd_session_remove_plugin (SimSession  *session,
				       SimCommand  *command)
{
  SimCommand  *cmd;
  SimPlugin   *plugin = NULL;

  g_return_if_fail (session != NULL);
  g_return_if_fail (SIM_IS_SESSION (session));
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));

  plugin = sim_container_get_plugin_by_id (ossim.container, command->data.session_remove_plugin.id);
  if (plugin)
    {
      session->_priv->plugins = g_list_remove (session->_priv->plugins, plugin);

      cmd = sim_command_new_from_type (SIM_COMMAND_TYPE_OK);
      cmd->id = command->id;

      sim_session_write (session, cmd);
      g_object_unref (cmd);
    }
  else
    {
      cmd = sim_command_new_from_type (SIM_COMMAND_TYPE_ERROR);
      cmd->id = command->id;

      sim_session_write (session, cmd);
      g_object_unref (cmd);
    }
}

static void
sim_session_cmd_server_get_sensors (SimSession  *session,
				    SimCommand  *command)
{
  SimCommand  *cmd;
  GList       *list;
  GList       *sessions;

  g_return_if_fail (session != NULL);
  g_return_if_fail (SIM_IS_SESSION (session));
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));

  sessions = sim_server_get_sessions (session->_priv->server);
  while (sessions)
    {
      SimSession *sess = (SimSession *) sessions->data;
      SimSensor *sensor = sim_session_get_sensor (sess);

      if (!sensor)
	{
	  sessions = sessions->next;
	  continue;
	}

      cmd = sim_command_new_from_type (SIM_COMMAND_TYPE_SENSOR);
      cmd->data.sensor.host = gnet_inetaddr_get_canonical_name (sess->_priv->ia);
      cmd->data.sensor.state = TRUE;

      sim_session_write (session, cmd);
      g_object_unref (cmd);

      sessions = sessions->next;
    }
  g_list_free (sessions);

  cmd = sim_command_new_from_type (SIM_COMMAND_TYPE_OK);
  cmd->id = command->id;
  
  sim_session_write (session, cmd);
  g_object_unref (cmd);
}

static void
sim_session_cmd_server_get_sensor_plugins (SimSession  *session,
					   SimCommand  *command)
{
  SimCommand  *cmd;
  GList       *list;
  GList       *sessions;

  g_return_if_fail (session != NULL);
  g_return_if_fail (SIM_IS_SESSION (session));
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));

  sessions = sim_server_get_sessions (session->_priv->server);
  while (sessions)
    {
      SimSession *sess = (SimSession *) sessions->data;

      list = sess->_priv->plugin_states;
      while (list)
	{
	  SimPluginState  *plugin_state = (SimPluginState *) list->data;
	  SimPlugin  *plugin = sim_plugin_state_get_plugin (plugin_state);

	  cmd = sim_command_new_from_type (SIM_COMMAND_TYPE_SENSOR_PLUGIN);
	  cmd->data.sensor_plugin.plugin_id = sim_plugin_get_id (plugin);
	  cmd->data.sensor_plugin.sensor = gnet_inetaddr_get_canonical_name (sess->_priv->ia);
	  cmd->data.sensor_plugin.state = sim_plugin_state_get_state (plugin_state);
	  cmd->data.sensor_plugin.enabled = sim_plugin_state_get_enabled (plugin_state);

	  sim_session_write (session, cmd);
	  g_object_unref (cmd);
	  
	  list = list->next;
	}

      sessions = sessions->next;
    }
  g_list_free (sessions);

  cmd = sim_command_new_from_type (SIM_COMMAND_TYPE_OK);
  cmd->id = command->id;
  
  sim_session_write (session, cmd);
  g_object_unref (cmd);
}

static void
sim_session_cmd_sensor_plugin_start (SimSession  *session,
				     SimCommand  *command)
{
  SimCommand  *cmd;
  GList       *sessions;
  GInetAddr   *ia;

  g_return_if_fail (session != NULL);
  g_return_if_fail (SIM_IS_SESSION (session));
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));

  ia = gnet_inetaddr_new_nonblock (command->data.sensor_plugin_start.sensor, 0);
  sessions = sim_server_get_sessions (session->_priv->server);
  while (sessions)
    {
      SimSession *sess = (SimSession *) sessions->data;

      if (gnet_inetaddr_noport_equal (sess->_priv->ia, ia))
	{
	  cmd = sim_command_new_from_type (SIM_COMMAND_TYPE_PLUGIN_START);
	  cmd->data.plugin_start.plugin_id = command->data.sensor_plugin_start.plugin_id;
	  sim_session_write (sess, cmd);
	  g_object_unref (cmd);
	}

      sessions = sessions->next;
    }
  if (ia) gnet_inetaddr_unref (ia);
}

static void
sim_session_cmd_sensor_plugin_stop (SimSession  *session,
				    SimCommand  *command)
{
  SimCommand  *cmd;
  GList       *sessions;
  GInetAddr   *ia;

  g_return_if_fail (session != NULL);
  g_return_if_fail (SIM_IS_SESSION (session));
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));

  ia = gnet_inetaddr_new_nonblock (command->data.sensor_plugin_stop.sensor, 0);
  sessions = sim_server_get_sessions (session->_priv->server);
  while (sessions)
    {
      SimSession *sess = (SimSession *) sessions->data;

      if (gnet_inetaddr_noport_equal (sess->_priv->ia, ia))
	{
	  cmd = sim_command_new_from_type (SIM_COMMAND_TYPE_PLUGIN_STOP);
	  cmd->data.plugin_stop.plugin_id = command->data.sensor_plugin_stop.plugin_id;
	  sim_session_write (sess, cmd);
	  g_object_unref (cmd);
	}

      sessions = sessions->next;
    }
  if (ia) gnet_inetaddr_unref (ia);
}

static void
sim_session_cmd_sensor_plugin_enabled (SimSession  *session,
				     SimCommand  *command)
{
  SimCommand  *cmd;
  GList       *sessions;
  GInetAddr   *ia;

  g_return_if_fail (session != NULL);
  g_return_if_fail (SIM_IS_SESSION (session));
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));

  ia = gnet_inetaddr_new_nonblock (command->data.sensor_plugin_enabled.sensor, 0);
  sessions = sim_server_get_sessions (session->_priv->server);
  while (sessions)
    {
      SimSession *sess = (SimSession *) sessions->data;

      if (gnet_inetaddr_noport_equal (sess->_priv->ia, ia))
	{
	  cmd = sim_command_new_from_type (SIM_COMMAND_TYPE_PLUGIN_ENABLED);
	  cmd->data.plugin_enabled.plugin_id = command->data.sensor_plugin_enabled.plugin_id;
	  sim_session_write (sess, cmd);
	  g_object_unref (cmd);
	}

      sessions = sessions->next;
    }
  if (ia) gnet_inetaddr_unref (ia);
}

static void
sim_session_cmd_sensor_plugin_disabled (SimSession  *session,
					SimCommand  *command)
{
  SimCommand  *cmd;
  GList       *sessions;
  GInetAddr   *ia;

  g_return_if_fail (session != NULL);
  g_return_if_fail (SIM_IS_SESSION (session));
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));

  ia = gnet_inetaddr_new_nonblock (command->data.sensor_plugin_disabled.sensor, 0);
  sessions = sim_server_get_sessions (session->_priv->server);
  while (sessions)
    {
      SimSession *sess = (SimSession *) sessions->data;

      if (gnet_inetaddr_noport_equal (sess->_priv->ia, ia))
	{
	  cmd = sim_command_new_from_type (SIM_COMMAND_TYPE_PLUGIN_DISABLED);
	  cmd->data.plugin_disabled.plugin_id = command->data.sensor_plugin_disabled.plugin_id;
	  sim_session_write (sess, cmd);
	  g_object_unref (cmd);
	}

      sessions = sessions->next;
    }
  if (ia) gnet_inetaddr_unref (ia);
}

static void
sim_session_cmd_plugin_start (SimSession  *session,
			      SimCommand  *command)
{
  SimCommand  *cmd;
  GList       *sessions;
  GList       *list;
  
  g_return_if_fail (session != NULL);
  g_return_if_fail (SIM_IS_SESSION (session));
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));

  list = session->_priv->plugin_states;
  while (list)
    {
      SimPluginState  *plugin_state = (SimPluginState *) list->data;
      SimPlugin  *plugin = sim_plugin_state_get_plugin (plugin_state);
      gint id = sim_plugin_get_id (plugin);

      if (id == command->data.plugin_start.plugin_id)
	sim_plugin_state_set_state (plugin_state, 1);

      list = list->next;
    }
}

/*
 *
 *
 *
 */
static void
sim_session_cmd_plugin_stop (SimSession  *session,
			      SimCommand  *command)
{
  SimCommand  *cmd;
  GList       *sessions;
  GList       *list;
  
  g_return_if_fail (session != NULL);
  g_return_if_fail (SIM_IS_SESSION (session));
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));

  list = session->_priv->plugin_states;
  while (list)
    {
      SimPluginState  *plugin_state = (SimPluginState *) list->data;
      SimPlugin  *plugin = sim_plugin_state_get_plugin (plugin_state);
      gint id = sim_plugin_get_id (plugin);

      if (id == command->data.plugin_stop.plugin_id)
	sim_plugin_state_set_state (plugin_state, 2);

      list = list->next;
    }
}

/*
 *
 *
 *
 */
static void
sim_session_cmd_plugin_enabled (SimSession  *session,
				SimCommand  *command)
{
  SimCommand  *cmd;
  GList       *sessions;
  GList       *list;
  
  g_return_if_fail (session != NULL);
  g_return_if_fail (SIM_IS_SESSION (session));
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));

  list = session->_priv->plugin_states;
  while (list)
    {
      SimPluginState  *plugin_state = (SimPluginState *) list->data;
      SimPlugin  *plugin = sim_plugin_state_get_plugin (plugin_state);
      gint id = sim_plugin_get_id (plugin);

      if (id == command->data.plugin_enabled.plugin_id)
	sim_plugin_state_set_enabled (plugin_state, TRUE);

      list = list->next;
    }
}

/*
 *
 *
 *
 */
static void
sim_session_cmd_plugin_disabled (SimSession  *session,
				 SimCommand  *command)
{
  SimCommand  *cmd;
  GList       *sessions;
  GList       *list;
  
  g_return_if_fail (session != NULL);
  g_return_if_fail (SIM_IS_SESSION (session));
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));

  list = session->_priv->plugin_states;
  while (list)
    {
      SimPluginState  *plugin_state = (SimPluginState *) list->data;
      SimPlugin  *plugin = sim_plugin_state_get_plugin (plugin_state);
      gint id = sim_plugin_get_id (plugin);

      if (id == command->data.plugin_disabled.plugin_id)
	sim_plugin_state_set_enabled (plugin_state, FALSE);

      list = list->next;
    }
}

/*
 *
 *
 *
 */
static void
sim_session_cmd_alert (SimSession  *session,
		       SimCommand  *command)
{
  SimPluginSid  *plugin_sid;
  SimAlert      *alert;

  g_return_if_fail (session != NULL);
  g_return_if_fail (SIM_IS_SESSION (session));
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));

  alert = sim_command_get_alert (command);

  if (!alert)
    return;

  if (alert->type == SIM_ALERT_TYPE_NONE)
    {
      g_object_unref (alert);
      return;
    }

  if ((alert->plugin_id) && (alert->plugin_sid))
    {
      plugin_sid = sim_container_get_plugin_sid_by_pky (ossim.container,
						       alert->plugin_id,
						       alert->plugin_sid);
      alert->priority = sim_plugin_sid_get_priority (plugin_sid);
      alert->reliability = sim_plugin_sid_get_reliability (plugin_sid);
    }

  sim_container_push_alert (ossim.container, alert);
}

/*
 *
 *
 *
 */
static void
sim_session_cmd_reload_plugins (SimSession  *session,
				SimCommand  *command)
{
  SimCommand  *cmd;

  g_return_if_fail (session);
  g_return_if_fail (SIM_IS_SESSION (session));
  g_return_if_fail (command);
  g_return_if_fail (SIM_IS_COMMAND (command));


  sim_container_free_plugins (ossim.container);
  sim_container_db_load_plugins (ossim.container, ossim.dbossim);
  
  cmd = sim_command_new_from_type (SIM_COMMAND_TYPE_OK);
  cmd->id = command->id;
  
  sim_session_write (session, cmd);
  g_object_unref (cmd);
}

/*
 *
 *
 *
 */
static void
sim_session_cmd_reload_sensors (SimSession  *session,
				SimCommand  *command)
{
  SimCommand  *cmd;

  g_return_if_fail (session);
  g_return_if_fail (SIM_IS_SESSION (session));
  g_return_if_fail (command);
  g_return_if_fail (SIM_IS_COMMAND (command));


  sim_container_free_sensors (ossim.container);
  sim_container_db_load_sensors (ossim.container, ossim.dbossim);

  cmd = sim_command_new_from_type (SIM_COMMAND_TYPE_OK);
  cmd->id = command->id;
  
  sim_session_write (session, cmd);
  g_object_unref (cmd);
}

/*
 *
 *
 *
 */
static void
sim_session_cmd_reload_hosts (SimSession  *session,
			      SimCommand  *command)
{
  SimCommand  *cmd;

  g_return_if_fail (session);
  g_return_if_fail (SIM_IS_SESSION (session));
  g_return_if_fail (command);
  g_return_if_fail (SIM_IS_COMMAND (command));


  sim_container_free_hosts (ossim.container);
  sim_container_db_load_hosts (ossim.container,
			       ossim.dbossim);

  cmd = sim_command_new_from_type (SIM_COMMAND_TYPE_OK);
  cmd->id = command->id;
  
  sim_session_write (session, cmd);
  g_object_unref (cmd);
}

/*
 *
 *
 *
 */
static void
sim_session_cmd_reload_nets (SimSession  *session,
			     SimCommand  *command)
{
  SimCommand  *cmd;

  g_return_if_fail (session);
  g_return_if_fail (SIM_IS_SESSION (session));
  g_return_if_fail (command);
  g_return_if_fail (SIM_IS_COMMAND (command));

  sim_container_free_nets (ossim.container);
  sim_container_db_load_nets (ossim.container,
			      ossim.dbossim);

  cmd = sim_command_new_from_type (SIM_COMMAND_TYPE_OK);
  cmd->id = command->id;
  
  sim_session_write (session, cmd);
  g_object_unref (cmd);
}

/*
 *
 *
 *
 */
static void
sim_session_cmd_reload_policies (SimSession  *session,
				 SimCommand  *command)
{
  SimCommand  *cmd;

  g_return_if_fail (session != NULL);
  g_return_if_fail (SIM_IS_SESSION (session));
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));

  sim_container_free_policies (ossim.container);
  sim_container_db_load_policies (ossim.container, ossim.dbossim);

  cmd = sim_command_new_from_type (SIM_COMMAND_TYPE_OK);
  cmd->id = command->id;
  
  sim_session_write (session, cmd);
  g_object_unref (cmd);
}

/*
 *
 *
 *
 */
static void
sim_session_cmd_reload_directives (SimSession  *session,
				   SimCommand  *command)
{
  SimCommand  *cmd;

  g_return_if_fail (session != NULL);
  g_return_if_fail (SIM_IS_SESSION (session));
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));

  sim_container_db_delete_plugin_sid_directive_ul (ossim.container, ossim.dbossim);
  sim_container_db_delete_backlogs_ul (ossim.container, ossim.dbossim);

  sim_container_free_backlogs (ossim.container);
  sim_container_free_directives (ossim.container);
  sim_container_load_directives_from_file (ossim.container,
					   ossim.dbossim,
					   SIM_XML_DIRECTIVE_FILE);

  cmd = sim_command_new_from_type (SIM_COMMAND_TYPE_OK);
  cmd->id = command->id;
  
  sim_session_write (session, cmd);
  g_object_unref (cmd);
}

/*
 *
 *
 *
 */
static void
sim_session_cmd_reload_all (SimSession  *session,
			    SimCommand  *command)
{
  SimCommand  *cmd;
  SimConfig   *config;

  g_return_if_fail (session != NULL);
  g_return_if_fail (SIM_IS_SESSION (session));
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));

  config = session->_priv->config;

  sim_container_free_directives (ossim.container);
  sim_container_free_backlogs (ossim.container);
  sim_container_free_net_levels (ossim.container);
  sim_container_free_host_levels (ossim.container);
  sim_container_free_policies (ossim.container);
  sim_container_free_nets (ossim.container);
  sim_container_free_hosts (ossim.container);
  sim_container_free_sensors (ossim.container);
  sim_container_free_plugin_sids (ossim.container);
  sim_container_free_plugins (ossim.container);
  sim_container_free_classifications (ossim.container);
  sim_container_free_categories (ossim.container);

  sim_container_db_delete_plugin_sid_directive_ul (ossim.container, ossim.dbossim);
  sim_container_db_delete_backlogs_ul (ossim.container, ossim.dbossim);

  sim_container_db_load_categories (ossim.container, ossim.dbossim);
  sim_container_db_load_classifications (ossim.container, ossim.dbossim);
  sim_container_db_load_plugins (ossim.container, ossim.dbossim);
  sim_container_db_load_plugin_sids (ossim.container, ossim.dbossim);
  sim_container_db_load_sensors (ossim.container, ossim.dbossim);
  sim_container_db_load_hosts (ossim.container, ossim.dbossim);
  sim_container_db_load_nets (ossim.container, ossim.dbossim);
  sim_container_db_load_policies (ossim.container, ossim.dbossim);
  sim_container_db_load_host_levels (ossim.container, ossim.dbossim);
  sim_container_db_load_net_levels (ossim.container, ossim.dbossim);

  if ((config->directive.filename) && (g_file_test (config->directive.filename, G_FILE_TEST_EXISTS)))
    sim_container_load_directives_from_file (ossim.container, ossim.dbossim, config->directive.filename);

  sim_server_reload (session->_priv->server);

  cmd = sim_command_new_from_type (SIM_COMMAND_TYPE_OK);
  cmd->id = command->id;
  sim_session_write (session, cmd);
  g_object_unref (cmd);
}

/*
 *
 *
 *
 */
static void
sim_session_cmd_host_os_change (SimSession  *session,
				SimCommand  *command)
{
  SimConfig   *config;
  SimAlert    *alert;
  GInetAddr   *ia;
  gchar       *os = NULL;
  struct tm    tm;

  g_return_if_fail (session);
  g_return_if_fail (SIM_IS_SESSION (session));
  g_return_if_fail (command);
  g_return_if_fail (SIM_IS_COMMAND (command));
  g_return_if_fail (command->data.host_os_change.date);
  g_return_if_fail (command->data.host_os_change.host);
  g_return_if_fail (command->data.host_os_change.os);
  g_return_if_fail (command->data.host_os_change.plugin_id > 0);
  g_return_if_fail (command->data.host_os_change.plugin_sid > 0);

  config = session->_priv->config;

  ia = gnet_inetaddr_new_nonblock (command->data.host_os_change.host, 0);
  os = sim_container_db_get_host_os_ul (ossim.container,
				       ossim.dbossim,
				       ia);

  if (!os)
    {
      sim_container_db_insert_host_os_ul (ossim.container,
					 ossim.dbossim,
					 ia,
					 command->data.host_os_change.date,
					 command->data.host_os_change.os);
      gnet_inetaddr_unref (ia);
      return;
    }

  if (!g_ascii_strcasecmp (os, command->data.host_os_change.os))
    {
      g_free (os);
      gnet_inetaddr_unref (ia);
      return;
    }

  sim_container_db_update_host_os_ul (ossim.container,
				     ossim.dbossim,
				     ia,
				     command->data.host_os_change.date,
				     command->data.host_os_change.os,
				     os);

  alert = sim_alert_new ();
  alert->type = SIM_ALERT_TYPE_DETECTOR;
  alert->alarm = FALSE;

  if (config->sensor.ip)
    alert->sensor = g_strdup (config->sensor.ip);
  if (config->sensor.interface)
    alert->interface = g_strdup (config->sensor.interface);
  if (strptime (command->data.host_os_change.date, "%Y-%m-%d %H:%M:%S", &tm))
    alert->time =  mktime (&tm);
  else
    alert->time = time (NULL);

  alert->plugin_id = command->data.host_os_change.plugin_id;
  alert->plugin_sid = command->data.host_os_change.plugin_sid;
  alert->src_ia = ia;

  alert->data = g_strdup_printf ("%s --> %s", os,
				 command->data.host_os_change.os);

  sim_container_push_alert (ossim.container, alert);
  g_free (os);
}

/*
 *
 *
 *
 */
static void
sim_session_cmd_host_mac_change (SimSession  *session,
				 SimCommand  *command)
{
  SimConfig   *config;
  SimAlert    *alert;
  GInetAddr   *ia;
  gchar       *mac = NULL;
  gchar       *vendor = NULL;
  struct tm    tm;

  g_return_if_fail (session != NULL);
  g_return_if_fail (SIM_IS_SESSION (session));
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));
  g_return_if_fail (command->data.host_mac_change.date);
  g_return_if_fail (command->data.host_mac_change.host);
  g_return_if_fail (command->data.host_mac_change.mac);
  g_return_if_fail (command->data.host_mac_change.plugin_id > 0);
  g_return_if_fail (command->data.host_mac_change.plugin_sid > 0);

  config = session->_priv->config;

  ia = gnet_inetaddr_new_nonblock (command->data.host_mac_change.host, 0);
  mac = sim_container_db_get_host_mac_ul (ossim.container,
					 ossim.dbossim,
					 ia);

  if (!mac)
    {
      sim_container_db_insert_host_mac_ul (ossim.container,
					  ossim.dbossim,
					  ia,
					  command->data.host_mac_change.date,
					  command->data.host_mac_change.mac,
					  command->data.host_mac_change.vendor);
      gnet_inetaddr_unref (ia);
      return;
    }
  
  if (!g_ascii_strcasecmp (mac, command->data.host_mac_change.mac))
    {
      g_free (mac);
      gnet_inetaddr_unref (ia);
      return;
    }

  vendor = sim_container_db_get_host_mac_vendor_ul (ossim.container,
						    ossim.dbossim,
						    ia);

  sim_container_db_update_host_mac_ul (ossim.container,
				      ossim.dbossim,
				      ia,
				      command->data.host_mac_change.date,
				      command->data.host_mac_change.mac,
				      mac,
				      command->data.host_mac_change.vendor);

  alert = sim_alert_new ();
  alert->type = SIM_ALERT_TYPE_DETECTOR;
  alert->alarm = FALSE;

  if (config->sensor.ip)
    alert->sensor = g_strdup (config->sensor.ip);
  if (config->sensor.interface)
    alert->interface = g_strdup (config->sensor.interface);
  if (strptime (command->data.host_mac_change.date, "%Y-%m-%d %H:%M:%S", &tm))
    alert->time =  mktime (&tm);
  else
    alert->time = time (NULL);

  alert->plugin_id = command->data.host_mac_change.plugin_id;
  alert->plugin_sid = command->data.host_mac_change.plugin_sid;
  alert->src_ia = ia;

  alert->data = g_strdup_printf ("%s|%s --> %s|%s", mac, (vendor) ? vendor : "",
				 command->data.host_mac_change.mac,
				 (command->data.host_mac_change.vendor) ? command->data.host_mac_change.vendor : "");

  sim_container_push_alert (ossim.container, alert);

  g_free (mac);
  if (vendor) g_free (vendor);
}

/*
 *
 *
 *
 */
static void
sim_session_cmd_ok (SimSession  *session,
		    SimCommand  *command)
{
  g_return_if_fail (session != NULL);
  g_return_if_fail (SIM_IS_SESSION (session));
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));

}

/*
 *
 *
 *
 */
static void
sim_session_cmd_error (SimSession  *session,
		       SimCommand  *command)
{
  g_return_if_fail (session != NULL);
  g_return_if_fail (SIM_IS_SESSION (session));
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));

}

/*
 *

 *
 *
 */
void
sim_session_read (SimSession  *session)
{
  SimCommand  *cmd;
  SimCommand  *res;
  GIOError     error;
  gchar        buffer[BUFFER_SIZE];
  guint        n;

  g_return_if_fail (session != NULL);
  g_return_if_fail (SIM_IS_SESSION (session));

  session->_priv->io = gnet_tcp_socket_get_io_channel (session->_priv->socket);

  while (!(session->_priv->close) && 
	 (error = gnet_io_channel_readline (session->_priv->io, buffer, BUFFER_SIZE, &n)) == G_IO_ERROR_NONE && (n > 0))
    {
      if (error != G_IO_ERROR_NONE)
	{
	  g_message ("Recived error %d (closing socket)", error);
	  break;
	}

      if (!buffer)
	continue;

      if (strlen (buffer) <= 2)
	continue;

      g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_read: %s", buffer);
      cmd = sim_command_new_from_buffer (buffer);

      if (!cmd)
	{
	  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_read: error command null");
	  continue;
	}

      if (cmd->type == SIM_COMMAND_TYPE_NONE)
	{
	  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_read: error command type none");
	  g_object_unref (cmd);
	  continue;
	}

      switch (cmd->type)
	{
	case SIM_COMMAND_TYPE_CONNECT:
	  sim_session_cmd_connect (session, cmd);
	  break;
	case SIM_COMMAND_TYPE_SESSION_APPEND_PLUGIN:
	  sim_session_cmd_session_append_plugin (session, cmd);
	  break;
	case SIM_COMMAND_TYPE_SESSION_REMOVE_PLUGIN:
	  sim_session_cmd_session_remove_plugin (session, cmd);
	  break;
	case SIM_COMMAND_TYPE_SERVER_GET_SENSORS:
	  sim_session_cmd_server_get_sensors (session, cmd);
	  break;
	case SIM_COMMAND_TYPE_SERVER_GET_SENSOR_PLUGINS:
	  sim_session_cmd_server_get_sensor_plugins (session, cmd);
	  break;
	case SIM_COMMAND_TYPE_SENSOR_PLUGIN_START:
	  sim_session_cmd_sensor_plugin_start (session, cmd);
	  break;
	case SIM_COMMAND_TYPE_SENSOR_PLUGIN_STOP:
	  sim_session_cmd_sensor_plugin_stop (session, cmd);
	  break;
	case SIM_COMMAND_TYPE_SENSOR_PLUGIN_ENABLED:
	  sim_session_cmd_sensor_plugin_enabled (session, cmd);
	  break;
	case SIM_COMMAND_TYPE_SENSOR_PLUGIN_DISABLED:
	  sim_session_cmd_sensor_plugin_disabled (session, cmd);
	  break;
	case SIM_COMMAND_TYPE_PLUGIN_START:
	  sim_session_cmd_plugin_start (session, cmd);
	  break;
	case SIM_COMMAND_TYPE_PLUGIN_STOP:
	  sim_session_cmd_plugin_stop (session, cmd);
	  break;
	case SIM_COMMAND_TYPE_PLUGIN_ENABLED:
	  sim_session_cmd_plugin_enabled (session, cmd);
	  break;
	case SIM_COMMAND_TYPE_PLUGIN_DISABLED:
	  sim_session_cmd_plugin_disabled (session, cmd);
	  break;
	case SIM_COMMAND_TYPE_ALERT:
	  sim_session_cmd_alert (session, cmd);
	  break;
	case SIM_COMMAND_TYPE_RELOAD_PLUGINS:
	  sim_session_cmd_reload_plugins (session, cmd);
	  break;
	case SIM_COMMAND_TYPE_RELOAD_SENSORS:
	  sim_session_cmd_reload_sensors (session, cmd);
	  break;
	case SIM_COMMAND_TYPE_RELOAD_HOSTS:
	  sim_session_cmd_reload_hosts (session, cmd);
	  break;
	case SIM_COMMAND_TYPE_RELOAD_NETS:
	  sim_session_cmd_reload_nets (session, cmd);
	  break;
	case SIM_COMMAND_TYPE_RELOAD_POLICIES:
	  sim_session_cmd_reload_policies (session, cmd);
	  break;
	case SIM_COMMAND_TYPE_RELOAD_DIRECTIVES:
	  sim_session_cmd_reload_directives (session, cmd);
	  break;
	case SIM_COMMAND_TYPE_RELOAD_ALL:
	  sim_session_cmd_reload_all (session, cmd);
	  break;
	case SIM_COMMAND_TYPE_HOST_OS_CHANGE:
	  sim_session_cmd_host_os_change (session, cmd);
	  break;
	case SIM_COMMAND_TYPE_HOST_MAC_CHANGE:
	  sim_session_cmd_host_mac_change (session, cmd);
	  break;
	case SIM_COMMAND_TYPE_OK:
	  sim_session_cmd_ok (session, cmd);
	  break;
	case SIM_COMMAND_TYPE_ERROR:
	  sim_session_cmd_error (session, cmd);
	  break;
	defalut:
	  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_read: error command unknown type");
	  res = sim_command_new_from_type (SIM_COMMAND_TYPE_ERROR);
	  res->id = cmd->id;

	  sim_session_write (session, res);
	  g_object_unref (res);
	  break;
	}

      g_object_unref (cmd);
    }
}

/*
 *
 *
 *
 */
gint
sim_session_write (SimSession  *session,
		   SimCommand  *command)
{
  GIOError  error;
  gchar    *str;
  guint     n;

  g_return_val_if_fail (session != NULL, 0);
  g_return_val_if_fail (SIM_IS_SESSION (session), 0);
  g_return_val_if_fail (session->_priv->io != NULL, 0);

  str = sim_command_get_string (command);
  if (!str)
    return 0;

  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_write: %s", str);

  error = gnet_io_channel_writen (session->_priv->io, str, strlen(str), &n);

  g_free (str);

  if  (error != G_IO_ERROR_NONE)
    return 0;

  return n;
}

/*
 *
 *
 *
 */
gboolean
sim_session_has_plugin_type (SimSession     *session,
			     SimPluginType   type)
{
  GList  *list;
  gboolean  found = FALSE;

  g_return_val_if_fail (session != NULL, FALSE);
  g_return_val_if_fail (SIM_IS_SESSION (session), FALSE);
  
  list = session->_priv->plugins;
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

  return found;
}

/*
 *
 *
 *
 */
gboolean
sim_session_has_plugin_id (SimSession     *session,
			   gint            plugin_id)
{
  GList  *list;
  gboolean  found = FALSE;

  g_return_val_if_fail (session != NULL, FALSE);
  g_return_val_if_fail (SIM_IS_SESSION (session), FALSE);
  
  list = session->_priv->plugin_states;
  while (list)
    {
      SimPluginState  *plugin_state = (SimPluginState *) list->data;
      SimPlugin  *plugin = sim_plugin_state_get_plugin (plugin_state);

      if (sim_plugin_get_id (plugin) == plugin_id)
	{
	  found = TRUE;
	  break;
	}

      list = list->next;
    }

  return found;
}


/*
 *
 *
 *
 */
void
sim_session_reload (SimSession     *session)
{
  GList  *list;
  list = session->_priv->plugin_states;
  while (list)
    {
      SimPluginState  *plugin_state = (SimPluginState *) list->data;
      gint plugin_id = sim_plugin_state_get_plugin_id (plugin_state);

      SimPlugin *plugin = sim_container_get_plugin_by_id (ossim.container, plugin_id);

      sim_plugin_state_set_plugin (plugin_state, plugin);

      list = list->next;
    }
}

/*
 *
 *
 *
 */
SimSensor*
sim_session_get_sensor (SimSession *session)
{
  g_return_val_if_fail (session, NULL);
  g_return_val_if_fail (SIM_IS_SESSION (session), NULL);

  return session->_priv->sensor;
}

/*
 *
 *
 *
 */
gboolean
sim_session_is_connected (SimSession *session)
{
  g_return_val_if_fail (session, FALSE);
  g_return_val_if_fail (SIM_IS_SESSION (session), FALSE);

  return session->_priv->connect; 
} 

/*
 *
 *
 *
 */
void
sim_session_close (SimSession *session)
{
  g_return_if_fail (session);
  g_return_if_fail (SIM_IS_SESSION (session));
  
  session->_priv->close = TRUE;
}
