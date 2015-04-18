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

#include <config.h>

#include "sim-container.h"
#include "sim-xml-directive.h"

enum
{
  DESTROY,
  LAST_SIGNAL
};

struct _SimContainerPrivate {
  GList        *hosts;
  GList        *nets;
  GList        *signatures;
  GList        *policies;
  GList        *directives;

  GList        *host_levels;
  GList        *net_levels;
  GList        *backlogs;

  GAsyncQueue  *messages;
};

typedef struct {
  gint      id;
  gint      subtype;
  GNode    *node;
} SimSignatureData;

static gpointer parent_class = NULL;
static gint sim_container_signals[LAST_SIGNAL] = { 0 };

/* GType Functions */

static void 
sim_container_impl_dispose (GObject  *gobject)
{
  G_OBJECT_CLASS (parent_class)->dispose (gobject);
}

static void 
sim_container_impl_finalize (GObject  *gobject)
{
  SimContainer  *container = SIM_CONTAINER (gobject);
  GList         *list;

  /* Free Host */
  list = container->_priv->hosts;
  while (list)
    {
      SimHost *host = (SimHost *) list->data;
      g_object_unref (host);
      list = list->next;
    }
  g_list_free (container->_priv->hosts);

  /* Free Nets */
  list = container->_priv->nets;
  while (list)
    {
      SimNet *net = (SimNet *) list->data;
      g_object_unref (net);
      list = list->next;
    }
  g_list_free (container->_priv->nets);

  /* Free Policies */
  list = container->_priv->policies;
  while (list)
    {
      SimPolicy *policy = (SimPolicy *) list->data;
      g_object_unref (policy);
      list = list->next;
    }
  g_list_free (container->_priv->policies);

  /* Free Directives */
  list = container->_priv->directives;
  while (list)
    {
      SimDirective *directive = (SimDirective *) list->data;
      g_object_unref (directive);
      list = list->next;
    }
  g_list_free (container->_priv->directives);

  /* Free Host Levels */
  list = container->_priv->host_levels;
  while (list)
    {
      SimHostLevel *host_level = (SimHostLevel *) list->data;
      g_object_unref (host_level);
      list = list->next;
    }
  g_list_free (container->_priv->host_levels);

  /* Free Net Levels */
  list = container->_priv->net_levels;
  while (list)
    {
      SimNetLevel *net_level = (SimNetLevel *) list->data;
      g_object_unref (net_level);
      list = list->next;
    }
  g_list_free (container->_priv->net_levels);

  /* Free Backlogs */
  list = container->_priv->backlogs;
  while (list)
    {
      SimDirective *backlog = (SimDirective *) list->data;
      g_object_unref (backlog);
      list = list->next;
    }
  g_list_free (container->_priv->backlogs);

  /* Free Messages */
  while (g_async_queue_length (container->_priv->messages))
    {
      SimMessage *message = (SimMessage *) g_async_queue_pop (container->_priv->messages);
      g_object_unref (message);
    }
  g_async_queue_unref (container->_priv->messages);

  G_OBJECT_CLASS (parent_class)->finalize (gobject);
}

static void
sim_container_class_init (SimContainerClass * class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  parent_class = g_type_class_ref (G_TYPE_OBJECT);

  object_class->dispose = sim_container_impl_dispose;
  object_class->finalize = sim_container_impl_finalize;
}

static void
sim_container_instance_init (SimContainer *container)
{
  container->_priv = g_new0 (SimContainerPrivate, 1);

  container->_priv->hosts = NULL;
  container->_priv->nets = NULL;
  container->_priv->signatures = NULL;

  container->_priv->policies = NULL;
  container->_priv->directives = NULL;

  container->_priv->host_levels = NULL;
  container->_priv->net_levels = NULL;
  container->_priv->backlogs = NULL;

  container->_priv->messages = g_async_queue_new ();
}

/* Public Methods */

GType
sim_container_get_type (void)
{
  static GType object_type = 0;
 
  if (!object_type)
  {
    static const GTypeInfo type_info = {
              sizeof (SimContainerClass),
              NULL,
              NULL,
              (GClassInitFunc) sim_container_class_init,
              NULL,
              NULL,                       /* class data */
              sizeof (SimContainer),
              0,                          /* number of pre-allocs */
              (GInstanceInitFunc) sim_container_instance_init,
              NULL                        /* value table */
    };
    
    g_type_init ();
                                                                                                                             
    object_type = g_type_register_static (G_TYPE_OBJECT, "SimContainer", &type_info, 0);
  }

  return object_type;
}

/*
 *
 *
 *
 *
 */
SimContainer*
sim_container_new (void)
{
  SimContainer *container = NULL;

  container = SIM_CONTAINER (g_object_new (SIM_TYPE_CONTAINER, NULL));

  return container;
}

/*
 *
 *
 *
 *
 */
gint
sim_container_db_get_recovery (SimContainer  *container,
			       SimDatabase   *database)
{
  GdaDataModel  *dm;
  GdaValue      *value;
  gchar         *query = "SELECT recovery FROM conf";
  gint           row;
  gint           recovery = 1;

  g_return_if_fail (container != NULL);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database != NULL);
  g_return_if_fail (SIM_IS_DATABASE (database));

  dm = sim_database_execute_single_command (database, query);
  if (dm)
    {
      for (row = 0; row < gda_data_model_get_n_rows (dm); row++)
	{
	  /* Recovery */
	  value = (GdaValue *) gda_data_model_get_value_at (dm, 0, row);
	  recovery = gda_value_get_integer (value);
	}
      
      g_object_unref(dm);
    }
  else
    {
      g_message ("RECOVERY DATA MODEL ERROR");
    }

  return recovery;
}

