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

#include "os-sim.h"
#include "sim-session.h"
#include "sim-server.h"
#include "sim-sensor.h"
#include <signal.h>
#include <config.h>

extern SimMain    ossim;

enum 
{
  DESTROY,
  LAST_SIGNAL
};

struct _SimServerPrivate {
  SimConfig       *config;

  GTcpSocket      *socket;

  gint             port;

  GList           *sessions;

	gchar						*ip;
	gchar						*name;
};

typedef struct {
  SimConfig     *config;
  SimServer     *server;
  GTcpSocket    *socket;
} SimSessionData;

static gpointer sim_server_session (gpointer data);

static gpointer parent_class = NULL;
static gint sim_server_signals[LAST_SIGNAL] = { 0 };

/* GType Functions */

static void 
sim_server_impl_dispose (GObject  *gobject)
{
  G_OBJECT_CLASS (parent_class)->dispose (gobject);
}

static void 
sim_server_impl_finalize (GObject  *gobject)
{
  SimServer *server = SIM_SERVER (gobject);

  g_free (server->_priv);

  G_OBJECT_CLASS (parent_class)->finalize (gobject);

}

static void
sim_server_class_init (SimServerClass * class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  parent_class = g_type_class_peek_parent (class);

  object_class->dispose = sim_server_impl_dispose;
  object_class->finalize = sim_server_impl_finalize;
}

static void
sim_server_instance_init (SimServer * server)
{
  server->_priv = g_new0 (SimServerPrivate, 1);

  server->_priv->config = NULL;
  server->_priv->socket = NULL;

  server->_priv->port = 40001;

  server->_priv->sessions = NULL;
  
  server->_priv->ip = NULL;
  server->_priv->name = NULL;
}

/* Public Methods */

GType
sim_server_get_type (void)
{
  static GType object_type = 0;
 
  if (!object_type)
  {
    static const GTypeInfo type_info = {
              sizeof (SimServerClass),
              NULL,
              NULL,
              (GClassInitFunc) sim_server_class_init,
              NULL,
              NULL,                       /* class data */
              sizeof (SimServer),
              0,                          /* number of pre-allocs */
              (GInstanceInitFunc) sim_server_instance_init,
              NULL                        /* value table */
    };
    
    g_type_init ();
                                                                                                                             
    object_type = g_type_register_static (G_TYPE_OBJECT, "SimServer", &type_info, 0);
  }
                                                                                                                             
  return object_type;
}

/*
 *
 *
 *
 *
 */
SimServer*
sim_server_new (SimConfig  *config)
{
  SimServer *server;

  g_return_val_if_fail (config, NULL);
  g_return_val_if_fail (SIM_IS_CONFIG (config), NULL);

  server = SIM_SERVER (g_object_new (SIM_TYPE_SERVER, NULL));
  server->_priv->config = config;

	if (config->server.name)
		server->_priv->name = g_strdup (config->server.name);
	
	if (simCmdArgs.port > 0)
    server->_priv->port = simCmdArgs.port;
	else
	if (config->server.port > 0) //anti-moron sanity check
    server->_priv->port = config->server.port;
	
  if (simCmdArgs.ip)
		server->_priv->ip = g_strdup (simCmdArgs.ip);
	else
	if (config->server.ip)
		server->_priv->ip = g_strdup (config->server.ip);


  return server;
}


/*
 *
 *
 *
 */
void
sim_server_run (SimServer *server)
{
  SimSession		*session;
  SimSensor			*sensor;
  SimSessionData	*session_data;
  GTcpSocket		*socket;
  GThread				*thread;
	GError 				*error;
	GInetAddr			*serverip;
  
  g_return_if_fail (server);
  g_return_if_fail (SIM_IS_SERVER (server));

  g_message ("Waiting for connections...");

	if (!server->_priv->ip)
		server->_priv->ip = g_strdup("0.0.0.0");
	
	serverip = gnet_inetaddr_new_nonblock(server->_priv->ip, 0);
	if (!serverip)
	{
	  g_message("Error creating server address. Please check that the ip %s has the right format",server->_priv->ip);
	  exit (EXIT_FAILURE);	
	}
  
	server->_priv->socket = gnet_tcp_socket_server_new_full (serverip ,server->_priv->port); //bind in the interface defined
	
  if (!server->_priv->socket)
  {
    printf("Error in bind; may be another app is running in port %d?",server->_priv->port); //the log file may be in use.
    g_message("Error in bind; may be another app is running in port %d?",server->_priv->port);
    exit (EXIT_FAILURE);   
  }

  while ((socket = gnet_tcp_socket_server_accept (server->_priv->socket)) != NULL)
  {
    GInetAddr *ia = gnet_tcp_socket_get_remote_inetaddr (socket);
    sensor = sim_container_get_sensor_by_ia (ossim.container, ia);
    if (sensor)
		{
		  session = sim_server_get_session_by_sensor (server, sensor);
		//FIXME: little memory leak to avoid some crashes.. :( fix ASAP!
		      if (session)
		        sim_session_close (session);		
		}
    gnet_inetaddr_unref (ia);

    session_data = g_new0 (SimSessionData, 1);
    session_data->config = server->_priv->config;
    session_data->server = server;
    session_data->socket = socket;
    
		/* Session Thread */		
    thread = g_thread_create(sim_server_session, session_data, FALSE, &error);
		
	  if (thread == NULL)
			g_message ("thread error %d: %s", error->code, error->message);
		else
			continue;
										 
  }

}

