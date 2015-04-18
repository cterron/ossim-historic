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

#include "sim-command.h"
#include "sim-message.h"

typedef enum {
  SIM_COMMAND_SCOPE_COMMAND,
  SIM_COMMAND_SCOPE_MESSAGE
} SimCommandScopeType;

typedef enum {
  SIM_COMMAND_SYMBOL_INVALID = G_TOKEN_LAST,
  SIM_COMMAND_SYMBOL_CONNECT,
  SIM_COMMAND_SYMBOL_CONNECT_OK,
  SIM_COMMAND_SYMBOL_MESSAGE,
  SIM_COMMAND_SYMBOL_ERROR,
  SIM_COMMAND_SYMBOL_MESSAGE_TYPE,
  SIM_COMMAND_SYMBOL_MESSAGE_DATE,
  SIM_COMMAND_SYMBOL_MESSAGE_SENSOR,
  SIM_COMMAND_SYMBOL_MESSAGE_PLUGIN,
  SIM_COMMAND_SYMBOL_MESSAGE_TPLUGIN,
  SIM_COMMAND_SYMBOL_MESSAGE_PRIORITY,
  SIM_COMMAND_SYMBOL_MESSAGE_PROTOCOL,
  SIM_COMMAND_SYMBOL_MESSAGE_SRC_IP,
  SIM_COMMAND_SYMBOL_MESSAGE_SRC_PORT,
  SIM_COMMAND_SYMBOL_MESSAGE_DST_IP,
  SIM_COMMAND_SYMBOL_MESSAGE_DST_PORT
} SimCommandSymbolType;

static const struct
{
  gchar *name;
  guint token;
} command_symbols[] = {
  { "connect", SIM_COMMAND_SYMBOL_CONNECT },
  { "connect-ok", SIM_COMMAND_SYMBOL_CONNECT_OK },
  { "message", SIM_COMMAND_SYMBOL_MESSAGE },
  { "error", SIM_COMMAND_SYMBOL_ERROR }
};

static const struct
{
  gchar *name;
  guint token;
} message_symbols[] = {
  { "type", SIM_COMMAND_SYMBOL_MESSAGE_TYPE },
  { "date", SIM_COMMAND_SYMBOL_MESSAGE_DATE },
  { "sensor", SIM_COMMAND_SYMBOL_MESSAGE_SENSOR },
  { "plugin", SIM_COMMAND_SYMBOL_MESSAGE_PLUGIN },
  { "tplugin", SIM_COMMAND_SYMBOL_MESSAGE_TPLUGIN },
  { "priority", SIM_COMMAND_SYMBOL_MESSAGE_PRIORITY },
  { "protocol", SIM_COMMAND_SYMBOL_MESSAGE_PROTOCOL },
  { "src_ip", SIM_COMMAND_SYMBOL_MESSAGE_SRC_IP },
  { "src_port", SIM_COMMAND_SYMBOL_MESSAGE_SRC_PORT },
  { "dst_ip", SIM_COMMAND_SYMBOL_MESSAGE_DST_IP },
  { "dst_port", SIM_COMMAND_SYMBOL_MESSAGE_DST_PORT }
};


enum 
{
  DESTROY,
  LAST_SIGNAL
};

struct _SimCommandPrivate {
  union {
    SimMessage *msg;
  } data;
};

static void sim_command_scan (SimCommand    *command,
			      const gchar   *buffer);
static void sim_command_message_scan (SimCommand    *command,
				      GScanner      *scanner);


static gpointer parent_class = NULL;
static gint sim_server_signals[LAST_SIGNAL] = { 0 };

/* GType Functions */

static void 
sim_command_impl_dispose (GObject  *gobject)
{
  G_OBJECT_CLASS (parent_class)->dispose (gobject);
}

static void 
sim_command_impl_finalize (GObject  *gobject)
{
  G_OBJECT_CLASS (parent_class)->finalize (gobject);
}

