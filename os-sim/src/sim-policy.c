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

#include "sim-policy.h"
 
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

  GList  *src_ias;
  GList  *dst_ias;
  GList  *ports;
  GList  *categories;
  GList  *sensors;
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

  sim_policy_free_src_ias (policy);
  sim_policy_free_dst_ias (policy);
  sim_policy_free_ports (policy);
  sim_policy_free_categories (policy);
  sim_policy_free_sensors (policy);

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

  policy->_priv->src_ias = NULL;
  policy->_priv->dst_ias = NULL;
  policy->_priv->ports = NULL;
  policy->_priv->categories = NULL;
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

  if (policy->_priv->priority < 0)
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

  if (priority < 0)
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
 *
 *
 *
 */
void
sim_policy_append_src_ia (SimPolicy        *policy,
			  GInetAddr        *ia)
{
  g_return_if_fail (policy);
  g_return_if_fail (SIM_IS_POLICY (policy));
  g_return_if_fail (ia);

  policy->_priv->src_ias = g_list_append (policy->_priv->src_ias, ia);
}

/*
 *
 *
 *
 */
void
sim_policy_remove_src_ia (SimPolicy        *policy,
			  GInetAddr        *ia)
{
  g_return_if_fail (policy);
  g_return_if_fail (SIM_IS_POLICY (policy));
  g_return_if_fail (ia);

  policy->_priv->src_ias = g_list_remove (policy->_priv->src_ias, ia);
}

/*
 *
 *
 *
 */
GList*
sim_policy_get_src_ias (SimPolicy* policy)
{
  g_return_val_if_fail (policy, NULL);
  g_return_val_if_fail (SIM_IS_POLICY (policy), NULL);

  return policy->_priv->src_ias;
}

/*
 *
 *
 *
 */
void
sim_policy_free_src_ias (SimPolicy* policy)
{
  GList   *list;
  g_return_if_fail (policy);
  g_return_if_fail (SIM_IS_POLICY (policy));

  list = policy->_priv->src_ias;
  while (list)
    {
      GInetAddr *ia = (GInetAddr *) list->data;
      gnet_inetaddr_unref (ia);
      list = list->next;
    }
  g_list_free (policy->_priv->src_ias);
}

/*
 *
 *
 *
 */
void
sim_policy_append_dst_ia (SimPolicy        *policy,
			  GInetAddr        *ia)
{
  g_return_if_fail (policy);
  g_return_if_fail (SIM_IS_POLICY (policy));
  g_return_if_fail (ia);

  policy->_priv->dst_ias = g_list_append (policy->_priv->dst_ias, ia);
}

/*
 *
 *
 *
 */
void
sim_policy_remove_dst_ia (SimPolicy        *policy,
			  GInetAddr        *ia)
{
  g_return_if_fail (policy);
  g_return_if_fail (SIM_IS_POLICY (policy));
  g_return_if_fail (ia);

  policy->_priv->dst_ias = g_list_remove (policy->_priv->dst_ias, ia);
}

/*
 *
 *
 *
 */
GList*
sim_policy_get_dst_ias (SimPolicy* policy)
{
  g_return_val_if_fail (policy, NULL);
  g_return_val_if_fail (SIM_IS_POLICY (policy), NULL);

  return policy->_priv->dst_ias;
}

/*
 *
 *
 *
 */
void
sim_policy_free_dst_ias (SimPolicy* policy)
{
  GList   *list;
  g_return_if_fail (policy);
  g_return_if_fail (SIM_IS_POLICY (policy));

  list = policy->_priv->dst_ias;
  while (list)
    {
      GInetAddr *ia = (GInetAddr *) list->data;
      gnet_inetaddr_unref (ia);
      list = list->next;
    }
  g_list_free (policy->_priv->dst_ias);
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
sim_policy_append_category (SimPolicy        *policy,
			     gchar            *category)
{
  g_return_if_fail (policy);
  g_return_if_fail (SIM_IS_POLICY (policy));
  g_return_if_fail (category);

  policy->_priv->categories = g_list_append (policy->_priv->categories, category);
}

/*
 *
 *
 *
 */
void
sim_policy_remove_category (SimPolicy        *policy,
			     gchar            *category)
{
  g_return_if_fail (policy);
  g_return_if_fail (SIM_IS_POLICY (policy));
  g_return_if_fail (category);

  policy->_priv->categories = g_list_remove (policy->_priv->categories, category);
}

/*
 *
 *
 *
 */
GList*
sim_policy_get_categories (SimPolicy* policy)
{
  g_return_val_if_fail (policy, NULL);
  g_return_val_if_fail (SIM_IS_POLICY (policy), NULL);

  return policy->_priv->categories;
}

/*
 *
 *
 *
 */
void
sim_policy_free_categories (SimPolicy* policy)
{
  GList   *list;
  g_return_if_fail (policy);
  g_return_if_fail (SIM_IS_POLICY (policy));

  list = policy->_priv->categories;
  while (list)
    {
      gchar *category = (gchar *) list->data;
      g_free (category);
      list = list->next;
    }
  g_list_free (policy->_priv->categories);
  policy->_priv->categories = NULL;
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
 *
 *
 */
gboolean
sim_policy_match (SimPolicy        *policy,
		  gint              date,
		  GInetAddr        *src_ia,
		  GInetAddr        *dst_ia,
		  SimPortProtocol  *pp,
		  const gchar      *category)
{
  GList     *list;
  gboolean   found = FALSE;
  gint       start, end;

  g_return_val_if_fail (policy, FALSE);
  g_return_val_if_fail (SIM_IS_POLICY (policy), FALSE);
  g_return_val_if_fail (src_ia, FALSE);
  g_return_val_if_fail (dst_ia, FALSE);
  g_return_val_if_fail (pp, FALSE);
  g_return_val_if_fail (category, FALSE);

  start = ((policy->_priv->begin_day - 1) * 7 + policy->_priv->begin_hour);
  end = ((policy->_priv->end_day - 1) * 7 + policy->_priv->end_hour);
  
  if ((start > date) || (end < date))
    return FALSE;

  /* Find source ip*/
  found = FALSE;
  list = policy->_priv->src_ias;
  while (list)
    {
      GInetAddr *cmp = (GInetAddr *) list->data;

      if ((gnet_inetaddr_is_reserved (cmp)) || (gnet_inetaddr_noport_equal (cmp, src_ia)))
	{
	  found = TRUE;
	  break;
	}

      list = list->next;
    }
  if (!found) return FALSE;

  /* Find destination ip */
  found = FALSE;
  list = policy->_priv->dst_ias;
  while (list)
    {
      GInetAddr *cmp = (GInetAddr *) list->data;
      
      if ((gnet_inetaddr_is_reserved (cmp)) || (gnet_inetaddr_noport_equal (cmp, dst_ia)))
	{
	  found = TRUE;
	  break;
	}

      list = list->next;
    }
  if (!found) return FALSE;

  /* Find port */
  found = FALSE;
  list = policy->_priv->ports;
  while (list)
    {
      SimPortProtocol *cmp = (SimPortProtocol *) list->data;
      
      if (sim_port_protocol_equal (cmp, pp))
	{
	  found = TRUE;
	  break;
	}

      list = list->next;
    }
  if (!found) return FALSE;

  /* Find category subgroups  */
  found = FALSE;
  list = policy->_priv->categories;
  while (list)
    {
      gchar *cmp = (gchar *) list->data;
      
      if (!strcmp (cmp, category))
	{
	  found = TRUE;
	  break;
	}

      list = list->next;
    }
  if (!found) return FALSE;

  return TRUE;
}
