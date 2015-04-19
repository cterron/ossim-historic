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
#include <string.h>

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
#include <config.h>

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

  guint          watch; 
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
 * FIXME: This function won't be called anymore. I'll remove it someday...
 *
 */
gboolean
async_client_iofunc (GIOChannel* iochannel, GIOCondition condition,
                     gpointer data)
{
  SimSession *session = (SimSession *) data;
  gboolean error=FALSE;

  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session: Entering async_client_iofunc. iochannel count: %d",iochannel->ref_count);
  
  //three error checking
  if (condition & (G_IO_ERR))
  {
      g_message ("async_client_iofunc G_IO_ERROR. iochannel count: %d",iochannel->ref_count);
      error=TRUE;
  }
  if (condition & (G_IO_HUP))
  {
      g_message ("async_client_iofunc G_IO_HUP. iochannel count: %d",iochannel->ref_count);
      error=TRUE;
  }
  if (condition & (G_IO_NVAL)) //FIX: when this happens (and it happens), the file descriptor is not open. Why!?!?
  {
      g_message ("async_client_iofunc G_IO_NVAL. iochannel count: %d",iochannel->ref_count);
      error=TRUE;
  }
  if (condition & (G_IO_IN))
    if (!sim_session_read(session))
    {
	  g_message ("Read failed, closing socket. iochannel count: %d",iochannel->ref_count);
          error=TRUE;
    }
  
  if (error)
  {
//      g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session Error: iochannel count: %d , source tag: %d",iochannel->ref_count,session->_priv->watch);
      g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session Error");

      g_source_remove (session->_priv->watch);
      session->_priv->close = TRUE;
      return FALSE;
  }

  if (condition & (G_IO_OUT)) //FIXME: this will never occurs inside this function.
    g_message ("async_client_iofunc G_IO_OUT");

  return TRUE;   // Returning TRUE will make sure the callback remains associated to the channel and the event source will not be removed.
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
  gchar *tempi;

  g_return_val_if_fail (server, NULL);
  g_return_val_if_fail (SIM_IS_SERVER (server), NULL);
  g_return_val_if_fail (config, NULL);
  g_return_val_if_fail (SIM_IS_CONFIG (config), NULL);
  g_return_val_if_fail (socket, NULL);

  session = SIM_SESSION (g_object_new (SIM_TYPE_SESSION, NULL));
  session->_priv->config = config;
  session->_priv->server = server;
  session->_priv->socket = socket;
  session->_priv->close=FALSE;
		
  session->_priv->ia = gnet_tcp_socket_get_remote_inetaddr (socket);

  if (gnet_inetaddr_is_loopback (session->_priv->ia)) //if the agent is in the same host than the server, we should get the real ip.
  {
    gnet_inetaddr_unref (session->_priv->ia);
    session->_priv->ia = gnet_inetaddr_get_host_addr ();
  }


  session->_priv->io = gnet_tcp_socket_get_io_channel (session->_priv->socket);

	if (!session->_priv->io) //FIXME: Why does this happens?
  {
    g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_new Error: channel with IP %s has been closed (NULL value)",gnet_inetaddr_get_canonical_name(session->_priv->ia));
    
    session->_priv->close=TRUE;
    return session;
  }
/****************ASYNC
  session->_priv->watch = g_io_add_watch (session->_priv->io, G_IO_IN | G_IO_ERR | G_IO_HUP | G_IO_NVAL,
					  async_client_iofunc, session);

  g_message ("sim_session_new: %s", gnet_inetaddr_get_canonical_name (session->_priv->ia));
*********/
  return session;
}

/*
 *
 *
 *
 */
GInetAddr*
sim_session_get_ia (SimSession *session)
{
  g_return_val_if_fail (session, NULL);
  g_return_val_if_fail (SIM_IS_SESSION (session), NULL);

  return session->_priv->ia;
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
  
  switch (command->data.connect.type)
    {
    case SIM_SESSION_TYPE_SERVER:
      sensor = sim_container_get_sensor_by_ia (ossim.container, session->_priv->ia);
      session->_priv->sensor = sensor;
      session->type = SIM_SESSION_TYPE_SERVER;
      break;
    case SIM_SESSION_TYPE_SENSOR:
      sensor = sim_container_get_sensor_by_ia (ossim.container, session->_priv->ia);
      session->_priv->sensor = sensor;
      session->type = SIM_SESSION_TYPE_SENSOR;
      break;
    case SIM_SESSION_TYPE_WEB:
      sensor = sim_container_get_sensor_by_ia (ossim.container, session->_priv->ia);
      session->_priv->sensor = sensor;
      session->type = SIM_SESSION_TYPE_WEB;
      break;
    default:
      break;
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
      gboolean is_sensor = sim_session_is_sensor (sess);

      if (!is_sensor)
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
	  cmd->data.sensor_plugin.sensor = gnet_inetaddr_get_canonical_name (sess->_priv->ia); //if this is not defined
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

  if (ia = gnet_inetaddr_new_nonblock (command->data.sensor_plugin_stop.sensor, 0))
  {
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
  else
    g_message("Error: Data sent from agent; trying to stop a plugin from a sensor IP wrong : %s",command->data.sensor_plugin_stop.sensor);
       
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

  if (ia = gnet_inetaddr_new_nonblock (command->data.sensor_plugin_disabled.sensor, 0))
  {
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
  else
    g_message("Error: Data sent from agent; trying to disable a plugin from a sensor IP wrong : %s",command->data.sensor_plugin_disabled.sensor);
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

static void
sim_session_cmd_plugin_unknown (SimSession  *session,
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

      if (id == command->data.plugin_unknown.plugin_id)
	sim_plugin_state_set_state (plugin_state, 3);

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
sim_session_cmd_event (SimSession  *session,
								       SimCommand  *command)
{
  SimPluginSid  *plugin_sid;
  SimEvent      *event;

  g_return_if_fail (session != NULL);
  g_return_if_fail (SIM_IS_SESSION (session));
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));

  event = sim_command_get_event (command); //generates an event from the command received

  if (!event)
    return;

  if (event->type == SIM_EVENT_TYPE_NONE)
    {
      g_object_unref (event);
      return;
    }

  if ((event->plugin_id) && (event->plugin_sid))
    {
      plugin_sid = sim_container_get_plugin_sid_by_pky (ossim.container,
																								       event->plugin_id,
																								       event->plugin_sid);
      if( (event->priority = sim_plugin_sid_get_priority(plugin_sid)) == -1 )
      {
        g_message("Error: Unable to fetch priority for plugin id %d, plugin sid %d", event->plugin_id, event->plugin_sid);
        event->priority = 1;
      }      
      if( (event->reliability = sim_plugin_sid_get_reliability(plugin_sid)) == -1 )
      {
        g_message("Error: Unable to fetch reliability for plugin id %d, plugin sid %d.", event->plugin_id, event->plugin_sid);
        event->reliability = 1;
      }
    }
	else
		{
      g_message("Error: Plugin_id or plugin_sid from event is wrong: plugin id %d, plugin sid %d", event->plugin_id, event->plugin_sid);
		  g_object_unref (event);
			return;
		}

  if (session->type == SIM_SESSION_TYPE_RSERVER)
    {
      event->id = 0;
      event->snort_cid = 0;
      event->snort_sid = 0;
      event->rserver = TRUE;
    }


	
  sim_container_push_event (ossim.container, event); //push the event in the queue

	GInetAddr *sensor = gnet_inetaddr_new_nonblock (command->data.event.sensor, 0);
	sim_container_set_sensor_event_number (ossim.container, SIM_EVENT_EVENT, sensor);
	gnet_inetaddr_unref (sensor);
		
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

  sim_container_db_delete_plugin_sid_directive_ul (ossim.container, ossim.dbossim);
  sim_container_db_delete_backlogs_ul (ossim.container, ossim.dbossim);

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
void
sim_session_cmd_host_os_event (SimSession  *session,
															SimCommand  *command)
{
  SimConfig   *config;
  SimEvent    *event;
  GInetAddr   *ia=NULL;
  GInetAddr   *sensor=NULL;
  gchar       *os = NULL;
  struct tm    tm;

		
	g_return_if_fail (session);
	g_return_if_fail (SIM_IS_SESSION (session));
	g_return_if_fail (command);
	g_return_if_fail (SIM_IS_COMMAND (command));
	g_return_if_fail (command->data.host_os_event.date);
	g_return_if_fail (command->data.host_os_event.host);
	g_return_if_fail (command->data.host_os_event.os);
	g_return_if_fail (command->data.host_os_event.sensor);
	g_return_if_fail (command->data.host_os_event.interface);
	g_return_if_fail (command->data.host_os_event.plugin_id > 0);
	g_return_if_fail (command->data.host_os_event.plugin_sid > 0);
	
	config = session->_priv->config;

	if (ia = gnet_inetaddr_new_nonblock (command->data.host_os_event.host, 0))
	{
		sensor = gnet_inetaddr_new_nonblock (command->data.host_os_event.sensor, 0);    
		
		os = sim_container_db_get_host_os_ul (ossim.container,
																				 ossim.dbossim,
																				 ia,
																				 sensor);
		event = sim_event_new ();
		
		if (!os) //the new event is inserted into db.
		{
			event->plugin_sid = EVENT_NEW;
		}
		else
		if (!g_ascii_strcasecmp (os, command->data.host_os_event.os))
		{
			event->plugin_sid = EVENT_SAME;
		}
		else // we insert the event, but it's in database at this moment.
		{
			event->plugin_sid = EVENT_CHANGE;      
		}
		
		
		event->type = SIM_EVENT_TYPE_DETECTOR;
		event->alarm = FALSE;
		event->protocol=SIM_PROTOCOL_TYPE_HOST_OS_EVENT;
		event->plugin_id = command->data.host_os_event.plugin_id;

		event->sensor = g_strdup (command->data.host_os_event.sensor);
		event->interface = g_strdup (command->data.host_os_event.interface);
		if (strptime (command->data.host_os_event.date, "%Y-%m-%d %H:%M:%S", &tm))
			event->time =  mktime (&tm);
		else
			event->time = time (NULL);

		if (gnet_inetaddr_get_canonical_name(ia))
		{
			event->src_ia = ia;
		}
		else
		{
			event->src_ia = gnet_inetaddr_new_nonblock ("0.0.0.0", 0);
		}
		
		event->dst_ia = gnet_inetaddr_new_nonblock ("0.0.0.0", 0);							

	  event->data = g_strdup_printf ("%s --> %s", (os) ? os : command->data.host_os_event.os,
																							 command->data.host_os_event.os);

  	//this is used to pass the event data to sim-organizer, so it can insert it into database
    event->data_storage = g_new(gchar*, 2);
   	event->data_storage[0] = g_strdup((os) ? os : command->data.host_os_event.os);
  	event->data_storage[1] = NULL;  

		sim_container_push_event (ossim.container, event);
		sim_container_set_sensor_event_number (ossim.container, SIM_EVENT_HOST_OS_EVENT, sensor);
	
	
		if (os)
		  g_free (os);
		gnet_inetaddr_unref (sensor);
	  
  }
  else
    g_message("Error: Data sent from agent; host OS event wrong src IP %s",command->data.host_os_event.host);
}

/*
 *
 *
 *
 */
static void
sim_session_cmd_host_mac_event (SimSession  *session,
												        SimCommand  *command)
{
  SimConfig   *config;
  SimEvent    *event;
  GInetAddr   *ia=NULL;
  gchar       *mac = NULL;
  gchar       *vendor = NULL;
  GInetAddr   *sensor;   
  struct tm    tm;

  g_return_if_fail (session != NULL);
  g_return_if_fail (SIM_IS_SESSION (session));
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));
  g_return_if_fail (command->data.host_mac_event.date);
  g_return_if_fail (command->data.host_mac_event.host);
  g_return_if_fail (command->data.host_mac_event.mac);
  g_return_if_fail (command->data.host_mac_event.sensor);
  g_return_if_fail (command->data.host_mac_event.vendor); 
  g_return_if_fail (command->data.host_mac_event.interface); 
  g_return_if_fail (command->data.host_mac_event.plugin_id > 0);
  g_return_if_fail (command->data.host_mac_event.plugin_sid > 0);
 
  config = session->_priv->config;

	//FIXME: I don't know why, but sometimes the MAC is not correctly compared with the g_ascii_strcasecmp() below, 
	//so we put all the letters in uppercase to try to avoid the failures. /me remove the FIXME when checked everything.
	gchar *aux = 	g_ascii_strup(command->data.host_mac_event.mac, -1);
	g_free (command->data.host_mac_event.mac);
	command->data.host_mac_event.mac = aux;	
	
	g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_cmd_host_mac_event: command->data.host_mac_event.mac: %s",command->data.host_mac_event.mac);

  if (ia = gnet_inetaddr_new_nonblock (command->data.host_mac_event.host, 0))
  {
    sensor = gnet_inetaddr_new_nonblock (command->data.host_mac_event.sensor, 0);    
    mac = sim_container_db_get_host_mac_ul (ossim.container, //get the mac wich should be the ia mac.
																					 ossim.dbossim,
																					 ia,
																					 sensor);

		g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_cmd_host_mac_event: mac: %s",mac);
    
    event = sim_event_new ();
    if (!mac) //if the ia-sensor pair doesn't obtains a mac in the database, inserts the new one.
    {
			/*
      sim_container_db_insert_host_mac_ul (ossim.container,
																				  ossim.dbossim,
																				  ia,
																				  command->data.host_mac_event.date,
																				  command->data.host_mac_event.mac,
																				  command->data.host_mac_event.vendor,
																				  command->data.host_mac_event.interface,
																				  sensor);*/
      event->plugin_sid = EVENT_NEW; 
			g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_cmd_host_mac_event: EVENT_NEW");
    }
    else  
    if (!g_ascii_strcasecmp (mac, command->data.host_mac_event.mac)) //the mac IS the same (0 = exact match)
    {
      event->plugin_sid = EVENT_SAME;
			g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_cmd_host_mac_event: EVENT_SAME");
    }
    else //the mac is different
    {
			g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_cmd_host_mac_event: EVENT_CHANGE");

			event->plugin_sid = EVENT_CHANGE;       
    }

    event->type = SIM_EVENT_TYPE_DETECTOR;
    event->alarm = FALSE;
    event->plugin_id = command->data.host_mac_event.plugin_id;
		event->protocol=SIM_PROTOCOL_TYPE_HOST_ARP_EVENT;

    event->sensor = g_strdup (command->data.host_mac_event.sensor);
    event->interface = g_strdup (command->data.host_mac_event.interface);
    if (strptime (command->data.host_mac_event.date, "%Y-%m-%d %H:%M:%S", &tm))
      event->time =  mktime (&tm);
    else
      event->time = time (NULL);

		if (gnet_inetaddr_get_canonical_name(ia))
	    event->src_ia = ia;
		else
		{
			event->src_ia = gnet_inetaddr_new_nonblock ("0.0.0.0", 0);
    	g_message("Error: Data sent from agent; host MAC event wrong IP %s",command->data.host_mac_event.host);
		}

	  event->dst_ia = gnet_inetaddr_new_nonblock ("0.0.0.0", 0);							
		//if the format of event->data changes, it must be changed also in sim_organizer_snort
		event->data = g_strdup_printf ("%s|%s --> %s|%s", (mac) ? mac : command->data.host_mac_event.mac,
																											(vendor) ? vendor : "",
																											command->data.host_mac_event.mac,
																											(command->data.host_mac_event.vendor) ? command->data.host_mac_event.vendor : "");
	
  	//this is used to pass the event data to sim-organizer, so it can insert it into database
    event->data_storage = g_new(gchar*, 3);
   	event->data_storage[0] = g_strdup((mac) ? mac : command->data.host_mac_event.mac);
	  event->data_storage[1] = g_strdup((vendor) ? vendor : "");
  	event->data_storage[2] = NULL;  


    sim_container_push_event (ossim.container, event);
		sim_container_set_sensor_event_number (ossim.container, SIM_EVENT_HOST_MAC_EVENT, sensor);
	
    if (mac)
      g_free (mac);
    gnet_inetaddr_unref (sensor);
    if (vendor)
      g_free (vendor);
    
  }
  else
    g_message("Error: Data sent from agent; host MAC event wrong IP %s",command->data.host_mac_event.host);

	g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "command->data.host_mac_event.date: %s",command->data.host_mac_event.date);
	g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_cmd_host_mac_event: TYPE: %d",event->plugin_sid);
				
}

/*
 * PADS plugin (or redirect to MAC plugin)
 *
 *
 */
static void
sim_session_cmd_host_service_event (SimSession  *session,
																	  SimCommand  *command)
{
  SimConfig		*config;
  SimEvent		*event;
  GInetAddr		*ia=NULL;
  gint				port = 0;
  gint				protocol = 0;
  gchar				*mac = NULL;
  gchar				*vendor = NULL;
  GInetAddr 	*sensor;   
  gchar				*application = NULL;
  struct tm		tm;
  SimCommand  *cmd;

  g_return_if_fail (session != NULL);
  g_return_if_fail (SIM_IS_SESSION (session));
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));
  g_return_if_fail (command->data.host_service_event.date);
  g_return_if_fail (command->data.host_service_event.host);
  g_return_if_fail (command->data.host_service_event.service);
  g_return_if_fail (command->data.host_service_event.sensor);
  g_return_if_fail (command->data.host_service_event.interface);
  g_return_if_fail (command->data.host_service_event.application);
  g_return_if_fail (command->data.host_service_event.plugin_id > 0);
  g_return_if_fail (command->data.host_service_event.plugin_sid > 0);

  // We don't use icmp. Maybe useful for a list of active hosts....
  if (command->data.host_service_event.protocol == 1)
    return;
  
  if (ia = gnet_inetaddr_new_nonblock (command->data.host_service_event.host, 0))
  {
    config = session->_priv->config;
  
    sensor = gnet_inetaddr_new_nonblock (command->data.host_service_event.sensor, 0);
    
    // Check if we've got a mac to call host_mac_event and insert it.
    if (!g_ascii_strcasecmp (command->data.host_service_event.service, "ARP"))
    {
      //as the pads plugin uses the same variables to store mac changes and services changes, we must normalize it.
      cmd = sim_command_new_from_type(SIM_COMMAND_TYPE_HOST_MAC_EVENT);
      cmd->data.host_mac_event.date = command->data.host_service_event.date;
      cmd->data.host_mac_event.host = command->data.host_service_event.host;
      cmd->data.host_mac_event.mac = command->data.host_service_event.application;
      cmd->data.host_mac_event.sensor = command->data.host_service_event.sensor;
      cmd->data.host_mac_event.interface = command->data.host_service_event.interface;
      cmd->data.host_mac_event.vendor = g_strdup_printf(" "); //FIXME: this will be usefull when pads get patched to know the vendor
      cmd->data.host_mac_event.plugin_id = sim_container_get_plugin_id_by_name(ossim.container, "arpwatch");
      cmd->data.host_mac_event.plugin_sid = EVENT_UNKNOWN;
  
      sim_session_cmd_host_mac_event (session, cmd);
    }
    else //ok, this is not a MAC change event, its a service change event
    {
      event = sim_event_new ();
    
      port = command->data.host_service_event.port;
      protocol = command->data.host_service_event.protocol;
      application = sim_container_db_get_host_service_ul (ossim.container, ossim.dbossim, ia, port, protocol, sensor);
 
      if (!application) //first time this service is saw
      {
				event->plugin_sid = EVENT_NEW;
      }
      else
      if (!g_ascii_strcasecmp (application, command->data.host_service_event.application)) //service is the same
      {
        event->plugin_sid = EVENT_SAME;
      }
      else //The service is different
				event->plugin_sid = EVENT_CHANGE;

  
      event->type = SIM_EVENT_TYPE_DETECTOR;
      event->alarm = FALSE;
      event->plugin_id = command->data.host_service_event.plugin_id;
			event->protocol=SIM_PROTOCOL_TYPE_HOST_SERVICE_EVENT;
  
    	event->sensor = g_strdup (command->data.host_service_event.sensor);
	    event->interface = g_strdup (command->data.host_service_event.interface);
      if (strptime (command->data.host_service_event.date, "%Y-%m-%d %H:%M:%S", &tm))
        event->time =  mktime (&tm);
      else
        event->time = time (NULL);
  
			if (gnet_inetaddr_get_canonical_name(ia))
	      event->src_ia = ia;
			else
	    {
  	    event->src_ia = gnet_inetaddr_new_nonblock ("0.0.0.0", 0);
    	  g_message("Error: Data sent from agent; host Service event wrong IP %s",command->data.host_service_event.host);
	    }
			
	  	event->dst_ia = gnet_inetaddr_new_nonblock ("0.0.0.0", 0);							
      event->data = g_strdup_printf ("%d/%d - %s/%s", port, protocol, command->data.host_service_event.service, (application) ? application: command->data.host_service_event.application );
			
	    //this is used to pass the event data to sim-organizer, so it can insert it into database
  	  event->data_storage = g_new(gchar*, 5);
			event->data_storage[0] = g_strdup_printf ("%d", port); 
			event->data_storage[1] = g_strdup_printf ("%d", protocol); 
    	event->data_storage[2] = g_strdup(command->data.host_service_event.service);
	    event->data_storage[3] = g_strdup( (application) ? application: command->data.host_service_event.application);
    	event->data_storage[4] = NULL;  

     	sim_container_push_event (ossim.container, event);
			sim_container_set_sensor_event_number (ossim.container, SIM_EVENT_HOST_SERVICE_EVENT, sensor);
	
      g_free (application);
      gnet_inetaddr_unref (sensor);
    }	
  }
	else
    g_message("Error: Data sent from agent; host MAC or OS event wrong IP %s",command->data.host_service_event.host);


}

/*
 *
 * HIDS
 *
 */
static void
sim_session_cmd_host_ids_event (SimSession  *session,
															  SimCommand  *command)
{
  SimConfig	*config;
  SimEvent	*event;
  GInetAddr	*ia=NULL;
  GInetAddr	*sensor;
  struct tm	tm;

  g_return_if_fail (session != NULL);
  g_return_if_fail (SIM_IS_SESSION (session));
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));
  g_return_if_fail (command->data.host_ids_event.date);
  g_return_if_fail (command->data.host_ids_event.host);
  g_return_if_fail (command->data.host_ids_event.hostname);
  g_return_if_fail (command->data.host_ids_event.event_type);
  g_return_if_fail (command->data.host_ids_event.target);
  g_return_if_fail (command->data.host_ids_event.what);
  g_return_if_fail (command->data.host_ids_event.extra_data);
  g_return_if_fail (command->data.host_ids_event.sensor);
  g_return_if_fail (command->data.host_ids_event.plugin_id > 0);
  g_return_if_fail (command->data.host_ids_event.plugin_sid > 0);
  g_return_if_fail (command->data.host_ids_event.log);

  if (ia = gnet_inetaddr_new_nonblock (command->data.host_ids_event.host, 0))
  {
    config = session->_priv->config;

    event = sim_event_new ();
    event->type = SIM_EVENT_TYPE_DETECTOR;
    event->alarm = FALSE;
  
    event->sensor = g_strdup (command->data.host_ids_event.sensor);
    if (strptime (command->data.host_ids_event.date, "%Y-%m-%d %H:%M:%S", &tm))
      event->time =  mktime (&tm);
    else
      event->time = time (NULL);
  
    event->plugin_id = command->data.host_ids_event.plugin_id;
    event->plugin_sid = command->data.host_ids_event.plugin_sid;
    if (gnet_inetaddr_get_canonical_name(ia))
      event->src_ia = ia;
    else
    {
      event->src_ia = gnet_inetaddr_new_nonblock ("0.0.0.0", 0);
      g_message("Error: Data sent from agent; host Service event wrong IP %s",command->data.host_service_event.host);
    }

	  event->dst_ia = gnet_inetaddr_new_nonblock ("0.0.0.0", 0);							
		event->interface = g_strdup("unknown");
  
    event->data = g_strdup(command->data.host_ids_event.log);

		//this is used to pass the event data to sim-organizer, so it can insert it into database
		event->data_storage = g_new(gchar*, 6);
		event->data_storage[0] = g_strdup(command->data.host_ids_event.hostname);
		event->data_storage[1] = g_strdup(command->data.host_ids_event.event_type);
		event->data_storage[2] = g_strdup(command->data.host_ids_event.target);
		event->data_storage[3] = g_strdup(command->data.host_ids_event.what);
		event->data_storage[4] = g_strdup(command->data.host_ids_event.extra_data);
		event->data_storage[5] = NULL;	//this is needed to free this (inside sim_organizer_snort, btw)
		
		event->protocol=SIM_PROTOCOL_TYPE_HOST_IDS_EVENT;
  
    sim_container_push_event (ossim.container, event);
    sim_container_set_sensor_event_number (ossim.container, SIM_EVENT_HOST_IDS_EVENT, sensor);
		
  }
  else
    g_message("Error: Data sent from agent; error from host ids event, IP: %s",command->data.host_ids_event.host);
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
gboolean
sim_session_read (SimSession  *session)
{
  SimCommand  *cmd;
  SimCommand  *res;
  GIOError     error;
  gchar        buffer[BUFFER_SIZE];
  guint        n;

  g_return_val_if_fail (session != NULL, FALSE);
  g_return_val_if_fail (SIM_IS_SESSION (session), FALSE);

  memset(buffer, 0, BUFFER_SIZE);

  while ( (!session->_priv->close) && 
					(error = gnet_io_channel_readline (session->_priv->io, buffer, BUFFER_SIZE, &n)) == G_IO_ERROR_NONE && (n>0) )
  {
	    g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_read: Entering while. Session: %x", session);
	    g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_read: strlen(buffer)=%d; n=%d",strlen(buffer),n);
	    g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_read: Buffer: %s", buffer);

//      g_message("GIOError: %d",error);	 
      
      //sanity checks...
    if (error != G_IO_ERROR_NONE)
    {
		  g_message ("Received error, closing socket: %d: %s", error, g_strerror(error));
	  	return FALSE;
    }

/*
      if (strnlen(buffer,BUFFER_SIZE) == BUFFER_SIZE) 
      {
        g_message("Error: Data received from the agent > %d, line truncated.");
	return FALSE;
      }
      
      if (strnlen(buffer,BUFFER_SIZE) < n-1 )
      {
         g_message("Error: Data received from the agent has a \"0\" character before newline");
         return FALSE;
      }
  */   
    if (n == 0)
		{
		  g_message ("0 bytes read (closing socket)");
	  	return FALSE;
		}

    if (!buffer)
		{
    	g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_read: Buffer NULL");
			return FALSE;
		}

		//FIXME: WHY the F*CK this happens?? strlen(buffer) sometimes is =1!!!
		//g_message("Data received: -%s- Count: %d  n: %d",buffer,strnlen(buffer,BUFFER_SIZE),n);	 
		if (strlen (buffer) <= 2) 
		{
	    g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_read: Buffer <= 2 bytes");
			continue;
//			return FALSE; 
		}

    cmd = sim_command_new_from_buffer (buffer); //this gets the command and all of the parameters associated.

		if (!cmd)
		{
		  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_read: error command null");
	  	return FALSE;
		}

    g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_read: Command from buffer type:%d ; id=%d",cmd->type,cmd->id);
      
    if (cmd->type == SIM_COMMAND_TYPE_NONE)
		{
	  	g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_read: error command type none");
		  g_object_unref (cmd);
		  return FALSE;
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
			case SIM_COMMAND_TYPE_PLUGIN_UNKNOWN:
				sim_session_cmd_plugin_unknown (session, cmd);
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
			case SIM_COMMAND_TYPE_EVENT:
				sim_session_cmd_event (session, cmd);
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
			case SIM_COMMAND_TYPE_HOST_OS_EVENT:
				sim_session_cmd_host_os_event (session, cmd);
				break;
			case SIM_COMMAND_TYPE_HOST_MAC_EVENT:
				sim_session_cmd_host_mac_event (session, cmd);
				break;
			case SIM_COMMAND_TYPE_HOST_SERVICE_EVENT:
				sim_session_cmd_host_service_event (session, cmd);
				break;
			case SIM_COMMAND_TYPE_HOST_IDS_EVENT:
				sim_session_cmd_host_ids_event (session, cmd); 
				break;
			case SIM_COMMAND_TYPE_OK:
				sim_session_cmd_ok (session, cmd);
				break;
			case SIM_COMMAND_TYPE_ERROR:
				sim_session_cmd_error (session, cmd);
				break;
			default:
				g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_read: error command unknown type");
				res = sim_command_new_from_type (SIM_COMMAND_TYPE_ERROR);
				res->id = cmd->id;

				sim_session_write (session, res);
				g_object_unref (res);
				break;
		}

    g_object_unref (cmd);

		n=0;
  	memset(buffer, 0, BUFFER_SIZE);
		

	}
	g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_read: exiting function in session: %x", session);
			
  return TRUE;
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

  // cipher

  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_write: %s", str);

  error = gnet_io_channel_writen (session->_priv->io, str, strlen(str), &n);

  g_free (str);

  if  (error != G_IO_ERROR_NONE)
    {
      session->_priv->close = TRUE;
      return 0;
    }

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
Is the session from a sensor ?
*/

gboolean
sim_session_is_sensor (SimSession *session)
{
  g_return_val_if_fail (session, FALSE);
  g_return_val_if_fail (SIM_IS_SESSION (session), FALSE);

  if(session->type == SIM_SESSION_TYPE_SENSOR) return TRUE;

  return FALSE;
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
			
	g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_close: closing session: %x",session);
		
}

/*
 *
 *
 *
 */
gboolean
sim_session_is_close (SimSession *session)
{
  g_return_if_fail (session);
  g_return_if_fail (SIM_IS_SESSION (session));
  
  return session->_priv->close;
}
// vim: set tabstop=2:
