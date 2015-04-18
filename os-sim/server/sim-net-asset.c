/**
 *
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <netdb.h>
#include <config.h>
 
#include "sim-database.h"
#include "sim-net-asset.h"

enum 
{
  DESTROY,
  LAST_SIGNAL
};

struct _SimNetAssetPrivate {
  gchar           *net_name;
  struct in_addr   ip;
  gint             mask;
  gint             asset;
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
  net_asset->_priv = g_new0 (SimNetAssetPrivate, 1);

  net_asset->_priv->net_name;
  net_asset->_priv->ip;
  net_asset->_priv->mask;
  net_asset->_priv->asset;
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

/*
 *
 *
 *
 */
GList*
sim_net_asset_load_from_db(GObject *db)
{
  SimNetAsset   *net_asset;
  GdaDataModel  *dm;
  GdaValue      *value;
  GList         *net_assets = NULL;
  GList         *list = NULL;
  GList         *node = NULL; 
  gint           row_id;

  gchar *query = "select * from net";

  g_return_if_fail (db != NULL);
  g_return_if_fail (SIM_IS_DATABASE (db));

  /* List of nets */
  list = sim_database_execute_command (SIM_DATABASE (db), query);
  if (list != NULL)
    {
      for (node = g_list_first (list); node != NULL; node = g_list_next (node))
	{
	  dm = (GdaDataModel *) node->data;
	  if (dm == NULL)
	    {
	      g_message ("NETS ASSETS DATA MODEL ERROR");
	    }
	  else
	    {
	      for (row_id = 0; row_id < gda_data_model_get_n_rows (dm); row_id++)
		{
		  /* New net */
		  net_asset  = sim_net_asset_new ();

		  /* Set net_name*/
		  value = (GdaValue *) gda_data_model_get_value_at (dm, 0, row_id);
		  net_asset->_priv->net_name = gda_value_stringify (value);

		  /* Set asset */
		  value = (GdaValue *) gda_data_model_get_value_at (dm, 2, row_id);
		  net_asset->_priv->asset = gda_value_get_integer (value);

		  /* Added net */
		  net_assets = g_list_append (net_assets, net_asset);
		}

	      g_object_unref(dm);
	    }
	}
    }
  else
    {
      g_message ("NETS ASSETS LIST ERROR");
    }

  return net_assets;
}
