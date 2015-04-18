/**
 *
 *
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <config.h>

#include "sim-config.h"

typedef enum {
  SIM_CONFIG_SYMBOL_INVALID = G_TOKEN_LAST,
  SIM_CONFIG_SYMBOL_LOG_FILE,
  SIM_CONFIG_SYMBOL_DATABASE,
  SIM_CONFIG_SYMBOL_HOST,
  SIM_CONFIG_SYMBOL_PORT,
  SIM_CONFIG_SYMBOL_USERNAME,
  SIM_CONFIG_SYMBOL_PASSWORD,
  SIM_CONFIG_SYMBOL_UPDATE_INTERVAL
} SimConfigSymbolType;

static const struct
{
  gchar *name;
  guint token;
} symbols[] = {
  { "ossim_log", SIM_CONFIG_SYMBOL_LOG_FILE },
  { "ossim_base", SIM_CONFIG_SYMBOL_DATABASE },
  { "ossim_host", SIM_CONFIG_SYMBOL_HOST },
  { "ossim_port", SIM_CONFIG_SYMBOL_PORT },
  { "ossim_user", SIM_CONFIG_SYMBOL_USERNAME },
  { "ossim_pass", SIM_CONFIG_SYMBOL_PASSWORD },
  { "UPDATE_INTERVAL", SIM_CONFIG_SYMBOL_UPDATE_INTERVAL }
};

typedef struct
{
  SimConfigPropertyType   type;
  gchar                  *value;
} SimConfigProperty;

enum
{
  DESTROY,
  LAST_SIGNAL
};

struct _SimConfigPrivate {
  GList     *properties;
};

static gpointer parent_class = NULL;
static gint sim_config_signals[LAST_SIGNAL] = { 0 };

static void sim_config_scan (SimConfig    *config,
			     const gchar *filename);

/* GType Functions */

static void
sim_config_class_init (SimConfigClass * class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  parent_class = g_type_class_peek_parent (class);
}

static void
sim_config_instance_init (SimConfig *config)
{
  config->_priv = g_new0 (SimConfigPrivate, 1);

  config->_priv->properties = NULL;
}

/* Public Methods */

GType
sim_config_get_type (void)
{
  static GType object_type = 0;
 
  if (!object_type)
  {
    static const GTypeInfo type_info = {
              sizeof (SimConfigClass),
              NULL,
              NULL,
              (GClassInitFunc) sim_config_class_init,
              NULL,
              NULL,                       /* class data */
              sizeof (SimConfig),
              0,                          /* number of pre-allocs */
              (GInstanceInitFunc) sim_config_instance_init,
              NULL                        /* value table */
    };
    
    g_type_init ();
                                                                                                                             
    object_type = g_type_register_static (G_TYPE_OBJECT, "SimConfig", &type_info, 0);
  }
                                                                                                                             
  return object_type;
}

/*
 *
 *
 *
 */
SimConfig*
sim_config_new (const gchar    *filename)
{
  SimConfig *config = NULL;

  g_return_val_if_fail (filename != NULL, NULL);

  config = SIM_CONFIG (g_object_new (SIM_TYPE_CONFIG, NULL));

  sim_config_scan (config, filename);

  return config;
}

/*
 *
 *
 *
 */
static void 
sim_config_scan (SimConfig    *config,
		 const gchar  *filename)
{
  SimConfigProperty *property;
  GScanner    *scanner;
  gint         fd;
  gint         i;

  g_return_if_fail (config != NULL);
  g_return_if_fail (SIM_IS_CONFIG (config));
  g_return_if_fail (filename != NULL);

  /* open config file */
  fd = open (filename, O_RDONLY);

  /* Create scanner */
  scanner = g_scanner_new (NULL);

  /* Config scanner */
  scanner->config->cset_skip_characters = ( "\t" );
  scanner->config->cset_identifier_first = (G_CSET_a_2_z "/_" G_CSET_A_2_Z);
  scanner->config->cset_identifier_nth = (G_CSET_a_2_z "/._-0123456789" G_CSET_A_2_Z);
  scanner->config->case_sensitive = TRUE;
  scanner->config->identifier_2_string = TRUE;
  scanner->config->scan_float = FALSE;
  scanner->config->numbers_2_int = TRUE;
  scanner->config->symbol_2_token = TRUE;

  /* Added symbols */
  for (i = 0; i < G_N_ELEMENTS (symbols); i++)
    g_scanner_scope_add_symbol (scanner, 0, symbols[i].name, GINT_TO_POINTER (symbols[i].token));

  /* Sets file descriptor */
  g_scanner_input_file (scanner, fd);
  scanner->input_name = "sim_config_scan";

  do
    {
      g_scanner_get_next_token (scanner);

      switch (scanner->token)
	{
	case SIM_CONFIG_SYMBOL_LOG_FILE:
	  g_scanner_get_next_token (scanner); /* = */
	  g_scanner_get_next_token (scanner); /* value */

	  property = g_new0 (SimConfigProperty, 1);
	  property->type = SIM_CONFIG_PROPERTY_TYPE_LOG_FILE;
	  property->value = g_strdup (scanner->value.v_string);

	  config->_priv->properties = g_list_append (config->_priv->properties, property);
	  break;
	case SIM_CONFIG_SYMBOL_DATABASE:
	  g_scanner_get_next_token (scanner); /* = */
	  g_scanner_get_next_token (scanner); /* value */

	  property = g_new0 (SimConfigProperty, 1);
	  property->type = SIM_CONFIG_PROPERTY_TYPE_DATABASE;
	  property->value = g_strdup (scanner->value.v_string);

	  config->_priv->properties = g_list_append (config->_priv->properties, property);	  
	  break;
	case SIM_CONFIG_SYMBOL_USERNAME:
	  g_scanner_get_next_token (scanner); /* = */
	  g_scanner_get_next_token (scanner); /* value */

	  property = g_new0 (SimConfigProperty, 1);
	  property->type = SIM_CONFIG_PROPERTY_TYPE_USERNAME;
	  property->value = g_strdup (scanner->value.v_string);

	  config->_priv->properties = g_list_append (config->_priv->properties, property);
	  break;
	case SIM_CONFIG_SYMBOL_PASSWORD:
	  g_scanner_get_next_token (scanner); /* = */
	  g_scanner_get_next_token (scanner); /* value */

	  property = g_new0 (SimConfigProperty, 1);
	  property->type = SIM_CONFIG_PROPERTY_TYPE_PASSWORD;

	  if (scanner->token == G_TOKEN_STRING)
	    property->value = g_strdup (scanner->value.v_string);
	  else
	    property->value = NULL;

	  config->_priv->properties = g_list_append (config->_priv->properties, property);
	  break;
	case SIM_CONFIG_SYMBOL_UPDATE_INTERVAL:
	  g_scanner_get_next_token (scanner); /* = */
	  g_scanner_get_next_token (scanner); /* value */

	  property = g_new0 (SimConfigProperty, 1);
	  property->type = SIM_CONFIG_PROPERTY_TYPE_UPDATE_INTERVAL;

	  if (scanner->token == G_TOKEN_INT)
	    property->value = g_strdup_printf ("%d", scanner->value.v_int);
	  else
	    property->value = NULL;

	  config->_priv->properties = g_list_append (config->_priv->properties, property);
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
gchar*
sim_config_get_property_value              (SimConfig              *config,
					    SimConfigPropertyType   type)
{
  SimConfigProperty *property;
  gchar             *value = NULL;
  gint               i;

  g_return_if_fail (config != NULL);
  g_return_if_fail (SIM_IS_CONFIG (config));

  for (i = 0; i < g_list_length(config->_priv->properties); i++)
    {
      property = (SimConfigProperty *) g_list_nth_data (config->_priv->properties, i);
      
      if (property->type == type)
	{
	  value = property->value;
	}
    }

  return value;
}
