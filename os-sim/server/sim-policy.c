/**
 *
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <config.h>

#include "sim-database.h"
#include "sim-policy.h"
 
enum 
{
  DESTROY,
  LAST_SIGNAL
};

struct _SimPolicyPrivate {
  gint   id;
  gchar *description;

  gint   priority;
  gint   begin_hour;
  gint   end_hour;
  gint   begin_day;
  gint   end_day;

  GList *src_ips;
  GList *dst_ips;
  GList *ports;
  GList *signatures;
  GList *sensors;
};

struct {
  gint     port;
  
  GList   *protocols;
} SimPolicyPort;

static gpointer parent_class = NULL;
static gint sim_policy_signals[LAST_SIGNAL] = { 0 };

/* GType Functions */

static void
sim_policy_class_init (SimPolicyClass * class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  parent_class = g_type_class_peek_parent (class);
}

static void
sim_policy_instance_init (SimPolicy *policy)
{
  policy->_priv = g_new0 (SimPolicyPrivate, 1);

  policy->_priv->src_ips = NULL;
  policy->_priv->dst_ips = NULL;
  policy->_priv->ports = NULL;
  policy->_priv->signatures = NULL;
  policy->_priv->sensors = NULL;
}

/* Public Methods */

GType
sim_policy_get_type (void)
{
  static GType object_type = 0;
 
  if (!object_type)
  {
    static const GTypeInfo type_info = {
              sizeof (SimPolicyClass),
              NULL,
              NULL,
              (GClassInitFunc) sim_policy_class_init,
              NULL,
              NULL,                       /* class data */
              sizeof (SimPolicy),
              0,                          /* number of pre-allocs */
              (GInstanceInitFunc) sim_policy_instance_init,
              NULL                        /* value table */
    };
    
    g_type_init ();
                                                                                                                             
    object_type = g_type_register_static (G_TYPE_OBJECT, "SimPolicy", &type_info, 0);
  }
                                                                                                                             
  return object_type;
}

/*
 *
 *
 *
 *
 */
SimPolicy *
sim_policy_new (void)
{
  SimPolicy *policy = NULL;

  policy = SIM_POLICY (g_object_new (SIM_TYPE_POLICY, NULL));

  return policy;
}

/**
 *
 *
 *
 *
 */
