/**
 *
 *
 */

#include "sim-message.h"

#include <time.h>
#include <config.h>

#define DELIMITER_MSG " "
#define SNORT_DEFAULT_PRIORITY  2
#define FW1_DEFAULT_PRIORITY    1
#define RRD_DEFAULT_PRIORITY    5

typedef enum {
  SIM_MESSAGE_SYMBOL_INVALID = G_TOKEN_LAST,
  SIM_MESSAGE_SYMBOL_SNORT,
  SIM_MESSAGE_SYMBOL_LOGGER,
  SIM_MESSAGE_SYMBOL_RRD
} SimMessageSymbolType;

static const struct
{
  gchar *name;
  guint token;
} symbols[] = {
  { "snort", SIM_MESSAGE_SYMBOL_SNORT },
  { "logger", SIM_MESSAGE_SYMBOL_LOGGER },
  { "RRD", SIM_MESSAGE_SYMBOL_RRD }
};

enum 
{
  DESTROY,
  LAST_SIGNAL
};

struct _SimMessagePrivate {
  /* Globals */
  gint     plugin;
  gint     tplugin;

  gchar    *protocol;
  gchar    *source_ip; 
  gchar    *dest_ip;
  gint     source_port;
  gint     dest_port;

  /* snort specific */
  gint     is_icmp;
  guint    priority_snort;

  /* fw-1 specific */
  gchar    service[64];
  gchar    action[24];
  gint     action_type;
  gchar    sensor_fw[16];
  gint     rule;
  guint    priority_fw1;
  time_t   global_time;
  time_t  *global_time_loc;
  gint     update_interval;

  /* rrd specific */
  guint    priority_rrd;
  gchar    what[64];
};

static gpointer parent_class = NULL;
static gint sim_server_signals[LAST_SIGNAL] = { 0 };

/* Util functions */
static gchar*
sim_message_scan_date (GScanner *scanner);
static gchar*
sim_message_scan_host (GScanner *scanner);


static SimMessage*
sim_message_scan (gchar *buffer);


static guint
sim_message_scan_snort (SimMessage *message,
			GScanner *scanner);
static guint
sim_message_scan_snort_engine (SimMessage *message,
			       GScanner *scanner);
static guint
sim_message_scan_snort_spade (SimMessage *message,
			      GScanner *scanner);
