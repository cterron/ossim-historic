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
#include <gnet.h>

#include "os-sim.h"
#include "sim-session.h"
#include "sim-server.h"
#include "sim-sensor.h"

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

  if (config->server.port > 0)
    server->_priv->port = config->server.port;

  return server;
}

/*
 *
 *
 *
 *
 *
 */
void
sim_server_run (SimServer *server)
{
  SimSession		*session;
  SimSensor		*sensor;
  SimSessionData	*session_data;
  GTcpSocket		*socket;
  GThread		*thread;
  
  g_return_if_fail (server);
  g_return_if_fail (SIM_IS_SERVER (server));

  g_message ("Waiting for connections...");
  server->_priv->socket = gnet_tcp_socket_server_new_with_port (server->_priv->port);
  while ((socket = gnet_tcp_socket_server_accept (server->_priv->socket)) != NULL)
    {
      GInetAddr *ia = gnet_tcp_socket_get_remote_inetaddr (socket);
      sensor = sim_container_get_sensor_by_ia (ossim.container, ia);
      if (sensor)
	{
	  session = sim_server_get_session_by_sensor (server, sensor);
	  if (session)
	    sim_session_close (session);
	}
      gnet_inetaddr_unref (ia);

      session_data = g_new0 (SimSessionData, 1);
      session_data->config = server->_priv->config;
      session_data->server = server;
      session_data->socket = socket;

      /* Session Thread */
      thread = g_thread_create(sim_server_session, session_data, TRUE, NULL);
      if (thread != NULL) continue;
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

  g_message ("New session");

  session = sim_session_new (G_OBJECT (server), config, socket);
  sim_server_append_session (server, session);

  sim_session_read (session);

  g_message ("Remove Session");
  sim_server_remove_session (server, session);

  g_object_unref (session);
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
void
sim_server_remove_session (SimServer     *server,
			   SimSession    *session)
{
  g_return_if_fail (server);
  g_return_if_fail (SIM_IS_SERVER (server));
  g_return_if_fail (session);
  g_return_if_fail (SIM_IS_SESSION (session));

  server->_priv->sessions = g_list_remove (server->_priv->sessions, session);
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
 *
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

      if (session_type == SIM_SESSION_TYPE_ALL || 
	  session_type == session->type)
	{
	    sim_session_write (session, command); 
	}

      list = list->next;
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
sim_server_push_session_plugin_command (SimServer       *server,
					SimSessionType   session_type,
					gint             plugin_id,
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
      if(session == NULL || !SIM_IS_SESSION(session)) continue;

      if (session_type == SIM_SESSION_TYPE_ALL ||
	  session_type == session->type)
	{
	  if (sim_session_has_plugin_id (session, plugin_id))
	    sim_session_write (session, command); 
	}

      list = list->next;
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
sim_server_reload (SimServer       *server)
{
  GList *list;

  g_return_if_fail (server);
  g_return_if_fail (SIM_IS_SERVER (server));

  list = server->_priv->sessions;
  while (list)
    {
      SimSession *session = (SimSession *) list->data;

      sim_session_reload (session);

      list = list->next;
    }
}

/*
 *
 *
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

      if (sim_session_get_sensor (session) == sensor)
	return session;

      list = list->next;
    }

  return NULL;
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

      if (session_type == SIM_SESSION_TYPE_ALL ||
	  session_type == session->type)
	{
	  GInetAddr *tmp = sim_session_get_ia (session);
	  if (gnet_inetaddr_noport_equal (tmp, ia))
	    return session;
	}

      list = list->next;
    }

  return NULL;
}