GList*
sim_policy_load_from_db (GObject  *db)
{
  SimPolicy *policy;
  GdaDataModel *dm;
  GdaDataModel *dm1;
  GdaValue *value;
  GList *policies = NULL;
  GList *list = NULL;
  GList *list2 = NULL;
  GList *node0 = NULL;
  GList *node = NULL; 
  GList *node2 = NULL;
  gchar *query2 = NULL;
  gint row_id;
  gint row_id1;
  gint i;

  gchar *query = "select * from policy";

  g_return_if_fail (db != NULL);
  g_return_if_fail (SIM_IS_DATABASE (db));

  /* List of policies */
  list = sim_database_execute_command (SIM_DATABASE (db), query);
  if (list != NULL)
    {
      for (node = g_list_first (list); node != NULL; node = g_list_next (node))
	{
	  dm = (GdaDataModel *) node->data;
	  if (dm == NULL)
	    {
	      g_message ("POLICIES DATA MODEL ERROR");
	    }
	  else
	    {
	      for (row_id = 0; row_id < gda_data_model_get_n_rows (dm); row_id++)
		{
		  /* New policy */
		  policy  = sim_policy_new ();

		  /* Set id*/
		  value = (GdaValue *) gda_data_model_get_value_at (dm, 0, row_id);
		  policy->_priv->id = gda_value_get_integer (value);

		  /* Set  priority */
		  value = (GdaValue *) gda_data_model_get_value_at (dm, 1, row_id);
		  policy->_priv->priority = gda_value_get_smallint (value);

		  /* Set description */
		  value = (GdaValue *) gda_data_model_get_value_at (dm, 2, row_id);
		  policy->_priv->description = gda_value_stringify (value);

		  /* Added policy */
		  policies = g_list_append (policies, policy);
		}

	      g_object_unref(dm);
	    }
	}
    }
  else
    {
      g_message ("POLICIES LIST ERROR");
    }

  for (i = 0; i < g_list_length (policies); i++)
    {
      node0 = g_list_nth (policies, i);
      policy = (SimPolicy *) node0->data;

      /* Gets ip, source */
      query = g_strdup_printf ("select * from policy_host_reference  where policy_id = %d and direction = 'source'",
			       policy->_priv->id);

      list = sim_database_execute_command (SIM_DATABASE (db), query);
      if (list != NULL)
	{
	  for (node = g_list_first (list); node != NULL; node = g_list_next (node))
	    {
	      dm = (GdaDataModel *) node->data;
	      if (dm == NULL)
		{
		  g_message ("POLICIES DATA MODEL ERROR 1");
		}
	      else
		{
		  for (row_id = 0; row_id < gda_data_model_get_n_rows (dm); row_id++)
		    {
		      gchar *source_ip;

		      /* Set source ip */
		      value = (GdaValue *) gda_data_model_get_value_at (dm, 1, row_id);
		      source_ip = gda_value_stringify (value);
 
		      if (!g_ascii_strncasecmp (source_ip, "any", 3))
			{
			  g_free (source_ip);
			  source_ip = g_strdup ("0.0.0.0");
			}
   
		      /* Added source ip */
		      policy->_priv->src_ips = g_list_append (policy->_priv->src_ips, source_ip);
		    }
		  
		  g_object_unref(dm);
		}
	    }
	}
      else
	{
	  g_message ("POLICIES LIST ERROR 1");
	}
      g_free (query);


      /* Gets ip, dest */
      query = g_strdup_printf ("select * from policy_host_reference  where policy_id = %d and direction = 'dest'",
			       policy->_priv->id);
      
      list = sim_database_execute_command (SIM_DATABASE (db), query);
      if (list != NULL)
	{
	  for (node = g_list_first (list); node != NULL; node = g_list_next (node))
	    {
	      dm = (GdaDataModel *) node->data;
	      if (dm == NULL)
		{
		  g_message ("POLICIES DATA MODEL ERROR 1");
		}
	      else
		{
		  for (row_id = 0; row_id < gda_data_model_get_n_rows (dm); row_id++)
		    {
		      gchar *dest_ip;

		      /* Set dest ip */
		      value = (GdaValue *) gda_data_model_get_value_at (dm, 1, row_id);
		      dest_ip = gda_value_stringify (value);
    
		      if (!g_ascii_strncasecmp (dest_ip, "any", 3))
			{
			  g_free (dest_ip);
			  dest_ip = g_strdup ("0.0.0.0");
			}
   
		      /* Added dest ip */
		      policy->_priv->dst_ips = g_list_append (policy->_priv->dst_ips, dest_ip);
		    }
		  
		  g_object_unref(dm);
		}
	    }
	}
      else
	{
	  g_message ("POLICIES LIST ERROR 1");
	}
      g_free (query);


      /* Gets sensor */
      query = g_strdup_printf ("select * from policy_sensor_reference where policy_id = %d",
			       policy->_priv->id);
      
      list = sim_database_execute_command (SIM_DATABASE (db), query);
      if (list != NULL)
	{
	  for (node = g_list_first (list); node != NULL; node = g_list_next (node))
	    {
	      dm = (GdaDataModel *) node->data;
	      if (dm == NULL)
		{
		  g_message ("POLICIES DATA MODEL ERROR 1");
		}
	      else
		{
		  for (row_id = 0; row_id < gda_data_model_get_n_rows (dm); row_id++)
		    {
		      gchar *sensor;

		      /* Set sensor */
		      value = (GdaValue *) gda_data_model_get_value_at (dm, 1, row_id);
		      sensor = gda_value_stringify (value);
    
		      /* Added sensor */
		      policy->_priv->sensors = g_list_append (policy->_priv->sensors, sensor);
		    }
		  
		  g_object_unref(dm);
		}
	    }
	}
      else
	{
	  g_message ("POLICIES LIST ERROR 1");
	}
      g_free (query);


      /* Get ports */
      query = g_strdup_printf ("select * from policy_port_reference  where policy_id = %d",
			       policy->_priv->id);
      
      list = sim_database_execute_command (SIM_DATABASE (db), query);
      if (list != NULL)
	{
	  for (node = g_list_first (list); node != NULL; node = g_list_next (node))
	    {
	      dm = (GdaDataModel *) node->data;
	      if (dm == NULL)
		{
		  g_message ("POLICIES DATA MODEL ERROR 1");
		}
	      else
		{
		  for (row_id = 0; row_id < gda_data_model_get_n_rows (dm); row_id++)
		    {
		      gchar *str;

		      value = (GdaValue *) gda_data_model_get_value_at (dm, 1, row_id);
		      str = gda_value_stringify (value);

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
				      gchar  *protocol, *str;
				      gint    port;

				      /* Set port */
				      value = (GdaValue *) gda_data_model_get_value_at (dm1, 1, row_id1);
				      port = gda_value_get_integer (value);
				      
				      /* Set protocol */
				      value = (GdaValue *) gda_data_model_get_value_at (dm1, 2, row_id1);
				      protocol = gda_value_stringify (value);
				      
				      str = g_strdup_printf ("%d/%s", port, protocol);
				      
				      /* Added port */
				      policy->_priv->ports = g_list_append (policy->_priv->ports, str);

				      g_free (protocol);
				    }
				  
				  g_object_unref(dm1);
				}
			    }
			}

		      g_free (str);
		    }
		  
		  g_object_unref(dm);
		}
	    }
	}
      else
	{
	  g_message ("POLICIES LIST ERROR 1");
	}
      g_free (query);


      /* Get signatures */
      query = g_strdup_printf ("select * from policy_sig_reference where policy_id = %d",
			       policy->_priv->id);
      
      list = sim_database_execute_command (SIM_DATABASE (db), query);
      if (list != NULL)
	{
	  for (node = g_list_first (list); node != NULL; node = g_list_next (node))
	    {
	      dm = (GdaDataModel *) node->data;
	      if (dm == NULL)
		{
		  g_message ("POLICIES DATA MODEL ERROR 1");
		}
	      else
		{
		  for (row_id = 0; row_id < gda_data_model_get_n_rows (dm); row_id++)
		    {
		      gchar *str;

		      value = (GdaValue *) gda_data_model_get_value_at (dm, 1, row_id);
		      str = gda_value_stringify (value);

		      query2 = g_strdup_printf ("select * from signature_group_reference where sig_group_name = '%s'", str);

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
				      gchar *sig;
				      
				      /* Set sig */
				      value = (GdaValue *) gda_data_model_get_value_at (dm1, 1, row_id1);
				      sig = gda_value_stringify (value);
				      
				      /* Added sig */
				      policy->_priv->signatures = g_list_append (policy->_priv->signatures, sig);
				    }
				  
				  g_object_unref(dm1);
				}
			    }
			}

		      g_free (str);
		    }
		  
		  g_object_unref(dm);
		}
	    }
	}
      else
	{
	  g_message ("POLICIES LIST ERROR 1");
	}
      g_free (query);

      /* Get sources nets */
      query = g_strdup_printf ("select * from policy_net_reference  where policy_id = %d and direction = 'source'",
			       policy->_priv->id);
      
      list = sim_database_execute_command (SIM_DATABASE (db), query);
      if (list != NULL)
	{
	  for (node = g_list_first (list); node != NULL; node = g_list_next (node))
	    {
	      dm = (GdaDataModel *) node->data;
	      if (dm == NULL)
		{
		  g_message ("POLICIES DATA MODEL ERROR 1");
		}
	      else
		{
		  for (row_id = 0; row_id < gda_data_model_get_n_rows (dm); row_id++)
		    {
		      gchar *str;

		      value = (GdaValue *) gda_data_model_get_value_at (dm, 1, row_id);
		      str = gda_value_stringify (value);

		      query2 = g_strdup_printf ("select * from net_host_reference where net_name = '%s'", str);

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
				      gchar *net;
				      
				      /* Set net */
				      value = (GdaValue *) gda_data_model_get_value_at (dm1, 1, row_id1);
				      net = gda_value_stringify (value);
				      
				      /* Added net */
				      policy->_priv->src_ips = g_list_append (policy->_priv->src_ips, net);
				    }
				  
				  g_object_unref(dm1);
				}
			    }
			}

		      g_free (str);
		    }
		  
		  g_object_unref(dm);
		}
	    }
	}
      else
	{
	  g_message ("POLICIES LIST ERROR 1");
	}
      g_free (query);


      /* Get dest  nets */
      query = g_strdup_printf ("select * from policy_net_reference  where policy_id = %d and direction = 'dest'",
			       policy->_priv->id);
      
      list = sim_database_execute_command (SIM_DATABASE (db), query);
      if (list != NULL)
	{
	  for (node = g_list_first (list); node != NULL; node = g_list_next (node))
	    {
	      dm = (GdaDataModel *) node->data;
	      if (dm == NULL)
		{
		  g_message ("POLICIES DATA MODEL ERROR 1");
		}
	      else
		{
		  for (row_id = 0; row_id < gda_data_model_get_n_rows (dm); row_id++)
		    {
		      gchar *str;

		      value = (GdaValue *) gda_data_model_get_value_at (dm, 1, row_id);
		      str = gda_value_stringify (value);

		      query2 = g_strdup_printf ("select * from net_host_reference where net_name = '%s'", str);

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
				      gchar *net;
				      
				      /* Set net */
				      value = (GdaValue *) gda_data_model_get_value_at (dm1, 1, row_id1);
				      net = gda_value_stringify (value);
				      
				      /* Added net */
				      policy->_priv->dst_ips = g_list_append (policy->_priv->dst_ips, net);
				    }
				  
				  g_object_unref(dm1);
				}
			    }
			}

		      g_free (str);
		    }
		  
		  g_object_unref(dm);
		}
	    }
	}
      else
	{
	  g_message ("POLICIES LIST ERROR 1");
	}
      g_free (query);


      /* Get timeframe */
      query = g_strdup_printf ("select * from policy_time where policy_id = %d",
			       policy->_priv->id);
      
      list = sim_database_execute_command (SIM_DATABASE (db), query);
      if (list != NULL)
	{
	  for (node = g_list_first (list); node != NULL; node = g_list_next (node))
	    {
	      dm = (GdaDataModel *) node->data;
	      if (dm == NULL)
		{
		  g_message ("POLICIES DATA MODEL ERROR 1");
		}
	      else
		{
		  for (row_id = 0; row_id < gda_data_model_get_n_rows (dm); row_id++)
		    {
		      /* Set begin_hour */
		      value = (GdaValue *) gda_data_model_get_value_at (dm, 1, row_id);
		      policy->_priv->begin_hour = gda_value_get_smallint (value);

		      /* Set end_hour */
		      value = (GdaValue *) gda_data_model_get_value_at (dm, 2, row_id);
		      policy->_priv->end_hour = gda_value_get_smallint (value);

		      /* Set begin_day */
		      value = (GdaValue *) gda_data_model_get_value_at (dm, 3, row_id);
		      policy->_priv->begin_day = gda_value_get_smallint (value);

		      /* Set end_day */
		      value = (GdaValue *) gda_data_model_get_value_at (dm, 4, row_id);
		      policy->_priv->end_day = gda_value_get_smallint (value);
		    }
		  
		  g_object_unref(dm);
		}
	    }
	}
      else
	{
	  g_message ("POLICIES LIST ERROR 1");
	}
      g_free (query);

    }

  return policies;
}

