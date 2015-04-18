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

#include "sim-util.h"
#include "sim-xml-directive.h"
#include "sim-config.h"
#include "sim-database.h"
#include "sim-signature.h"
#include "sim-policy.h"
#include "sim-host.h"
#include "sim-net.h"
#include "sim-host-level.h"
#include "sim-net-level.h"
#include "sim-message.h"
#include "sim-command.h"
#include "sim-directive.h"
#include "sim-alert.h"
#include "sim-session.h"
#include "sim-scheduler.h"
#include "sim-syslog.h"
#include "sim-organizer.h"
#include "sim-server.h"

enum 
{
  DESTROY,
  LAST_SIGNAL
};

struct _SimServerPrivate {
  SimContainer    *container;
  SimDatabase     *database;

  GTcpSocket      *socket;
};

typedef struct {
  SimServer    *server;
  GTcpSocket   *socket;
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

  server->_priv->container = NULL;
  server->_priv->database = NULL;
  server->_priv->socket = NULL;
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
sim_server_new (SimContainer  *container,
		SimDatabase   *database)
{
  SimServer *server = NULL;

  g_return_val_if_fail (container != NULL, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);
  g_return_val_if_fail (database != NULL, NULL);
  g_return_val_if_fail (SIM_IS_DATABASE (database), NULL);

  server = SIM_SERVER (g_object_new (SIM_TYPE_SERVER, NULL));
  server->_priv->container = container;
  server->_priv->database = database;

  return server;
}

/*
 *
 *
 *
 *
 */
SimContainer*
sim_server_get_container (SimServer     *server)
{
  g_return_if_fail (server != NULL);
  g_return_if_fail (SIM_IS_SERVER (server));

  return server->_priv->container;
}

/*
 * sim_server_run:
 *
 *   arguments:
 *
 *   results:
 */
void
sim_server_run (SimServer *server)
{
  SimSessionData  *session_data;
  GTcpSocket      *socket;
  GThread         *thread;

  g_return_if_fail (server != NULL);
  g_return_if_fail (SIM_IS_SERVER (server));

  g_message ("Waiting for connections...");

  server->_priv->socket = gnet_tcp_socket_server_new_with_port (40001);
  while ((socket = gnet_tcp_socket_server_accept (server->_priv->socket)) != NULL)
    {
      session_data = g_new0 (SimSessionData, 1);
      session_data->server = server;
      session_data->socket = socket;

      /* Session Thread */
      thread = g_thread_create(sim_server_session, session_data, TRUE, NULL);
      if (thread != NULL) continue;
    }
}



/*
 * sim_server_session:
 *
 *   arguments:
 *
 *   results:
 */
static gpointer
sim_server_session (gpointer data)
{
  SimSessionData  *session_data = (SimSessionData *) data;
  SimSession  *session;
                                 
  g_return_val_if_fail (session_data != NULL, NULL);
  g_return_val_if_fail (session_data->server != NULL, NULL);
  g_return_val_if_fail (SIM_IS_SERVER (session_data->server), NULL);
  g_return_val_if_fail (session_data->socket != NULL, NULL);
                                 
  g_message ("New session");
                                 
  session = sim_session_new (session_data->server,
                             session_data->socket);

  sim_session_read (session);

  g_object_unref (session);
  g_free (session_data);
                                 
  return NULL;
}
