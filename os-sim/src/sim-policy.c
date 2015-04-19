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


#include "sim-policy.h"
#include "sim-sensor.h"
#include "sim-inet.h"
#include "sim-event.h"
/*****/
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#ifdef BSD
#define KERNEL
#include <netinet/in.h>
#endif
/*****/
#include <config.h>

enum 
{
  DESTROY,
  LAST_SIGNAL
};

struct _SimPolicyPrivate {
  gint    id;
  gint    priority;
  gchar  *description;

  gint    begin_hour;
  gint    end_hour;
  gint    begin_day;
  gint    end_day;

  SimRole	*role;				//this is not intended to match. This is the behaviour of the events that matches with this policy

  GList  *src;  				// SimInet objects
  GList  *dst;
  GList  *ports;				//port & protocol list, SimPortProtocol object.
  GList  *categories;
  GList  *sensors; 			//gchar* sensor's ip (i.e. "1.1.1.1")
  GList  *plugin_ids; 	//(guint *) list with each one of the plugin_id's
  GList  *plugin_sids;	//
  GList  *plugin_groups;	// *Plugin_PluginSid structs
};

static gpointer parent_class = NULL;
static gint sim_policy_signals[LAST_SIGNAL] = { 0 };

/* GType Functions */

static void 
sim_policy_impl_dispose (GObject  *gobject)
{
  G_OBJECT_CLASS (parent_class)->dispose (gobject);
}

static void 
sim_policy_impl_finalize (GObject  *gobject)
{
  SimPolicy  *policy = SIM_POLICY (gobject);

  if (policy->_priv->description)
    g_free (policy->_priv->description);

  sim_policy_free_src (policy);
  sim_policy_free_dst (policy);
  sim_policy_free_ports (policy);
  sim_policy_free_sensors (policy);
	//FIXME: sim_policy_free_plugin_id y sid.

	if (policy->_priv->role)
		g_free (policy->_priv->role);
	
  g_free (policy->_priv);

  G_OBJECT_CLASS (parent_class)->finalize (gobject);
}

static void
sim_policy_class_init (SimPolicyClass * class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  parent_class = g_type_class_peek_parent (class);

  object_class->dispose = sim_policy_impl_dispose;
  object_class->finalize = sim_policy_impl_finalize;
}

