/**
 *
 *
 */

#include "sim-net.h"
#include "sim-server.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netdb.h>
#include <config.h>
 
#define BUFFER_SIZE 1024

enum 
{
  DESTROY,
  LAST_SIGNAL
};

struct _SimNetPrivate {
  SimServer   *server;

  gchar net_name[MAX_NET_NAME];
  glong a;
  glong c;
};

static gpointer parent_class = NULL;
static gint sim_net_signals[LAST_SIGNAL] = { 0 };

/* GType Functions */

static void
sim_net_class_init (SimNetClass * class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  parent_class = g_type_class_peek_parent (class);
}

static void
sim_net_instance_init (SimNet *net)
{
  net->_priv = g_new0 (SimNetPrivate, 1);

}

/* Public Methods */

GType
sim_net_get_type (void)
{
  static GType object_type = 0;
 
  if (!object_type)
  {
    static const GTypeInfo type_info = {
              sizeof (SimNetClass),
              NULL,
              NULL,
              (GClassInitFunc) sim_net_class_init,
              NULL,
              NULL,                       /* class data */
              sizeof (SimNet),
              0,                          /* number of pre-allocs */
              (GInstanceInitFunc) sim_net_instance_init,
              NULL                        /* value table */
    };
    
    g_type_init ();
                                                                                                                             
    object_type = g_type_register_static (G_TYPE_OBJECT, "SimNet", &type_info, 0);
  }
                                                                                                                             
  return object_type;
}

/*
 *
 *
 *
 *
 */
SimNet *
sim_net_new (void)
{
  SimNet *net = NULL;

  net = SIM_NET (g_object_new (SIM_TYPE_NET, NULL));

  return net;
}
