/**
 * Database
 *
 */

#include <libgda/libgda.h>

#include "sim-database.h"

enum 
{
  DESTROY,
  LAST_SIGNAL
};

struct _SimDatabasePrivate {
  GdaClient       *client;      /* Connection Pool */
  GdaConnection   *conn;        /* Connection */

  gchar           *datasource;  /* Data Source */
  gchar           *username;    /* User Name */
  gchar           *password;    /* Password */
};

static gpointer parent_class = NULL;
static gint sim_database_signals[LAST_SIGNAL] = { 0 };

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

  database->_priv->client = NULL;
  database->_priv->conn = NULL;
  database->_priv->datasource = NULL;
  database->_priv->username = NULL;
  database->_priv->password = NULL;
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
sim_database_new (gchar *datasource,
		  gchar *username,
		  gchar *password)
{
  SimDatabase *db = NULL;

  g_return_val_if_fail (datasource != NULL, NULL);
  g_return_val_if_fail (username != NULL, NULL);

  db = SIM_DATABASE (g_object_new (SIM_TYPE_DATABASE, NULL));

  db->_priv->datasource = datasource;
  db->_priv->username = username;
  db->_priv->password = password;

  db->_priv->client = gda_client_new ();
  db->_priv->conn = gda_client_open_connection (db->_priv->client,
						db->_priv->datasource,
						db->_priv->username,
						db->_priv->password,
						GDA_CONNECTION_OPTIONS_DONT_SHARE);

  return db;
}

/*
 *
 *
 *
 */
gint
sim_database_execute_no_query  (SimDatabase *database,
				gchar       *buffer)
{
  GdaCommand *command;
  GdaError   *error;
  GList      *errors = NULL;
  gint        ret, i;

  g_return_val_if_fail (database != NULL, -1);
  g_return_val_if_fail (SIM_IS_DATABASE (database), -1);
  g_return_val_if_fail (buffer != NULL, -1);

  command = gda_command_new (buffer, 
			     GDA_COMMAND_TYPE_SQL, 
			     GDA_COMMAND_OPTION_STOP_ON_ERRORS);
  ret = gda_connection_execute_non_query (database->_priv->conn,
					  command,
					  NULL);

  if (ret < 0)
    {
      errors = gda_error_list_copy (gda_connection_get_errors (database->_priv->conn));
      for (i = 0; i < g_list_length(errors); i++)
	{
	  error = (GdaError *) g_list_nth_data (errors, i);

	  g_message ("ERROR NO QUERY %d: %s", gda_error_get_number (error), gda_error_get_description (error));
	}
      gda_error_list_free (errors);
    }

  gda_command_free (command);

  return ret;
}

/*
 *
 *
 *
 */
GList*
sim_database_execute_command (SimDatabase *database,
			      gchar       *buffer)
{
  GdaCommand *command;
  GList *list;

  g_return_val_if_fail (database != NULL, NULL);
  g_return_val_if_fail (SIM_IS_DATABASE (database), NULL);
  g_return_val_if_fail (buffer != NULL, NULL);

  command = gda_command_new (buffer,
			     GDA_COMMAND_TYPE_SQL,
			     GDA_COMMAND_OPTION_STOP_ON_ERRORS);
  list = gda_connection_execute_command (database->_priv->conn,
					 command,
					 NULL);
  gda_command_free (command);

  return list;
}