/*
 *
 *
 *
 */
gint
sim_policy_get_priority (SimPolicy* policy)
{
  g_return_val_if_fail (policy != NULL, 0);
  g_return_val_if_fail (SIM_IS_POLICY (policy), 0);

  return policy->_priv->priority;
}

/*
 *
 *
 *
 */
void
sim_policy_set_priority (SimPolicy* policy,
			 gint       priority)
{
  g_return_if_fail (policy != NULL);
  g_return_if_fail (SIM_IS_POLICY (policy));

  policy->_priv->priority = priority;
}

/*
 *
 *
 *
 */
gint
sim_policy_get_begin_day (SimPolicy* policy)
{
  g_return_val_if_fail (policy != NULL, 0);
  g_return_val_if_fail (SIM_IS_POLICY (policy), 0);

  return policy->_priv->begin_day;
}

/*
 *
 *
 *
 */
void
sim_policy_set_begin_day (SimPolicy* policy,
			 gint       begin_day)
{
  g_return_if_fail (policy != NULL);
  g_return_if_fail (SIM_IS_POLICY (policy));

  policy->_priv->begin_day = begin_day;
}

/*
 *
 *
 *
 */
gint
sim_policy_get_end_day (SimPolicy* policy)
{
  g_return_val_if_fail (policy != NULL, 0);
  g_return_val_if_fail (SIM_IS_POLICY (policy), 0);

  return policy->_priv->end_day;
}