static void
sim_policy_instance_init (SimPolicy *policy)
{
  policy->_priv = g_new0 (SimPolicyPrivate, 1);

  policy->_priv->id = 0;
  policy->_priv->priority = 1;
  policy->_priv->description = NULL;

  policy->_priv->begin_hour = 0;
  policy->_priv->end_hour = 0;
  policy->_priv->begin_day = 0;
  policy->_priv->end_day = 0;

  policy->_priv->src = NULL;
  policy->_priv->dst = NULL;
  policy->_priv->ports = NULL;
  policy->_priv->categories = NULL;
  policy->_priv->sensors = NULL;
  policy->_priv->plugin_ids = NULL;
  policy->_priv->plugin_sids = NULL;
  policy->_priv->plugin_groups = NULL;
 
	policy->_priv->role = g_new0 (SimRole, 1);
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
SimPolicy*
sim_policy_new (void)
{
  SimPolicy *policy;

  policy = SIM_POLICY (g_object_new (SIM_TYPE_POLICY, NULL));

  return policy;
}

/*
 *
 *
 *
 */
SimPolicy*
sim_policy_new_from_dm (GdaDataModel  *dm,
			gint           row)
{
  SimPolicy  *policy;
  GdaValue   *value;

  g_return_val_if_fail (dm, NULL);
  g_return_val_if_fail (GDA_IS_DATA_MODEL (dm), NULL);

  policy = SIM_POLICY (g_object_new (SIM_TYPE_POLICY, NULL));

  value = (GdaValue *) gda_data_model_get_value_at (dm, 0, row);
  policy->_priv->id = gda_value_get_integer (value);
  
  value = (GdaValue *) gda_data_model_get_value_at (dm, 1, row);
  policy->_priv->priority = gda_value_get_smallint (value);

  value = (GdaValue *) gda_data_model_get_value_at (dm, 2, row);
  policy->_priv->description = gda_value_stringify (value);

  value = (GdaValue *) gda_data_model_get_value_at (dm, 3, row);
  policy->_priv->begin_hour = gda_value_get_smallint (value);

  value = (GdaValue *) gda_data_model_get_value_at (dm, 4, row);
  policy->_priv->end_hour = gda_value_get_smallint (value);

  value = (GdaValue *) gda_data_model_get_value_at (dm, 5, row);
  policy->_priv->begin_day = gda_value_get_smallint (value);

  value = (GdaValue *) gda_data_model_get_value_at (dm, 6, row);
  policy->_priv->end_day = gda_value_get_smallint (value);

  return policy;
}

/*
 *
 *
 *
 */
gint
sim_policy_get_id (SimPolicy* policy)
{
  g_return_val_if_fail (policy, 0);
  g_return_val_if_fail (SIM_IS_POLICY (policy), 0);

  return policy->_priv->id;
}

/*
 *
 *
 *
 */
void
sim_policy_set_id (SimPolicy* policy,
		   gint       id)
{
  g_return_if_fail (policy);
  g_return_if_fail (SIM_IS_POLICY (policy));

  policy->_priv->id = id;
}

/*
 *
 *
 *
 */
gint
sim_policy_get_priority (SimPolicy* policy)
{
  g_return_val_if_fail (policy, 0);
  g_return_val_if_fail (SIM_IS_POLICY (policy), 0);

  if (policy->_priv->priority < -1) //-1 means "don't change priority"
    return 0;
  if (policy->_priv->priority > 5)
    return 5;

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
  g_return_if_fail (policy);
  g_return_if_fail (SIM_IS_POLICY (policy));

  if (priority < -1)
    policy->_priv->priority = 0;
  else if (priority > 5)
    policy->_priv->priority = 5;
  else policy->_priv->priority = priority;
}

/*
 *
 *
 *
 */
gint
sim_policy_get_begin_day (SimPolicy* policy)
{
  g_return_val_if_fail (policy, 0);
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
  g_return_if_fail (policy);
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
  g_return_val_if_fail (policy, 0);
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
  g_return_if_fail (policy);
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
  g_return_val_if_fail (policy, 0);
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
  g_return_if_fail (policy);
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
  g_return_val_if_fail (policy, 0);
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
  g_return_if_fail (policy);
  g_return_if_fail (SIM_IS_POLICY (policy));

  policy->_priv->end_hour = end_hour;
}

/*
 * This set, tells if the events that match in the policy must be stored in database
 * or not.
 *//*
void
sim_policy_set_store (SimPolicy *policy, gboolean store)
{
  g_return_if_fail (policy);
  g_return_if_fail (SIM_IS_POLICY (policy));

  policy->_priv->store_in_DB = store;  
}*/

/*
 * Get if the events that match in the policy must be stored.
 *//*
gboolean
sim_policy_get_store (SimPolicy *policy)
{
  g_return_val_if_fail (policy, FALSE);
  g_return_val_if_fail (SIM_IS_POLICY (policy), FALSE);

  return policy->_priv->store_in_DB;
}*/
/*
 *
 *
 *
 */
void
sim_policy_append_src (SimPolicy     *policy,
								       SimInet        *src) //SimInet objects can store hosts or networks, so we'll use it in the policy
{
  g_return_if_fail (policy);
  g_return_if_fail (SIM_IS_POLICY (policy));
  g_return_if_fail (src);

  policy->_priv->src = g_list_append (policy->_priv->src, src); //FIXME: I'll probably change it with g_list_prepend to increase efficiency
}

/*
 *
 *
 *
 */
void
sim_policy_remove_src (SimPolicy        *policy,
		       SimInet           *src)
{
  g_return_if_fail (policy);
  g_return_if_fail (SIM_IS_POLICY (policy));
  g_return_if_fail (src);

  policy->_priv->src = g_list_remove (policy->_priv->src, src);
}

/*
 *
 * Returns all the src's from a policy
 *
 */
GList*
sim_policy_get_src (SimPolicy* policy)
{
  g_return_val_if_fail (policy, NULL);
  g_return_val_if_fail (SIM_IS_POLICY (policy), NULL);

  return policy->_priv->src;
}

/*
 *
 *
 *
 */
void
sim_policy_free_src (SimPolicy* policy)
{
  GList   *list;
  g_return_if_fail (policy);
  g_return_if_fail (SIM_IS_POLICY (policy));

  list = policy->_priv->src;
  while (list)
    {
      SimInet *src = (SimInet *) list->data;
      g_object_unref(src);
      list = list->next;
    }
  g_list_free (policy->_priv->src);
}

/*
 *
 *
 *
 */
void
sim_policy_append_dst (SimPolicy        *policy,
		       SimInet        	*dst)
{
  g_return_if_fail (policy);
  g_return_if_fail (SIM_IS_POLICY (policy));
  g_return_if_fail (dst);

  policy->_priv->dst = g_list_append (policy->_priv->dst, dst);
}

/*
 *
 *
 *
 */
void
sim_policy_remove_dst (SimPolicy        *policy,
		       SimInet 	    	*dst)
{
  g_return_if_fail (policy);
  g_return_if_fail (SIM_IS_POLICY (policy));
  g_return_if_fail (dst);

  policy->_priv->dst = g_list_remove (policy->_priv->dst, dst);
}

/*
 *
 * Returns a SimNet object with all the hosts and/or networks in a specific policy rule.
 *
 */
GList*
sim_policy_get_dst (SimPolicy* policy)
{
  g_return_val_if_fail (policy, NULL);
  g_return_val_if_fail (SIM_IS_POLICY (policy), NULL);

  return policy->_priv->dst;
}

/*
 *
 *
 *
 */
void
sim_policy_free_dst (SimPolicy* policy)
{
  GList   *list;
  g_return_if_fail (policy);
  g_return_if_fail (SIM_IS_POLICY (policy));

  list = policy->_priv->dst;
  while (list)
    {
      SimInet *dst = (SimInet *) list->data;
      g_object_unref (dst);
      list = list->next;
    }
  g_list_free (policy->_priv->dst);
}

/*
 *
 *
 *
 */
void
sim_policy_append_port (SimPolicy        *policy,
			SimPortProtocol  *pp)
{
  g_return_if_fail (policy);
  g_return_if_fail (SIM_IS_POLICY (policy));
  g_return_if_fail (pp);

  policy->_priv->ports = g_list_append (policy->_priv->ports, pp);
}

/*
 *
 *
 *
 */
void
sim_policy_remove_port (SimPolicy        *policy,
			SimPortProtocol  *pp)
{
  g_return_if_fail (policy);
  g_return_if_fail (SIM_IS_POLICY (policy));
  g_return_if_fail (pp);

  policy->_priv->ports = g_list_remove (policy->_priv->ports, pp);
}

/*
 *
 *
 *
 */
GList*
sim_policy_get_ports (SimPolicy* policy)
{
  g_return_val_if_fail (policy, NULL);
  g_return_val_if_fail (SIM_IS_POLICY (policy), NULL);

  return policy->_priv->ports;
}

/*
 *
 *
 *
 */
void
sim_policy_free_ports (SimPolicy* policy)
{
  GList   *list;
  g_return_if_fail (policy);
  g_return_if_fail (SIM_IS_POLICY (policy));

  list = policy->_priv->ports;
  while (list)
    {
      SimPortProtocol *port = (SimPortProtocol *) list->data;
      g_free (port);
      list = list->next;
    }
  g_list_free (policy->_priv->ports);
}



/*
 *
 *
 *
 */
void
sim_policy_append_sensor (SimPolicy        *policy,
								          gchar            *sensor)
{
  g_return_if_fail (policy);
  g_return_if_fail (SIM_IS_POLICY (policy));
  g_return_if_fail (sensor);

  policy->_priv->sensors = g_list_append (policy->_priv->sensors, sensor);
}

/*
 *
 *
 *
 */
void
sim_policy_remove_sensor (SimPolicy        *policy,
								           gchar            *sensor)
{
  g_return_if_fail (policy);
  g_return_if_fail (SIM_IS_POLICY (policy));
  g_return_if_fail (sensor);

  policy->_priv->sensors = g_list_remove (policy->_priv->sensors, sensor);
}


/*
 *
 *
 *
 */
GList*
sim_policy_get_sensors (SimPolicy* policy)
{
  g_return_val_if_fail (policy, NULL);
  g_return_val_if_fail (SIM_IS_POLICY (policy), NULL);

  return policy->_priv->sensors;
}

/*
 *
 *
 *
 */
void
sim_policy_free_sensors (SimPolicy* policy)
{
  GList   *list;
  g_return_if_fail (policy);
  g_return_if_fail (SIM_IS_POLICY (policy));

  list = policy->_priv->sensors;
  while (list)
  {
    gchar *sensor = (gchar *) list->data;
    g_free (sensor);
    list = list->next;
  }
  g_list_free (policy->_priv->sensors);
}


/*
 *
 */
void
sim_policy_append_plugin_id (SimPolicy        *policy,
		                         guint            *plugin_id)
{
  g_return_if_fail (policy);
  g_return_if_fail (SIM_IS_POLICY (policy));
  g_return_if_fail (plugin_id);
	
  policy->_priv->plugin_ids = g_list_append (policy->_priv->plugin_ids, plugin_id);
}

/*
 * 
 */
void
sim_policy_remove_plugin_id (SimPolicy        *policy,
                             guint            *plugin_id)
{
  g_return_if_fail (policy);
  g_return_if_fail (SIM_IS_POLICY (policy));
  g_return_if_fail (plugin_id);

  policy->_priv->plugin_ids = g_list_remove (policy->_priv->plugin_ids, plugin_id);
}

/*
 *
 */
GList*
sim_policy_get_plugin_ids (SimPolicy* policy)
{
  g_return_val_if_fail (policy, NULL);
  g_return_val_if_fail (SIM_IS_POLICY (policy), NULL);

  return policy->_priv->plugin_ids;
}

/*
 *
 *
 *
 */
void
sim_policy_free_plugin_ids (SimPolicy* policy)
{
  GList   *list;
  g_return_if_fail (policy);
  g_return_if_fail (SIM_IS_POLICY (policy));

  list = policy->_priv->plugin_ids;
  while (list)
  {
    guint *plugin_id = (guint *) list->data;
    g_free (plugin_id);
    list = list->next;
  }
  g_list_free (policy->_priv->plugin_ids);
}


/*
 *
 */
void
sim_policy_append_plugin_sid (SimPolicy        *policy,
		                      		guint            *plugin_sid)
{
  g_return_if_fail (policy);
  g_return_if_fail (SIM_IS_POLICY (policy));
  g_return_if_fail (plugin_sid);

  policy->_priv->plugin_sids = g_list_append (policy->_priv->plugin_sids, plugin_sid);
}

/*
 * 
 */
void
sim_policy_remove_plugin_sid (SimPolicy        *policy,
	                            guint            *plugin_sid)
{
  g_return_if_fail (policy);
  g_return_if_fail (SIM_IS_POLICY (policy));
  g_return_if_fail (plugin_sid);

  policy->_priv->plugin_sids = g_list_remove (policy->_priv->plugin_sids, plugin_sid);
}

/*
 *
 */
GList*
sim_policy_get_plugin_sids (SimPolicy* policy)
{
  g_return_val_if_fail (policy, NULL);
  g_return_val_if_fail (SIM_IS_POLICY (policy), NULL);

  return policy->_priv->plugin_sids;
}

/*
 *
 *
 *
 */
void
sim_policy_free_plugin_sids (SimPolicy* policy)
{
  GList   *list;
  g_return_if_fail (policy);
  g_return_if_fail (SIM_IS_POLICY (policy));

  list = policy->_priv->plugin_sids;
  while (list)
  {
    guint *plugin_sid = (guint *) list->data;
    g_free (plugin_sid);
    list = list->next;
  }
  g_list_free (policy->_priv->plugin_sids);
}

/*
 *
 */
void
sim_policy_append_plugin_group (SimPolicy					 *policy,
																Plugin_PluginSid   *plugin_group)
{
  g_return_if_fail (policy);
  g_return_if_fail (SIM_IS_POLICY (policy));
  g_return_if_fail (plugin_group);
	
  policy->_priv->plugin_groups = g_list_append (policy->_priv->plugin_groups, plugin_group);
}

/*
 * 
 */
void
sim_policy_remove_plugin_group (SimPolicy        *policy,
																Plugin_PluginSid   *plugin_group)
{
  g_return_if_fail (policy);
  g_return_if_fail (SIM_IS_POLICY (policy));
  g_return_if_fail (plugin_group);

  policy->_priv->plugin_groups = g_list_remove (policy->_priv->plugin_groups, plugin_group);
}

/*
 *
 */
GList*
sim_policy_get_plugin_groups (SimPolicy* policy)
{
  g_return_val_if_fail (policy, NULL);
  g_return_val_if_fail (SIM_IS_POLICY (policy), NULL);

  return policy->_priv->plugin_groups;
}

/*
 *
 *
 *
 */
void
sim_policy_free_plugin_groups (SimPolicy* policy)
{
  GList   *list;
  GList   *list2;
  g_return_if_fail (policy);
  g_return_if_fail (SIM_IS_POLICY (policy));

  list = policy->_priv->plugin_groups;
  while (list)
  {
    Plugin_PluginSid *plugin_group = (Plugin_PluginSid *) list->data;
		list2 = plugin_group->plugin_sid;
		while (list2)
		{
			gint *plugin_sid = (gint *) list2->data;
			g_free (plugin_sid);
			list2 = list2->next;
		}			
    g_free (plugin_group);
    list = list->next;
  }
  g_list_free (policy->_priv->plugin_groups);
}


/*
 *
 *
 *
 */
gboolean
sim_policy_match (SimPolicy        *policy,
								  gint              date,
								  GInetAddr        *src_ia,
								  GInetAddr        *dst_ia,
								  SimPortProtocol  *pp,
									gchar							*sensor,
									guint							plugin_id,
									guint							plugin_sid)
{
  GList     *list;
  gboolean   found = FALSE;
  gint       start, end;

  g_return_val_if_fail (policy, FALSE);
  g_return_val_if_fail (SIM_IS_POLICY (policy), FALSE);
  g_return_val_if_fail (src_ia, FALSE);
  g_return_val_if_fail (dst_ia, FALSE);
  g_return_val_if_fail (pp, FALSE);
  g_return_val_if_fail (sensor, FALSE);
    
	g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_policy_match, Policy ID: %d", policy->_priv->id);

  start = ((policy->_priv->begin_day - 1) * 24 + policy->_priv->begin_hour);
  end = ((policy->_priv->end_day - 1) * 24 + policy->_priv->end_hour);
  
  if ((start > date) || (end < date))
	{
		g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_policy_match: Not match: BAD DATE");
    return FALSE;
	}
//	else
//	  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "DATE OK");
			
	
  /* Find source ip*/
  found = FALSE;
  list = policy->_priv->src;

	if (!list)
	  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_policy_match: NO POLICY!!!");
			
  while (list)
  {
    SimInet *cmp = (SimInet *) list->data;

//    g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "       src_ip: %s", gnet_inetaddr_get_canonical_name(src_ia));

		if (sim_inet_is_reserved(cmp)) //check if "any" is the source
 	  {
	    found = TRUE;
		  break;
	  }
		
	  SimInet *src = sim_inet_new_from_ginetaddr(src_ia); //a bit speed improve separating both checks...
		if (sim_inet_has_inet(cmp, src))  //check if src belongs to cmp.
		{
			found=TRUE;
			break;
		}

    list = list->next;
  }
	
  if (!found)
	{
//	  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "       src_ip: %s Doesn't matches with any", gnet_inetaddr_get_canonical_name(src_ia));
		return FALSE;
	}
//	else
//	  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "       src_ip: %s OK!; Match with policy: %d", gnet_inetaddr_get_canonical_name(src_ia),policy->_priv->id);

  /* Find destination ip */
  found = FALSE;
  list = policy->_priv->dst;
  while (list)
  {
    SimInet *cmp = (SimInet *) list->data;
	
		/**********DEBUG**************
    //struct sockaddr_in* sa_in = (struct sockaddr_in*) &cmp->_priv->sa;
    struct sockaddr_in* sa_in = (struct sockaddr_storage*) &cmp->_priv->sa;

    guint32 val1 = ntohl (sa_in->sin_addr.s_addr);

    gchar *temp = g_strdup_printf ("%d.%d.%d.%d",
                             (val1 >> 24) & 0xFF,
                             (val1 >> 16) & 0xFF,
                             (val1 >> 8) & 0xFF,
                             (val1) & 0xFF);
    g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Policy dst: %d bits: %s",cmp->_priv->bits, temp);*/
//    g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "       dst_ip: %s", gnet_inetaddr_get_canonical_name(dst_ia));
//		g_free(temp);

		/**************end debug**************/
	
    if (sim_inet_is_reserved(cmp))
    {
      found = TRUE;
      break;
    }

    SimInet *dst = sim_inet_new_from_ginetaddr(dst_ia);
    if (sim_inet_has_inet(cmp, dst)) 
 	  {
 	    found=TRUE;
	    break;
    }
    
		list = list->next;
  }
	
  if (!found) return FALSE;

  //g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "       dst_ip MATCH");
	
  /* Find port & protocol */
  found = FALSE;
  list = policy->_priv->ports;
  while (list)
  {
    SimPortProtocol *cmp = (SimPortProtocol *) list->data;
      
    if (sim_port_protocol_equal (cmp, pp))
		{
		  found = TRUE;
		//	g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "       port MATCH");
	  	break;
		}
    list = list->next;
  }
  if (!found) return FALSE;

	/* Find sensor */
  found = FALSE;
  list = policy->_priv->sensors;
  while (list)
  {
    gchar *cmp = (gchar *) list->data;
		g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "       event sensor: -%s-",sensor);
		g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "       policy sensor:-%s-",cmp);
		
    if (!strcmp (sensor, cmp) || !strcmp (cmp, "0")) //if match
		{
		  found = TRUE;
			g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "       sensor MATCH");
	  	break;
		}
    list = list->next;
  }
  if (!found)
	{
		g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "       sensor NOT MATCH");
		return FALSE;
	}
	
  /* Find plugin_groups */
	
  found = FALSE;
  list = policy->_priv->plugin_groups;
  while (list)
  {
    Plugin_PluginSid *plugin_group = (Plugin_PluginSid *) list->data;
    g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "                               plugin_id: %d",plugin_group->plugin_id);
		gint cmp = plugin_group->plugin_id;
		if (plugin_id == 0) //if match (0 is ANY)
    {
      found = TRUE;
      break;
    }
		if (plugin_id == cmp) //if match
		{
	    GList *list2 = plugin_group->plugin_sid;
  	  while (list2)
  	 	{
	      gint *aux_plugin_sid = (gint *) list2->data;
				if ((*aux_plugin_sid == plugin_sid) || (*aux_plugin_sid == 0)) //match!
				{
					found = TRUE;
					break;
				}
      	list2 = list2->next;
	    }
		}
  	list = list->next;
  }
  if (!found) return FALSE;

  //g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "       plugin group MATCH");
	
  return TRUE;
}

void sim_policy_debug_print_policy	(SimPolicy	*policy) //print hexa values
{

	g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_policy_debug_print_policy: id: %d",policy->_priv->id);
	g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "                               description: %s",policy->_priv->description);
	g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "                               begin_hour:  %s",policy->_priv->begin_hour);
	g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "                               src:         %x",policy->_priv->src);
	g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "                               dst:         %x",policy->_priv->dst);
	g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "                               ports:       %x",policy->_priv->ports);
	g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "                               categories:  %x",policy->_priv->categories);
	g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "                               sensors:     %x",policy->_priv->sensors);
	g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "                               plugin_groups: %x",policy->_priv->plugin_groups);
	g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "                               priority: %d",policy->_priv->priority);
//	g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "                               plugin_ids:  %x",policy->_priv->plugin_ids);
//	g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "                               plugin_sids: %x",policy->_priv->plugin_sids);

	GList *list = policy->_priv->plugin_groups;
  while (list)
  {
    Plugin_PluginSid *plugin_group = (Plugin_PluginSid *) list->data;
		g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "                               plugin_id: %d",plugin_group->plugin_id);
    GList *list2 = plugin_group->plugin_sid;
    while (list2)
    {
      gint *plugin_sid = (gint *) list2->data;
			g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "                               plugin_ids: %d",*plugin_sid);
      list2 = list2->next;
    }
    list = list->next;
	}

}

/*
 * Given a specific policy, it returns the role associated to it.
 */
SimRole *
sim_policy_get_role	(SimPolicy *policy)
{
  g_return_val_if_fail (policy, FALSE);
  g_return_val_if_fail (SIM_IS_POLICY (policy), FALSE);
  g_return_val_if_fail (policy, FALSE);
  g_return_val_if_fail (SIM_IS_POLICY (policy), FALSE);

	return policy->_priv->role;
}

void
sim_policy_set_role	(SimPolicy *policy,
											SimRole	*role)
{
  g_return_if_fail (policy);
  g_return_if_fail (SIM_IS_POLICY (policy));
  g_return_if_fail (policy);
  g_return_if_fail (SIM_IS_POLICY (policy));

	policy->_priv->role = role;
}
// vim: set tabstop=2:
