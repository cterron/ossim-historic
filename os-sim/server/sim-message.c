/**
 *
 *
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
  gchar            *source_ip; 
  gchar            *dest_ip;
  gint              source_port;
  gint              dest_port;

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
sim_message_class_init (SimMessageClass * class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  parent_class = g_type_class_peek_parent (class);
}

static void
sim_message_instance_init (SimMessage *msg)
{
  msg->_priv = g_new0 (SimMessagePrivate, 1);
  
  msg->type = SIM_MESSAGE_TYPE_INVALID;

  msg->_priv->plugin = 0;
  msg->_priv->tplugin = 0;
  msg->_priv->priority = 0;

  msg->_priv->source_ip = NULL; 
  msg->_priv->dest_ip = NULL;
  msg->_priv->source_port = 0;
  msg->_priv->dest_port = 0;
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
  SimMessageType type;

  g_return_val_if_fail (buffer != NULL, NULL);

  msg = sim_message_scan (buffer);

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
	msg->_priv->source_ip = sim_message_scan_host (scanner);
	if (msg->_priv->protocol != SIM_PROTOCOL_TYPE_ICMP)
	  {
	    g_scanner_get_next_token (scanner);        /* : */
	    g_scanner_get_next_token (scanner);        /* source port */ 
	    msg->_priv->source_port = scanner->value.v_int;
	  }
	
	g_scanner_get_next_token (scanner);        /* - */
	g_scanner_get_next_token (scanner);        /* > */
	
	msg->_priv->dest_ip = sim_message_scan_host (scanner);
	if (msg->_priv->protocol != SIM_PROTOCOL_TYPE_ICMP)
	  {
	    g_scanner_get_next_token (scanner);        /* : */
	    g_scanner_get_next_token (scanner);        /* dest  port */
	    msg->_priv->dest_port = scanner->value.v_int;
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
  gchar *service;

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
	msg->_priv->source_ip = sim_message_scan_host (scanner);
	break;
      case SIM_MESSAGE_SYMBOL_LOGGER_S_PORT:
	g_scanner_get_next_token (scanner);
	g_scanner_get_next_token (scanner);
	msg->_priv->source_port = scanner->value.v_int;
	break;
      case SIM_MESSAGE_SYMBOL_LOGGER_DST:
	g_scanner_get_next_token (scanner);
	msg->_priv->dest_ip = sim_message_scan_host (scanner);
	break;
      case SIM_MESSAGE_SYMBOL_LOGGER_SERVICE:
	g_scanner_get_next_token (scanner);
	g_scanner_get_next_token (scanner);

	switch (scanner->token)
	  {
	  case G_TOKEN_INT:
	    msg->_priv->dest_port = scanner->value.v_int;
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
	msg->_priv->source_ip = sim_message_scan_host (scanner);
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
  g_return_if_fail (msg != NULL);
  g_return_if_fail (SIM_IS_MESSAGE (msg));

  return msg->_priv->plugin;
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
gchar*
sim_message_get_source_ip (SimMessage *msg)
{
  g_return_if_fail (msg != NULL);
  g_return_if_fail (SIM_IS_MESSAGE (msg));

  return msg->_priv->source_ip;
}

gchar*
/*
 *
 *
 *
 */
sim_message_get_destination_ip (SimMessage *msg)
{
  g_return_if_fail (msg != NULL);
  g_return_if_fail (SIM_IS_MESSAGE (msg));

  return msg->_priv->dest_ip;
}

/*
 *
 *
 *
 */
gint
sim_message_get_source_port (SimMessage *msg)
{
  g_return_if_fail (msg != NULL);
  g_return_if_fail (SIM_IS_MESSAGE (msg));

  return msg->_priv->source_port;
}

/*
 *
 *
 *
 */
gint
sim_message_get_destination_port (SimMessage *msg)
{
  g_return_if_fail (msg != NULL);
  g_return_if_fail (SIM_IS_MESSAGE (msg));

  return msg->_priv->dest_port;
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
sim_message_print (SimMessage *msg)
{
  g_return_if_fail (msg != NULL);
  g_return_if_fail (SIM_IS_MESSAGE (msg));

  switch (msg->type)
    {
    case SIM_MESSAGE_TYPE_INVALID:
      g_print ("Type:             INVALID\n");
      break;
    case SIM_MESSAGE_TYPE_SNORT:
      g_print ("Type:             SNORT\n");
      break;
    case SIM_MESSAGE_TYPE_LOGGER:
      g_print ("Type:             LOGGER\n");
      break;
    case SIM_MESSAGE_TYPE_RRD:
      g_print ("Type:             RRD\n");
      break;
    }

  g_print ("Plugin:           %d\n", msg->_priv->plugin);
  g_print ("Tplugin:          %d\n", msg->_priv->tplugin);
  g_print ("Priority          %d\n", msg->_priv->priority);
  g_print ("Protocol:         %d\n", msg->_priv->protocol);
  g_print ("Source IP:        %s\n", msg->_priv->source_ip);
  g_print ("Destination IP:   %s\n", msg->_priv->dest_ip);
  g_print ("Source Port:      %d\n", msg->_priv->source_port);
  g_print ("Destination Port: %d\n", msg->_priv->dest_port);
  g_print ("\n");
  g_print ("What:             %s\n", msg->_priv->what);
  g_print ("\n");
}
