/**
 *
 *
 */

#include <sys/types.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <netdb.h>
#include <fcntl.h>
#include <config.h>

#include "sim-database.h"
#include "sim-host-asset.h"
 
enum 
{
  DESTROY,
  LAST_SIGNAL
};

struct _SimHostAssetPrivate {
  struct in_addr   ip;

  gint             asset;
  gchar           *sensors;
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

/*
 *
 *
 *
 */
GList*
sim_host_asset_load_from_db(GObject *db)
{
  SimHostAsset  *host_asset;
  GdaDataModel  *dm;
  GdaDataModel  *dm1;
  GdaValue      *value;
  GList         *host_assets = NULL;
  GList         *list = NULL;
  GList         *list2 = NULL;
  GList         *node = NULL; 
  GList         *node2 = NULL;
  gchar         *query2;
  gint           row_id;
  gint           row_id1;

  gchar *query = "select * from host";

  g_return_if_fail (db != NULL);
  g_return_if_fail (SIM_IS_DATABASE (db));

  /* List of hosts */
  list = sim_database_execute_command (SIM_DATABASE (db), query);
  if (list != NULL)
    {
      for (node = g_list_first (list); node != NULL; node = g_list_next (node))
	{
	  dm = (GdaDataModel *) node->data;
	  if (dm == NULL)
	    {
	      g_message ("HOSTS DATA MODEL ERROR");
	    }
	  else
	    {
	      for (row_id = 0; row_id < gda_data_model_get_n_rows (dm); row_id++)
		{
		  gchar *str;

		  /* New host */
		  host_asset  = sim_host_asset_new ();

		  value = (GdaValue *) gda_data_model_get_value_at (dm, 0, row_id);
		  str = gda_value_stringify (value);
		  inet_aton(str, &host_asset->_priv->ip);

		  value = (GdaValue *) gda_data_model_get_value_at (dm, 2, row_id);
		  host_asset->_priv->asset = gda_value_get_smallint (value);

		  query2 = g_strdup_printf ("select * from port_group_reference where port_group_name = '%s'", str);

		  list2 = sim_database_execute_command (SIM_DATABASE (db), query2);
		  if (list2 != NULL)
		    {
		      for (node2 = g_list_first (list2); node2 != NULL; node2 = g_list_next (node2))
			{
			  dm1 = (GdaDataModel *) node2->data;
			  if (dm1 == NULL)
			    {
			      g_message ("POLICIES DATA MODEL ERROR 1");
			    }
			  else
			    {
			      for (row_id1 = 0; row_id1 < gda_data_model_get_n_rows (dm1); row_id1++)
				{
				  /* Set ip */
				  value = (GdaValue *) gda_data_model_get_value_at (dm1, 1, row_id1);
				  host_asset->_priv->sensors = gda_value_stringify (value);
				}
			      
			      g_object_unref(dm1);
			    }
			}
		    }
		  host_assets = g_list_append (host_assets, host_asset);

		  g_free (str);
		}

	      g_object_unref(dm);
	    }
	}
    }
  else
    {
      g_message ("HOSTS LIST ERROR");
    }

  return host_assets;
}

/*
 *
 *
 *
 */
struct in_addr
sim_host_asset_get_ip (SimHostAsset *host_asset)
{
  g_return_if_fail (host_asset != NULL);
  g_return_if_fail (SIM_IS_HOST_ASSET (host_asset));

  return host_asset->_priv->ip;
}

/*
 *
 *
 *
 */
gint
sim_host_asset_get_asset (SimHostAsset *host_asset)
{
  g_return_val_if_fail (host_asset != NULL, 0);
  g_return_val_if_fail (SIM_IS_HOST_ASSET (host_asset), 0);

  return host_asset->_priv->asset;
}
