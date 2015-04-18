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
#include "sim-server.h"
#include "sim-container.h"
#include "sim-command.h"

enum 
{
  DESTROY,
  LAST_SIGNAL
};

struct _SimSessionPrivate {
  SimServer   *server;
  GTcpSocket  *socket;

  GIOChannel  *io;

  
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
  session->_priv->io = NULL;
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
sim_session_new (SimServer   *server,
		 GTcpSocket  *socket)
{
  SimSession *session = NULL;

  g_return_val_if_fail (server != NULL, NULL);
  g_return_val_if_fail (SIM_IS_SERVER (server), NULL);
  g_return_val_if_fail (socket != NULL, NULL);

  session = SIM_SESSION (g_object_new (SIM_TYPE_SESSION, NULL));
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

  g_return_if_fail (session != NULL);
  g_return_if_fail (SIM_IS_SESSION (session));
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));

  cmd = sim_command_new0 (SIM_COMMAND_TYPE_CONNECT_OK);

  sim_session_write (session, cmd);
}

/*
 *
 *
 *
 */
static void
sim_session_cmd_connect_ok (SimSession  *session,
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
sim_session_cmd_message (SimSession  *session,
			 SimCommand  *command)
{
  SimContainer  *container;
  SimMessage    *msg;

  g_return_if_fail (session != NULL);
  g_return_if_fail (SIM_IS_SESSION (session));
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));

  container = sim_server_get_container (session->_priv->server);

  msg = sim_command_get_message (command);

  if (!msg)
    return;

  if (msg->type == SIM_MESSAGE_TYPE_INVALID)
    return;

  sim_container_push_message (container, msg);
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

      cmd = sim_command_new (buffer);

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
	case SIM_COMMAND_TYPE_CONNECT_OK:
	  sim_session_cmd_connect_ok (session, cmd);
	  break;
	case SIM_COMMAND_TYPE_MESSAGE:
	  sim_session_cmd_message (session, cmd);
	  break;
	case SIM_COMMAND_TYPE_ERROR:
	  sim_session_cmd_error (session, cmd);
	  break;
	defalut:
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

  str = sim_command_get_str (command);
  if (!str)
    return 0;

  error = gnet_io_channel_writen (session->_priv->io, str, strlen(str), &n);

  g_free (str);

  if  (error != G_IO_ERROR_NONE)
    return 0;

  return n;
}
