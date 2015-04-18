/**
 *
 *
 */

#include "sim-host-asset.h"
#include "sim-server.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <netdb.h>
#include <fcntl.h>
#include <config.h>
 
#define BUFFER_SIZE 1024

enum 
{
  DESTROY,
  LAST_SIGNAL
};

struct _SimHostAssetPrivate {
  SimServer   *server;

  struct in_addr ip;
  gint asset;
  gchar sensors[MAX_SENSORS];
};

static gpointer parent_class = NULL;
static gint sim_host_asset_signals[LAST_SIGNAL] = { 0 };

/* GType Functions */

static void
sim_host_asset_class_init (SimHostAssetClass * class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  parent_class = g_type_class_peek_parent (class);
}

static void
sim_host_asset_instance_init (SimHostAsset *host_asset)
{
  host_asset->_priv = g_new0 (SimHostAssetPrivate, 1);

}

/* Public Methods */

GType
sim_host_asset_get_type (void)
{
  static GType object_type = 0;
 
  if (!object_type)
  {
    static const GTypeInfo type_info = {
              sizeof (SimHostAssetClass),
              NULL,
              NULL,
              (GClassInitFunc) sim_host_asset_class_init,
              NULL,
              NULL,                       /* class data */
              sizeof (SimHostAsset),
              0,                          /* number of pre-allocs */
              (GInstanceInitFunc) sim_host_asset_instance_init,
              NULL                        /* value table */
    };
    
    g_type_init ();
                                                                                                                             
    object_type = g_type_register_static (G_TYPE_OBJECT, "SimHostAsset", &type_info, 0);
  }
                                                                                                                             
  return object_type;
}

/*
 *
 *
 *
 *
 */
SimHostAsset *
sim_host_asset_new (void)
{
  SimHostAsset *host_asset = NULL;

  host_asset = SIM_HOST_ASSET (g_object_new (SIM_TYPE_HOST_ASSET, NULL));

  return host_asset;
}
