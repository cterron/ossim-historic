/**
 *
 *
 *
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <config.h>
#include <libgda/libgda.h>

#include "sim-config.h"
#include "sim-database.h"
#include "sim-signature.h"
#include "sim-policy.h"
#include "sim-host.h"
#include "sim-host-asset.h"
#include "sim-net.h"
#include "sim-net-asset.h"
#include "sim-message.h"
#include "sim-scheduler.h"
#include "sim-syslog.h"
#include "sim-organizer.h"
#include "sim-server.h"

enum 
{
  DESTROY,
  LAST_SIGNAL
};

struct _SimServerPrivate {
  SimConfig       *config;
  SimDatabase     *database;

  GMainLoop       *loop;

  GQueue          *messages;

  gint             recovery;

  GList           *signatures;
  GList           *policies;
  GList           *hosts;
  GList           *host_assets;
  GList           *nets;
  GList           *net_assets;

  GNode           *sig_root;

};

typedef struct {
  gint      id;
  gint      subtype;
  GNode    *node;
} SimSignatureData;

static gpointer sim_server_scheduler (gpointer data);
static gpointer sim_server_syslog (gpointer data);
static gpointer sim_server_organizer (gpointer data);

static gpointer parent_class = NULL;
static gint sim_server_signals[LAST_SIGNAL] = { 0 };

/* GType Functions */

static void
sim_server_class_init (SimServerClass * class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  parent_class = g_type_class_peek_parent (class);
}

static void
sim_server_instance_init (SimServer * server)
{
  server->_priv = g_new0 (SimServerPrivate, 1);

  server->_priv->database = NULL;
  server->_priv->config = NULL;
  
  server->_priv->loop = NULL;
  
  server->_priv->recovery = 1;

  server->_priv->messages = g_queue_new ();
  
  server->_priv->signatures = NULL;
  server->_priv->policies = NULL;
  server->_priv->hosts = NULL;
  server->_priv->host_assets = NULL;
  server->_priv->nets = NULL;
  server->_priv->net_assets = NULL;

  server->_priv->sig_root = g_node_new (NULL);
}

/* Public Methods */

GType
sim_server_get_type (void)
{
  static GType object_type = 0;
 
  if (!object_type)
  {
    static const GTypeInfo type_info = {
              sizeof (SimServerClass),
              NULL,
              NULL,
              (GClassInitFunc) sim_server_class_init,
              NULL,
              NULL,                       /* class data */
              sizeof (SimServer),
              0,                          /* number of pre-allocs */
              (GInstanceInitFunc) sim_server_instance_init,
              NULL                        /* value table */
    };
    
    g_type_init ();
                                                                                                                             
    object_type = g_type_register_static (G_TYPE_OBJECT, "SimServer", &type_info, 0);
  }
                                                                                                                             
  return object_type;
}

SimServer*
sim_server_new (SimConfig *config)
{
  SimServer *server = NULL;

  g_return_val_if_fail (config != NULL, NULL);
  g_return_val_if_fail (SIM_IS_CONFIG (config), NULL);

  server = SIM_SERVER (g_object_new (SIM_TYPE_SERVER, NULL));
  server->_priv->config = config;

  return server;
}

/**
 * sim_server_run
 *
 *
 */
void
sim_server_run (SimServer *server)
{
  GThread  *thread;
  gchar    *database;
  gchar    *username;
  gchar    *password;
 
  g_return_if_fail (server != NULL);
  g_return_if_fail (SIM_IS_SERVER (server));
  g_message ("sim_server_run thread");

  database = sim_config_get_property_value (server->_priv->config,
					    SIM_CONFIG_PROPERTY_TYPE_DATABASE);
  username = sim_config_get_property_value (server->_priv->config,
					    SIM_CONFIG_PROPERTY_TYPE_USERNAME);
  password = sim_config_get_property_value (server->_priv->config,
					    SIM_CONFIG_PROPERTY_TYPE_PASSWORD);

  /* Database init */
  server->_priv->database = sim_database_new (database, username, password);

  /* Load the configuration from the data base */
  sim_server_db_load_config (server);

  /* Sets signatures */
  server->_priv->signatures = sim_signature_load_from_db (G_OBJECT (server->_priv->database), 
							  server->_priv->sig_root);

  /* Sets policies */
  server->_priv->policies = sim_policy_load_from_db (G_OBJECT (server->_priv->database));

  /* Sets hosts */
  server->_priv->hosts = sim_host_load_from_db (G_OBJECT (server->_priv->database));

  /* Sets host_assets */
  server->_priv->host_assets = sim_host_asset_load_from_db (G_OBJECT (server->_priv->database));

  /* Sets nets */
  server->_priv->nets = sim_server_db_get_nets (server);

  /* Sets net_assets */
  server->_priv->net_assets = sim_net_asset_load_from_db (G_OBJECT (server->_priv->database));

  /* Syslog Thread */
  thread = g_thread_create(sim_server_syslog, server, TRUE, NULL);
  g_return_if_fail (thread != NULL);

  /* Organizer Thread */
  thread = g_thread_create(sim_server_organizer, server, TRUE, NULL);
  g_return_if_fail (thread != NULL);

  /* Scheduler Thread */
  thread = g_thread_create(sim_server_scheduler, server, TRUE, NULL);
  g_return_if_fail (thread != NULL);

  /* Main Loop */

  server->_priv->loop = g_main_loop_new (NULL, FALSE);
  g_main_loop_run (server->_priv->loop);
}

/*****************************************
 * sim_server_scheduler:
 *
 *   arguments:
 *
 *   results:
 *****************************************/

static gpointer
sim_server_scheduler (gpointer data)
{
  SimServer    *server = (SimServer *) data;
  SimScheduler *scheduler;
 
  g_return_if_fail (server != NULL);
  g_return_if_fail (SIM_IS_SERVER (server));
  g_message ("sim_server_scheduler thread");

  scheduler = sim_scheduler_new ();
  sim_scheduler_set_server(scheduler, server);
  sim_scheduler_run (scheduler);

  return NULL;
}

/*****************************************
 * sim_server_syslog:
 *
 *   arguments:
 *
 *   results:
 *****************************************/

static gpointer
sim_server_syslog (gpointer data)
{
  SimServer    *server = (SimServer *) data;
  SimSyslog    *syslog;
  gchar        *log_file;
 
  g_return_if_fail (server != NULL);
  g_return_if_fail (SIM_IS_SERVER (server));
  g_message ("sim_server_syslog thread");

  log_file = sim_config_get_property_value (server->_priv->config,
					    SIM_CONFIG_PROPERTY_TYPE_LOG_FILE);

  syslog = sim_syslog_new (log_file);

  g_assert (syslog != NULL);

  sim_syslog_set_server(syslog, server);
  sim_syslog_run (syslog);

  return NULL;
}

/**
 * sim_server_organizer:
 *
 *   arguments:
 *
 *   results:
 */

static gpointer
sim_server_organizer (gpointer data)
{
  SimServer    *server = (SimServer *) data;
  SimOrganizer *organizer;
 
  g_return_if_fail (server != NULL);
  g_return_if_fail (SIM_IS_SERVER (server));
  g_message ("sim_server_organizer thread");

  organizer = sim_organizer_new ();
  sim_organizer_set_server(organizer, server);
  sim_organizer_run (organizer);

  return NULL;
}

/**
 *
 *
 *
 */
GObject*
sim_server_get_config (SimServer *server)
{
  g_return_val_if_fail (server != NULL, NULL);
  g_return_val_if_fail (SIM_IS_SERVER (server), NULL);
  g_return_val_if_fail (server->_priv->config != NULL, NULL);

  return G_OBJECT (server->_priv->config);
}

/**
 *
 *
 *
 */
void
sim_server_set_config (SimServer *server,
		       GObject   *config)
{
  g_return_if_fail (server != NULL);
  g_return_if_fail (SIM_IS_SERVER (server));
  g_return_if_fail (config != NULL);
  g_return_if_fail (SIM_IS_CONFIG (config));

  server->_priv->config = SIM_CONFIG (config);
}

/**
 *
 *
 *
 */
void
sim_server_db_load_config (SimServer *server)
{
  GdaDataModel  *dm;
  GdaValue      *value;
  GList         *list = NULL;
  GList         *node = NULL;
  gchar         *query;
  gint           row_id;

  g_return_if_fail (server != NULL);
  g_return_if_fail (SIM_IS_SERVER (server));
  g_return_if_fail (server->_priv->database != NULL);

  query = g_strdup_printf ("SELECT recovery FROM conf");

  /* List of nets */
  list = sim_database_execute_command (server->_priv->database, query);
  if (list != NULL)
    {
      for (node = g_list_first (list); node != NULL; node = g_list_next (node))
	{
	  dm = (GdaDataModel *) node->data;
	  if (dm == NULL)
	    {
	      g_message ("RECOVERY DATA MODEL ERROR");
	    }
	  else
	    {
	      for (row_id = 0; row_id < gda_data_model_get_n_rows (dm); row_id++)
		{
		  /* Recovery */
		  value = (GdaValue *) gda_data_model_get_value_at (dm, 0, row_id);
		  server->_priv->recovery = gda_value_get_integer (value);
		}

	      g_object_unref(dm);
	    }
	}
    }
  else
    {
      g_message ("RECOVERY LIST ERROR");
    }

  g_free (query);

}

/**
 *
 *
 *
 */
gint
sim_server_get_recovery (SimServer *server)
{
  g_return_val_if_fail (server != NULL, 0);
  g_return_val_if_fail (SIM_IS_SERVER (server), 0);

  return server->_priv->recovery;
}

/**
 *
 *
 *
 */
void
sim_server_set_recovery (SimServer *server,
			 gint       recovery)
{
  g_return_if_fail (server != NULL);
  g_return_if_fail (SIM_IS_SERVER (server));
  g_return_if_fail (recovery > 0);

  server->_priv->recovery = recovery;
}

/**
 *
 *
 *
 */
void
sim_server_push_tail_messages (SimServer *server, GObject *message)
{
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_if_fail (server != NULL);
  g_return_if_fail (SIM_IS_SERVER (server));
  g_return_if_fail (message != NULL);
  g_return_if_fail (SIM_IS_MESSAGE (message));

  g_static_mutex_lock (&mutex);
  g_queue_push_tail (server->_priv->messages, message);
  g_static_mutex_unlock (&mutex);
}

/**
 *
 *
 *
 */
GObject*
sim_server_pop_head_messages (SimServer *server)
{
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;
  GObject *msg;

  g_return_val_if_fail (server != NULL, NULL);
  g_return_val_if_fail (SIM_IS_SERVER (server), NULL);

  g_static_mutex_lock (&mutex);
  msg = g_queue_pop_head (server->_priv->messages);
  g_static_mutex_unlock (&mutex);

  return msg;
}

/*
 *
 *
 *
 */
gint
sim_server_get_messages_num (SimServer *server)
{
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;
  gint num;

  g_return_val_if_fail (server != NULL, 0);
  g_return_val_if_fail (SIM_IS_SERVER (server), 0);

  g_static_mutex_lock (&mutex);
  num = server->_priv->messages->length;
  g_static_mutex_unlock (&mutex);

  return num;
}

/*
 * Policies Functions
 */

/*
 *
 *
 *
 */
void
sim_server_add_policy (SimServer *server,
		       GObject   *policy)
{
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_if_fail (server != NULL);
  g_return_if_fail (SIM_IS_SERVER (server));
  g_return_if_fail (policy != NULL);
  g_return_if_fail (SIM_IS_POLICY (policy));

  g_static_mutex_lock (&mutex);
  server->_priv->policies = g_list_append (server->_priv->policies, policy);
  g_static_mutex_unlock (&mutex);
}

/*
 *
 *
 *
 */
void
sim_server_remove_policy (SimServer *server,
			  GObject   *policy)
{
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_if_fail (server != NULL);
  g_return_if_fail (SIM_IS_SERVER (server));
  g_return_if_fail (policy != NULL);
  g_return_if_fail (SIM_IS_POLICY (policy));

  g_static_mutex_lock (&mutex);
  server->_priv->policies = g_list_remove (server->_priv->policies, policy);
  g_static_mutex_unlock (&mutex);
}

/*
 *
 *
 *
 */
GList*
sim_server_get_policies (SimServer *server)
{
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;
  GList *list = NULL;

  g_return_val_if_fail (server != NULL, NULL);
  g_return_val_if_fail (SIM_IS_SERVER (server), NULL);

  g_static_mutex_lock (&mutex);
  list = g_list_copy (server->_priv->policies);
  g_static_mutex_unlock (&mutex);

  return list;
}

/*
 *
 *
 *
 */
GObject*
sim_server_get_policy_by_match (SimServer        *server,
				gint              date,
				gchar            *src_ip,
				gchar            *dst_ip,
				gint              port,
				SimProtocolType   protocol,
				gchar            *signature)
{
  SimPolicy  *policy = NULL;
  gint        i;

  g_return_val_if_fail (server != NULL, NULL);
  g_return_val_if_fail (SIM_IS_SERVER (server), NULL);
  g_return_val_if_fail (src_ip != NULL, NULL);
  g_return_val_if_fail (dst_ip != NULL, NULL);
  g_return_val_if_fail (signature != NULL, NULL);

  for (i = 0; i < g_list_length(server->_priv->policies); i++)
    {
      policy = (SimPolicy *) g_list_nth_data (server->_priv->policies, i);

      if (sim_policy_match (policy, date, src_ip, dst_ip, port, protocol, signature))
	{
	  return G_OBJECT (policy);
	}
    }

  return NULL;
}

/*
 * Hosts Functions
 */

/*
 *
 *
 *
 */
void
sim_server_add_host (SimServer *server,
		       GObject   *host)
{
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_if_fail (server != NULL);
  g_return_if_fail (SIM_IS_SERVER (server));
  g_return_if_fail (host != NULL);
  g_return_if_fail (SIM_IS_HOST (host));

  g_static_mutex_lock (&mutex);
  server->_priv->hosts = g_list_append (server->_priv->hosts, host);
  g_static_mutex_unlock (&mutex);
}

/*
 *
 *
 *
 */
void
sim_server_remove_host (SimServer *server,
			  GObject   *host)
{
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_if_fail (server != NULL);
  g_return_if_fail (SIM_IS_SERVER (server));
  g_return_if_fail (host != NULL);
  g_return_if_fail (SIM_IS_HOST (host));

  g_static_mutex_lock (&mutex);
  server->_priv->hosts = g_list_remove (server->_priv->hosts, host);
  g_static_mutex_unlock (&mutex);
}

/*
 *
 *
 *
 */
GList*
sim_server_get_hosts (SimServer *server)
{
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;
  GList *list = NULL;

  g_return_val_if_fail (server != NULL, NULL);
  g_return_val_if_fail (SIM_IS_SERVER (server), NULL);

  g_static_mutex_lock (&mutex);
  list = g_list_copy (server->_priv->hosts);
  g_static_mutex_unlock (&mutex);

  return list;
}

/*
 *
 *
 *
 */
GObject*
sim_server_get_host_by_ip (SimServer *server,
			   struct in_addr ip)
{
  SimHost         *host = NULL;
  gboolean         found = FALSE;
  struct in_addr   tmp;
  gint             i;

  g_return_val_if_fail (server != NULL, NULL);
  g_return_val_if_fail (SIM_IS_SERVER (server), NULL);

  for (i = 0; i < g_list_length(server->_priv->hosts); i++)
    {
      host = (SimHost *) g_list_nth_data (server->_priv->hosts, i);

      tmp = sim_host_get_ip (host);

      if (tmp.s_addr == ip.s_addr) 
	{
	  found = TRUE;
	  break;
	}
    }

  if (!found)
    return NULL;

  return G_OBJECT (host);
}

/*
 *
 *
 *
 */
void
sim_server_set_hosts_recovery (SimServer *server,
			      gint       recovery)
{
  SimHost         *host = NULL;
  GList           *removes = NULL;
  gint             i;
  gint             c;
  gint             a;

  g_return_if_fail (server != NULL);
  g_return_if_fail (SIM_IS_SERVER (server));
  g_return_if_fail (recovery > 0);

  for (i = 0; i < g_list_length (server->_priv->hosts); i++)
    {
      host = (SimHost *) g_list_nth_data (server->_priv->hosts, i);

      sim_host_set_recovery (host, recovery); /* Update Memory */

      c = sim_host_get_c (host);
      a = sim_host_get_a (host);

      if (c == 0 && a == 0)
	{
	  removes = g_list_append (removes, host);
	}
      else
	{
	  sim_server_db_update_host (server, G_OBJECT (host)); /* Update DB */
	}
    }

  for (i = 0; i < g_list_length (removes); i++)
    {
      host = (SimHost *) g_list_nth_data (removes, i);
      sim_server_remove_host (server, G_OBJECT (host));
      sim_server_db_delete_host (server, G_OBJECT (host)); /* Delete DB */
    }
}

/*
 *
 *
 *
 */
gint
sim_server_db_insert_host (SimServer *server,
			   GObject   *host)
{
  gchar           *insert; 
  gint             ret;

  g_return_if_fail (server != NULL);
  g_return_if_fail (SIM_IS_SERVER (server));
  g_return_if_fail (host != NULL);
  g_return_if_fail (SIM_IS_HOST (host));
  g_return_if_fail (server->_priv->database !=  NULL);

  insert = g_strdup_printf ("INSERT INTO host_qualification VALUES ('%s', %d, %d)",
			    inet_ntoa (sim_host_get_ip (SIM_HOST (host))), 
			    sim_host_get_c (SIM_HOST (host)), 
			    sim_host_get_a (SIM_HOST (host)));

  ret = sim_database_execute_no_query (server->_priv->database,
				       insert);

  g_message ("%s, %d", insert, ret);

  g_free (insert);

  return ret;
}

