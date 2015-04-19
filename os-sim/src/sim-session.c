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

  SimServer		*server;
  SimConfig		*config;

  SimSensor		*sensor;
  GList				*plugins;
  GList				*plugin_states;

  GIOChannel	*io;

  GInetAddr		*ia;
  gint				seq;
  gboolean		close;
  gboolean		connect;
	gchar				*hostname;	//name of the machine connected. This can be a server name (it can be up or down in the architecture)
													//, a sensor name or even a frameworkd name
  guint       watch; 
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

	if (sim_session_is_sensor (session))
		g_message ("Session Sensor: REMOVED");
	else
	if (sim_session_is_web (session))
		g_message ("Session Web: REMOVED");
	else
	if (sim_session_is_master_server (session))
		g_message ("Session Master server: REMOVED");
	else
	if (sim_session_is_children_server (session))
		g_message ("Session Children Server: REMOVED");

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
  
	session->_priv->hostname = NULL;
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

  g_return_val_if_fail (server, NULL);
  g_return_val_if_fail (SIM_IS_SERVER (server), NULL);
  g_return_val_if_fail (config, NULL);
  g_return_val_if_fail (SIM_IS_CONFIG (config), NULL);
  g_return_val_if_fail (socket, NULL);

  session = SIM_SESSION (g_object_new (SIM_TYPE_SESSION, NULL));
  session->_priv->config = config;
  session->_priv->server = server;
  session->_priv->socket = socket;
  session->_priv->close = FALSE;
		
  session->_priv->ia = gnet_tcp_socket_get_remote_inetaddr (socket);

  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_new: remote IP/port: %s/%d", gnet_inetaddr_get_canonical_name(session->_priv->ia), 
																																										gnet_inetaddr_get_port (session->_priv->ia));
		
  if (gnet_inetaddr_is_loopback (session->_priv->ia)) //if the agent is in the same host than the server, we should get the real ip.
  {
    gnet_inetaddr_unref (session->_priv->ia);
    session->_priv->ia = gnet_inetaddr_get_host_addr ();
    g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_new Remote address is loopback, applying new address: %s ",gnet_inetaddr_get_canonical_name(session->_priv->ia));
  }

  session->_priv->io = gnet_tcp_socket_get_io_channel (session->_priv->socket);
  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_new session->_priv->io: %x",session->_priv->io);

	if (!session->_priv->io) //FIXME: Why does this happens?
  {
    g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_new Error: channel with IP %s has been closed (NULL value)",gnet_inetaddr_get_canonical_name(session->_priv->ia));
    
    session->_priv->close=TRUE;
    return session;
  }
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
 * The hostname in a session means the name of the connected machine. This can be i.e. the hostname of a server.
 * This has nothing to do with the FQDN of the machine, this is the OSSIM name.
 */
void
sim_session_set_hostname (SimSession *session,
													gchar *hostname)
{
  g_return_if_fail (session);
  g_return_if_fail (SIM_IS_SESSION (session));

	session->_priv->hostname = g_strdup (hostname);
}

/*
 */
gchar*
sim_session_get_hostname (SimSession *session)
{
  g_return_if_fail (session);
  g_return_if_fail (SIM_IS_SESSION (session));

	return session->_priv->hostname;
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
  
  sensor = sim_container_get_sensor_by_ia (ossim.container, session->_priv->ia);
  session->_priv->sensor = sensor;	//if the connection is from a server or frameworkd, this will be NULL.
	
  switch (command->data.connect.type)
  {
    case SIM_SESSION_TYPE_SERVER_DOWN:
		      session->type = SIM_SESSION_TYPE_SERVER_DOWN;
				  break;
    case SIM_SESSION_TYPE_SENSOR:
		      session->type = SIM_SESSION_TYPE_SENSOR;
				  break;
    case SIM_SESSION_TYPE_WEB:
		      session->type = SIM_SESSION_TYPE_WEB;
				  break;
    default:
		      session->type = SIM_SESSION_TYPE_NONE;
		      break;
  }
	
	if (command->data.connect.hostname)
		sim_session_set_hostname (session, command->data.connect.hostname);
	
	if (session->type != SIM_SESSION_TYPE_NONE)
	{
	  cmd = sim_command_new_from_type (SIM_COMMAND_TYPE_OK);
		cmd->id = command->id;

		sim_session_write (session, cmd);
	  g_object_unref (cmd);
		session->_priv->connect = TRUE;
	}
	else
	{
	  cmd = sim_command_new_from_type (SIM_COMMAND_TYPE_ERROR);
		cmd->id = command->id;

		sim_session_write (session, cmd);
	  g_object_unref (cmd);

		g_message("Received a strange session type. Clossing connection....");
		sim_session_close (session);
	}

}

/*
 * This command add one to the session plugin count in the server.
 *
 * If the plugin is a Monitor plugin, and it matches with a root node directive,
 * a msg is sent to the agent to test if it matches.
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

  session->type = SIM_SESSION_TYPE_SENSOR;	//FIXME: This will be desappear. A session always must be initiated
																						//with a "connect" command

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

/*
 * Send to the session connected (master server or frameworkd) a list with all the sensors connected.
 */