static guint
sim_message_scan_snort_scan2 (SimMessage *message,
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
sim_message_scan_date (GScanner *scanner)
{
  gchar *ret = NULL;

  /* Month */
  g_scanner_get_next_token (scanner);
  /* Day */
  g_scanner_get_next_token (scanner);
  /* Hour */
  g_scanner_get_next_token (scanner);
  /* : */
  g_scanner_get_next_token (scanner);
  /* Minute */
  g_scanner_get_next_token (scanner);
  /* : */
  g_scanner_get_next_token (scanner);
  /* Second */
  g_scanner_get_next_token (scanner);

  return ret;
}

static gchar* 
sim_message_scan_host (GScanner *scanner)
{
  gint a, b, c, d;
  gchar *ret = NULL;

  /* a OR name*/
  g_scanner_get_next_token (scanner);
  if (scanner->token == G_TOKEN_INT)
    {
      a = scanner->value.v_int;
      /* . */
      g_scanner_get_next_token (scanner);
      /* b */
      g_scanner_get_next_token (scanner);
      b = scanner->value.v_int;
      /* . */
      g_scanner_get_next_token (scanner);
      /* c */
      g_scanner_get_next_token (scanner);
      c = scanner->value.v_int;
      /* . */
      g_scanner_get_next_token (scanner);
      /* d */
      g_scanner_get_next_token (scanner);
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
  SimMessage *msg = NULL;
  GScanner *scanner;
  guint symbol, expected;
  gchar *date, *sensor;
  gint i;

  g_print ("%s", buffer);

  /* Create scanner */
  scanner = g_scanner_new (NULL);

  /* Config scanner */
  scanner->config->scan_float = FALSE;
  scanner->config->numbers_2_int = TRUE;
  scanner->config->symbol_2_token = TRUE;

  /* Added symbols */
  for (i = 0; i < G_N_ELEMENTS (symbols); i++)
    g_scanner_scope_add_symbol (scanner, 0, symbols[i].name, GINT_TO_POINTER (symbols[i].token));

  /* Sets buffer */
  g_scanner_input_text (scanner, buffer, strlen(buffer));
  scanner->input_name = "sim message_buffer";

  do
    {
      /* Gets the date */
      date = sim_message_scan_date (scanner);
      if (!date) expected = G_TOKEN_ERROR;

      /* Gets the sensor */
      sensor = sim_message_scan_host (scanner);
      if (!sensor) expected = G_TOKEN_ERROR;

      /* Gets the program */
      g_scanner_get_next_token (scanner);
      symbol = scanner->token;

      switch (symbol)
	{
	case SIM_MESSAGE_SYMBOL_SNORT:
	  msg = SIM_MESSAGE (g_object_new (SIM_TYPE_MESSAGE, NULL));
	  expected = sim_message_scan_snort (msg, scanner);
	  break;
	case SIM_MESSAGE_SYMBOL_LOGGER:
	  msg = SIM_MESSAGE (g_object_new (SIM_TYPE_MESSAGE, NULL));
	  expected = sim_message_scan_logger (msg, scanner);
	  break;
	case SIM_MESSAGE_SYMBOL_RRD:
	  msg = SIM_MESSAGE (g_object_new (SIM_TYPE_MESSAGE, NULL));
	  expected = sim_message_scan_rrd (msg, scanner);
	  break;
	default:
	  g_free (date);
	  g_free (sensor);
	  expected = G_TOKEN_ERROR;
	  break;
	}
    }
  while((expected == G_TOKEN_NONE) &&
	(scanner->token != G_TOKEN_EOF) &&
	(scanner->token != G_TOKEN_ERROR));

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
  gint plug3;
  guint expected;

  g_return_if_fail (msg != NULL);
  g_return_if_fail (SIM_IS_MESSAGE (msg));

  msg->type = SIM_MESSAGE_TYPE_SNORT;

  /* : */
  g_scanner_get_next_token (scanner);
  /* [ */
  g_scanner_get_next_token (scanner);

  /* int */
  g_scanner_get_next_token (scanner);
  msg->_priv->plugin = scanner->value.v_int;

  /* : */
  g_scanner_get_next_token (scanner);
  /* int */
  g_scanner_get_next_token (scanner);
  msg->_priv->tplugin = scanner->value.v_int;

  /* : */
  g_scanner_get_next_token (scanner);
  /* int */
  g_scanner_get_next_token (scanner);
  plug3 = scanner->value.v_int;
  /* ] */
  g_scanner_get_next_token (scanner);

  switch (msg->_priv->plugin)
    {
    case GENERATOR_SNORT_ENGINE: /* snort */
      expected = sim_message_scan_snort_engine (msg, scanner);
      break;
    case GENERATOR_SPP_SPADE:    /* spade */
      expected = sim_message_scan_snort_spade (msg, scanner);
      break;
    case GENERATOR_SPP_SCAN2:    /* portscan */
      expected = sim_message_scan_snort_scan2 (msg, scanner);
      break;
    default:
      expected = G_TOKEN_ERROR;
      break;
    }

  return expected;
}

static guint
sim_message_scan_snort_engine (SimMessage *msg,
			      GScanner *scanner)
{
  /* Block: Description*/
  do {
    g_scanner_get_next_token (scanner);
  } while(scanner->token != G_TOKEN_LEFT_BRACE);


  /* Block: Clasification*/
  /* Classification */
  g_scanner_get_next_token (scanner);
  /* : */
  g_scanner_get_next_token (scanner);
  do {
    g_scanner_get_next_token (scanner);
  } while(scanner->token != G_TOKEN_RIGHT_BRACE);


  /* Block: Priority*/
  /* [ */
  g_scanner_get_next_token (scanner);
  /* Priority */
  g_scanner_get_next_token (scanner);
  /* : */
  g_scanner_get_next_token (scanner);
  /* int */
  g_scanner_get_next_token (scanner);
  msg->_priv->priority_snort = scanner->value.v_int;	  
  /* ] */
  g_scanner_get_next_token (scanner);
  /* : */
  g_scanner_get_next_token (scanner);


  /* Block: Protocol */
  /* { */
  g_scanner_get_next_token (scanner);
  /* Protocol */
  g_scanner_get_next_token (scanner);
  msg->_priv->protocol = g_strdup(scanner->value.v_string);	  
  /* } */
  g_scanner_get_next_token (scanner);


 /* Block: IPs */
  msg->_priv->source_ip = sim_message_scan_host (scanner);
  g_scanner_get_next_token (scanner);
  g_scanner_get_next_token (scanner);
  msg->_priv->dest_ip = sim_message_scan_host (scanner);


  /* Block: is icmp */
  if (!g_strcasecmp(msg->_priv->protocol, "ICMP"))
    msg->_priv->is_icmp = 1;
  else
    msg->_priv->is_icmp = 0;

  return G_TOKEN_EOF;
}

static guint
sim_message_scan_snort_spade (SimMessage *msg,
			      GScanner *scanner)
{
  return G_TOKEN_EOF;
}

static guint
sim_message_scan_snort_scan2 (SimMessage *msg,
			      GScanner *scanner)
{
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
  g_return_if_fail (msg != NULL);
  g_return_if_fail (SIM_IS_MESSAGE (msg));

  msg->type = SIM_MESSAGE_TYPE_LOGGER;

  msg->_priv->priority_fw1 = FW1_DEFAULT_PRIORITY;

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

  msg->_priv->priority_rrd = RRD_DEFAULT_PRIORITY;

  return G_TOKEN_EOF;
}