/*
 *
 *
 *
 *
 *
 */
static gpointer
sim_server_session (gpointer data)
{
  SimSessionData  *session_data = (SimSessionData *) data;
  SimConfig       *config = session_data->config;
  SimServer       *server = session_data->server;
  GTcpSocket      *socket = session_data->socket;
  SimSession      *session;

  g_return_val_if_fail (config, NULL);
  g_return_val_if_fail (SIM_IS_CONFIG (config), NULL);
  g_return_val_if_fail (server, NULL);
  g_return_val_if_fail (SIM_IS_SERVER (server), NULL);
  g_return_val_if_fail (socket, NULL);

  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_server_session: Trying to do a sim_session_new: pid %d. Session: %x", getpid(), session);
 
  session = sim_session_new (G_OBJECT (server), config, socket);
  
  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_server_session: New Session: pid %d; session address: %x", getpid(), session);
  g_message ("New session");
 
	if (!sim_session_is_close(session))
	{
	  sim_server_append_session (server, session);

	  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_server_session: Session Append: pid %d; session address: %x", getpid(), session);
		g_message ("Session Append");

	  sim_session_read (session);


	  if (sim_server_remove_session (server, session))
		{
			g_object_unref (session);
		
		  g_message ("Session Removed");
			g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_server_session: After remove session: pid %d. session: %x", getpid(),session);
		}
		else
			g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_server_session: Error removing session: %x", session);
	}
	else
	{
    g_object_unref (session);
    g_message ("Session Removed: error");
    g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_server_session: Error: after remove session: pid %d. session: %x", getpid(),session);							 
	}
  
	g_free (session_data);
     
  return NULL;
}

/*
 *
 *
 *
 *
 *
 */
void
sim_server_append_session (SimServer     *server,
			   SimSession    *session)
{
  g_return_if_fail (server);
  g_return_if_fail (SIM_IS_SERVER (server));
  g_return_if_fail (session);
  g_return_if_fail (SIM_IS_SESSION (session));

  server->_priv->sessions = g_list_append (server->_priv->sessions, session);
}

/*
 *
 *
 *
 *
 *
 */
gint
sim_server_remove_session (SimServer     *server,
												   SimSession    *session)
{
  g_return_val_if_fail (server, 0);
  g_return_val_if_fail (SIM_IS_SERVER (server), 0);
  g_return_val_if_fail (session, 0);
  g_return_val_if_fail (SIM_IS_SESSION (session), 0);
	
	guint length = g_list_length (server->_priv->sessions);
  server->_priv->sessions = g_list_remove (server->_priv->sessions, session);

	if (length == g_list_length (server->_priv->sessions)) //if the lenght is the same, we didn't removed anything-> error
		return 0;
	else
		return 1;
}

/*
 *
 *
 *
 *
 *
 */
GList*
sim_server_get_sessions (SimServer     *server)
{
  g_return_val_if_fail (server, NULL);
  g_return_val_if_fail (SIM_IS_SERVER (server), NULL);

  return g_list_copy (server->_priv->sessions);

}

/*
 *
 * This is called just from sim_organizer_run
 *
 *
 */
void
sim_server_push_session_command (SimServer       *server,
																 SimSessionType   session_type,
																 SimCommand      *command)
{
  GList *list;

  g_return_if_fail (server);
  g_return_if_fail (SIM_IS_SERVER (server));
  g_return_if_fail (command);
  g_return_if_fail (SIM_IS_COMMAND (command));

  list = server->_priv->sessions;
  while (list)
  {
    SimSession *session = (SimSession *) list->data;

    if ((session != NULL) && SIM_IS_SESSION(session))
      if (session_type == SIM_SESSION_TYPE_ALL || session_type == session->type)
				sim_session_write (session, command); 

    list = list->next;
  }
}

/*
 *
 *	Now, depending on the rule, we'll generate a specific command that will be sent
 *	with the data from the rule to the agent who issued the event that
 *	made match with the alarm.
 *
 *
 */
void
sim_server_push_session_plugin_command (SimServer       *server,
																				SimSessionType   session_type,
																				gint             plugin_id,
																				SimRule					*rule)
{
  GList *list;
					
  g_return_if_fail (server);
  g_return_if_fail (SIM_IS_SERVER (server));
  g_return_if_fail (rule);
  g_return_if_fail (SIM_IS_RULE (rule));
		
  list = server->_priv->sessions;
  while (list)
  {
    SimSession *session = (SimSession *) list->data;
      
		g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_server_push_session_plugin_command");
    if ((session != NULL) && SIM_IS_SESSION(session))
		{
      if (session_type == SIM_SESSION_TYPE_ALL || session_type == session->type)
      {
        if (sim_session_has_plugin_id (session, plugin_id))
				{
					monitor_requests	*data;
					GError						*error;	
					GThread *thread;

					SimCommand *cmd = sim_command_new_from_rule (rule); //this will be freed in sim_server_thread_monitor_requests()
					data->session = session;
					data->command = cmd;

				  thread = g_thread_create (sim_server_thread_monitor_requests, data, FALSE, &error);
			    if (thread == NULL)
			      g_message ("thread error %d: %s", error->code, error->message);										
				}
      }
		}
		else
		{			
		 //avoiding race condition; this happens when the agent disconnect from the server and there aren't any established session. FIXME: this will broke the correlation procedure in this event, I've to check this ASAP.
			g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_server_push_session_plugin_command: Error, session %x is invalid!!", session);
			break;
		}
      
    list = list->next;
  }
}

static gpointer 
sim_server_thread_monitor_requests(gpointer data)
{
	monitor_requests  *request = (monitor_requests *) data;

	sim_session_write (request->session, request->command);	

	//I don't like to reserve/free memory in different levels of execution, but it's the only way 
	//without change a bit more the code
	g_object_unref (request->command);
}


/*
 *
 *
 *
 *
 *
 */
void
sim_server_reload (SimServer       *server)
{
  GList *list;

  g_return_if_fail (server);
  g_return_if_fail (SIM_IS_SERVER (server));

  list = server->_priv->sessions;
  while (list)
    {
      SimSession *session = (SimSession *) list->data;

      if ((session != NULL) && SIM_IS_SESSION(session))
        sim_session_reload (session);

      list = list->next;
    }
}

/*
 *
 * We want to know wich is the session wich belongs to a specific sensor
 *
 *
 */
SimSession*
sim_server_get_session_by_sensor (SimServer   *server,
				  SimSensor   *sensor)
{
  GList *list;

  g_return_val_if_fail (server, NULL);
  g_return_val_if_fail (SIM_IS_SERVER (server), NULL);
  g_return_val_if_fail (sensor, NULL);
  g_return_val_if_fail (SIM_IS_SENSOR (sensor), NULL);

  list = server->_priv->sessions;
  while (list)
  {
    SimSession *session = (SimSession *) list->data;
    if ((session != NULL) && SIM_IS_SESSION(session))
      if (sim_session_get_sensor (session) == sensor)
	return session;

    list = list->next;
  }

  return NULL; //no sessions established
}


/*
 *
 *
 *
 *
 *
 */
SimSession*
sim_server_get_session_by_ia (SimServer       *server,
			      SimSessionType   session_type,
			      GInetAddr       *ia)
{
  GList *list;

  g_return_val_if_fail (server, NULL);
  g_return_val_if_fail (SIM_IS_SERVER (server), NULL);

  list = server->_priv->sessions;
  while (list)
  {
    SimSession *session = (SimSession *) list->data;
    if ((session != NULL) && SIM_IS_SESSION(session))
      if (session_type == SIM_SESSION_TYPE_ALL || session_type == session->type)
      {
        GInetAddr *tmp = sim_session_get_ia (session);
        if (gnet_inetaddr_noport_equal (tmp, ia))
          return session;
      }

    list = list->next;
  }
  return NULL;
}

/*
 *
 * Debug function: print the server sessions 
 *
 *
 */

void sim_server_debug_print_sessions (SimServer *server)
{
  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_server_debug_print_sessions:");
	GList *list;
	int a=0;
	
	list = server->_priv->sessions;
	while (list)
  {
    SimSession *session = (SimSession *) list->data;
    g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "session %d: %x", a, session);
		a++;		
		list = list->next;
	}							

}





// vim: set tabstop=2:
