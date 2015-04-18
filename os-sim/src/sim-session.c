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

#include "sim-session.h"
#include "sim-rule.h"
#include "sim-directive.h"
#include "sim-server.h"
#include "sim-container.h"
#include "sim-sensor.h"
#include "sim-command.h"

extern SimContainer  *sim_ctn;

G_LOCK_EXTERN (s_mutex_directives);

enum 
{
  DESTROY,
  LAST_SIGNAL
};

struct _SimSessionPrivate {
  GTcpSocket    *socket;

  SimServer     *server;
  SimConfig     *config;
  SimDatabase   *db_ossim;

  SimSensor     *sensor;
  GList         *plugins;

  GIOChannel    *io;

  gint           seq;
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

  if (session->_priv->io)
    g_io_channel_unref(session->_priv->io);

  g_object_unref (session->_priv->db_ossim);

  g_free (session->_priv);

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
  session->_priv->db_ossim = NULL;
  session->_priv->server = NULL;

  session->_priv->sensor = NULL;
  session->_priv->plugins = NULL;

  session->_priv->io = NULL;

  session->_priv->seq = 0;
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
  SimSession   *session = NULL;
  SimConfigDS  *ds;

  g_return_val_if_fail (server != NULL, NULL);
  g_return_val_if_fail (SIM_IS_SERVER (server), NULL);
  g_return_val_if_fail (config != NULL, NULL);
  g_return_val_if_fail (SIM_IS_CONFIG (config), NULL);
  g_return_val_if_fail (socket != NULL, NULL);

  ds = sim_config_get_ds_by_name (config, SIM_DS_OSSIM);

  session = SIM_SESSION (g_object_new (SIM_TYPE_SESSION, NULL));
  session->_priv->config = config;
  session->_priv->db_ossim = sim_database_new (ds);
  session->_priv->server = server;
  session->_priv->socket = socket;
  session->_priv->io = gnet_tcp_socket_get_io_channel (socket);

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

  if (command->data.connect.sensor)
    {
      sensor = sim_container_get_sensor_by_name (sim_ctn, command->data.connect.sensor);
      if (sensor)
	{
	  session->type = SIM_SESSION_TYPE_SENSOR;
	  session->_priv->sensor = sensor;
	}
    }

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
sim_session_cmd_session_append_plugin (SimSession  *session,
				       SimCommand  *command)
{
  SimCommand  *cmd;
  SimPlugin   *plugin = NULL;

  g_return_if_fail (session != NULL);
  g_return_if_fail (SIM_IS_SESSION (session));
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));

  session->type = SIM_SESSION_TYPE_SENSOR;

  plugin = sim_container_get_plugin_by_id (sim_ctn, command->data.session_append_plugin.id);
  if (plugin)
    {
      session->_priv->plugins = g_list_append (session->_priv->plugins, plugin);

      cmd = sim_command_new_from_type (SIM_COMMAND_TYPE_OK);
      cmd->id = command->id;

      sim_session_write (session, cmd);
      g_object_unref (cmd);
  
      /* Directives with root rule type MONITOR */
      if (plugin->type == SIM_PLUGIN_TYPE_MONITOR)
	{      
	  G_LOCK (s_mutex_directives);
	  GList *directives = sim_container_get_directives_ul (sim_ctn);
	  while (directives)
	    {
	      SimDirective *directive = (SimDirective *) directives->data;
	      SimRule *rule = sim_directive_get_root_rule (directive);

	      if (sim_rule_get_plugin_id (rule) == command->data.session_append_plugin.id)
		{
		  cmd = sim_command_new_from_rule (rule);
		  sim_session_write (session, cmd);
		}

	      directives = directives->next;
	    }
	  g_list_free (directives);
	  G_UNLOCK (s_mutex_directives);
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

  plugin = sim_container_get_plugin_by_id (sim_ctn, command->data.session_remove_plugin.id);
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

/*
 *
 *
 *
 */
static void
sim_session_cmd_alert (SimSession  *session,
		       SimCommand  *command)
{
  SimAlert    *alert;

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

  sim_container_push_alert (sim_ctn, alert);
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


  sim_container_free_plugins (sim_ctn);
  sim_container_db_load_plugins (sim_ctn, session->_priv->db_ossim);
  
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


  sim_container_free_sensors (sim_ctn);
  sim_container_db_load_sensors (sim_ctn, session->_priv->db_ossim);

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


  sim_container_free_hosts (sim_ctn);
  sim_container_db_load_hosts (sim_ctn,
			       session->_priv->db_ossim);

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

  sim_container_free_nets (sim_ctn);
  sim_container_db_load_nets (sim_ctn,
			      session->_priv->db_ossim);

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

  sim_container_free_policies (sim_ctn);
  sim_container_db_load_policies (sim_ctn, session->_priv->db_ossim);

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

  sim_container_free_directives (sim_ctn);
  sim_container_load_directives_from_file (sim_ctn,
					   session->_priv->db_ossim,
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

  g_return_if_fail (session != NULL);
  g_return_if_fail (SIM_IS_SESSION (session));
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));

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
  g_return_if_fail (session->_priv->io != NULL);
  
  while ((error = gnet_io_channel_readline (session->_priv->io, buffer, BUFFER_SIZE, &n)) == G_IO_ERROR_NONE && (n > 0))
    {
      if (error != G_IO_ERROR_NONE)
	{
	  g_message ("Recived error %d (closing socket)", error);
	  break;
	}
      g_message ("READ: %s", buffer);


      cmd = sim_command_new_from_buffer (buffer);

      if (!cmd)
	{
	  continue;
	}

      if (cmd->type == SIM_COMMAND_TYPE_NONE)
	{
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
	case SIM_COMMAND_TYPE_OK:
	  sim_session_cmd_ok (session, cmd);
	  break;
	case SIM_COMMAND_TYPE_ERROR:
	  sim_session_cmd_error (session, cmd);
	  break;
	defalut:
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

  g_message ("WRITE: %s", str);

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
  
  list = session->_priv->plugins;
  while (list)
    {
      SimPlugin *plugin = (SimPlugin *) list->data;

      if (sim_plugin_get_id (plugin) == plugin_id)
	{
	  found = TRUE;
	  break;
	}

      list = list->next;
    }

  return found;
}
