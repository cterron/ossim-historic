/**
 *
 *
 */

#include <sys/types.h>
#include <sys/socket.h>
//#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netdb.h>
#include <config.h>

#include "sim-database.h"
#include "sim-host.h"
 
enum 
{
  DESTROY,
  LAST_SIGNAL
};

struct _SimHostPrivate {
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

  host->_priv->c = 0;
  host->_priv->a = 0;
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
 */
SimHost *
sim_host_new (void)
{
  SimHost *host = NULL;

  host = SIM_HOST (g_object_new (SIM_TYPE_HOST, NULL));

  return host;
}

/*
 *
 *
 *
 */
GList*
sim_host_load_from_db(GObject *db)
{
  SimHost *host;
  GdaDataModel *dm;
  GdaValue *value;
  GList *hosts = NULL;
  GList *list = NULL;
  GList *node = NULL; 
  gint row_id;

  gchar *query = "select * from host_qualification";

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
		  gchar *sip;

		  /* New host */
		  host  = sim_host_new ();

		  /* Set ip*/
		  value = (GdaValue *) gda_data_model_get_value_at (dm, 0, row_id);
		  sip = gda_value_stringify (value);
		  inet_aton(sip, &host->_priv->ip);

		  /* Set c */
		  value = (GdaValue *) gda_data_model_get_value_at (dm, 1, row_id);
		  host->_priv->c = gda_value_get_integer (value);

		  /* Set a */
		  value = (GdaValue *) gda_data_model_get_value_at (dm, 2, row_id);
		  host->_priv->a = gda_value_get_integer (value);

		  /* Added host */
		  hosts = g_list_append (hosts, host);

		  g_free (sip);
		}

	      g_object_unref(dm);
	    }
	}
    }
  else
    {
      g_message ("HOSTS LIST ERROR");
    }

  return hosts;
}


/*
 *
 *
 *
 */
struct in_addr
sim_host_get_ip (SimHost         *host)
{
  return host->_priv->ip;
}

/*
 *
 *
 *
 */
void
sim_host_set_ip (SimHost         *host,
		 struct in_addr   ip)
{
  g_return_if_fail (host != NULL);
  g_return_if_fail (SIM_IS_HOST (host));

  host->_priv->ip = ip;
}

/*
 *
 *
 *
 */
gint
sim_host_get_c (SimHost  *host)
{
  g_return_val_if_fail (host != NULL, 0);
  g_return_val_if_fail (SIM_IS_HOST (host), 0);

  return host->_priv->c;
}

/*
 *
 *
 *
 */
void
sim_host_set_c (SimHost  *host,
		gint      c)
{
  g_return_if_fail (host != NULL);
  g_return_if_fail (SIM_IS_HOST (host));

  host->_priv->c = c;
}

/*
 *
 *
 *
 */
gint
sim_host_get_a (SimHost  *host)
{
  g_return_val_if_fail (host != NULL, 0);
  g_return_val_if_fail (SIM_IS_HOST (host), 0);

  return host->_priv->a;
}

/*
 *
 *
 *
 */
void
sim_host_set_a (SimHost  *host,
		gint      a)
{
  g_return_if_fail (host != NULL);
  g_return_if_fail (SIM_IS_HOST (host));

  host->_priv->a = a;
}


/*
 *
 *
 *
 */
void
sim_host_set_recovery (SimHost  *host,
		       gint      recovery)
{
  g_return_if_fail (host != NULL);
  g_return_if_fail (SIM_IS_HOST (host));
  g_return_if_fail (recovery > 0);

  if (host->_priv->c > recovery)
    host->_priv->c -= recovery;
  else
    host->_priv->c = 0;

  if (host->_priv->a > recovery)
    host->_priv->a -= recovery;
  else
    host->_priv->a = 0;
}