/*
 *
 *
 *
 *
 */
void
sim_container_db_load_hosts (SimContainer  *container,
			     SimDatabase   *database)
{
  SimHost       *host;
  GdaDataModel  *dm;
  gint           row;
  gchar         *query = "SELECT * FROM host";

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));

  dm = sim_database_execute_single_command (database, query);
  if (dm)
    {
      for (row = 0; row < gda_data_model_get_n_rows (dm); row++)
	{
	  host  = sim_host_new_from_dm (dm, row);	  
	  sim_container_append_host (container, host);
	}

      g_object_unref(dm);
    }
  else
    {
      g_message ("HOSTS DATA MODEL ERROR");
    }
}

/*
 *
 *
 *
 *
 */
void
sim_container_append_host (SimContainer  *container,
			   SimHost       *host)
{
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (host);
  g_return_if_fail (SIM_IS_HOST (host));

  g_static_mutex_lock (&mutex);
  container->_priv->hosts = g_list_append (container->_priv->hosts, host);
  g_static_mutex_unlock (&mutex);
}

/*
 *
 *
 *
 *
 */
void
sim_container_remove_host (SimContainer  *container,
			   SimHost       *host)
{
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (host);
  g_return_if_fail (SIM_IS_HOST (host));

  g_static_mutex_lock (&mutex);
  container->_priv->hosts = g_list_remove (container->_priv->hosts, host);
  g_static_mutex_unlock (&mutex);
}

/*
 *
 *
 *
 *
 */
GList*
sim_container_get_hosts (SimContainer  *container)
{
  GList *list;
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);

  g_static_mutex_lock (&mutex);
  list = g_list_copy (container->_priv->hosts);
  g_static_mutex_unlock (&mutex);

  return list;
}

/*
 *
 *
 *
 *
 */
void
sim_container_set_hosts (SimContainer  *container,
			 GList         *hosts)
{
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (hosts);

  g_static_mutex_lock (&mutex);
  while (hosts)
    {
      SimHost *host = (SimHost *) hosts->data;
      container->_priv->hosts = g_list_append (container->_priv->hosts, host);

      hosts = hosts->next;
    }
  g_static_mutex_unlock (&mutex);
}

/*
 *
 *
 *
 *
 */
void
sim_container_free_hosts (SimContainer  *container)
{
  GList *list;
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));

  g_static_mutex_lock (&mutex);
  list = container->_priv->hosts;
  while (list)
    {
      SimHost *host = (SimHost *) list->data;
      g_object_unref (host);

      list = list->next;
    }
  g_list_free (container->_priv->hosts);
  g_static_mutex_unlock (&mutex);
}

/*
 *
 *
 *
 *
 */
SimHost*
sim_container_get_host_by_ia (SimContainer  *container,
			      GInetAddr     *ia)
{
  SimHost   *host;
  GList     *list;
  gboolean   found = FALSE;
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);
  g_return_val_if_fail (ia, NULL);

  g_static_mutex_lock (&mutex);
  list = container->_priv->hosts;
  while (list)
    {
      host = (SimHost *) list->data;

      if (gnet_inetaddr_noport_equal (sim_host_get_ia (host), ia))
	{
	  found = TRUE;
	  break;
	}

      list = list->next;
    }
  g_static_mutex_unlock (&mutex);

  if (!found)
    return NULL;

  return host;
}

/*
 *
 *
 *
 *
 */
void
sim_container_db_load_nets (SimContainer  *container,
			    SimDatabase   *database)
{
  SimNet        *net;
  GdaDataModel  *dm;
  GdaDataModel  *dm2;
  GdaValue      *value;
  GInetAddr     *ia;
  gint           row;
  gint           row2;
  gchar         *query = "SELECT * FROM net";
  gchar         *query2;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));

  dm = sim_database_execute_single_command (database, query);
  if (dm)
    {
      for (row = 0; row < gda_data_model_get_n_rows (dm); row++)
	{
	  net  = sim_net_new_from_dm (dm, row);
	  sim_container_append_net (container, net);

	  query2 = g_strdup_printf ("SELECT host_ip FROM net_host_reference WHERE net_name = '%s'",
				    sim_net_get_name (net));

	  dm2 = sim_database_execute_single_command (database, query2);
	  if (dm2)
	    {
	      for (row2 = 0; row2 < gda_data_model_get_n_rows (dm2); row2++)
		{
		  gchar *ip;

		  value = (GdaValue *) gda_data_model_get_value_at (dm2, 0, row2);
		  ip = gda_value_stringify (value);

		  ia = gnet_inetaddr_new_nonblock (ip, 0);
		  sim_net_append_ia (net, ia);

		  g_free (ip);
		}
	      g_object_unref(dm2);
	    }
	  else
	    {
	      g_message ("NET HOST REFERENCES DATA MODEL ERROR");
	    }

	  g_free (query2);
	}
      
      g_object_unref(dm);
    }
  else
    {
      g_message ("NETS DATA MODEL ERROR");
    }
}

/*
 *
 *
 *
 *
 */
void
sim_container_append_net (SimContainer  *container,
			  SimNet       *net)
{
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (net);
  g_return_if_fail (SIM_IS_NET (net));

  g_static_mutex_lock (&mutex);
  container->_priv->nets = g_list_append (container->_priv->nets, net);
  g_static_mutex_unlock (&mutex);
}

/*
 *
 *
 *
 *
 */
void
sim_container_remove_net (SimContainer  *container,
			  SimNet       *net)
{
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (net);
  g_return_if_fail (SIM_IS_NET (net));

  g_static_mutex_lock (&mutex);
  container->_priv->nets = g_list_remove (container->_priv->nets, net);
  g_static_mutex_unlock (&mutex);
}

/*
 *
 *
 *
 *
 */
GList*
sim_container_get_nets (SimContainer  *container)
{
  GList *list;
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);

  g_static_mutex_lock (&mutex);
  list = g_list_copy (container->_priv->nets);
  g_static_mutex_unlock (&mutex);

  return list;
}

/*
 *
 *
 *
 *
 */
void
sim_container_set_nets (SimContainer  *container,
			GList         *nets)
{
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (nets);

  g_static_mutex_lock (&mutex);
  while (nets)
    {
      SimNet *net = (SimNet *) nets->data;
      container->_priv->nets = g_list_append (container->_priv->nets, net);

      nets = nets->next;
    }
  g_static_mutex_unlock (&mutex);
}

/*
 *
 *
 *
 *
 */
void
sim_container_free_nets (SimContainer  *container)
{
  GList *list;
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));

  g_static_mutex_lock (&mutex);
  list = container->_priv->nets;
  while (list)
    {
      SimNet *net = (SimNet *) list->data;
      g_object_unref (net);

      list = list->next;
    }
  g_list_free (container->_priv->nets);
  g_static_mutex_unlock (&mutex);
}

/*
 *
 *
 *
 *
 */
GList*
sim_container_get_nets_has_ia (SimContainer  *container,
			       GInetAddr     *ia)
{
  GList *list;
  GList *nets = NULL;
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;


  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);
  g_return_val_if_fail (ia, NULL);

  g_static_mutex_lock (&mutex);
  list = container->_priv->nets;
  while (list)
    {
      SimNet *net = (SimNet *) list->data;

      if (sim_net_has_ia (net, ia))
	{
	  nets = g_list_append (nets, net);
	}

      list = list->next;
    }
  g_static_mutex_unlock (&mutex);

  return nets;
}

/*
 *
 *
 *
 *
 */
SimNet*
sim_container_get_net_by_name (SimContainer  *container,
			       gchar         *name)
{
  SimNet    *net;
  GList     *list;
  gboolean   found = FALSE;
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);
  g_return_val_if_fail (name, NULL);

  g_static_mutex_lock (&mutex);
  list = container->_priv->nets;
  while (list)
    {
      net = (SimNet *) list->data;

      if (!strcmp (sim_net_get_name (net), name))
	{
	  found = TRUE;
	  break;
	}

      list = list->next;
    }
  g_static_mutex_unlock (&mutex);

  if (!found)
    return NULL;

  return net;
}

/*
 *
 *
 *
 *
 */
void
sim_container_db_load_signatures (SimContainer  *container,
				  SimDatabase   *database)
{
  SimSignature   *signature;
  SimSignature   *child;
  GdaDataModel   *dm;
  GdaValue       *value;
  gchar          *query = "select * from signature";
  gint            row;
  FILE           *fd;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));

  dm = sim_database_execute_single_command (database, query);
  if (dm)
    {
      for (row = 0; row < gda_data_model_get_n_rows (dm); row++)
	{
	  signature = sim_signature_new_from_dm (SIM_SIGNATURE_TYPE_GROUP, dm, row);

	  sim_container_append_signature (container, signature);
	}
    }
  else
    {
      g_message ("SIGNATURE DATA MODEL ERROR");
    }

  if ((fd = fopen ("sids", "r")) == NULL)
    {
      printf ("Can't open file %s\n", "sids");
      exit (-1);
    }
  
  while (!feof (fd))
    {
      gchar str[BUFFER_SIZE], *ret, **split;
      gchar *name;
      gint id;

      ret = fgets (str, BUFFER_SIZE, fd);
      
      split = g_strsplit (str, ";", 0);
      
      id = strtol(split[1], (char **)NULL, 10);
      name = g_strdup (split[2]);

      signature = sim_container_get_signature_group_by_name (container, split[0]);
      child = sim_signature_new (SIM_SIGNATURE_TYPE_SIGNATURE, id, name);      
      sim_signature_append_child (signature, child);
      
      g_strfreev (split);
    }

  fclose (fd);
}

/*
 *
 *
 *
 *
 */
void
sim_container_append_signature (SimContainer  *container,
				SimSignature  *signature)
{
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (signature);
  g_return_if_fail (SIM_IS_SIGNATURE (signature));

  g_static_mutex_lock (&mutex);
  container->_priv->signatures = g_list_append (container->_priv->signatures, signature);
  g_static_mutex_unlock (&mutex);
}

/*
 *
 *
 *
 *
 */
void
sim_container_remove_signature (SimContainer  *container,
				SimSignature  *signature)
{
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (signature);
  g_return_if_fail (SIM_IS_SIGNATURE (signature));

  g_static_mutex_lock (&mutex);
  container->_priv->signatures = g_list_remove (container->_priv->signatures, signature);
  g_static_mutex_unlock (&mutex);
}

/*
 *
 *
 *
 *
 */
GList*
sim_container_get_signatures (SimContainer  *container)
{
  GList *list;
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);

  g_static_mutex_lock (&mutex);
  list = g_list_copy (container->_priv->signatures);
  g_static_mutex_unlock (&mutex);

  return list;
}

/*
 *
 *
 *
 *
 */
