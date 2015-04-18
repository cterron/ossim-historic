/* Copyright (c) 2003 ossim.net
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission
 *    from the author.
 *
 * 4. Products derived from this software may not be called "Os-sim" nor
 *    may "Os-sim" appear in their names without specific prior written
 *    permission from the author.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
 * THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

  gchar           *name;        /* DS Name */
  gchar           *provider  ;  /* Data Source */
  gchar           *dsn;         /* User Name */
};

static gpointer parent_class = NULL;
static gint sim_database_signals[LAST_SIGNAL] = { 0 };

/* GType Functions */

static void 
sim_database_impl_dispose (GObject  *gobject)
{
  G_OBJECT_CLASS (parent_class)->dispose (gobject);
}

static void 
sim_database_impl_finalize (GObject  *gobject)
{
  SimDatabase *database = SIM_DATABASE (gobject);

  if (database->_priv->name)
    g_free (database->_priv->name);
  if (database->_priv->provider)
    g_free (database->_priv->provider);
  if (database->_priv->dsn)
    g_free (database->_priv->dsn);

  gda_connection_close (database->_priv->conn);
  g_object_unref (database->_priv->client);

  g_free (database->_priv);

  G_OBJECT_CLASS (parent_class)->finalize (gobject);
}

static void
sim_database_class_init (SimDatabaseClass * class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  parent_class = g_type_class_peek_parent (class);

  object_class->dispose = sim_database_impl_dispose;
  object_class->finalize = sim_database_impl_finalize;
}

static void
sim_database_instance_init (SimDatabase *database)
{
  database->_priv = g_new0 (SimDatabasePrivate, 1);

  database->_priv->client = NULL;
  database->_priv->conn = NULL;
  database->_priv->name = NULL;
  database->_priv->provider = NULL;
  database->_priv->dsn = NULL;
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
SimDatabase*
sim_database_new (SimConfigDS  *config)
{
  SimDatabase *db = NULL;
  GdaError       *error;
  GList          *errors = NULL;
  gint            i;

  g_return_val_if_fail (config, NULL);
  g_return_val_if_fail (config->name, NULL);
  g_return_val_if_fail (config->provider, NULL);
  g_return_val_if_fail (config->dsn, NULL);

  db = SIM_DATABASE (g_object_new (SIM_TYPE_DATABASE, NULL));

  db->_priv->name = g_strdup (config->name);
  db->_priv->provider = g_strdup (config->provider);
  db->_priv->dsn = g_strdup (config->dsn);

  db->_priv->client = gda_client_new ();
  db->_priv->conn = gda_client_open_connection_from_string  (db->_priv->client,
							     db->_priv->provider,
							     db->_priv->dsn,
							     GDA_CONNECTION_OPTIONS_DONT_SHARE);

  if (!gda_connection_is_open (db->_priv->conn))
    {
      g_print ("CONNECTION ERROR\n");
      g_print ("NAME: %s", db->_priv->name);
      g_print (" PROVIDER: %s", db->_priv->provider);
      g_print (" DSN: %s", db->_priv->dsn);
      g_print ("\n");
    }

  errors = gda_error_list_copy (gda_connection_get_errors (db->_priv->conn));
  for (i = 0; i < g_list_length(errors); i++)
    {
      error = (GdaError *) g_list_nth_data (errors, i);
      
      g_message ("ERROR %d: %s", gda_error_get_number (error), gda_error_get_description (error));
    }
  gda_error_list_free (errors);

  return db;
}

/*
 *
 *
 *
 */
gint
sim_database_execute_no_query  (SimDatabase  *database,
				const gchar  *buffer)
{
  GdaCommand     *command;
  GdaError       *error;
  GList          *errors = NULL;
  gint            ret, i;

  g_return_val_if_fail (database != NULL, -1);
  g_return_val_if_fail (SIM_IS_DATABASE (database), -1);
  g_return_val_if_fail (buffer != NULL, -1);

  command = gda_command_new (buffer, 
			     GDA_COMMAND_TYPE_SQL, 
			     GDA_COMMAND_OPTION_STOP_ON_ERRORS);

  if (!gda_connection_is_open (database->_priv->conn))
    {
      g_message ("Connection not is open");
      gda_connection_close (database->_priv->conn);
      database->_priv->conn = gda_client_open_connection_from_string (database->_priv->client,
								      database->_priv->provider,
								      database->_priv->dsn,
								      GDA_CONNECTION_OPTIONS_DONT_SHARE);
    }
  ret = gda_connection_execute_non_query (database->_priv->conn, command, NULL);

  if (ret < 0)
    {
      errors = gda_error_list_copy (gda_connection_get_errors (database->_priv->conn));
      for (i = 0; i < g_list_length(errors); i++)
	{
	  error = (GdaError *) g_list_nth_data (errors, i);

	  g_message ("ERROR %s %d: %s", buffer, gda_error_get_number (error), gda_error_get_description (error));
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
sim_database_execute_command (SimDatabase  *database,
			      const gchar  *buffer)
{
  GdaCommand     *command;
  GList          *list;

  g_return_val_if_fail (database != NULL, NULL);
  g_return_val_if_fail (SIM_IS_DATABASE (database), NULL);
  g_return_val_if_fail (buffer != NULL, NULL);

  command = gda_command_new (buffer,
			     GDA_COMMAND_TYPE_SQL,
			     GDA_COMMAND_OPTION_STOP_ON_ERRORS);

  list = gda_connection_execute_command (database->_priv->conn, command, NULL);

  gda_command_free (command);

  return list;
}

/*
 *
 *
 *
 */
GdaDataModel*
sim_database_execute_single_command (SimDatabase  *database,
				     const gchar  *buffer)
{
  GdaCommand     *command;
  GdaDataModel   *model;

  g_return_val_if_fail (database != NULL, NULL);
  g_return_val_if_fail (SIM_IS_DATABASE (database), NULL);
  g_return_val_if_fail (buffer != NULL, NULL);

  command = gda_command_new (buffer,
			     GDA_COMMAND_TYPE_SQL,
			     GDA_COMMAND_OPTION_STOP_ON_ERRORS);

  if (!gda_connection_is_open (database->_priv->conn))
    {
      g_message ("Connection not is open");
      gda_connection_close (database->_priv->conn);
      database->_priv->conn = gda_client_open_connection_from_string (database->_priv->client,
								      database->_priv->provider,
								      database->_priv->dsn,
								      GDA_CONNECTION_OPTIONS_DONT_SHARE);
    }
  model = gda_connection_execute_single_command (database->_priv->conn, command, NULL);

  gda_command_free (command);

  return model;
}

/*
 *
 *
 *
 */
GdaConnection*
sim_database_get_conn (SimDatabase  *database)
{
  g_return_val_if_fail (database != NULL, NULL);
  g_return_val_if_fail (SIM_IS_DATABASE (database), NULL);

  return database->_priv->conn;  
}