/*
 *
 *
 *
 */
void
sim_policy_set_end_day (SimPolicy* policy,
			 gint       end_day)
{
  g_return_if_fail (policy != NULL);
  g_return_if_fail (SIM_IS_POLICY (policy));

  policy->_priv->end_day = end_day;
}

/*
 *
 *
 *
 */
gint
sim_policy_get_begin_hour (SimPolicy* policy)
{
  g_return_val_if_fail (policy != NULL, 0);
  g_return_val_if_fail (SIM_IS_POLICY (policy), 0);

  return policy->_priv->begin_hour;
}

/*
 *
 *
 *
 */
void
sim_policy_set_begin_hour (SimPolicy* policy,
			 gint       begin_hour)
{
  g_return_if_fail (policy != NULL);
  g_return_if_fail (SIM_IS_POLICY (policy));

  policy->_priv->begin_hour = begin_hour;
}

/*
 *
 *
 *
 */
gint
sim_policy_get_end_hour (SimPolicy* policy)
{
  g_return_val_if_fail (policy != NULL, 0);
  g_return_val_if_fail (SIM_IS_POLICY (policy), 0);

  return policy->_priv->end_hour;
}

/*
 *
 *
 *
 */
void
sim_policy_set_end_hour (SimPolicy* policy,
			 gint       end_hour)
{
  g_return_if_fail (policy != NULL);
  g_return_if_fail (SIM_IS_POLICY (policy));

  policy->_priv->end_hour = end_hour;
}

