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


#include "sim-smtp.h"
#include <config.h>

#define SMTP_CONNECT   "220 "
#define SMTP_OK        "250 "
#define SMTP_DATA      "354 "
#define SMTP_QUIT      "221 "

enum
{
  DESTROY,
  LAST_SIGNAL
};

struct _SimSmtpPrivate 
{
  GTcpSocket  *socket;
  GIOChannel  *io;

  gchar       *hostname;
  gint         port;
};

static gpointer parent_class = NULL;
static gint sim_smtp_signals[LAST_SIGNAL] = { 0 };

/* GType Functions */

static void 
sim_smtp_impl_dispose (GObject  *gobject)
{
  G_OBJECT_CLASS (parent_class)->dispose (gobject);
}

static void
sim_smtp_impl_finalize (GObject  *gobject)
{
  SimSmtp *smtp = SIM_SMTP (gobject);

  g_free (smtp->_priv);

  G_OBJECT_CLASS (parent_class)->finalize (gobject);
}

static void
sim_smtp_class_init (SimSmtpClass * class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  parent_class = g_type_class_peek_parent (class);

  object_class->dispose = sim_smtp_impl_dispose;
  object_class->finalize = sim_smtp_impl_finalize;
}

static void
sim_smtp_instance_init (SimSmtp *smtp)
{
  smtp->_priv = g_new0 (SimSmtpPrivate, 1);
}

/* Public Methods */

GType
sim_smtp_get_type (void)
{
  static GType object_type = 0;
 
  if (!object_type)
  {
    static const GTypeInfo type_info = {
              sizeof (SimSmtpClass),
              NULL,
              NULL,
              (GClassInitFunc) sim_smtp_class_init,
              NULL,
              NULL,                       /* class data */
              sizeof (SimSmtp),
              0,                          /* number of pre-allocs */
              (GInstanceInitFunc) sim_smtp_instance_init,
              NULL                        /* value table */
    };
    
    g_type_init ();
                                                                                                                             
    object_type = g_type_register_static (G_TYPE_OBJECT, "SimSmtp", &type_info, 0);
  }
                                                                                                                             
  return object_type;
}

/*
 *
 *
 *
 *
 */
SimSmtp*
sim_smtp_new (const gchar  *hostname,
	      gint          port)
{
  GTcpSocket  *socket;
  SimSmtp     *smtp;

  g_return_val_if_fail (hostname, NULL);
  g_return_val_if_fail (port > 0, NULL);

  socket = gnet_tcp_socket_connect (hostname, port);
  if (!socket) return NULL;
  gnet_tcp_socket_unref (socket);

  smtp = SIM_SMTP (g_object_new (SIM_TYPE_SMTP, NULL));
  smtp->_priv->hostname = g_strdup (hostname);
  smtp->_priv->port = port;

  return smtp;
}
// vim: set tabstop=2:
