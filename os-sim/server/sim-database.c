/**
 * Database
 *
 */

#include <libgda/libgda.h>

#include "sim-server.h"
#include "sim-database.h"

enum 
{
  DESTROY,
  LAST_SIGNAL
};

struct _SimDatabasePrivate {
  SimServer   *server;

  GdaClient   *client;
};

static gpointer parent_class = NULL;
static gint sim_server_signals[LAST_SIGNAL] = { 0 };

/* GType Functions */

static void
sim_database_class_init (SimDatabaseClass * class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  parent_class = g_type_class_peek_parent (class);
}

static void
sim_database_instance_init (SimDatabase *database)
{
  database->_priv = g_new0 (SimDatabasePrivate, 1);

}

/* Public Methods */

GType
sim_database_get_type (void)
{
  static GType object_type = 0;
 
  if (!object_type)
  {
    static const GTypeInfo type_info = {
              sizeof (SimDatabaseClass),
              NULL,
              NULL,
              (GClassInitFunc) sim_database_class_init,
              NULL,
              NULL,                       /* class data */
              sizeof (SimDatabase),
              0,                          /* number of pre-allocs */
              (GInstanceInitFunc) sim_database_instance_init,
              NULL                        /* value table */
    };
    
    g_type_init ();
                                                                                                                             
    object_type = g_type_register_static (G_TYPE_OBJECT, "SimDatabase", &type_info, 0);
  }
                                                                                                                             
  return object_type;
}

/*
 *
 *
 */
SimDatabase *
sim_database_new (void)
{
  SimDatabase *database = NULL;

  database = SIM_DATABASE (g_object_new (SIM_TYPE_DATABASE, NULL));

  database->_priv->client = gda_client_new ();

  return database;
}