/*
 *
 *
 *
 */
GList*
sim_policy_get_sources (SimPolicy* policy)
{
  g_return_val_if_fail (policy != NULL, NULL);
  g_return_val_if_fail (SIM_IS_POLICY (policy), NULL);

  return policy->_priv->src_ips;
}

/*
 *
 *
 *
 */
GList*
sim_policy_get_destinations (SimPolicy* policy)
{
  g_return_val_if_fail (policy != NULL, NULL);
  g_return_val_if_fail (SIM_IS_POLICY (policy), NULL);

  return policy->_priv->dst_ips;
}

/*
 *
 *
 *
 */
GList*
sim_policy_get_ports (SimPolicy* policy)
{
  g_return_val_if_fail (policy != NULL, NULL);
  g_return_val_if_fail (SIM_IS_POLICY (policy), NULL);

  return policy->_priv->ports;
}

/*
 *
 *
 *
 */
GList*
sim_policy_get_signatures (SimPolicy* policy)
{
  g_return_val_if_fail (policy != NULL, NULL);
  g_return_val_if_fail (SIM_IS_POLICY (policy), NULL);

  return policy->_priv->signatures;
}

/*
 *
 *
 *
 */
GList*
sim_policy_get_sensors (SimPolicy* policy)
{
  g_return_val_if_fail (policy != NULL, NULL);
  g_return_val_if_fail (SIM_IS_POLICY (policy), NULL);

  return policy->_priv->sensors;
}

/*
 *
 *
 *
 */
gboolean 
sim_policy_match (SimPolicy        *policy,
		  gint              date,
		  gchar            *src_ip,
		  gchar            *dst_ip,
		  gint              port,
		  SimProtocolType   protocol,
		  gchar            *signature)
{
  gboolean   found = FALSE;
  gchar     *str, *sport;
  gchar     *sprotocol = NULL;
  gint       start, end;
  gint       i;

  g_return_val_if_fail (policy != NULL, FALSE);
  g_return_val_if_fail (SIM_IS_POLICY (policy), FALSE);

  start = ((policy->_priv->begin_day - 1) * 7 + policy->_priv->begin_hour);
  end = ((policy->_priv->end_day - 1) * 7 + policy->_priv->end_hour);
  
  if ((start > date) || (end < date))
    return FALSE;
  
  /* Find source ip*/
  found = FALSE;
  for (i = 0; i < g_list_length(policy->_priv->src_ips); i++)
    {
      str = (gchar *) g_list_nth_data (policy->_priv->src_ips, i);
      
      if ((!strcmp (str, "0.0.0.0")) || (!strcmp (str, src_ip)))
	{
	  found = TRUE;
	  break;
	}
    }
  if (!found)
    return FALSE;
  
  /* Find destination ip */
  found = FALSE;
  for (i = 0; i < g_list_length(policy->_priv->dst_ips); i++)
    {
      str = (gchar *) g_list_nth_data (policy->_priv->dst_ips, i);
      
      if ((!strcmp (str, "0.0.0.0")) || (!strcmp (str, dst_ip)))
	{
	  found = TRUE;
	  break;
	}
    }
  if (!found)
    return FALSE;

  switch (protocol)
    {
    case SIM_PROTOCOL_TYPE_ICMP:
      sprotocol = g_strdup ("icmp");
      break;
    case SIM_PROTOCOL_TYPE_UDP:
      sprotocol = g_strdup ("udp");
      break;
    case SIM_PROTOCOL_TYPE_TCP:
      sprotocol = g_strdup ("tcp");
      break;
    default:
      sprotocol = NULL;
      break;
    }
  sport = g_strdup_printf ("%d/%s", port, sprotocol);

  /* Find port */
  found = FALSE;
  for (i = 0; i < g_list_length(policy->_priv->ports); i++)
    {
      str = (gchar *) g_list_nth_data (policy->_priv->ports, i);
      
      if (!strcmp (str, sport))
	{
	  found = TRUE;
	  break;
	}
    }
  g_free (sprotocol);
  g_free (sport);

  if (!found)
    return FALSE;
  
  /* Find signature subgroups  */
  found = FALSE;
  for (i = 0; i < g_list_length(policy->_priv->signatures); i++)
    {
      str = (gchar *) g_list_nth_data (policy->_priv->signatures, i);
      
      if (!strcmp (str, signature))
	{
	  found = TRUE;
	  break;
	}
    }
  if (!found)
    return FALSE;
  
  return TRUE;
}
