/**
 *
 *
 */

#include "sim-server.h"
#include "sim-config.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <config.h>
 
enum 
{
  DESTROY,
  LAST_SIGNAL
};

struct _SimConfigPrivate {
  SimServer   *server;

};

static gpointer parent_class = NULL;
static gint sim_server_signals[LAST_SIGNAL] = { 0 };

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
 *
 */
SimConfig *
sim_config_new (void)
{
  SimConfig *config = NULL;

  config = SIM_CONFIG (g_object_new (SIM_TYPE_CONFIG, NULL));

  return config;
}
