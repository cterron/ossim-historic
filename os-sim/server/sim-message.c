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

#include <stdlib.h>
#include <time.h>
#include <config.h>

#include "sim-message.h"

typedef enum {
  SIM_MESSAGE_SYMBOL_INVALID = G_TOKEN_LAST,
  SIM_MESSAGE_SYMBOL_SNORT,
  SIM_MESSAGE_SYMBOL_SNORT_PRIORITY,
  SIM_MESSAGE_SYMBOL_LOGGER,
  SIM_MESSAGE_SYMBOL_LOGGER_SRC,
  SIM_MESSAGE_SYMBOL_LOGGER_S_PORT,
  SIM_MESSAGE_SYMBOL_LOGGER_DST,
  SIM_MESSAGE_SYMBOL_LOGGER_SERVICE,
  SIM_MESSAGE_SYMBOL_LOGGER_PROTO, 
  SIM_MESSAGE_SYMBOL_LOGGER_ACTION_ACCEPT,
  SIM_MESSAGE_SYMBOL_LOGGER_ACTION_REJECT,
  SIM_MESSAGE_SYMBOL_LOGGER_ACTION_DROP,
  SIM_MESSAGE_SYMBOL_LOGGER_RULE,
  SIM_MESSAGE_SYMBOL_RRD,
  SIM_MESSAGE_SYMBOL_RRD_HOST,
  SIM_MESSAGE_SYMBOL_RRD_WHAT,
  SIM_MESSAGE_SYMBOL_RRD_PRIORITY
} SimMessageSymbolType;

static const struct
{
  gchar *name;
  guint token;
} symbols[] = {
  { "snort", SIM_MESSAGE_SYMBOL_SNORT },
  { "Priority", SIM_MESSAGE_SYMBOL_SNORT_PRIORITY },
  { "logger", SIM_MESSAGE_SYMBOL_LOGGER },
  { "src", SIM_MESSAGE_SYMBOL_LOGGER_SRC },
  { "s_port", SIM_MESSAGE_SYMBOL_LOGGER_S_PORT },
  { "dst", SIM_MESSAGE_SYMBOL_LOGGER_DST },
  { "service", SIM_MESSAGE_SYMBOL_LOGGER_SERVICE },
  { "proto", SIM_MESSAGE_SYMBOL_LOGGER_PROTO },
  { "accept", SIM_MESSAGE_SYMBOL_LOGGER_ACTION_ACCEPT },
  { "reject", SIM_MESSAGE_SYMBOL_LOGGER_ACTION_REJECT },
  { "drop", SIM_MESSAGE_SYMBOL_LOGGER_ACTION_DROP },
  { "rule", SIM_MESSAGE_SYMBOL_LOGGER_RULE },
  { "RRD_anomaly", SIM_MESSAGE_SYMBOL_RRD },
  { "host", SIM_MESSAGE_SYMBOL_RRD_HOST },
  { "what", SIM_MESSAGE_SYMBOL_RRD_WHAT },
  { "priority", SIM_MESSAGE_SYMBOL_RRD_PRIORITY }
};

enum 
{
  DESTROY,
  LAST_SIGNAL
};

struct _SimMessagePrivate {
  /* Globals */
  gint              plugin;
  gint              tplugin;
  gint              priority;

  SimProtocolType   protocol;
  GInetAddr        *src_ia; 
  GInetAddr        *dst_ia;
  gint              src_port;
  gint              dst_port;

  /* rrd specific */
  gchar    *what;
};

static gpointer parent_class = NULL;
static gint sim_message_signals[LAST_SIGNAL] = { 0 };

/* Util functions */
static gchar*
sim_message_scan_host (GScanner *scanner);


static SimMessage*
sim_message_scan (gchar *buffer);


static guint
sim_message_scan_snort (SimMessage *message,
			GScanner *scanner);

static guint
sim_message_scan_logger (SimMessage *message,
			 GScanner *scanner);


static guint
sim_message_scan_rrd (SimMessage *message,
		      GScanner *scanner);


/* GType Functions */

static void 
sim_message_impl_dispose (GObject  *gobject)
{
  G_OBJECT_CLASS (parent_class)->dispose (gobject);
}

static void 
sim_message_impl_finalize (GObject  *gobject)
{
  SimMessage *msg = SIM_MESSAGE (gobject);

  if (msg->_priv->src_ia)
    gnet_inetaddr_unref (msg->_priv->src_ia);
  if (msg->_priv->dst_ia)
    gnet_inetaddr_unref (msg->_priv->dst_ia);
  g_free (msg->_priv->what);

  G_OBJECT_CLASS (parent_class)->finalize (gobject);
}

static void
sim_message_class_init (SimMessageClass * class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  parent_class = g_type_class_peek_parent (class);

  object_class->dispose = sim_message_impl_dispose;
  object_class->finalize = sim_message_impl_finalize;
}

static void
sim_message_instance_init (SimMessage *msg)
{
  msg->_priv = g_new0 (SimMessagePrivate, 1);
  
  msg->type = SIM_MESSAGE_TYPE_INVALID;

  msg->_priv->plugin = 0;
  msg->_priv->tplugin = 0;
  msg->_priv->priority = 1;

  msg->_priv->src_ia = NULL; 
  msg->_priv->dst_ia = NULL;
  msg->_priv->src_port = 0;
  msg->_priv->dst_port = 0;
  msg->_priv->protocol = SIM_PROTOCOL_TYPE_NONE;

  /* rrd specific */
  msg->_priv->what = NULL;

}

/* Public Methods */

GType
sim_message_get_type (void)
{
  static GType object_type = 0;
 
  if (!object_type)
  {
    static const GTypeInfo type_info = {
              sizeof (SimMessageClass),
              NULL,
              NULL,
              (GClassInitFunc) sim_message_class_init,
              NULL,
              NULL,                       /* class data */
              sizeof (SimMessage),
              0,                          /* number of pre-allocs */
              (GInstanceInitFunc) sim_message_instance_init,
              NULL                        /* value table */
    };

    g_type_init ();

    object_type = g_type_register_static (G_TYPE_OBJECT, "SimMessage", &type_info, 0);
  }

  return object_type;
}

SimMessage*
sim_message_new (gchar *buffer)
{
  SimMessage *msg;

  g_return_val_if_fail (buffer != NULL, NULL);

  msg = sim_message_scan (buffer);

  return msg;
}

SimMessage*
sim_message_new0 (SimMessageType  type)
{
  SimMessage *msg;

  msg = SIM_MESSAGE (g_object_new (SIM_TYPE_MESSAGE, NULL));
  msg->type = type;

  return msg;
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

/**
 * sim_message_scan
 *
 *
 */
static SimMessage*
sim_message_scan (gchar *buffer)
{
  SimMessage  *msg = NULL;
  GScanner    *scanner;
  gint         i;

  g_return_if_fail (buffer != NULL);

  /* Create scanner */
  scanner = g_scanner_new (NULL);

  /* Config scanner */
  scanner->config->cset_identifier_nth = (G_CSET_a_2_z "_-0123456789" G_CSET_A_2_Z);
  scanner->config->case_sensitive = TRUE;
  scanner->config->identifier_2_string = TRUE;
  scanner->config->scan_float = FALSE;
  scanner->config->numbers_2_int = TRUE;
  scanner->config->symbol_2_token = TRUE;

  /* Added symbols */
  for (i = 0; i < G_N_ELEMENTS (symbols); i++)
    g_scanner_scope_add_symbol (scanner, 0, symbols[i].name, GINT_TO_POINTER (symbols[i].token));

  /* Sets buffer */
  g_scanner_input_text (scanner, buffer, strlen(buffer));
  scanner->input_name = "sim_message_scan";

  do
    {
      g_scanner_get_next_token (scanner);

      switch (scanner->token)
	{
	case SIM_MESSAGE_SYMBOL_SNORT:
	  msg = SIM_MESSAGE (g_object_new (SIM_TYPE_MESSAGE, NULL));
	  sim_message_scan_snort (msg, scanner);
	  break;
	case SIM_MESSAGE_SYMBOL_LOGGER:
	  /*
	  msg = SIM_MESSAGE (g_object_new (SIM_TYPE_MESSAGE, NULL));
	  sim_message_scan_logger (msg, scanner);
	  */
	  break;
	case SIM_MESSAGE_SYMBOL_RRD:
	  /*
	  msg = SIM_MESSAGE (g_object_new (SIM_TYPE_MESSAGE, NULL));
	  sim_message_scan_rrd (msg, scanner);
	  */
	  break;
	default:
	  break;
	}
    }
  while(scanner->token != G_TOKEN_EOF);

  g_scanner_destroy (scanner);

  return msg;
}

/*
 * SNORT FUNCTIONS
 */

/**
 * sim_message_scan_snort
 *
 *
 */
static guint
sim_message_scan_snort (SimMessage *msg,
			GScanner *scanner)
{
  gchar  *ip;

  g_return_val_if_fail (msg != NULL, G_TOKEN_ERROR);
  g_return_val_if_fail (SIM_IS_MESSAGE (msg), G_TOKEN_ERROR);

  msg->type = SIM_MESSAGE_TYPE_SNORT;
  msg->_priv->priority = SNORT_DEFAULT_PRIORITY;

  /* Plugin Block */
  g_scanner_get_next_token (scanner);        /* : */
  g_scanner_get_next_token (scanner);        /* [ */
  g_scanner_get_next_token (scanner);        /* Plugin */
  msg->_priv->plugin = scanner->value.v_int;
  g_scanner_get_next_token (scanner);        /* : */
  g_scanner_get_next_token (scanner);        /* Tplugin */
  msg->_priv->tplugin = scanner->value.v_int;
  g_scanner_get_next_token (scanner);        /* : */
  g_scanner_get_next_token (scanner);        /* int */
  g_scanner_get_next_token (scanner);        /* ] */

  do {
    g_scanner_get_next_token (scanner);

    switch (scanner->token)
      {
      case SIM_MESSAGE_SYMBOL_SNORT_PRIORITY:
	g_scanner_get_next_token (scanner);
	g_scanner_get_next_token (scanner);
	msg->_priv->priority = abs (scanner->value.v_int - (SNORT_MAX_PRIORITY + 1));
	break;
      case G_TOKEN_LEFT_CURLY:    /* {PROTOCOL} */
	g_scanner_get_next_token (scanner);
	if (scanner->token != G_TOKEN_STRING)
	  break;

	if (!g_ascii_strncasecmp (scanner->value.v_string, "ICMP", 3))
	  {
	    msg->_priv->protocol = SIM_PROTOCOL_TYPE_ICMP;
	  }
	else if (!g_ascii_strncasecmp (scanner->value.v_string, "UDP", 3))
	  {
	    msg->_priv->protocol = SIM_PROTOCOL_TYPE_UDP;
	  }
	else if (!g_ascii_strncasecmp (scanner->value.v_string, "TCP", 3))
	  {
	    msg->_priv->protocol = SIM_PROTOCOL_TYPE_TCP;
	  }
	else
	  break;
	
	g_scanner_get_next_token (scanner);        /* } */
	
	/* Block: IPs */

	ip = sim_message_scan_host (scanner);
	msg->_priv->src_ia = gnet_inetaddr_new_nonblock (ip, 0);
	g_free (ip);
	if (msg->_priv->protocol != SIM_PROTOCOL_TYPE_ICMP)
	  {
	    g_scanner_get_next_token (scanner);        /* : */
	    g_scanner_get_next_token (scanner);        /* source port */ 
	    msg->_priv->src_port = scanner->value.v_int;
	  }
	
	g_scanner_get_next_token (scanner);        /* - */
	g_scanner_get_next_token (scanner);        /* > */
	
	ip = sim_message_scan_host (scanner);
	msg->_priv->dst_ia = gnet_inetaddr_new_nonblock (ip, 0);
	g_free (ip);
	if (msg->_priv->protocol != SIM_PROTOCOL_TYPE_ICMP)
	  {
	    g_scanner_get_next_token (scanner);        /* : */
	    g_scanner_get_next_token (scanner);        /* dest  port */
	    msg->_priv->dst_port = scanner->value.v_int;
	  }

	break;
      default:
	break;
      }
  } while(scanner->token != G_TOKEN_EOF);
 
  return G_TOKEN_EOF;
}

/*
 * LOGGER FUNCTIONS
 */

/**
 * sim_message_scan_logger
 *
 *
 */
static guint
sim_message_scan_logger (SimMessage *msg,
			 GScanner *scanner)
{
  gchar *ip;

  g_return_if_fail (msg != NULL);
  g_return_if_fail (SIM_IS_MESSAGE (msg));

  msg->type = SIM_MESSAGE_TYPE_LOGGER;
  msg->_priv->priority = FW1_DEFAULT_PRIORITY;

  do {
    g_scanner_get_next_token (scanner);

    switch (scanner->token)
      {
      case SIM_MESSAGE_SYMBOL_LOGGER_SRC:
	g_scanner_get_next_token (scanner);
	ip = sim_message_scan_host (scanner);
	msg->_priv->src_ia = gnet_inetaddr_new_nonblock (ip, 0);
	g_free (ip);
	break;
      case SIM_MESSAGE_SYMBOL_LOGGER_S_PORT:
	g_scanner_get_next_token (scanner);
	g_scanner_get_next_token (scanner);
	msg->_priv->src_port = scanner->value.v_int;
	break;
      case SIM_MESSAGE_SYMBOL_LOGGER_DST:
	g_scanner_get_next_token (scanner);
	ip = sim_message_scan_host (scanner);
	msg->_priv->dst_ia = gnet_inetaddr_new_nonblock (ip, 0);
	g_free (ip);
	break;
      case SIM_MESSAGE_SYMBOL_LOGGER_SERVICE:
	g_scanner_get_next_token (scanner);
	g_scanner_get_next_token (scanner);

	switch (scanner->token)
	  {
	  case G_TOKEN_INT:
	    msg->_priv->dst_port = scanner->value.v_int;
	    break;
	  default:
	    break;
	  }

	break;
      case SIM_MESSAGE_SYMBOL_LOGGER_PROTO:
	g_scanner_get_next_token (scanner);
	g_scanner_get_next_token (scanner);

	if (!g_ascii_strncasecmp (scanner->value.v_string, "ICMP", 3))
	  {
	    msg->_priv->protocol = SIM_PROTOCOL_TYPE_ICMP;
	  }
	else if (!g_ascii_strncasecmp (scanner->value.v_string, "UDP", 3))
	  {
	    msg->_priv->protocol = SIM_PROTOCOL_TYPE_UDP;
	  }
	else if (!g_ascii_strncasecmp (scanner->value.v_string, "TCP", 3))
	  {
	    msg->_priv->protocol = SIM_PROTOCOL_TYPE_TCP;
	  }

	break;
      case SIM_MESSAGE_SYMBOL_LOGGER_ACTION_ACCEPT:
	msg->_priv->plugin = GENERATOR_FW1;
	msg->_priv->tplugin = FW1_ACCEPT_TYPE;
        msg->_priv->priority = FW1_ACCEPT_PRIORITY;
	break;
      case SIM_MESSAGE_SYMBOL_LOGGER_ACTION_REJECT:
	msg->_priv->plugin = GENERATOR_FW1;
	msg->_priv->tplugin = FW1_REJECT_TYPE;
        msg->_priv->priority = FW1_REJECT_PRIORITY;
	break;
      case SIM_MESSAGE_SYMBOL_LOGGER_ACTION_DROP:
	msg->_priv->plugin = GENERATOR_FW1;
	msg->_priv->tplugin = FW1_ACCEPT_TYPE;
        msg->_priv->priority = FW1_DROP_PRIORITY;
	break;
      case SIM_MESSAGE_SYMBOL_LOGGER_RULE:	
	break;
      default:
	break;
      }

  } while(scanner->token != G_TOKEN_EOF);

  return G_TOKEN_EOF;
}

/*
 * RRD FUNCTIONS
 */

/**
 * sim_message_scan_rrd
 *
 *
 */
static guint
sim_message_scan_rrd (SimMessage *msg,
		      GScanner *scanner)
{
  gchar *ip;

  g_return_if_fail (msg != NULL);
  g_return_if_fail (SIM_IS_MESSAGE (msg));

  msg->type = SIM_MESSAGE_TYPE_RRD;
  msg->_priv->priority = RRD_DEFAULT_PRIORITY;

  do {
    g_scanner_get_next_token (scanner);

    switch (scanner->token)
      {
      case SIM_MESSAGE_SYMBOL_RRD_HOST:
	g_scanner_get_next_token (scanner);
	ip = sim_message_scan_host (scanner);
	msg->_priv->src_ia = gnet_inetaddr_new_nonblock (ip, 0);
	g_free (ip);
	break;
      case SIM_MESSAGE_SYMBOL_RRD_WHAT:
	g_scanner_get_next_token (scanner);
	g_scanner_get_next_token (scanner);
	msg->_priv->what = g_strdup(scanner->value.v_string);
	break;
      case SIM_MESSAGE_SYMBOL_RRD_PRIORITY:
	g_scanner_get_next_token (scanner);
	g_scanner_get_next_token (scanner);
	msg->_priv->priority = scanner->value.v_int;
	break;
      default:
	break;
      }

  } while(scanner->token != G_TOKEN_EOF);

  return G_TOKEN_EOF;
}


/*
 *
 *
 *
 */
gint
sim_message_get_plugin (SimMessage *msg)
{
  g_return_val_if_fail (msg != NULL, 0);
  g_return_val_if_fail (SIM_IS_MESSAGE (msg), 0);

  return msg->_priv->plugin;
}

/*
 *
 *
 *
 */
void
sim_message_set_plugin (SimMessage       *msg,
			gint              plugin)
{
  g_return_if_fail (msg != NULL);
  g_return_if_fail (SIM_IS_MESSAGE (msg));

  msg->_priv->plugin = plugin;
}

/*
 *
 *
 *
 */
gint
sim_message_get_tplugin (SimMessage *msg)
{
  g_return_if_fail (msg != NULL);
  g_return_if_fail (SIM_IS_MESSAGE (msg));

  return msg->_priv->tplugin;
}

/*
 *
 *
 *
 */
void
sim_message_set_tplugin (SimMessage       *msg,
			 gint              tplugin)
{
  g_return_if_fail (msg != NULL);
  g_return_if_fail (SIM_IS_MESSAGE (msg));

  msg->_priv->tplugin = tplugin;
}

/*
 *
 *
 *
 */
gint
sim_message_get_priority (SimMessage *msg)
{
  g_return_if_fail (msg != NULL);
  g_return_if_fail (SIM_IS_MESSAGE (msg));

  return msg->_priv->priority;
}

/*
 *
 *
 *
 */
void
sim_message_set_priority (SimMessage       *msg,
			  gint              priority)
{
  g_return_if_fail (msg != NULL);
  g_return_if_fail (SIM_IS_MESSAGE (msg));
  g_return_if_fail (priority > 0);

  msg->_priv->priority = priority;
}

/*
 *
 *
 *
 */
SimProtocolType
sim_message_get_protocol (SimMessage *msg)
{
  g_return_if_fail (msg != NULL);
  g_return_if_fail (SIM_IS_MESSAGE (msg));

  return msg->_priv->protocol;
}

/*
 *
 *
 *
 */
void
sim_message_set_protocol (SimMessage       *msg,
			  SimProtocolType   protocol)
{
  g_return_if_fail (msg != NULL);
  g_return_if_fail (SIM_IS_MESSAGE (msg));

  msg->_priv->protocol = protocol;
}

/*
 *
 *
 *
 */
GInetAddr*
sim_message_get_src_ia (SimMessage *msg)
{
  g_return_if_fail (msg != NULL);
  g_return_if_fail (SIM_IS_MESSAGE (msg));

  return msg->_priv->src_ia;
}

/*
 *
 *
 *
 */
void
sim_message_set_src_ia (SimMessage  *msg,
			GInetAddr   *src_ia)
{
  g_return_if_fail (msg != NULL);
  g_return_if_fail (SIM_IS_MESSAGE (msg));

  msg->_priv->src_ia = src_ia;;
}


/*
 *
 *
 *
 */
GInetAddr*
sim_message_get_dst_ia (SimMessage *msg)
{
  g_return_if_fail (msg != NULL);
  g_return_if_fail (SIM_IS_MESSAGE (msg));

  return msg->_priv->dst_ia;
}

/*
 *
 *
 *
 */
void
sim_message_set_dst_ia (SimMessage *msg,
			GInetAddr  *dst_ia)
{
  g_return_if_fail (msg != NULL);
  g_return_if_fail (SIM_IS_MESSAGE (msg));
  g_return_if_fail (dst_ia != NULL);

  msg->_priv->dst_ia = dst_ia;
}

/*
 *
 *
 *
 */
gint
sim_message_get_src_port (SimMessage *msg)
{
  g_return_if_fail (msg != NULL);
  g_return_if_fail (SIM_IS_MESSAGE (msg));

  return msg->_priv->src_port;
}

/*
 *
 *
 *
 */
void
sim_message_set_src_port (SimMessage *msg,
			  gint        port)
{
  g_return_if_fail (msg != NULL);
  g_return_if_fail (SIM_IS_MESSAGE (msg));

  msg->_priv->src_port = port;
}

/*
 *
 *
 *
 */
gint
sim_message_get_dst_port (SimMessage *msg)
{
  g_return_if_fail (msg != NULL);
  g_return_if_fail (SIM_IS_MESSAGE (msg));

  return msg->_priv->dst_port;
}

/*
 *
 *
 *
 */
void
sim_message_set_dst_port (SimMessage *msg,
				  gint        port)
{
  g_return_if_fail (msg != NULL);
  g_return_if_fail (SIM_IS_MESSAGE (msg));

  msg->_priv->dst_port = port;
}

/*
 *
 *
 *
 */
void
sim_message_print (SimMessage *msg)
{
  g_return_if_fail (msg != NULL);
  g_return_if_fail (SIM_IS_MESSAGE (msg));

  g_print ("Message: ");
  switch (msg->type)
    {
    case SIM_MESSAGE_TYPE_INVALID:
      g_print ("type=INVALID ");
      break;
    case SIM_MESSAGE_TYPE_SNORT:
      g_print ("type=snort ");
      break;
    case SIM_MESSAGE_TYPE_LOGGER:
      g_print ("type=logger ");
      break;
    case SIM_MESSAGE_TYPE_RRD:
      g_print ("type=rrd ");
      break;
    }

  g_print ("plugin=%d ", msg->_priv->plugin);
  g_print ("tplugin=%d ", msg->_priv->tplugin);
  g_print ("priority=%d ", msg->_priv->priority);
  g_print ("protocol=%d ", msg->_priv->protocol);
  g_print ("src_ia=%s ", (msg->_priv->src_ia) ? gnet_inetaddr_get_canonical_name (msg->_priv->src_ia) : NULL);
  g_print ("dst_ia=%s ", (msg->_priv->src_ia) ? gnet_inetaddr_get_canonical_name (msg->_priv->dst_ia) : NULL);
  g_print ("src_port=%d ", msg->_priv->src_port);
  g_print ("dst_port=%d ", msg->_priv->dst_port);
  g_print ("what=%s ", msg->_priv->what);
  g_print ("\n");
}