static void
sim_command_class_init (SimCommandClass * class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  parent_class = g_type_class_ref (G_TYPE_OBJECT);

  object_class->dispose = sim_command_impl_dispose;
  object_class->finalize = sim_command_impl_finalize;
}

static void
sim_command_instance_init (SimCommand *command)
{
  command->_priv = g_new0 (SimCommandPrivate, 1);

  command->type = SIM_COMMAND_TYPE_NONE;
}

/* Public Methods */

GType
sim_command_get_type (void)
{
  static GType object_type = 0;
 
  if (!object_type)
  {
    static const GTypeInfo type_info = {
              sizeof (SimCommandClass),
              NULL,
              NULL,
              (GClassInitFunc) sim_command_class_init,
              NULL,
              NULL,                       /* class data */
              sizeof (SimCommand),
              0,                          /* number of pre-allocs */
              (GInstanceInitFunc) sim_command_instance_init,
              NULL                        /* value table */
    };
    
    g_type_init ();
                                                                                                                             
    object_type = g_type_register_static (G_TYPE_OBJECT, "SimCommand", &type_info, 0);
  }

  return object_type;
}

/*
 *
 *
 *
 *
 */
SimCommand*
sim_command_new (const gchar    *buffer)
{
  SimCommand *command = NULL;

  g_return_val_if_fail (buffer != NULL, NULL);

  command = SIM_COMMAND (g_object_new (SIM_TYPE_COMMAND, NULL));

  sim_command_scan (command, buffer);

  return command;
}

/*
 *
 *
 *
 *
 */
SimCommand*
sim_command_new0 (SimCommandType  type)
{
  SimCommand *command = NULL;

  command = SIM_COMMAND (g_object_new (SIM_TYPE_COMMAND, NULL));
  command->type = type;

  return command;
}

static gchar* 
sim_message_scan_host (GScanner *scanner)
{
  gint a, b, c, d;
  gchar *ret = NULL;

  /* ip OR name*/
  g_scanner_get_next_token (scanner);
  if (scanner->token == G_TOKEN_INT)
    {
      a = scanner->value.v_int;
      g_scanner_get_next_token (scanner);        /* . */
      g_scanner_get_next_token (scanner);        /* b */
      b = scanner->value.v_int;
      g_scanner_get_next_token (scanner);        /* . */
      g_scanner_get_next_token (scanner);        /* c */
      c = scanner->value.v_int;
      g_scanner_get_next_token (scanner);        /* . */
      g_scanner_get_next_token (scanner);        /* d */
      d = scanner->value.v_int;

      ret = g_strdup_printf ("%d.%d.%d.%d", a, b, c, d);
    }
  else if (scanner->token == G_TOKEN_STRING)
    {
      ret = g_strdup (scanner->value.v_string);
    }

  return ret;
}

/*
 *
 *
 *
 */
static void
sim_command_scan (SimCommand    *command,
		  const gchar   *buffer)
{
  GScanner    *scanner;
  gint         i;
  
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));
  g_return_if_fail (buffer != NULL);
  
  /* Create scanner */
  scanner = g_scanner_new (NULL);

  /* Config scanner */
  scanner->config->cset_identifier_first = (G_CSET_a_2_z "_" G_CSET_A_2_Z);
  scanner->config->cset_identifier_nth = (G_CSET_a_2_z ":._-0123456789" G_CSET_A_2_Z);
  scanner->config->case_sensitive = TRUE;
  scanner->config->identifier_2_string = TRUE;
  scanner->config->scan_float = FALSE;
  scanner->config->numbers_2_int = TRUE;
  scanner->config->symbol_2_token = TRUE;
  
  /* Added command symbols */
  for (i = 0; i < G_N_ELEMENTS (command_symbols); i++)
    g_scanner_scope_add_symbol (scanner, SIM_COMMAND_SCOPE_COMMAND, command_symbols[i].name, GINT_TO_POINTER (command_symbols[i].token));
  
  /* Added message symbols */
  for (i = 0; i < G_N_ELEMENTS (message_symbols); i++)
    g_scanner_scope_add_symbol (scanner, SIM_COMMAND_SCOPE_MESSAGE, message_symbols[i].name, GINT_TO_POINTER (message_symbols[i].token));
  
  /* Sets input text */
  g_scanner_input_text (scanner, buffer, strlen (buffer));
  scanner->input_name = "sim_command_scan";

  g_scanner_set_scope (scanner, SIM_COMMAND_SCOPE_COMMAND);
  do
    {
      g_scanner_get_next_token (scanner);
 
      switch (scanner->token)
        {
        case SIM_COMMAND_SYMBOL_CONNECT:
	  command->type = SIM_COMMAND_TYPE_CONNECT;
          break;
        case SIM_COMMAND_SYMBOL_CONNECT_OK:
	  command->type = SIM_COMMAND_TYPE_CONNECT_OK;
          break;
        case SIM_COMMAND_SYMBOL_MESSAGE:
	  sim_command_message_scan (command, scanner);
          break;
        case SIM_COMMAND_SYMBOL_ERROR:
	  command->type = SIM_COMMAND_TYPE_ERROR;
          break;
        default:
          break;
        }
    }
  while(scanner->token != G_TOKEN_EOF);
                                                                                                                                                                            
  g_scanner_destroy (scanner);
}

/*
 *
 *
 *
 */
