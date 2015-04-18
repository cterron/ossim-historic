/**
 *
 *
 */

#include "sim-host.h"
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

struct _SimHostPrivate {
  SimServer   *server;

  struct in_addr ip;
  glong a;
  glong c;
};

static gpointer parent_class = NULL;
static gint sim_host_signals[LAST_SIGNAL] = { 0 };

/* GType Functions */

static void
sim_host_class_init (SimHostClass * class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  parent_class = g_type_class_peek_parent (class);
}

static void
sim_host_instance_init (SimHost *host)
{
  host->_priv = g_new0 (SimHostPrivate, 1);

}

/* Public Methods */

GType
sim_host_get_type (void)
{
  static GType object_type = 0;
 
  if (!object_type)
  {
    static const GTypeInfo type_info = {
              sizeof (SimHostClass),
              NULL,
              NULL,
              (GClassInitFunc) sim_host_class_init,
              NULL,
              NULL,                       /* class data */
              sizeof (SimHost),
              0,                          /* number of pre-allocs */
              (GInstanceInitFunc) sim_host_instance_init,
              NULL                        /* value table */
    };
    
    g_type_init ();
                                                                                                                             
    object_type = g_type_register_static (G_TYPE_OBJECT, "SimHost", &type_info, 0);
  }
                                                                                                                             
  return object_type;
}

/*
 *
 *
 *
 *
 */
SimHost *
sim_host_new (void)
{
  SimHost *host = NULL;

  host = SIM_HOST (g_object_new (SIM_TYPE_HOST, NULL));

  return host;
}