SimSignature*
sim_container_get_signature_group_by_id (SimContainer  *container,
					  gint          id)
{
  SimSignature  *signature;
  GList         *list;
  gboolean       found = FALSE;
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);
  
  g_static_mutex_lock (&mutex);
  list = container->_priv->signatures;
  while (list)
    {
      signature = (SimSignature *) list->data;

      if (sim_signature_get_id (signature) == id)
	{
	  found = TRUE;
	  break;
	}

      list = list->next;
    }
  g_static_mutex_unlock (&mutex);

  if (!found)
    return NULL;

  return signature;
}

/*
 *
 *
 *
 *
 */
SimSignature*
sim_container_get_signature_group_by_name (SimContainer  *container,
					   gchar         *name)
{
  SimSignature  *signature;
  GList         *list;
  gboolean       found = FALSE;
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);
  g_return_val_if_fail (name, NULL);
  
  g_static_mutex_lock (&mutex);
  list = container->_priv->signatures;
  while (list)
    {
      signature = (SimSignature *) list->data;

      if (!strcmp (sim_signature_get_name (signature), name))
	{
	  found = TRUE;
	  break;
	}

      list = list->next;
    }
  g_static_mutex_unlock (&mutex);

  if (!found)
    return NULL;

  return signature;
}

/*
 *
 *
 *
 *
 */
SimSignature*
sim_container_get_signature_group_by_sid (SimContainer  *container,
					  gint           sid)
{
  SimSignature  *signature;
  GList         *list;
  gboolean       found = FALSE;
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);

  g_static_mutex_lock (&mutex);
  list = container->_priv->signatures;
  while (list)
    {
      signature = (SimSignature *) list->data;

      if (sim_signature_has_child_id (signature, sid))
	{
	  found = TRUE;
	  break;
	}

      list = list->next;
    }
  g_static_mutex_unlock (&mutex);

  if (!found)
    return NULL;

  return signature;
}

/*
 *
 *
 *
 *
 */
void
sim_container_db_load_policies (SimContainer  *container,
				SimDatabase   *database)
{
  SimPolicy     *policy;
  GdaDataModel  *dm;
  GdaDataModel  *dm2;
  GdaValue      *value;
  GInetAddr     *ia;
  gint           row;
  gint           row2;
  gchar         *query = "SELECT policy.id, policy.priority, policy.descr, policy_time.begin_hour, policy_time.end_hour, policy_time.begin_day, policy_time.end_day FROM policy, policy_time WHERE policy.id = policy_time.policy_id;";
  gchar         *query2;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));

  dm = sim_database_execute_single_command (database, query);
  if (dm)
    {
      for (row = 0; row < gda_data_model_get_n_rows (dm); row++)
	{
	  policy  = sim_policy_new_from_dm (dm, row);

	  /* Host Source Inet Address */
	  query2 = g_strdup_printf ("SELECT host_ip FROM  policy_host_reference WHERE policy_id = %d AND direction = 'source'",
				    sim_policy_get_id (policy));
	  dm2 = sim_database_execute_single_command (database, query2);
	  if (dm2)
	    {
	      for (row2 = 0; row2 < gda_data_model_get_n_rows (dm2); row2++)
		{
		  gchar *src_ip;

		  value = (GdaValue *) gda_data_model_get_value_at (dm2, 0, row2);
		  src_ip = gda_value_stringify (value);

		  if (!g_ascii_strncasecmp (src_ip, SIM_IN_ADDR_ANY_CONST, 3))
		    ia = gnet_inetaddr_new_nonblock (SIM_IN_ADDR_ANY_IP_STR, 0);
		  else
		    ia = gnet_inetaddr_new_nonblock (src_ip, 0);

		  sim_policy_append_src_ia (policy, ia);

		  g_free (src_ip);
		}
	      g_object_unref(dm2);
	    }
	  else
	    {
	      g_message ("POLICY HOST SOURCE REFERENCES DATA MODEL ERROR");
	    }
	  g_free (query2);

	  /* Host Destination Inet Address */
	  query2 = g_strdup_printf ("SELECT host_ip FROM  policy_host_reference WHERE policy_id = %d AND direction = 'dest'",
				    sim_policy_get_id (policy));
	  dm2 = sim_database_execute_single_command (database, query2);
	  if (dm2)
	    {
	      for (row2 = 0; row2 < gda_data_model_get_n_rows (dm2); row2++)
		{
		  gchar *dst_ip;

		  value = (GdaValue *) gda_data_model_get_value_at (dm2, 0, row2);
		  dst_ip = gda_value_stringify (value);

		  if (!g_ascii_strncasecmp (dst_ip, SIM_IN_ADDR_ANY_CONST, 3))
		    ia = gnet_inetaddr_new_nonblock (SIM_IN_ADDR_ANY_IP_STR, 0);
		  else
		    ia = gnet_inetaddr_new_nonblock (dst_ip, 0);

		  sim_policy_append_dst_ia (policy, ia);

		  g_free (dst_ip);
		}
	      g_object_unref(dm2);
	    }
	  else
	    {
	      g_message ("POLICY HOST DEST REFERENCES DATA MODEL ERROR");
	    }
	  g_free (query2);


	  /* Net Source Inet Address */
	  query2 = g_strdup_printf ("SELECT host_ip FROM policy_net_reference, net_host_reference WHERE policy_net_reference.net_name = net_host_reference.net_name AND policy_net_reference.direction = 'source' AND policy_id = %d",
				    sim_policy_get_id (policy));
	  dm2 = sim_database_execute_single_command (database, query2);
	  if (dm2)
	    {
	      for (row2 = 0; row2 < gda_data_model_get_n_rows (dm2); row2++)
		{
		  gchar *src_ip;

		  value = (GdaValue *) gda_data_model_get_value_at (dm2, 0, row2);
		  src_ip = gda_value_stringify (value);

		  if (!g_ascii_strncasecmp (src_ip, SIM_IN_ADDR_ANY_CONST, 3))
		    ia = gnet_inetaddr_new_nonblock (SIM_IN_ADDR_ANY_IP_STR, 0);
		  else
		    ia = gnet_inetaddr_new_nonblock (src_ip, 0);

		  sim_policy_append_src_ia (policy, ia);

		  g_free (src_ip);
		}
	      g_object_unref(dm2);
	    }
	  else
	    {
	      g_message ("POLICY NET SOURCE REFERENCES DATA MODEL ERROR");
	    }
	  g_free (query2);


	  /* Net Destination Inet Address */
	  query2 = g_strdup_printf ("SELECT host_ip FROM policy_net_reference, net_host_reference WHERE policy_net_reference.net_name = net_host_reference.net_name AND policy_net_reference.direction = 'dest' AND policy_id = %d",
				    sim_policy_get_id (policy));
	  dm2 = sim_database_execute_single_command (database, query2);
	  if (dm2)
	    {
	      for (row2 = 0; row2 < gda_data_model_get_n_rows (dm2); row2++)
		{
		  gchar *dst_ip;

		  value = (GdaValue *) gda_data_model_get_value_at (dm2, 0, row2);
		  dst_ip = gda_value_stringify (value);

		  if (!g_ascii_strncasecmp (dst_ip, SIM_IN_ADDR_ANY_CONST, 3))
		    ia = gnet_inetaddr_new_nonblock (SIM_IN_ADDR_ANY_IP_STR, 0);
		  else
		    ia = gnet_inetaddr_new_nonblock (dst_ip, 0);

		  sim_policy_append_dst_ia (policy, ia);

		  g_free (dst_ip);
		}
	      g_object_unref(dm2);
	    }
	  else
	    {
	      g_message ("POLICY NET DEST REFERENCES DATA MODEL ERROR");
	    }
	  g_free (query2);

	  /* Signatures */
	  query2 = g_strdup_printf ("SELECT sig_name FROM policy_sig_reference, signature_group_reference WHERE policy_sig_reference.sig_group_name = signature_group_reference.sig_group_name AND policy_sig_reference.policy_id = %d",
				    sim_policy_get_id (policy));
	  dm2 = sim_database_execute_single_command (database, query2);
	  if (dm2)
	    {
	      for (row2 = 0; row2 < gda_data_model_get_n_rows (dm2); row2++)
		{
		  gchar *signature;

		  value = (GdaValue *) gda_data_model_get_value_at (dm2, 0, row2);
		  signature = gda_value_stringify (value);

		  sim_policy_append_signature (policy, signature);

		  g_free (signature);
		}
	      g_object_unref(dm2);
	    }
	  else
	    {
	      g_message ("POLICY SIGNATURE REFERENCES DATA MODEL ERROR");
	    }
	  g_free (query2);

	  sim_container_append_policy (container, policy);
	}
      
      g_object_unref(dm);
    }
  else
    {
      g_message ("POLICY DATA MODEL ERROR");
    }
}

