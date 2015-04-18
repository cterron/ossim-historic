/**
 *
 *
 */

#include "sim-net-asset.h"
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

struct _SimNetAssetPrivate {
  SimServer   *server;

  struct in_addr ips;
  gchar net_name[MAX_NET_NAME];
  gint mask;
  gint asset;
};

static gpointer parent_class = NULL;
static gint sim_net_asset_signals[LAST_SIGNAL] = { 0 };

/* GType Functions */

static void
sim_net_asset_class_init (SimNetAssetClass * class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  parent_class = g_type_class_peek_parent (class);
}

static void
sim_net_asset_instance_init (SimNetAsset *net_asset)
{
  gchar net_name[MAX_NET_NAME];
  struct in_addr ips;
  gint mask;
  gint asset;
  net_asset->_priv = g_new0 (SimNetAssetPrivate, 1);
}

/* Public Methods */

GType
sim_net_asset_get_type (void)
{
  static GType object_type = 0;
 
  if (!object_type)
  {
    static const GTypeInfo type_info = {
              sizeof (SimNetAssetClass),
              NULL,
              NULL,
              (GClassInitFunc) sim_net_asset_class_init,
              NULL,
              NULL,                       /* class data */
              sizeof (SimNetAsset),
              0,                          /* number of pre-allocs */
              (GInstanceInitFunc) sim_net_asset_instance_init,
              NULL                        /* value table */
    };
    
    g_type_init ();
                                                                                                                             
    object_type = g_type_register_static (G_TYPE_OBJECT, "SimNetAsset", &type_info, 0);
  }
                                                                                                                             
  return object_type;
}

/*
 *
 *
 *
 *
 */
SimNetAsset *
sim_net_asset_new (void)
{
  SimNetAsset *net_asset = NULL;

  net_asset = SIM_NET_ASSET (g_object_new (SIM_TYPE_NET_ASSET, NULL));

  return net_asset;
}