static void
sim_command_message_scan (SimCommand    *command,
			  GScanner      *scanner)
{
  SimMessage   *msg;
  GInetAddr    *ia;
  gchar        *ip;

  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));
  g_return_if_fail (scanner != NULL);

  command->type = SIM_COMMAND_TYPE_MESSAGE;

  msg = sim_message_new0 (SIM_MESSAGE_TYPE_INVALID);

  g_scanner_set_scope (scanner, SIM_COMMAND_SCOPE_MESSAGE);
  do
    {
      g_scanner_get_next_token (scanner);
 
      switch (scanner->token)
        {
        case SIM_COMMAND_SYMBOL_MESSAGE_TYPE:
	  g_scanner_get_next_token (scanner); /* = */
	  g_scanner_get_next_token (scanner); /* value */
	  
	  if (scanner->token != G_TOKEN_STRING)
	    break;

	  if (!g_ascii_strncasecmp (scanner->value.v_string, "snort", 5))
	    {
	      msg->type = SIM_MESSAGE_TYPE_SNORT;
	    }
	  else if (!g_ascii_strncasecmp (scanner->value.v_string, "logger", 6))
	    {
	      msg->type = SIM_MESSAGE_TYPE_LOGGER;
	    }

          break;
        case SIM_COMMAND_SYMBOL_MESSAGE_DATE:
	  g_scanner_get_next_token (scanner); /* = */
	  g_scanner_get_next_token (scanner); /* value */
          break;
        case SIM_COMMAND_SYMBOL_MESSAGE_SENSOR:
	  g_scanner_get_next_token (scanner); /* = */
	  g_scanner_get_next_token (scanner); /* value */
          break;
        case SIM_COMMAND_SYMBOL_MESSAGE_PLUGIN:
	  g_scanner_get_next_token (scanner); /* = */
	  g_scanner_get_next_token (scanner); /* value */

	  if (scanner->token != G_TOKEN_INT)
	    break;

	  sim_message_set_plugin (msg, scanner->value.v_int);
          break;
        case SIM_COMMAND_SYMBOL_MESSAGE_TPLUGIN:
	  g_scanner_get_next_token (scanner); /* = */
	  g_scanner_get_next_token (scanner); /* value */

	  if (scanner->token != G_TOKEN_INT)
	    break;

	  sim_message_set_tplugin (msg, scanner->value.v_int);
          break;
        case SIM_COMMAND_SYMBOL_MESSAGE_PRIORITY:
	  g_scanner_get_next_token (scanner); /* = */
	  g_scanner_get_next_token (scanner); /* value */

	  if (scanner->token != G_TOKEN_INT)
	    break;

	  sim_message_set_priority (msg, scanner->value.v_int);
          break;
        case SIM_COMMAND_SYMBOL_MESSAGE_PROTOCOL:
	  g_scanner_get_next_token (scanner); /* = */
	  g_scanner_get_next_token (scanner); /* value */

	  if (scanner->token != G_TOKEN_STRING)
	    break;

	  if (!g_ascii_strncasecmp (scanner->value.v_string, "ICMP", 3))
	    {
	      sim_message_set_protocol (msg, SIM_PROTOCOL_TYPE_ICMP);
	    }
	  else if (!g_ascii_strncasecmp (scanner->value.v_string, "UDP", 3))
	    {
	      sim_message_set_protocol (msg, SIM_PROTOCOL_TYPE_UDP);
	    }
	  else if (!g_ascii_strncasecmp (scanner->value.v_string, "TCP", 3))
	    {
	      sim_message_set_protocol (msg, SIM_PROTOCOL_TYPE_TCP);
	    }

          break;
        case SIM_COMMAND_SYMBOL_MESSAGE_SRC_IP:
	  g_scanner_get_next_token (scanner); /* = */
	  ip = sim_message_scan_host (scanner);  /* value */
	  ia = gnet_inetaddr_new_nonblock (ip, 0);
	  g_free (ip);

	  if (!ia)
	    {
	      msg->type = SIM_MESSAGE_TYPE_INVALID;
	      break;
	    }

	  sim_message_set_src_ia (msg, ia);
          break;
        case SIM_COMMAND_SYMBOL_MESSAGE_SRC_PORT:
	  g_scanner_get_next_token (scanner); /* = */
	  g_scanner_get_next_token (scanner); /* value */

	  if (scanner->token != G_TOKEN_INT)
	    break;

	  sim_message_set_src_port (msg, scanner->value.v_int);
          break;
        case SIM_COMMAND_SYMBOL_MESSAGE_DST_IP:
	  g_scanner_get_next_token (scanner); /* = */
	  ip = sim_message_scan_host (scanner);  /* value */
	  ia = gnet_inetaddr_new_nonblock (ip, 0);
	  g_free (ip);

	  if (!ia)
	    {
	      msg->type = SIM_MESSAGE_TYPE_INVALID;
	      break;
	    }

	  sim_message_set_dst_ia (msg, ia);
          break;
        case SIM_COMMAND_SYMBOL_MESSAGE_DST_PORT:
	  g_scanner_get_next_token (scanner); /* = */
	  g_scanner_get_next_token (scanner); /* value */

	  if (scanner->token != G_TOKEN_INT)
	    break;

	  sim_message_set_dst_port (msg, scanner->value.v_int);
          break;
        default:
          break;
        }
    }
  while(scanner->token != G_TOKEN_EOF);

  command->_priv->data.msg = msg;
}

/*
 *
 *
 *
 */
gchar*
sim_command_get_str (SimCommand    *command)
{
  gchar *str = NULL;

  g_return_val_if_fail (command != NULL, NULL);
  g_return_val_if_fail (SIM_IS_COMMAND (command), NULL);

  switch (command->type)
    {
    case SIM_COMMAND_TYPE_CONNECT:
      str = g_strdup ("connect\n");
      break;
    case SIM_COMMAND_TYPE_CONNECT_OK:
      str = g_strdup ("connect-ok\n");
      break;
    case SIM_COMMAND_TYPE_MESSAGE:
      str = g_strdup ("message\n");
      break;
    case SIM_COMMAND_TYPE_ERROR:
      str = g_strdup ("error\n");
      break;
    default:
      break;
    }

  return str;
}

/*
 *
 *
 *
 */
SimMessage*
sim_command_get_message (SimCommand     *command)
{
  g_return_val_if_fail (command != NULL, NULL);
  g_return_val_if_fail (SIM_IS_COMMAND (command), NULL);
  g_return_val_if_fail (command->type == SIM_COMMAND_TYPE_MESSAGE, NULL);

  return command->_priv->data.msg;
}