/*
 *
 *
 *
 *
 */
void
sim_container_append_policy (SimContainer  *container,
			   SimPolicy       *policy)
{
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (policy);
  g_return_if_fail (SIM_IS_POLICY (policy));

  g_static_mutex_lock (&mutex);
  container->_priv->policies = g_list_append (container->_priv->policies, policy);
  g_static_mutex_unlock (&mutex);
}

/*
 *
 *
 *
 *
 */
void
sim_container_remove_policy (SimContainer  *container,
			   SimPolicy       *policy)
{
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (policy);
  g_return_if_fail (SIM_IS_POLICY (policy));

  g_static_mutex_lock (&mutex);
  container->_priv->policies = g_list_remove (container->_priv->policies, policy);
  g_static_mutex_unlock (&mutex);
}

/*
 *
 *
 *
 *
 */
GList*
sim_container_get_policies (SimContainer  *container)
{
  GList *list;
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);

  g_static_mutex_lock (&mutex);
  list = g_list_copy (container->_priv->policies);
  g_static_mutex_unlock (&mutex);

  return list;
}

/*
 *
 *
 *
 */
SimPolicy*
sim_container_get_policy_match (SimContainer     *container,
				gint              date,
				GInetAddr        *src_ip,
				GInetAddr        *dst_ip,
				SimPortProtocol  *port,
				gchar            *signature)
{
  SimPolicy  *policy;
  GList      *list;
  gboolean    found = FALSE;
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_val_if_fail (container != NULL, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);
  g_return_val_if_fail (src_ip != NULL, NULL);
  g_return_val_if_fail (dst_ip != NULL, NULL);
  g_return_val_if_fail (port != NULL, NULL);
  g_return_val_if_fail (signature != NULL, NULL);

  g_static_mutex_lock (&mutex);
  list = container->_priv->policies;
  while (list)
    {
      policy = (SimPolicy *) list->data;

      if (sim_policy_match (policy, date, src_ip, dst_ip, port, signature))
	{
	  found = TRUE;
	  break;
	}

      list = list->next;
    }
  g_static_mutex_unlock (&mutex);

  if (!found)
    return FALSE;

  return policy;
}

/*
 *
 *
 *
 *
 */
void
sim_container_load_directives_from_file (SimContainer  *container,
					 const gchar   *filename)
{
  SimXmlDirective *xml_directive;

  xml_directive = sim_xml_directive_new_from_file (container, filename);
  container->_priv->directives = sim_xml_directive_get_directives (xml_directive);
}

/*
 *
 *
 *
 *
 */