/*
 *
 *
 *
 */
gint
sim_server_db_update_host (SimServer *server,
			   GObject   *host)
{
  gchar           *update;
  gint             ret;

  g_return_if_fail (server != NULL);
  g_return_if_fail (SIM_IS_SERVER (server));
  g_return_if_fail (host != NULL);
  g_return_if_fail (SIM_IS_HOST (host));
  g_return_if_fail (server->_priv->database !=  NULL);

  update = g_strdup_printf ("UPDATE host_qualification SET compromise = %d, attack = %d WHERE host_ip = '%s'",
			    sim_host_get_c (SIM_HOST (host)),
			    sim_host_get_a (SIM_HOST (host)),
			    inet_ntoa (sim_host_get_ip (SIM_HOST (host))));

  ret = sim_database_execute_no_query (server->_priv->database,
				 update);

  g_free (update);

  return ret;
}

/*
 *
 *
 *
 */
gint
sim_server_db_delete_host (SimServer *server,
			   GObject   *host)
{
  gchar           *delete;
  struct in_addr   ip;
  gint             ret;

  g_return_if_fail (server != NULL);
  g_return_if_fail (SIM_IS_SERVER (server));
  g_return_if_fail (host != NULL);
  g_return_if_fail (SIM_IS_HOST (host));
  g_return_if_fail (server->_priv->database !=  NULL);

  ip = sim_host_get_ip (SIM_HOST (host));

  delete = g_strdup_printf ("DELETE FROM host_qualification WHERE host_ip = '%s'",
			    inet_ntoa (ip));

  ret = sim_database_execute_no_query (server->_priv->database,
				 delete);

  g_free (delete);

  return ret;
}

/*
 * Host Assets Functions
 */

/*
 *
 *
 *
 */
void
sim_server_add_host_asset (SimServer *server,
			   GObject   *host_asset)
{
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_if_fail (server != NULL);
  g_return_if_fail (SIM_IS_SERVER (server));
  g_return_if_fail (host_asset != NULL);
  g_return_if_fail (SIM_IS_HOST_ASSET (host_asset));

  g_static_mutex_lock (&mutex);
  server->_priv->host_assets = g_list_append (server->_priv->host_assets, host_asset);
  g_static_mutex_unlock (&mutex);
}

/*
 *
 *
 *
 */
void
sim_server_remove_host_asset (SimServer *server,
			      GObject   *host_asset)
{
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_if_fail (server != NULL);
  g_return_if_fail (SIM_IS_SERVER (server));
  g_return_if_fail (host_asset != NULL);
  g_return_if_fail (SIM_IS_HOST_ASSET (host_asset));

  g_static_mutex_lock (&mutex);
  server->_priv->host_assets = g_list_remove (server->_priv->host_assets, host_asset);
  g_static_mutex_unlock (&mutex);
}

/*
 *
 *
 *
 */
GList*
sim_server_get_host_assets (SimServer *server)
{
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;
  GList *list = NULL;

  g_return_val_if_fail (server != NULL, NULL);
  g_return_val_if_fail (SIM_IS_SERVER (server), NULL);

  g_static_mutex_lock (&mutex);
  list = g_list_copy (server->_priv->host_assets);
  g_static_mutex_unlock (&mutex);

  return list;
}

/*
 *
 *
 *
 */
GObject*
sim_server_get_host_asset_by_ip (SimServer *server,
				 struct in_addr ip)
{
  SimHostAsset    *host_asset = NULL;
  struct in_addr   tmp;
  gboolean         found = FALSE;
  gint             i;

  g_return_val_if_fail (server != NULL, NULL);
  g_return_val_if_fail (SIM_IS_SERVER (server), NULL);

  for (i = 0; i < g_list_length(server->_priv->host_assets); i++)
    {
      host_asset = (SimHostAsset *) g_list_nth_data (server->_priv->host_assets, i);

      tmp = sim_host_asset_get_ip (host_asset);

      if (tmp.s_addr == ip.s_addr) 
	{
	  found = TRUE;
	  break;
	}
    }

  if (!found)
    return NULL;

  return G_OBJECT (host_asset);
}

/*
 * Nets Functions
 */
/*
 *
 *
 *
 */
