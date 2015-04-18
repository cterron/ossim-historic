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

  gchar           *datasource;  /* Data Source */
  gchar           *username;    /* User Name */
  gchar           *password;    /* Password */
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

  g_free (database->_priv->datasource);
  g_free (database->_priv->username);
  g_free (database->_priv->password);

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

  if (!gda_connection_is_open (database->_priv->conn))
    database->_priv->conn = gda_client_open_connection (database->_priv->client,
							database->_priv->datasource,
							database->_priv->username,
							database->_priv->password,
							GDA_CONNECTION_OPTIONS_DONT_SHARE);

  ret = gda_connection_execute_non_query (database->_priv->conn,
					  command,
					  NULL);

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

  if (!gda_connection_is_open (database->_priv->conn))
    database->_priv->conn = gda_client_open_connection (database->_priv->client,
							database->_priv->datasource,
							database->_priv->username,
							database->_priv->password,
							GDA_CONNECTION_OPTIONS_DONT_SHARE);
  
  list = gda_connection_execute_command (database->_priv->conn,
					 command,
					 NULL);
  gda_command_free (command);

  return list;
}

/*
 *
 *
 *
 */
GdaDataModel*
sim_database_execute_single_command (SimDatabase *database,
				     gchar       *buffer)
{
  GdaCommand *command;
  GdaDataModel *model;

  g_return_val_if_fail (database != NULL, NULL);
  g_return_val_if_fail (SIM_IS_DATABASE (database), NULL);
  g_return_val_if_fail (buffer != NULL, NULL);

  command = gda_command_new (buffer,
			     GDA_COMMAND_TYPE_SQL,
			     GDA_COMMAND_OPTION_STOP_ON_ERRORS);

  if (!gda_connection_is_open (database->_priv->conn))
    database->_priv->conn = gda_client_open_connection (database->_priv->client,
							database->_priv->datasource,
							database->_priv->username,
							database->_priv->password,
							GDA_CONNECTION_OPTIONS_DONT_SHARE);
  
  model = gda_connection_execute_single_command (database->_priv->conn,
						 command,
						 NULL);
  gda_command_free (command);

  return model;
}