void
sim_container_append_directive (SimContainer  *container,
				SimDirective  *directive)
{
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (directive);
  g_return_if_fail (SIM_IS_DIRECTIVE (directive));
  
  g_static_mutex_lock (&mutex);
  container->_priv->directives = g_list_append (container->_priv->directives, directive);
  g_static_mutex_unlock (&mutex);
}

/*
 *
 *
 *
 *
 */
void
sim_container_remove_directive (SimContainer  *container,
				SimDirective  *directive)
{
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (directive);
  g_return_if_fail (SIM_IS_DIRECTIVE (directive));
  
  g_static_mutex_lock (&mutex);
  container->_priv->directives = g_list_remove (container->_priv->directives, directive);
  g_static_mutex_unlock (&mutex);
}

/*
 *
 *
 *
 *
 */
GList*
sim_container_get_directives (SimContainer  *container)
{
  GList *list;
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;
  
  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);

  g_static_mutex_lock (&mutex);
  list = g_list_copy (container->_priv->directives);
  g_static_mutex_unlock (&mutex);
  
  return list;
}

/*
 *
 *
 *
 */
void
sim_container_db_load_host_levels  (SimContainer  *container,
				    SimDatabase   *database)
{
  SimHostLevel  *host_level;
  GdaDataModel  *dm;
  gint           row;
  gchar         *query = "SELECT * FROM host_qualification";

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));

  dm = sim_database_execute_single_command (database, query);
  if (dm)
    {
      for (row = 0; row < gda_data_model_get_n_rows (dm); row++)
	{
	  host_level  = sim_host_level_new_from_dm (dm, row);	  
	  sim_container_append_host_level (container, host_level);
	}

      g_object_unref(dm);
    }
  else
    {
      g_message ("HOST LEVELS DATA MODEL ERROR");
    }
}

/*
 *
 *
 *
 *
 */
void
sim_container_db_insert_host_level (SimContainer  *container,
				    SimDatabase   *database,
				    SimHostLevel  *host_level)
{
  gchar *query;
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));
  g_return_if_fail (host_level);
  g_return_if_fail (SIM_IS_HOST_LEVEL (host_level));

  g_static_mutex_lock (&mutex);
  query = sim_host_level_get_insert_clause (host_level);
  sim_database_execute_no_query (database, query);
  g_static_mutex_unlock (&mutex);
}

/*
 *
 *
 *
 *
 */
void 
sim_container_db_update_host_level (SimContainer  *container,
				    SimDatabase   *database,
				    SimHostLevel  *host_level)
{
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;
  gchar *query;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));
  g_return_if_fail (host_level);
  g_return_if_fail (SIM_IS_HOST_LEVEL (host_level));

  g_static_mutex_lock (&mutex);
  query = sim_host_level_get_update_clause (host_level);
  sim_database_execute_no_query (database, query);
  g_static_mutex_unlock (&mutex);
}

/*
 *
 *
 *
 *
 */
void 
sim_container_db_delete_host_level (SimContainer  *container,
				    SimDatabase   *database,
				    SimHostLevel  *host_level)
{
  gchar *query;
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));
  g_return_if_fail (host_level);
  g_return_if_fail (SIM_IS_HOST_LEVEL (host_level));

  g_static_mutex_lock (&mutex);
  query = sim_host_level_get_delete_clause (host_level);
  sim_database_execute_no_query (database, query);
  g_static_mutex_unlock (&mutex);
}

/*
 *
 *
 *
 *
 */
void
sim_container_append_host_level (SimContainer  *container,
				 SimHostLevel  *host_level)
{
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (host_level);
  g_return_if_fail (SIM_IS_HOST_LEVEL (host_level));

  g_static_mutex_lock (&mutex);
  container->_priv->host_levels = g_list_append (container->_priv->host_levels, host_level);
  g_static_mutex_unlock (&mutex);
}

/*
 *
 *
 *
 *
 */
void
sim_container_remove_host_level (SimContainer  *container,
				 SimHostLevel  *host_level)
{
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (host_level);
  g_return_if_fail (SIM_IS_HOST_LEVEL (host_level));

  g_static_mutex_lock (&mutex);
  container->_priv->host_levels = g_list_remove (container->_priv->host_levels, host_level);
  g_static_mutex_unlock (&mutex);
}

/*
 *
 *
 *
 *
 */
GList*
sim_container_get_host_levels (SimContainer  *container)
{
  GList *list;
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);

  g_static_mutex_lock (&mutex);
  list = g_list_copy (container->_priv->host_levels);
  g_static_mutex_unlock (&mutex);

  return list;
}

/*
 *
 *
 *
 *
 */
SimHostLevel*
sim_container_get_host_level_by_ia (SimContainer  *container,
				    GInetAddr     *ia)
{
  SimHostLevel  *host_level;
  GList         *list;
  gboolean       found = FALSE;
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);
  g_return_val_if_fail (ia, NULL);

  g_static_mutex_lock (&mutex);
  list = container->_priv->host_levels;
  while (list)
    {
      host_level = (SimHostLevel *) list->data;

      if (gnet_inetaddr_noport_equal (sim_host_level_get_ia (host_level), ia))
	{
	  found = TRUE;
	  break;
	}

      list = list->next;
    }
  g_static_mutex_unlock (&mutex);

  if (!found)
    return NULL;

  return host_level;
}

/*
 *
 *
 *
 */
void
sim_container_set_host_levels_recovery (SimContainer  *container,
					SimDatabase   *database,
					gint           recovery)
{
  GList           *list;
  gint             c;
  gint             a;
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_if_fail (container != NULL);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (recovery > 0);

  g_static_mutex_lock (&mutex);
  list = g_list_copy (container->_priv->host_levels);
  while (list)
    {
      SimHostLevel *host_level = (SimHostLevel *) list->data;

      sim_host_level_set_recovery (host_level, recovery); /* Update Memory */

      c = sim_host_level_get_c (host_level);
      a = sim_host_level_get_a (host_level);

      if (c == 0 && a == 0)
	{
	  container->_priv->host_levels = g_list_remove (container->_priv->host_levels, host_level); /* Delete Container List */
	  sim_container_db_delete_host_level (container, database, host_level); /* Delete DB */
	}
      else
	{
	  sim_container_db_update_host_level (container, database, host_level); /* Update DB */
	}

      list = list->next;
    }
  g_list_free (list);
  g_static_mutex_unlock (&mutex);
}

/*
 *
 *
 *
 */
void
sim_container_db_load_net_levels (SimContainer  *container,
				  SimDatabase   *database)
{
  SimNetLevel   *net_level;
  GdaDataModel  *dm;
  gint           row;
  gchar         *query = "SELECT * FROM net_qualification";
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));

  dm = sim_database_execute_single_command (database, query);
  if (dm)
    {
      for (row = 0; row < gda_data_model_get_n_rows (dm); row++)
	{
	  net_level  = sim_net_level_new_from_dm (dm, row);	  
	  sim_container_append_net_level (container, net_level);
	}

      g_object_unref(dm);
    }
  else
    {
      g_message ("NET LEVELS DATA MODEL ERROR");
    }
}

/*
 *
 *
 *
 *
 */
void
sim_container_db_insert_net_level (SimContainer  *container,
				   SimDatabase   *database,
				   SimNetLevel   *net_level)
{
  gchar *query;
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));
  g_return_if_fail (net_level);
  g_return_if_fail (SIM_IS_NET_LEVEL (net_level));

  g_static_mutex_lock (&mutex);
  query = sim_net_level_get_insert_clause (net_level);
  sim_database_execute_no_query (database, query);
  g_static_mutex_unlock (&mutex);
}

/*
 *
 *
 *
 *
 */
void 
sim_container_db_update_net_level (SimContainer  *container,
				   SimDatabase   *database,
				   SimNetLevel   *net_level)
{
  gchar *query;
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));
  g_return_if_fail (net_level);
  g_return_if_fail (SIM_IS_NET_LEVEL (net_level));

  g_static_mutex_lock (&mutex);
  query = sim_net_level_get_update_clause (net_level);
  sim_database_execute_no_query (database, query);
  g_static_mutex_unlock (&mutex);
}

/*
 *
 *
 *
 *
 */
void 
sim_container_db_delete_net_level (SimContainer  *container,
				   SimDatabase   *database,
				   SimNetLevel   *net_level)
{
  gchar *query;
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));
  g_return_if_fail (net_level);
  g_return_if_fail (SIM_IS_NET_LEVEL (net_level));

  g_static_mutex_lock (&mutex);
  query = sim_net_level_get_delete_clause (net_level);
  sim_database_execute_no_query (database, query);
  g_static_mutex_unlock (&mutex);
}

/*
 *
 *
 *
 *
 */
void
sim_container_append_net_level (SimContainer  *container,
				SimNetLevel   *net_level)
{
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (net_level);
  g_return_if_fail (SIM_IS_NET_LEVEL (net_level));

  g_static_mutex_lock (&mutex);
  container->_priv->net_levels = g_list_append (container->_priv->net_levels, net_level);
  g_static_mutex_unlock (&mutex);
}

/*
 *
 *
 *
 *
 */
void
sim_container_remove_net_level (SimContainer  *container,
				SimNetLevel   *net_level)
{
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (net_level);
  g_return_if_fail (SIM_IS_NET_LEVEL (net_level));

  g_static_mutex_lock (&mutex);
  container->_priv->net_levels = g_list_remove (container->_priv->net_levels, net_level);
  g_static_mutex_unlock (&mutex);
}

/*
 *
 *
 *
 *
 */
GList*
sim_container_get_net_levels (SimContainer  *container)
{
  GList *list;
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);

  g_static_mutex_lock (&mutex);
  list = g_list_copy (container->_priv->net_levels);
  g_static_mutex_unlock (&mutex);

  return list;
}

/*
 *
 *
 *
 *
 */
SimNetLevel*
sim_container_get_net_level_by_name (SimContainer  *container,
				     gchar         *name)
{
  SimNetLevel  *net_level;
  GList        *list;
  gboolean      found = FALSE;
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);
  g_return_val_if_fail (name, NULL);

  g_static_mutex_lock (&mutex);
  list = container->_priv->net_levels;
  while (list)
    {
      net_level = (SimNetLevel *) list->data;

      if (!strcmp (sim_net_level_get_name (net_level), name))
	{
	  found = TRUE;
	  break;
	}

      list = list->next;
    }
  g_static_mutex_unlock (&mutex);

  if (!found)
    return NULL;

  return net_level;
}

/*
 *
 *
 *
 */
void
sim_container_set_net_levels_recovery (SimContainer  *container,
				       SimDatabase   *database,
				       gint           recovery)
{
  GList           *list;
  gint             c;
  gint             a;
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_if_fail (container != NULL);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (recovery > 0);

  g_static_mutex_lock (&mutex);
  list = g_list_copy (container->_priv->net_levels);
  while (list)
    {
      SimNetLevel *net_level = (SimNetLevel *) list->data;

      sim_net_level_set_recovery (net_level, recovery); /* Update Memory */

      c = sim_net_level_get_c (net_level);
      a = sim_net_level_get_a (net_level);

      if (c == 0 && a == 0)
	{
	  /* Fix this in the PostgreSQL version */
	  //container->_priv->net_levels = g_list_remove (container->_priv->net_levels, net_level); /* Delete Container List */
	  //sim_container_db_delete_net_level (container, database, net_level); /* Delete DB */
	  sim_container_db_update_net_level (container, database, net_level); /* Update DB */
	}
      else
	{
	  sim_container_db_update_net_level (container, database, net_level); /* Update DB */
	}

      list = list->next;
    }
  g_list_free (list);
  g_static_mutex_unlock (&mutex);
}

/*
 *
 *
 *
 *
 */
void
sim_container_db_insert_backlog (SimContainer  *container,
				 SimDatabase   *database,
				 SimDirective  *backlog)
{
  gchar *query;
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));
  g_return_if_fail (backlog);
  g_return_if_fail (SIM_IS_DIRECTIVE (backlog));

  g_static_mutex_lock (&mutex);
  query = sim_directive_backlog_get_insert_clause (backlog);
  sim_database_execute_no_query (database, query);
  g_static_mutex_unlock (&mutex);
}

/*
 *
 *
 *
 *
 */
void 
sim_container_db_update_backlog (SimContainer  *container,
				 SimDatabase   *database,
				 SimDirective   *backlog)
{
  gchar *query;
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));
  g_return_if_fail (backlog);
  g_return_if_fail (SIM_IS_DIRECTIVE (backlog));

  g_static_mutex_lock (&mutex);
  query = sim_directive_backlog_get_update_clause (backlog);
  sim_database_execute_no_query (database, query);
  g_static_mutex_unlock (&mutex);
}

/*
 *
 *
 *
 *
 */
void 
sim_container_db_delete_backlog (SimContainer  *container,
				 SimDatabase   *database,
				 SimDirective   *backlog)
{
  gchar *query;
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));
  g_return_if_fail (backlog);
  g_return_if_fail (SIM_IS_DIRECTIVE (backlog));

  g_static_mutex_lock (&mutex);
  query = sim_directive_backlog_get_delete_clause (backlog);
  sim_database_execute_no_query (database, query);
  g_static_mutex_unlock (&mutex);
}

/*
 *
 *
 *
 *
 */
void
sim_container_append_backlog (SimContainer  *container,
			      SimDirective  *backlog)
{
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (backlog);
  g_return_if_fail (SIM_IS_DIRECTIVE (backlog));
  
  g_static_mutex_lock (&mutex);
  container->_priv->backlogs = g_list_append (container->_priv->backlogs, backlog);
  g_static_mutex_unlock (&mutex);
}

/*
 *
 *
 *
 *
 */
void
sim_container_remove_backlog (SimContainer  *container,
			      SimDirective  *backlog)
{
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (backlog);
  g_return_if_fail (SIM_IS_DIRECTIVE (backlog));
  
  g_static_mutex_lock (&mutex);
  container->_priv->backlogs = g_list_remove (container->_priv->backlogs, backlog);
  g_static_mutex_unlock (&mutex);
}

/*
 *
 *
 *
 *
 */
GList*
sim_container_get_backlogs (SimContainer  *container)
{
  GList *list;
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;
  
  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);

  g_static_mutex_lock (&mutex);
  list = g_list_copy (container->_priv->backlogs);
  g_static_mutex_unlock (&mutex);
  
  return list;
}

/*
 *
 *
 *
 *
 */
void
sim_container_time_out_backlogs (SimContainer  *container,
				 SimDatabase   *database)
{
  GList         *list;
  GTimeVal       curr_time;
  GTime          time_last;
  GTime          time_out;
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));
  
  g_static_mutex_lock (&mutex);
  list = g_list_copy (container->_priv->backlogs);
  while (list)
    {
      SimDirective *backlog = (SimDirective *) list->data;

      GTime time_out = sim_directive_get_time_out (backlog);
      GTime time_last = sim_directive_get_time_last (backlog);
      gint level = sim_directive_get_level (backlog);

      g_get_current_time (&curr_time);

      if (curr_time.tv_sec >= (time_last + time_out))
	{
	  container->_priv->backlogs = g_list_remove (container->_priv->backlogs, backlog);

	  if (level <= 1)
	    sim_container_db_delete_backlog (container, database, backlog);

	  g_object_unref (backlog);
	}

      list = list->next;
    }
  g_list_free (list);
  g_static_mutex_unlock (&mutex);
}


/*
 *
 *
 *
 *
 */
void
sim_container_push_message (SimContainer  *container,
			    SimMessage    *message)
{
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (message);
  g_return_if_fail (SIM_IS_MESSAGE (message));

  g_async_queue_push (container->_priv->messages, message);
}

/*
 *
 *
 *
 *
 */
SimMessage*
sim_container_pop_message (SimContainer  *container)
{
  SimMessage *message;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);

  message = (SimMessage *) g_async_queue_pop (container->_priv->messages);

  return message;
}

/*
 *
 *
 *
 *
 */
gboolean
sim_container_is_empty_messages (SimContainer  *container)
{
  gboolean empty;

  g_return_val_if_fail (container, TRUE);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), TRUE);

  if (g_async_queue_length (container->_priv->messages))
    empty = FALSE;
  else
    empty = TRUE;

  return empty;
}

/*
 *
 *
 *
 *
 */
gint
sim_container_length_messages (SimContainer  *container)
{
  gint legth;

  g_return_val_if_fail (container, 0);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), 0);

  legth = g_async_queue_length (container->_priv->messages);

  return legth;
}