void
sim_server_add_net (SimServer *server,
		       GObject   *net)
{
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_if_fail (server != NULL);
  g_return_if_fail (SIM_IS_SERVER (server));
  g_return_if_fail (net != NULL);
  g_return_if_fail (SIM_IS_NET (net));

  g_static_mutex_lock (&mutex);
  server->_priv->nets = g_list_append (server->_priv->nets, net);
  g_static_mutex_unlock (&mutex);
}

/*
 *
 *
 *
 */
void
sim_server_remove_net (SimServer *server,
			  GObject   *net)
{
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_if_fail (server != NULL);
  g_return_if_fail (SIM_IS_SERVER (server));
  g_return_if_fail (net != NULL);
  g_return_if_fail (SIM_IS_NET (net));

  g_static_mutex_lock (&mutex);
  server->_priv->nets = g_list_remove (server->_priv->nets, net);
  g_static_mutex_unlock (&mutex);
}

/*
 *
 *
 *
 */
GList*
sim_server_get_nets (SimServer *server)
{
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;
  GList *list = NULL;

  g_return_val_if_fail (server != NULL, NULL);
  g_return_val_if_fail (SIM_IS_SERVER (server), NULL);

  g_static_mutex_lock (&mutex);
  list = g_list_copy (server->_priv->nets);
  g_static_mutex_unlock (&mutex);

  return list;
}

/*
 *
 *
 *
 */
GList*
sim_server_get_nets_by_host (SimServer *server,
			     GObject   *host)
{
  SimNet  *net = NULL;
  GList   *list = NULL;
  gchar   *ip;
  gint     i;

  g_return_val_if_fail (server != NULL, NULL);
  g_return_val_if_fail (SIM_IS_SERVER (server), NULL);
  g_return_val_if_fail (host != NULL, NULL);
  g_return_val_if_fail (SIM_IS_HOST (host), NULL);

  for (i = 0; i < g_list_length(server->_priv->nets); i++)
    {
      net = (SimNet *) g_list_nth_data (server->_priv->nets, i);

      if (sim_net_has_host (net, host))
	{
	  list = g_list_append (list, net);
	}
    }

  return list;
}

/*
 *
 *
 *
 */
void
sim_server_set_nets_recovery (SimServer *server,
			      gint       recovery)
{
  SimNet         *net = NULL;
  gint             i;

  g_return_if_fail (server != NULL);
  g_return_if_fail (SIM_IS_SERVER (server));
  g_return_if_fail (recovery > 0);

  for (i = 0; i < g_list_length(server->_priv->nets); i++)
    {
      net = (SimNet *) g_list_nth_data (server->_priv->nets, i);

      sim_net_set_recovery (net, recovery); /* Update Memory */
      sim_server_db_update_net (server, G_OBJECT (net)); /* Update DB */
    }
}

/*
 *
 *
 *
 */
GList*
sim_server_db_get_nets (SimServer *server)
{
  SimNet        *net;
  SimHost       *host;
  GdaDataModel  *dm;
  GdaDataModel  *dm1;
  GdaValue      *value;
  GList         *nets = NULL;
  GList         *list = NULL;
  GList         *list1 = NULL;
  GList         *node = NULL;
  GList         *node1 = NULL;
  gchar         *query;
  gchar         *query1;
  gint           row_id;
  gint           row_id1;
  gchar         *name;
  gint           c;
  gint           a;

  g_return_val_if_fail (server != NULL, NULL);
  g_return_val_if_fail (SIM_IS_SERVER (server), NULL);
  g_return_val_if_fail (server->_priv->database != NULL, NULL);

  query = g_strdup_printf ("select * from net_qualification");

  /* List of nets */
  list = sim_database_execute_command (server->_priv->database, query);
  if (list != NULL)
    {
      for (node = g_list_first (list); node != NULL; node = g_list_next (node))
	{
	  dm = (GdaDataModel *) node->data;
	  if (dm == NULL)
	    {
	      g_message ("NETS DATA MODEL ERROR");
	    }
	  else
	    {
	      for (row_id = 0; row_id < gda_data_model_get_n_rows (dm); row_id++)
		{
		  /* Set ip*/
		  value = (GdaValue *) gda_data_model_get_value_at (dm, 0, row_id);
		  name = gda_value_stringify (value);

		  /* Set c */
		  value = (GdaValue *) gda_data_model_get_value_at (dm, 1, row_id);
		  c = gda_value_get_integer (value);

		  /* Set a */
		  value = (GdaValue *) gda_data_model_get_value_at (dm, 2, row_id);
		  a = gda_value_get_integer (value);

		  /* New net */
		  net  = sim_net_new (name, c, a);

		  query1 = g_strdup_printf ("SELECT host_ip FROM net_host_reference WHERE net_name = '%s'",
					    name);

		  list1 = sim_database_execute_command (server->_priv->database, query1);
		  if (list1 != NULL)
		    {
		      for (node1 = g_list_first (list1); node1 != NULL; node1 = g_list_next (node1))
			{
			  dm1 = (GdaDataModel *) node1->data;
			  if (dm1 == NULL)
			    {
			      g_message ("NETS HOST DATA MODEL ERROR");
			    }
			  else
			    {
			      for (row_id1 = 0; row_id1 < gda_data_model_get_n_rows (dm1); row_id1++)
				{
				  gchar *ip;

				  value = (GdaValue *) gda_data_model_get_value_at (dm1, 0, row_id1);
				  ip = gda_value_stringify (value);

				  /* Add host to the net */
				  sim_net_add_host_ip (net, ip);
				}

			      g_object_unref(dm1);
			    }
			}
		    }
		  else
		    {
		      g_message ("NETS HOST LIST ERROR %s", query1);
		    }

		  /* Added net */
		  nets = g_list_append (nets, net);

		  g_free (query1);
		}

	      g_object_unref(dm);
	    }
	}
    }
  else
    {
      g_message ("NETS LIST ERROR");
    }

  g_free (query);

  return nets;
}

/*
 *
 *
 *
 */
gint
sim_server_db_insert_net (SimServer *server,
			  GObject   *net)
{
  gchar           *insert;
  gint             ret;

  g_return_if_fail (server != NULL);
  g_return_if_fail (SIM_IS_SERVER (server));
  g_return_if_fail (net != NULL);
  g_return_if_fail (SIM_IS_NET (net));
  g_return_if_fail (server->_priv->database !=  NULL);

  insert = g_strdup_printf ("INSERT INTO net_qualification VALUES ('%s', %d, %d)",
			    sim_net_get_name (SIM_NET (net)),
			    sim_net_get_c (SIM_NET (net)),
			    sim_net_get_a (SIM_NET (net)));

  ret = sim_database_execute_no_query (server->_priv->database,
				 insert);

  g_free (insert);

  return ret;
}

/*
 *
 *
 *
 */
gint
sim_server_db_update_net (SimServer *server,
			  GObject   *net)
{
  gchar           *update;
  gint             ret;

  g_return_if_fail (server != NULL);
  g_return_if_fail (SIM_IS_SERVER (server));
  g_return_if_fail (net != NULL);
  g_return_if_fail (SIM_IS_NET (net));
  g_return_if_fail (server->_priv->database !=  NULL);

  update = g_strdup_printf ("UPDATE net_qualification SET compromise = %d, attack = %d WHERE net_name = '%s'",
			    sim_net_get_c (SIM_NET (net)),
			    sim_net_get_a (SIM_NET (net)),
			    sim_net_get_name (SIM_NET (net)));

  ret = sim_database_execute_no_query (server->_priv->database,
				 update);


  g_free (update);

  return ret;
}

/*
 *
 *
 *
 */
gint
sim_server_db_delete_net (SimServer *server,
			  GObject   *net)
{
  gchar           *delete;
  gint             ret;

  g_return_if_fail (server != NULL);
  g_return_if_fail (SIM_IS_SERVER (server));
  g_return_if_fail (net != NULL);
  g_return_if_fail (SIM_IS_NET (net));
  g_return_if_fail (server->_priv->database !=  NULL);

  delete = g_strdup_printf ("DELETE FROM net_qualification WHERE net_name = '%s'",
			    sim_net_get_name (SIM_NET (net)));

  ret = sim_database_execute_no_query (server->_priv->database,
				 delete);

  g_free (delete);

  return ret;
}


/*
 *
 *
 *
 */
GObject*
sim_server_get_net_by_name (SimServer *server,
			    gchar     *name)
{
  SimNet          *net = NULL;
  gboolean         found = FALSE;
  gint             i;

  g_return_val_if_fail (server != NULL, NULL);
  g_return_val_if_fail (SIM_IS_SERVER (server), NULL);

  for (i = 0; i < g_list_length(server->_priv->nets); i++)
    {
      net = (SimNet *) g_list_nth_data (server->_priv->nets, i);

      if (!strcmp (sim_net_get_name (net), name)) 
	{
	  found = TRUE;
	  break;
	}
    }

  g_return_val_if_fail (found, NULL);

  return G_OBJECT (net);
}

/*
 * Net Assets Functions
 */

/*
 *
 *
 *
 */
void
sim_server_add_net_asset (SimServer *server,
			   GObject   *net_asset)
{
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_if_fail (server != NULL);
  g_return_if_fail (SIM_IS_SERVER (server));
  g_return_if_fail (net_asset != NULL);
  g_return_if_fail (SIM_IS_NET_ASSET (net_asset));

  g_static_mutex_lock (&mutex);
  server->_priv->net_assets = g_list_append (server->_priv->net_assets, net_asset);
  g_static_mutex_unlock (&mutex);
}

/*
 *
 *
 *
 */
void
sim_server_remove_net_asset (SimServer *server,
			      GObject   *net_asset)
{
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_if_fail (server != NULL);
  g_return_if_fail (SIM_IS_SERVER (server));
  g_return_if_fail (net_asset != NULL);
  g_return_if_fail (SIM_IS_NET_ASSET (net_asset));

  g_static_mutex_lock (&mutex);
  server->_priv->net_assets = g_list_remove (server->_priv->net_assets, net_asset);
  g_static_mutex_unlock (&mutex);
}

/*
 *
 *
 *
 */
GList*
sim_server_get_net_assets (SimServer *server)
{
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;
  GList *list = NULL;

  g_return_val_if_fail (server != NULL, NULL);
  g_return_val_if_fail (SIM_IS_SERVER (server), NULL);

  g_static_mutex_lock (&mutex);
  list = g_list_copy (server->_priv->net_assets);
  g_static_mutex_unlock (&mutex);

  return list;
}

/*
 * Signatures Functions
 */

/*
 *
 *
 *
 */
static gboolean
sim_server_get_sig_subgroup_from_sid_func (GNode    *node,
					   gpointer  sig_data)
{
  SimSignatureData *data;
  SimSignature     *signature;
  gint              sid;

  data = (SimSignatureData *) sig_data;
  signature = (SimSignature *) node->data;

  g_return_val_if_fail (signature != NULL, FALSE);
  g_return_val_if_fail (SIM_IS_SIGNATURE (signature), FALSE);

  if (signature->type != SIM_SIGNATURE_TYPE_SIGNATURE)
    return FALSE;

  sid = sim_signature_get_id (signature);

  if (sid != data->id)
    return FALSE;

  data->node = node->parent;

  return TRUE;
}

/*
 *
 *
 *
 */
static gboolean
sim_server_get_sig_subgroup_from_type_func (GNode    *node,
					   gpointer  sig_data)
{
  SimSignatureData *data;
  SimSignature     *signature;

  data = (SimSignatureData *) sig_data;
  signature = (SimSignature *) node->data;

  g_return_val_if_fail (signature != NULL, FALSE);
  g_return_val_if_fail (SIM_IS_SIGNATURE (signature), FALSE);

  if (signature->type != SIM_SIGNATURE_TYPE_SUBGROUP)
    return FALSE;

  if (signature->subgroup != data->subtype)
    return FALSE;

  data->node = node;

  return TRUE;
}

/*
 *
 *
 *
 */
GObject*
sim_server_get_sig_subgroup_from_sid (SimServer *server,
				      gint       sid)
{
  SimSignatureData *data;
  GObject *signature = NULL;

  g_return_val_if_fail (server != NULL, NULL);
  g_return_val_if_fail (SIM_IS_SERVER (server), NULL);
  g_return_val_if_fail (sid != 0, NULL);

  data = g_malloc (sizeof (SimSignatureData));

  data->id = sid;
  data->node = NULL;

  g_node_traverse (server->_priv->sig_root,
		   G_IN_ORDER, 
		   G_TRAVERSE_ALL, 
		   -1,
		   sim_server_get_sig_subgroup_from_sid_func, 
		   data);

  if (data->node)
    signature = data->node->data;

  return signature;
}

/*
 *
 *
 *
 */
GObject*
sim_server_get_sig_subgroup_from_type (SimServer *server,
				       gint       subtype)
{
  SimSignatureData *data;
  GObject *signature = NULL;

  g_return_val_if_fail (server != NULL, NULL);
  g_return_val_if_fail (SIM_IS_SERVER (server), NULL);

  data = g_malloc (sizeof (SimSignatureData));

  data->subtype = subtype;
  data->node = NULL;

  g_node_traverse (server->_priv->sig_root,
		   G_IN_ORDER,
		   G_TRAVERSE_ALL,
		   -1,
		   sim_server_get_sig_subgroup_from_type_func,
		   data);

  if (data->node)
    signature = data->node->data;

  return signature;
}