static void
sim_session_cmd_server_get_sensors (SimSession  *session,
																    SimCommand  *command)
{
  SimCommand  *cmd;
  GList       *list;
	gboolean		 for_this_server;

  g_return_if_fail (session != NULL);
  g_return_if_fail (SIM_IS_SESSION (session));
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));

	if (sim_session_is_master_server (session) ||
			sim_session_is_web (session))
	{
		g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_cmd_server_get_sensors Inside");
    SimServer *server = session->_priv->server;
		
		//Check if the message is for this server....
    if ((!command->data.server_get_sensors.servername) || //FIXME: If the server name isn't specified, for backwards compatibility
																													//we will assume that this is the dst server. This should be removed in 
																													//a near future. All the commands from frameworkd should issue a server name.
        (!g_ascii_strcasecmp (sim_server_get_name (server), command->data.server_get_sensors.servername)))
    	for_this_server = TRUE;
		else
    	for_this_server = FALSE;
					
			g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_cmd_server_get_sensors: %s, %s", sim_server_get_name (server), command->data.server_get_sensors.servername);

		list = sim_server_get_sessions (server);
	  while (list)	//list of the sessions connected to the server
		{
			SimSession *sess = (SimSession *) list->data;
			
			if (for_this_server)	//execute the command in this server
		  {
				g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_cmd_server_get_sensors Inside 2");
				if (sim_session_is_sensor (sess))	
				{
				  cmd = sim_command_new_from_type (SIM_COMMAND_TYPE_SENSOR);
					cmd->data.sensor.host = gnet_inetaddr_get_canonical_name (sess->_priv->ia);
					cmd->data.sensor.state = TRUE;
						
					sim_session_write (session, cmd);	//write the sensor info in the server master or web session
					g_object_unref (cmd);
				}
			}
			else	//resend the command buffer to the children servers whose name match.
			{
				if (sim_session_is_children_server (sess) && 
						!g_ascii_strcasecmp (command->data.server_get_sensors.servername, sim_session_get_hostname (sess)) )
				{
					sim_session_write_from_buffer (sess, command->buffer);
				}
			}
			list = list->next;
		}
		
	  g_list_free (list);
			
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
 * The state of the plugins, and if they are enabled or not, are "injected" to
 * the server each watchdog.interval seconds with the command
 *
 */
static void
sim_session_cmd_server_get_sensor_plugins (SimSession  *session,
																				   SimCommand  *command)
{
  SimCommand  *cmd;
  GList       *list;
  GList       *plugin_states;
	gboolean 		for_this_server;

  g_return_if_fail (session != NULL);
  g_return_if_fail (SIM_IS_SESSION (session));
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));

	if (sim_session_is_master_server (session) ||
			sim_session_is_web (session))
	{
    SimServer *server = session->_priv->server;
		
		//Check if the message is for this server....
    if ((!command->data.server_get_sensor_plugins.servername) || //FIXME: If the server name isn't specified, for backwards compatibility
																													//we will assume that this is the dst server. This should be removed in 
																													//a near future. All the commands from frameworkd should issue a server name.
        (!g_ascii_strcasecmp (sim_server_get_name (server), command->data.server_get_sensor_plugins.servername)))
    	for_this_server = TRUE;
		else
    	for_this_server = FALSE;
					
			g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_cmd_server_get_sensor_plugins: %s, %s", sim_server_get_name (server), command->data.server_get_sensor_plugins.servername);

		list = sim_server_get_sessions (server);
	  while (list)	//list of the sessions connected to the server
		{
			SimSession *sess = (SimSession *) list->data;
			
			if (for_this_server)	//execute the command in this server
		  {
				if (sim_session_is_sensor (sess))	
				{
		      plugin_states = sess->_priv->plugin_states;
		      while (plugin_states)
		      {
    	      SimPluginState  *plugin_state = (SimPluginState *) plugin_states->data;
      		  SimPlugin  *plugin = sim_plugin_state_get_plugin (plugin_state);

		        cmd = sim_command_new_from_type (SIM_COMMAND_TYPE_SENSOR_PLUGIN);
    		    cmd->data.sensor_plugin.plugin_id = sim_plugin_get_id (plugin);
		        cmd->data.sensor_plugin.sensor = gnet_inetaddr_get_canonical_name (sess->_priv->ia); //if this is not defined
    		    cmd->data.sensor_plugin.state = sim_plugin_state_get_state (plugin_state);
        		cmd->data.sensor_plugin.enabled = sim_plugin_state_get_enabled (plugin_state);

						sim_session_write (session, cmd);	//write the sensor info in the server master or web session
        		g_object_unref (cmd);

		        plugin_states = plugin_states->next;
      		}
				}
			}
			else	//resend the command buffer to the children servers whose name match.
			{
				if (sim_session_is_children_server (sess) && 
						!g_ascii_strcasecmp (command->data.server_get_sensor_plugins.servername, sim_session_get_hostname (sess)) )
				{
					sim_session_write_from_buffer (sess, command->buffer);
				}
			}
			list = list->next;
		    
		}
		
	  g_list_free (list);
			
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
 * tell to a specific server what should be done with the events that it receives
 *
 */
static void
sim_session_cmd_server_set_data_role (SimSession  *session,
                                    	SimCommand  *command)
{
  SimCommand  *cmd;
  GList       *list;
  GList       *sessions;

  g_return_if_fail (session != NULL);
  g_return_if_fail (SIM_IS_SESSION (session));
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));

	if (sim_session_is_master_server (session)) //check if the remote server has rights to send data to this server
	{	
		SimServer *server = session->_priv->server;
	  
		g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_cmd_server_set_data_role: servername: %s; set to server: %s", sim_server_get_name (server), command->data.server_set_data_role.servername);
		
		//Check if the command is regarding this server to get the data and store it in memory & database
		if (!g_ascii_strcasecmp (sim_server_get_name (server), command->data.server_set_data_role.servername))
		{
			sim_server_set_data_role (server, command);	

		}
		else
		{
			//send the data to other servers down in the architecture
		  list = sim_server_get_sessions (session->_priv->server);
			while (list)
			{
      	SimSession *sess = (SimSession *) list->data;

	      gboolean is_server = sim_session_is_children_server (sess);
	
  	    if (is_server)
    	  {
					gchar *hostname = sim_session_get_hostname (sess);
					if (!g_ascii_strcasecmp (hostname, command->data.server_set_data_role.servername))
					{

	  	    	cmd = sim_command_new_from_type (SIM_COMMAND_TYPE_SERVER_SET_DATA_ROLE);

						cmd->data.server_set_data_role.servername = g_strdup (command->data.server_set_data_role.servername);
						cmd->data.server_set_data_role.store = command->data.server_set_data_role.store;
						cmd->data.server_set_data_role.correlate = command->data.server_set_data_role.correlate;
						cmd->data.server_set_data_role.cross_correlate = command->data.server_set_data_role.cross_correlate;
						cmd->data.server_set_data_role.qualify = command->data.server_set_data_role.qualify;
						cmd->data.server_set_data_role.resend_alarm = command->data.server_set_data_role.resend_alarm;
						cmd->data.server_set_data_role.resend_event = command->data.server_set_data_role.resend_event;

					  sim_session_write (sess, cmd);
						g_object_unref (cmd);
						break; //just one server per message plz...
					}
				}
				
				list = list->next;

			}
	    g_list_free (list); //FIXME: check this and all other functions so session list are returned, not copied. Add mutexes to sessions.
		
		}

	}
	else
	{
		GInetAddr *ia;
		ia = sim_session_get_ia (session);
		g_message ("Error: Warning, %s is trying to send server role without rights!", gnet_inetaddr_get_canonical_name (ia));
	
	  cmd = sim_command_new_from_type (SIM_COMMAND_TYPE_ERROR);
		cmd->id = command->id;
  
	  sim_session_write (session, cmd);
		g_object_unref (cmd);
	

	}
}

/*
 * Tell to a sensor that it must start a specific plugin
 */
static void
sim_session_cmd_sensor_plugin_start (SimSession  *session,
																     SimCommand  *command)
{
  SimCommand  *cmd;
  GList       *list;
  GInetAddr   *ia;
	gboolean 		for_this_server;

  g_return_if_fail (session != NULL);
  g_return_if_fail (SIM_IS_SESSION (session));
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));

	if (sim_session_is_master_server (session) ||
			sim_session_is_web (session))
	{
    SimServer *server = session->_priv->server;
		
		//Check if the message is for this server....
    if ((!command->data.sensor_plugin_start.servername) || //FIXME: If the server name isn't specified, for backwards compatibility
																													//we will assume that this is the dst server. This should be removed in 
																													//a near future. All the commands from frameworkd should issue a server name.
        (!g_ascii_strcasecmp (sim_server_get_name (server), command->data.sensor_plugin_start.servername)))
    	for_this_server = TRUE;
		else
    	for_this_server = FALSE;
					
			g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_cmd_sensor_plugin_start: %s, %s", sim_server_get_name (server), command->data.sensor_plugin_start.servername);

  	ia = gnet_inetaddr_new_nonblock (command->data.sensor_plugin_start.sensor, 0); //FIXME: Remember to check this as soon as event arrive!!

		list = sim_server_get_sessions (server);
	  while (list)	//list of the sessions connected to the server
		{
			SimSession *sess = (SimSession *) list->data;
			
			if (for_this_server)	//execute the command in this server
		  {
				if (sim_session_is_sensor (sess))	
				{
	  	  	if (gnet_inetaddr_noport_equal (sess->_priv->ia, ia))	//FIXME:when agent support send names, this should be changed with sensor name
					{
		//				cmd = sim_command_new_from_type (SIM_COMMAND_TYPE_PLUGIN_START); //this is the command isssued TO sensor
						//Now we take the data from the command issued from web (in
						//cmd->data.sensor_plugin_start struct) and we copy it to resend it to the sensor in
						//cmd->data.plugin_start struct)
			//		  cmd->data.plugin_start.plugin_id = command->data.sensor_plugin_start.plugin_id;						
						sim_session_write (sess, command);	
				//		g_object_unref (cmd);
						gnet_inetaddr_unref (ia);
					}
				}
			}
			else	//resend the command buffer to the children servers whose name match.
			{
				if (sim_session_is_children_server (sess) && 
						!g_ascii_strcasecmp (command->data.sensor_plugin_start.servername, sim_session_get_hostname (sess)) )
				{
					sim_session_write_from_buffer (sess, command->buffer);
				}
			}
			list = list->next;
		}
		
	  g_list_free (list);
			
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
sim_session_cmd_sensor_plugin_stop (SimSession  *session,
																    SimCommand  *command)
{
  SimCommand  *cmd;
  GList       *list;
  GInetAddr   *ia;
	gboolean 		for_this_server;

  g_return_if_fail (session != NULL);
  g_return_if_fail (SIM_IS_SESSION (session));
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));

	if (sim_session_is_master_server (session) ||
			sim_session_is_web (session))
	{
    SimServer *server = session->_priv->server;
		g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_cmd_server_get_sensors: %s, %s", sim_server_get_name (server), command->data.sensor_plugin_stop.servername);
		
		//Check if the message is for this server....
    if ((!command->data.sensor_plugin_stop.servername) || //FIXME: If the server name isn't specified, for backwards compatibility
																													//we will assume that this is the dst server. This should be removed in 
																													//a near future. All the commands from frameworkd should issue a server name.
        (!g_ascii_strcasecmp (sim_server_get_name (server), command->data.sensor_plugin_stop.servername)))
    	for_this_server = TRUE;
		else
    	for_this_server = FALSE;
					

  	ia = gnet_inetaddr_new_nonblock (command->data.sensor_plugin_stop.sensor, 0);
		list = sim_server_get_sessions (server);
	  while (list)	//list of the sessions connected to the server
		{
			SimSession *sess = (SimSession *) list->data;
			
			if (for_this_server)	//execute the command in this server
		  {
				g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_cmd_server_get_sensors Inside 2");
				if (sim_session_is_sensor (sess))	
				{
		      if (gnet_inetaddr_noport_equal (sess->_priv->ia, ia))
    		  {
//		        cmd = sim_command_new_from_type (SIM_COMMAND_TYPE_PLUGIN_STOP);
//    		    cmd->data.plugin_stop.plugin_id = command->data.sensor_plugin_stop.plugin_id;
        		sim_session_write (sess, command);
//		        g_object_unref (cmd);
						gnet_inetaddr_unref (ia);
    		  }
				}
			}
			else	//resend the command buffer to the children servers whose name match.
			{
				if (sim_session_is_children_server (sess) && 
						!g_ascii_strcasecmp (command->data.sensor_plugin_stop.servername, sim_session_get_hostname (sess)) )
				{
					sim_session_write_from_buffer (sess, command->buffer);
				}
			}
			list = list->next;
		}
		
	  g_list_free (list);
			
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
 * This command can arrive from the web or a master server. It says that a
 * specific plugin must be enabled in a specific sensor.
 */
static void
sim_session_cmd_sensor_plugin_enable (SimSession  *session,
				     SimCommand  *command)
{
  SimCommand  *cmd;
  GList       *list;
  GInetAddr   *ia;
	gboolean 		for_this_server;

  g_return_if_fail (session != NULL);
  g_return_if_fail (SIM_IS_SESSION (session));
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));

	if (sim_session_is_master_server (session) ||
			sim_session_is_web (session))
	{
    SimServer *server = session->_priv->server;
		g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_cmd_sensor_plugin_enable: %s, %s", sim_server_get_name (server), command->data.sensor_plugin_enable.servername);
		
		//Check if the message is for this server....
    if ((!command->data.sensor_plugin_enable.servername) || //FIXME: If the server name isn't specified, for backwards compatibility
																													//we will assume that this is the dst server. This should be removed in 
																													//a near future. All the commands from frameworkd should issue a server name.
        (!g_ascii_strcasecmp (sim_server_get_name (server), command->data.sensor_plugin_enable.servername)))
    	for_this_server = TRUE;
		else
    	for_this_server = FALSE;					

  	ia = gnet_inetaddr_new_nonblock (command->data.sensor_plugin_enable.sensor, 0);
		list = sim_server_get_sessions (server);
	  while (list)	//list of the sessions connected to the server
		{
			SimSession *sess = (SimSession *) list->data;
			
			if (for_this_server)	//execute the command in this server
		  {
				if (sim_session_is_sensor (sess))	
				{
 			  	if (gnet_inetaddr_noport_equal (sess->_priv->ia, ia))
					{
//					  cmd = sim_command_new_from_type (SIM_COMMAND_TYPE_PLUGIN_ENABLED);
//						cmd->data.plugin_enabled.plugin_id = command->data.sensor_plugin_enabled.plugin_id;
					  sim_session_write (sess, command);
//					  g_object_unref (cmd);
						gnet_inetaddr_unref (ia);
    		  }
				}
			}
			else	//resend the command buffer to the children servers whose name match.
			{
				if (sim_session_is_children_server (sess) && 
						!g_ascii_strcasecmp (command->data.sensor_plugin_enable.servername, sim_session_get_hostname (sess)) )
				{
					sim_session_write_from_buffer (sess, command->buffer);
				}
			}
			list = list->next;
		}
	  g_list_free (list);
			
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
sim_session_cmd_sensor_plugin_disable (SimSession  *session,
					SimCommand  *command)
{
  SimCommand  *cmd;
  GList       *list;
  GInetAddr   *ia;
	gboolean 		for_this_server;

  g_return_if_fail (session != NULL);
  g_return_if_fail (SIM_IS_SESSION (session));
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));

	if (sim_session_is_master_server (session) ||
			sim_session_is_web (session))
	{
    SimServer *server = session->_priv->server;
		g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_cmd_sensor_plugin_disable: %s, %s", sim_server_get_name (server), command->data.sensor_plugin_disable.servername);
		
		//Check if the message is for this server....
    if ((!command->data.sensor_plugin_disable.servername) || //FIXME: If the server name isn't specified, for backwards compatibility
																													//we will assume that this is the dst server. This should be removed in 
																													//a near future. All the commands from frameworkd should issue a server name.
        (!g_ascii_strcasecmp (sim_server_get_name (server), command->data.sensor_plugin_disable.servername)))
    	for_this_server = TRUE;
		else
    	for_this_server = FALSE;					

  	ia = gnet_inetaddr_new_nonblock (command->data.sensor_plugin_disable.sensor, 0);
		list = sim_server_get_sessions (server);
	  while (list)	//list of the sessions connected to the server
		{
			SimSession *sess = (SimSession *) list->data;
			
			if (for_this_server)	//execute the command in this server
		  {
				if (sim_session_is_sensor (sess))	
				{
 			  	if (gnet_inetaddr_noport_equal (sess->_priv->ia, ia))
					{
//		        cmd = sim_command_new_from_type (SIM_COMMAND_TYPE_PLUGIN_DISABLED);
 //   		    cmd->data.plugin_disabled.plugin_id = command->data.sensor_plugin_disabled.plugin_id;
					  sim_session_write (sess, command);
//					  g_object_unref (cmd);
						gnet_inetaddr_unref (ia);
    		  }
				}
			}
			else	//resend the command buffer to the children servers whose name match.
			{
				if (sim_session_is_children_server (sess) && 
						!g_ascii_strcasecmp (command->data.sensor_plugin_disable.servername, sim_session_get_hostname (sess)) )
				{
					sim_session_write_from_buffer (sess, command->buffer);
				}
			}
			list = list->next;
		}
		
	  g_list_free (list);
			
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
 * This info has been sended already to the server in the first message, the
 * "session-append-plugin". But now we need to remember it each certain time.
 * The sensor sends this information each (agent) watchdog.interval seconds, 
 * so the server learn it perodically and never is able to ask for it in a
 * specific message.
 *
 */
static void
sim_session_cmd_plugin_state_started (SimSession  *session,
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

    if (id == command->data.plugin_state_started.plugin_id)
			sim_plugin_state_set_state (plugin_state, 1);

    list = list->next;
  }
}

static void
sim_session_cmd_plugin_state_unknown (SimSession  *session,
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

      if (id == command->data.plugin_state_unknown.plugin_id)
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
sim_session_cmd_plugin_state_stopped (SimSession  *session,
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

      if (id == command->data.plugin_state_stopped.plugin_id)
	sim_plugin_state_set_state (plugin_state, 2);

      list = list->next;
    }
}

/*
 *
 * Enabled means that the process is actively sending msgs to the server
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
sim_session_cmd_event (SimSession	*session,
								       SimCommand	*command)
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

  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_read: Inside");
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

			if (!plugin_sid)
			{
        g_message("Error: Unable to get plugin id %d and plugin sid %d from DB. Please check that it's inserted.", event->plugin_id, event->plugin_sid);
	      g_object_unref (event);
				return;
			}
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

/*
  if (session->type == SIM_SESSION_TYPE_SERVER_DOWN)	//if the info was sended by another server...
    {
			//FIXME: CHECK THIS ASAP!
      event->id = 0;
      event->snort_cid = 0;
      event->snort_sid = 0;
      event->rserver = TRUE;
    }
*/
	
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
	GList				*list;
	gboolean		for_this_server;

  g_return_if_fail (session);
  g_return_if_fail (SIM_IS_SESSION (session));
  g_return_if_fail (command);
  g_return_if_fail (SIM_IS_COMMAND (command));

	if (sim_session_is_master_server (session) ||
			sim_session_is_web (session))
	{
    SimServer *server = session->_priv->server;
		
		//Check if the message is for this server....
    if ((!command->data.reload_plugins.servername) || //FIXME: If the server name isn't specified, for backwards compatibility
																													//we will assume that this is the dst server. This should be removed in 
																													//a near future. All the commands from frameworkd should issue a server name.
        (!g_ascii_strcasecmp (sim_server_get_name (server), command->data.reload_plugins.servername)))
    	for_this_server = TRUE;
		else
    	for_this_server = FALSE;
					
		g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_cmd_reload_plugins: %s, %s", sim_server_get_name (server), command->data.reload_plugins.servername);
		
		if (for_this_server)	//execute the command in this server
	  {
		  sim_container_free_plugins (ossim.container);
		  sim_container_db_load_plugins (ossim.container, ossim.dbossim);
		}
		else	//resend the command buffer to the children servers whose name match.
		{
			list = sim_server_get_sessions (server);
		  while (list)	//list of the sessions connected to the server
			{
				SimSession *sess = (SimSession *) list->data;
				if (sim_session_is_children_server (sess) && 
						!g_ascii_strcasecmp (command->data.reload_plugins.servername, sim_session_get_hostname (sess)) )
				{
					sim_session_write_from_buffer (sess, command->buffer);
				}
				list = list->next;
			}
	  	g_list_free (list);
		}
	
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
sim_session_cmd_reload_sensors (SimSession  *session,
																SimCommand  *command)
{
  SimCommand  *cmd;
	GList				*list;
	gboolean		for_this_server;

  g_return_if_fail (session);
  g_return_if_fail (SIM_IS_SESSION (session));
  g_return_if_fail (command);
  g_return_if_fail (SIM_IS_COMMAND (command));

	if (sim_session_is_master_server (session) ||
			sim_session_is_web (session))
	{
    SimServer *server = session->_priv->server;
		
		//Check if the message is for this server....
    if ((!command->data.reload_sensors.servername) || //FIXME: If the server name isn't specified, for backwards compatibility
																													//we will assume that this is the dst server. This should be removed in 
																													//a near future. All the commands from frameworkd should issue a server name.
        (!g_ascii_strcasecmp (sim_server_get_name (server), command->data.reload_sensors.servername)))
    	for_this_server = TRUE;
		else
    	for_this_server = FALSE;
					
		g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_cmd_reload_sensors: %s, %s", sim_server_get_name (server), command->data.reload_sensors.servername);
		
		if (for_this_server)	//execute the command in this server
	  {
		  sim_container_free_sensors (ossim.container);
		  sim_container_db_load_sensors (ossim.container, ossim.dbossim);
		}
		else	//resend the command buffer to the children servers whose name match.
		{
			list = sim_server_get_sessions (server);
		  while (list)	//list of the sessions connected to the server
			{
				SimSession *sess = (SimSession *) list->data;
				if (sim_session_is_children_server (sess) && 
						!g_ascii_strcasecmp (command->data.reload_sensors.servername, sim_session_get_hostname (sess)) )
				{
					sim_session_write_from_buffer (sess, command->buffer);
				}
				list = list->next;
			}
	  	g_list_free (list);
		}
	
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
sim_session_cmd_reload_hosts (SimSession  *session,
												      SimCommand  *command)
{
  SimCommand  *cmd;
	GList				*list;
	gboolean		for_this_server;

  g_return_if_fail (session);
  g_return_if_fail (SIM_IS_SESSION (session));
  g_return_if_fail (command);
  g_return_if_fail (SIM_IS_COMMAND (command));

	if (sim_session_is_master_server (session) ||
			sim_session_is_web (session))
	{
    SimServer *server = session->_priv->server;
		
		//Check if the message is for this server....
    if ((!command->data.reload_hosts.servername) || //FIXME: If the server name isn't specified, for backwards compatibility
																													//we will assume that this is the dst server. This should be removed in 
																													//a near future. All the commands from frameworkd should issue a server name.
        (!g_ascii_strcasecmp (sim_server_get_name (server), command->data.reload_hosts.servername)))
    	for_this_server = TRUE;
		else
    	for_this_server = FALSE;
					
		g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_cmd_reload_hosts: %s, %s", sim_server_get_name (server), command->data.reload_hosts.servername);
		
		if (for_this_server)	//execute the command in this server
	  {
  		sim_container_free_hosts (ossim.container);
		  sim_container_db_load_hosts (ossim.container, ossim.dbossim);
		}
		else	//resend the command buffer to the children servers whose name match.
		{
			list = sim_server_get_sessions (server);
		  while (list)	//list of the sessions connected to the server
			{
				SimSession *sess = (SimSession *) list->data;
				if (sim_session_is_children_server (sess) && 
						!g_ascii_strcasecmp (command->data.reload_hosts.servername, sim_session_get_hostname (sess)) )
				{
					sim_session_write_from_buffer (sess, command->buffer);
				}
				list = list->next;
			}
	  	g_list_free (list);
		}
	
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
sim_session_cmd_reload_nets (SimSession  *session,
			     SimCommand  *command)
{
  SimCommand  *cmd;
	GList				*list;
	gboolean		for_this_server;

  g_return_if_fail (session);
  g_return_if_fail (SIM_IS_SESSION (session));
  g_return_if_fail (command);
  g_return_if_fail (SIM_IS_COMMAND (command));

	if (sim_session_is_master_server (session) ||
			sim_session_is_web (session))
	{
    SimServer *server = session->_priv->server;
		
		//Check if the message is for this server....
    if ((!command->data.reload_nets.servername) || //FIXME: If the server name isn't specified, for backwards compatibility
																													//we will assume that this is the dst server. This should be removed in 
																													//a near future. All the commands from frameworkd should issue a server name.
        (!g_ascii_strcasecmp (sim_server_get_name (server), command->data.reload_nets.servername)))
    	for_this_server = TRUE;
		else
    	for_this_server = FALSE;
					
		g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_cmd_reload_nets: %s, %s", sim_server_get_name (server), command->data.reload_nets.servername);
		
		if (for_this_server)	//execute the command in this server
	  {
  		sim_container_free_nets (ossim.container);
		  sim_container_db_load_nets (ossim.container, ossim.dbossim);
		}
		else	//resend the command buffer to the children servers whose name match.
		{
			list = sim_server_get_sessions (server);
		  while (list)	//list of the sessions connected to the server
			{
				SimSession *sess = (SimSession *) list->data;
				if (sim_session_is_children_server (sess) && 
						!g_ascii_strcasecmp (command->data.reload_nets.servername, sim_session_get_hostname (sess)) )
				{
					sim_session_write_from_buffer (sess, command->buffer);
				}
				list = list->next;
			}
	  	g_list_free (list);
		}
	
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
sim_session_cmd_reload_policies (SimSession  *session,
																 SimCommand  *command)
{
  SimCommand  *cmd;
	GList				*list;
	gboolean		for_this_server;

  g_return_if_fail (session != NULL);
  g_return_if_fail (SIM_IS_SESSION (session));
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));

	if (sim_session_is_master_server (session) ||
			sim_session_is_web (session))
	{
    SimServer *server = session->_priv->server;
		
		//Check if the message is for this server....
    if ((!command->data.reload_policies.servername) || //FIXME: If the server name isn't specified, for backwards compatibility
																													//we will assume that this is the dst server. This should be removed in 
																													//a near future. All the commands from frameworkd should issue a server name.
        (!g_ascii_strcasecmp (sim_server_get_name (server), command->data.reload_policies.servername)))
    	for_this_server = TRUE;
		else
    	for_this_server = FALSE;
					
		g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_cmd_reload_policies: %s, %s", sim_server_get_name (server), command->data.reload_policies.servername);
		
		if (for_this_server)	//execute the command in this server
	  {
  		sim_container_free_policies (ossim.container);
		  sim_container_db_load_policies (ossim.container, ossim.dbossim);
		}
		else	//resend the command buffer to the children servers whose name match.
		{
			list = sim_server_get_sessions (server);
		  while (list)	//list of the sessions connected to the server
			{
				SimSession *sess = (SimSession *) list->data;
				if (sim_session_is_children_server (sess) && 
						!g_ascii_strcasecmp (command->data.reload_policies.servername, sim_session_get_hostname (sess)) )
				{
					sim_session_write_from_buffer (sess, command->buffer);
				}
				list = list->next;
			}
	  	g_list_free (list);
		}
	
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
sim_session_cmd_reload_directives (SimSession  *session,
																   SimCommand  *command)
{
  SimCommand  *cmd;
	GList				*list;
	gboolean		for_this_server;

  g_return_if_fail (session != NULL);
  g_return_if_fail (SIM_IS_SESSION (session));
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));

	if (sim_session_is_master_server (session) ||
			sim_session_is_web (session))
	{
    SimServer *server = session->_priv->server;
		
		//Check if the message is for this server....
    if ((!command->data.reload_directives.servername) || //FIXME: If the server name isn't specified, for backwards compatibility
																													//we will assume that this is the dst server. This should be removed in 
																													//a near future. All the commands from frameworkd should issue a server name.
        (!g_ascii_strcasecmp (sim_server_get_name (server), command->data.reload_directives.servername)))
    	for_this_server = TRUE;
		else
    	for_this_server = FALSE;
					
		g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_cmd_reload_directives: %s, %s", sim_server_get_name (server), command->data.reload_directives.servername);
		
		if (for_this_server)	//execute the command in this server
	  {
		  sim_container_db_delete_plugin_sid_directive_ul (ossim.container, ossim.dbossim);
		  sim_container_db_delete_backlogs_ul (ossim.container, ossim.dbossim);

		  sim_container_free_backlogs (ossim.container);
		  sim_container_free_directives (ossim.container);
		  sim_container_load_directives_from_file (ossim.container,
																						   ossim.dbossim,
																						   SIM_XML_DIRECTIVE_FILE);
		}
		else	//resend the command buffer to the children servers whose name match.
		{
			list = sim_server_get_sessions (server);
		  while (list)	//list of the sessions connected to the server
			{
				SimSession *sess = (SimSession *) list->data;
				if (sim_session_is_children_server (sess) && 
						!g_ascii_strcasecmp (command->data.reload_directives.servername, sim_session_get_hostname (sess)) )
				{
					sim_session_write_from_buffer (sess, command->buffer);
				}
				list = list->next;
			}
	  	g_list_free (list);
		}
	
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
sim_session_cmd_reload_all (SimSession  *session,
			    SimCommand  *command)
{
  SimCommand  *cmd;
  SimConfig   *config;
	GList				*list;
	gboolean		for_this_server;

  g_return_if_fail (session != NULL);
  g_return_if_fail (SIM_IS_SESSION (session));
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));

	if (sim_session_is_master_server (session) ||
			sim_session_is_web (session))
	{
    SimServer *server = session->_priv->server;
		
		//Check if the message is for this server....
    if ((!command->data.reload_all.servername) || //FIXME: If the server name isn't specified, for backwards compatibility
																									//we will assume that this is the dst server. This should be removed in 
																									//a near future. All the commands from frameworkd should issue a server name.
        (!g_ascii_strcasecmp (sim_server_get_name (server), command->data.reload_all.servername)))
    	for_this_server = TRUE;
		else
    	for_this_server = FALSE;
					
		g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_cmd_reload_all: %s, %s", sim_server_get_name (server), command->data.reload_all.servername);
		
		if (for_this_server)	//execute the command in this server
	  {
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

		}
		else	//resend the command buffer to the children servers whose name match.
		{
			list = sim_server_get_sessions (server);
		  while (list)	//list of the sessions connected to the server
			{
				SimSession *sess = (SimSession *) list->data;
				if (sim_session_is_children_server (sess) && 
						!g_ascii_strcasecmp (command->data.reload_all.servername, sim_session_get_hostname (sess)) )
				{
					sim_session_write_from_buffer (sess, command->buffer);
				}
				list = list->next;
			}
	  	g_list_free (list);
		}
	
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
 *	This function stores the following:
 *	Userdata1: OS
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

  if (command->data.host_os_event.sensor)
		sensor = gnet_inetaddr_new_nonblock (command->data.host_os_event.sensor, 0);
	if (!sensor)
		return;				
	
	if (ia = gnet_inetaddr_new_nonblock (command->data.host_os_event.host, 0))
	{
	
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

		event->buffer = g_strdup (command->buffer); //we need this to resend data to other servers, or to send
                                                //events that matched with policy to frameworkd (future implementation)
																								//
		event->userdata1 = g_strdup (command->data.host_os_event.os); //needed for correlation
																										
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
 *	This function also stores the following:
 *	Userdata1: MAC
 *	Userdata2: Vendor
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
	gchar				**mac_and_vendor;
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
  
	if (command->data.host_mac_event.sensor)
  	sensor = gnet_inetaddr_new_nonblock (command->data.host_mac_event.sensor, 0);
	if (!sensor)
		return;

  if (ia = gnet_inetaddr_new_nonblock (command->data.host_mac_event.host, 0))
  {
    mac_and_vendor = sim_container_db_get_host_mac_ul (ossim.container, //get the mac wich should be the ia mac.
																											 ossim.dbossim,
																											 ia,
																											 sensor);
		mac = mac_and_vendor[0];
		vendor = mac_and_vendor[1];
		
    event = sim_event_new ();
    if (!mac) //if the ia-sensor pair doesn't obtains a mac in the database, inserts the new one.
    {
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
		event->data = g_strdup_printf ("%s|%s --> %s|%s", (mac) ? mac : command->data.host_mac_event.mac,
																											(vendor) ? vendor : "",
																											command->data.host_mac_event.mac,
																											(command->data.host_mac_event.vendor) ? command->data.host_mac_event.vendor : "");
	
  	//this is used to pass the event data to sim-organizer, so it can insert it into database
    event->data_storage = g_new(gchar*, 3);
   	event->data_storage[0] = g_strdup((command->data.host_mac_event.mac) ? command->data.host_mac_event.mac : "");
	  event->data_storage[1] = g_strdup((command->data.host_mac_event.vendor) ? command->data.host_mac_event.vendor : "");
  	event->data_storage[2] = NULL; //this is needed for g_strfreev(). Don't remove. 

	  event->buffer = g_strdup (command->buffer); //we need this to resend data to other servers, or to send
		                                            //events that matched with policy to frameworkd (future implementation)
		
		event->userdata1 = g_strdup (command->data.host_mac_event.mac);	//needed for correlation
		if (command->data.host_mac_event.vendor)
			event->userdata2 = g_strdup (command->data.host_mac_event.vendor);

    sim_container_push_event (ossim.container, event);
		sim_container_set_sensor_event_number (ossim.container, SIM_EVENT_HOST_MAC_EVENT, sensor);
		
		if (mac)
			g_free (mac);
		if (vendor)
			g_free (vendor);	
    gnet_inetaddr_unref (sensor);
  }
  else
    g_message("Error: Data sent from agent; host MAC event wrong IP %s",command->data.host_mac_event.host);

	g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "command->data.host_mac_event.date: %s",command->data.host_mac_event.date);
	g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_cmd_host_mac_event: TYPE: %d",event->plugin_sid);
				
}

/*
 * PADS plugin (or redirect to MAC plugin)
 * This function also stores the following:
 * userdata1: application
 * userdata2: service
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
  gchar				*service = NULL;
  gchar				**application_and_service = NULL;
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
			 
    // Check if we've got a mac to call host_mac_event and insert it.
    if (!g_ascii_strcasecmp (command->data.host_service_event.service, "ARP"))
    {			
      //as the pads plugin uses the same variables to store mac changes and services changes, we must normalize it.
      cmd = sim_command_new_from_type(SIM_COMMAND_TYPE_HOST_MAC_EVENT);
      cmd->data.host_mac_event.date = g_strdup(command->data.host_service_event.date);
      cmd->data.host_mac_event.host = g_strdup(command->data.host_service_event.host);
      cmd->data.host_mac_event.mac = g_strdup(command->data.host_service_event.application);
      cmd->data.host_mac_event.sensor = g_strdup(command->data.host_service_event.sensor);
      cmd->data.host_mac_event.interface = g_strdup(command->data.host_service_event.interface);
      cmd->data.host_mac_event.vendor = g_strdup_printf(" "); //FIXME: this will be usefull when pads get patched to know the vendor
      cmd->data.host_mac_event.plugin_id = sim_container_get_plugin_id_by_name(ossim.container, "arpwatch");
      cmd->data.host_mac_event.plugin_sid = EVENT_UNKNOWN;
  
      sim_session_cmd_host_mac_event (session, cmd);
    }
    else //ok, this is not a MAC change event, its a service change event
    {
      event = sim_event_new ();
    
			if (command->data.host_service_event.sensor)
				event->sensor = g_strdup (command->data.host_service_event.sensor);
			if (!(sensor = gnet_inetaddr_new_nonblock (event->sensor, 0))) //sanitize
				return;
		
      port = command->data.host_service_event.port;
      protocol = command->data.host_service_event.protocol;
      application_and_service = sim_container_db_get_host_service_ul (ossim.container, ossim.dbossim, ia, port, protocol, sensor);
			
			if (!application_and_service)	//FIXME: check this with a new event. will it work?.
				return;
			
			application = application_and_service[0];
			service = application_and_service[1];			

      if (!application) //first time this service (apache, IIS...) is saw
      {
				event->plugin_sid = EVENT_NEW;
      }
      else
      if (!g_ascii_strcasecmp (application, command->data.host_service_event.application)) //service is the same
      {
				if (!g_ascii_strcasecmp (service, command->data.host_service_event.service))
		       event->plugin_sid = EVENT_SAME;
				else
				   event->plugin_sid = EVENT_CHANGE;				
      }
      else //The service is different
				event->plugin_sid = EVENT_CHANGE;

/*	    g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_cmd_host_service_event app1: %s", application);
	    g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_cmd_host_service_event app2: %s", command->data.host_service_event.application);
	    g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_cmd_host_service_event service1: %s", service);
	    g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_cmd_host_service_event service2: %s", command->data.host_service_event.service);
	    g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_cmd_host_service_event event: %d", event->plugin_sid);
  */
      event->type = SIM_EVENT_TYPE_DETECTOR;
      event->alarm = FALSE;
      event->plugin_id = command->data.host_service_event.plugin_id;
			event->protocol=SIM_PROTOCOL_TYPE_HOST_SERVICE_EVENT;
  
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
    	event->data_storage[4] = NULL;  //this is needed for g_strfreev(). Don't remove.

			event->buffer = g_strdup (command->buffer); //we need this to resend data to other servers, or to send
	                                                //events that matched with policy to frameworkd (future implementation)
			
			event->userdata1 = g_strdup (command->data.host_service_event.application);	//may be needed in correlation
			event->userdata2 = g_strdup (command->data.host_service_event.service);
			
     	sim_container_push_event (ossim.container, event);
			sim_container_set_sensor_event_number (ossim.container, SIM_EVENT_HOST_SERVICE_EVENT, sensor);
			
			if (application)	
	      g_free (application);
			if (service)
				g_free (service);
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
  GInetAddr	*ia_temp=NULL;
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
  
    if (command->data.host_ids_event.sensor)
			event->sensor = g_strdup (command->data.host_ids_event.sensor);
	  if (!(ia_temp = gnet_inetaddr_new_nonblock (event->sensor, 0))) //sanitize
		  return;
		else
			gnet_inetaddr_unref (ia_temp);
		
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
      g_message("Error: Data sent from agent; host Service event wrong IP %s",command->data.host_ids_event.host);
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
              
//		g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_cmd_host_ids_event filename: %s", command->data.host_ids_event.filename);
							
		if (command->data.host_ids_event.filename)
			event->filename = g_strdup (command->data.host_ids_event.filename);
		if (command->data.host_ids_event.username)
			event->username = g_strdup (command->data.host_ids_event.username);
		if (command->data.host_ids_event.password)
			event->password = g_strdup (command->data.host_ids_event.password);
		if (command->data.host_ids_event.userdata1)
			event->userdata1 = g_strdup (command->data.host_ids_event.userdata1);
		if (command->data.host_ids_event.userdata2)
			event->userdata2 = g_strdup (command->data.host_ids_event.userdata2);
		if (command->data.host_ids_event.userdata3)
			event->userdata3 = g_strdup (command->data.host_ids_event.userdata3);
		if (command->data.host_ids_event.userdata4)
			event->userdata4 = g_strdup (command->data.host_ids_event.userdata4);
		if (command->data.host_ids_event.userdata5)
			event->userdata5 = g_strdup (command->data.host_ids_event.userdata5);
		if (command->data.host_ids_event.userdata6)
			event->userdata6 = g_strdup (command->data.host_ids_event.userdata6);
		if (command->data.host_ids_event.userdata7)
			event->userdata7 = g_strdup (command->data.host_ids_event.userdata7);
		if (command->data.host_ids_event.userdata8)
			event->userdata8 = g_strdup (command->data.host_ids_event.userdata8);
		if (command->data.host_ids_event.userdata9)
			event->userdata9 = g_strdup (command->data.host_ids_event.userdata9);

		event->buffer = g_strdup (command->buffer);	//we need this to resend data to other servers, or to send
																								//events that matched with policy to frameworkd (future implementation)
																							 
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
  SimCommand  *cmd = NULL;
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
		
		//FIXME: This not a OSSIM fixme, IMHO this is a GLib fixme. If strlen(buffer) > n, gscanner will crash
		//This can be easily reproduced commenting the "if" below, and doing a telnet to the server port, and sending one event. After that, do
		//a CTRL-C, and a quit. Next event will crash the server, and gdb will show:
		//(gdb) bt
		//#0  0xb7d8765e in g_scanner_scope_add_symbol () from /usr/lib/libglib-2.0.so.0
		//#1  0xb7d88a52 in g_scanner_get_next_token () from /usr/lib/libglib-2.0.so.0
		//#2  0x0807e840 in sim_command_scan (command=0x8397980,
		//Also, scanner->buffer is not 0 in the next iteration. If we set it to 0, it still crashes.
		//I'll be very glad is someone has some time to check what's happening) :)
		if (strlen(buffer) != n)
		{
		  g_message ("Received error. Inconsistent data entry, closing socket: %d: %s", error, g_strerror(error));
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

		//this two variables are used in SIM_COMMAND_TYPE_EVENT
		SimServer	*server = session->_priv->server;
		SimConfig	*config = sim_server_get_config (server);
	
		//this messages can arrive from other servers (up in the architecture -a master server-, down in the
		//architecture -a children server-, or at the same level -HA server-), from some sensor (an agent) or from the frameworkd.
    switch (cmd->type)
		{
			case SIM_COMMAND_TYPE_CONNECT:															//from children server / frameworkd / sensor
						sim_session_cmd_connect (session, cmd);
						break;
			//a partir de aqui revisar TODOS los de fraeworkd/masterserver
			case SIM_COMMAND_TYPE_SERVER_GET_SENSORS:										//from frameworkd / master server
						sim_session_cmd_server_get_sensors (session, cmd);
						break;
			case SIM_COMMAND_TYPE_SERVER_GET_SENSOR_PLUGINS:						//from frameworkd / master server
						sim_session_cmd_server_get_sensor_plugins (session, cmd);
						break;
			case SIM_COMMAND_TYPE_SERVER_SET_DATA_ROLE:									//from frameworkd / master server
						sim_session_cmd_server_set_data_role (session, cmd);
						break;
			case SIM_COMMAND_TYPE_SENSOR_PLUGIN_START:									//from frameworkd / master server
						sim_session_cmd_sensor_plugin_start (session, cmd);
						break;
			case SIM_COMMAND_TYPE_SENSOR_PLUGIN_STOP:										//from frameworkd / master server
						sim_session_cmd_sensor_plugin_stop (session, cmd);
						break;
			case SIM_COMMAND_TYPE_SENSOR_PLUGIN_ENABLE:								//from frameworkd / master server
						sim_session_cmd_sensor_plugin_enable (session, cmd);
						break;
			case SIM_COMMAND_TYPE_SENSOR_PLUGIN_DISABLE:								//from frameworkd / master server
						sim_session_cmd_sensor_plugin_disable (session, cmd);
						break;
			case SIM_COMMAND_TYPE_RELOAD_PLUGINS:
						sim_session_cmd_reload_plugins (session, cmd);				// from frameworkd / master server
						break;
			case SIM_COMMAND_TYPE_RELOAD_SENSORS:												// from frameworkd / master server
						sim_session_cmd_reload_sensors (session, cmd);
						break;
			case SIM_COMMAND_TYPE_RELOAD_HOSTS:													// from frameworkd / master server
						sim_session_cmd_reload_hosts (session, cmd);
						break;
			case SIM_COMMAND_TYPE_RELOAD_NETS:													// from frameworkd / master server
						sim_session_cmd_reload_nets (session, cmd);
						break;
			case SIM_COMMAND_TYPE_RELOAD_POLICIES:											// from frameworkd / master server
						sim_session_cmd_reload_policies (session, cmd);
						break;
			case SIM_COMMAND_TYPE_RELOAD_DIRECTIVES:										// from frameworkd / master server
						sim_session_cmd_reload_directives (session, cmd);
						break;
			case SIM_COMMAND_TYPE_RELOAD_ALL:														// from frameworkd / master server
						sim_session_cmd_reload_all (session, cmd);
						break;
			case SIM_COMMAND_TYPE_SESSION_APPEND_PLUGIN:								//from sensor
						sim_session_cmd_session_append_plugin (session, cmd);
						break;
			case SIM_COMMAND_TYPE_SESSION_REMOVE_PLUGIN:								//from sensor
						sim_session_cmd_session_remove_plugin (session, cmd);
						break;
			case SIM_COMMAND_TYPE_PLUGIN_STATE_STARTED:									//from sensor (just information for the server)
						sim_session_cmd_plugin_state_started (session, cmd);
						break;
			case SIM_COMMAND_TYPE_PLUGIN_STATE_UNKNOWN:									//from sensor
						sim_session_cmd_plugin_state_unknown (session, cmd);
						break;
			case SIM_COMMAND_TYPE_PLUGIN_STATE_STOPPED:									//from sensor
						sim_session_cmd_plugin_state_stopped (session, cmd);
						break;
			case SIM_COMMAND_TYPE_PLUGIN_ENABLED:												//from sensor
						sim_session_cmd_plugin_enabled (session, cmd);
						break;
			case SIM_COMMAND_TYPE_PLUGIN_DISABLED:											//from sensor
						sim_session_cmd_plugin_disabled (session, cmd);
						break;
			case SIM_COMMAND_TYPE_EVENT:																//from sensor / server children
						//if we're just a "redirecter", only send the buffer to other servers. If the server
						//is a redirecter, and also some other thing, this will be done later. This is just to try to accelerate
						//up to the maximum the functionality.
						if ((!config->server.role->correlate) &&
								(!config->server.role->cross_correlate) &&
								(!config->server.role->store) &&
								(!config->server.role->qualify) &&
								(!config->server.role->resend_alarm))
						{
	            g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_read: DENTRO");
							sim_session_resend_buffer (buffer);
						}
						else		
							sim_session_cmd_event (session, cmd);
						break;
			case SIM_COMMAND_TYPE_HOST_OS_EVENT:								        // from sensor / children server
						sim_session_cmd_host_os_event (session, cmd);
						break;
			case SIM_COMMAND_TYPE_HOST_MAC_EVENT:												// from sensor / children server
						sim_session_cmd_host_mac_event (session, cmd);
						break;
			case SIM_COMMAND_TYPE_HOST_SERVICE_EVENT:										// from sensor / children server
						sim_session_cmd_host_service_event (session, cmd);
						break;
			case SIM_COMMAND_TYPE_HOST_IDS_EVENT:												// from sensor / children server
						sim_session_cmd_host_ids_event (session, cmd); 
						break;				
			case SIM_COMMAND_TYPE_OK:																		//from *
						sim_session_cmd_ok (session, cmd);
						break;
			case SIM_COMMAND_TYPE_ERROR:																//from *
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
		cmd = NULL;

		n=0;
  	memset(buffer, 0, BUFFER_SIZE);
		

	}
	g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_session_read: exiting function in session: %x", session);
			
  return TRUE;
}

/*
 * Send the command specified (usually it will be a SIM_COMMAND_TYPE_EVENT or something like that) 
 * to all the master servers (servers UP in the architecture).
 */
void 
sim_session_resend_command (SimSession *session,	//FIXME: is this function deprecated?
														SimCommand	*command)
{
  g_return_if_fail (session != NULL);
  g_return_if_fail (SIM_IS_SESSION (session));
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));

	SimServer *server = session->_priv->server;

	GList *list = sim_server_get_sessions (server);
	while (list)
	{
		SimSession *session = (SimSession *) list->data;
		if (sim_session_is_master_server (session))
			sim_session_write (session, command);	//FIXME: use another thread ,this will block a lot.

		list = list->next;
	}


}

/*
 * This will resend the buffer specified to master servers.
 */
void 
sim_session_resend_buffer (gchar	*buffer)
{
  g_return_if_fail (buffer != NULL);

	GList *list = sim_server_get_sessions (ossim.server);
	while (list)
	{
		SimSession *session = (SimSession *) list->data;
		if (sim_session_is_master_server (session))
			sim_session_write_from_buffer (session, buffer);

		list = list->next;
	}
	
	g_list_free (list);

}
/*
 * This function may be used to send data to sensors, to other servers, or to the frameworkd (if needed)
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
 * write a specific buffer into a session channel. returns the bytes written.
 * FIXME: Use another thread, this will block.
 */
guint
sim_session_write_from_buffer (SimSession	*session,
																gchar			*buffer)
{
	GIOError	error;
	guint n;
	
  g_return_val_if_fail (session != NULL, 0);
  g_return_val_if_fail (SIM_IS_SESSION (session), 0);
  g_return_val_if_fail (session->_priv->io != NULL, 0);
			
  error = gnet_io_channel_writen (session->_priv->io, buffer, strlen (buffer), &n);
		
	if	(error != G_IO_ERROR_NONE)
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
 * Returns the server associated with this session (this server);
 */
SimServer*
sim_session_get_server (SimSession *session)
{
  g_return_val_if_fail (session, NULL);
  g_return_val_if_fail (SIM_IS_SESSION (session), NULL);

  return session->_priv->server;
}


/*
 *Is the session from a sensor ?
 */
gboolean
sim_session_is_sensor (SimSession *session)
{
  g_return_val_if_fail (session, FALSE);
  g_return_val_if_fail (SIM_IS_SESSION (session), FALSE);

  if (session->type == SIM_SESSION_TYPE_SENSOR) 
		return TRUE;

  return FALSE;
}

/*
 * Is the session from a master server? (a server which is "up" in the architecture)
 */
gboolean
sim_session_is_master_server (SimSession *session)
{
  g_return_val_if_fail (session, FALSE);
  g_return_val_if_fail (SIM_IS_SESSION (session), FALSE);

  if (session->type == SIM_SESSION_TYPE_SERVER_UP)
    return TRUE;

  return FALSE;
}

/*
 * Is the session from a children server? (a server which is "down" in the architecture)
 */
gboolean
sim_session_is_children_server (SimSession *session)
{
  g_return_val_if_fail (session, FALSE);
  g_return_val_if_fail (SIM_IS_SESSION (session), FALSE);

  if (session->type == SIM_SESSION_TYPE_SERVER_DOWN)
    return TRUE;

  return FALSE;
}


/*
Is the session from the web ? FIXME: soon this will be from the frameworkd
*/
gboolean
sim_session_is_web (SimSession *session)
{
  g_return_val_if_fail (session, FALSE);
  g_return_val_if_fail (SIM_IS_SESSION (session), FALSE);

  if (session->type == SIM_SESSION_TYPE_WEB)
    return TRUE;

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
sim_session_must_close (SimSession *session)
{
  g_return_if_fail (session);
  g_return_if_fail (SIM_IS_SESSION (session));
  
  return session->_priv->close;
}
// vim: set tabstop=2 sts=2 noexpandtab:
