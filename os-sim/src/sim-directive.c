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
#include <gnet.h>

#include "sim-directive.h"
#include "sim-rule.h"
#include "sim-action.h"

#include <time.h>

enum 
{
  DESTROY,
  LAST_SIGNAL
};

struct _SimDirectivePrivate {
  gint       id;
  gchar     *name;

  gint       priority;

  gboolean   matched;

  GTime      time_out;
  gint64     time;
  GTime      time_last;

  GNode     *rule_root;
  GNode     *rule_curr;

  GList     *actions;
};

static gpointer parent_class = NULL;
static gint sim_server_signals[LAST_SIGNAL] = { 0 };

/* GType Functions */

static void 
sim_directive_impl_dispose (GObject  *gobject)
{
  G_OBJECT_CLASS (parent_class)->dispose (gobject);
}

static void 
sim_directive_impl_finalize (GObject  *gobject)
{
  SimDirective *directive = SIM_DIRECTIVE (gobject);
  GList        *list;

  if (directive->_priv->name)
    g_free (directive->_priv->name);

  sim_directive_node_data_destroy (directive->_priv->rule_root);
  g_node_destroy (directive->_priv->rule_root);

  g_free (directive->_priv);
  
  G_OBJECT_CLASS (parent_class)->finalize (gobject);
}

static void
sim_directive_class_init (SimDirectiveClass * class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  parent_class = g_type_class_ref (G_TYPE_OBJECT);

  object_class->dispose = sim_directive_impl_dispose;
  object_class->finalize = sim_directive_impl_finalize;
}

static void
sim_directive_instance_init (SimDirective *directive)
{
  directive->_priv = g_new0 (SimDirectivePrivate, 1);

  directive->_priv->id = 0;
  directive->_priv->name = NULL;

  directive->_priv->time_out = 300;
  directive->_priv->time = 0;
  directive->_priv->time_last = 0;

  directive->_priv->priority = 0;
  directive->_priv->matched = FALSE;

  directive->_priv->rule_root = NULL;
  directive->_priv->rule_curr = NULL;

  directive->_priv->actions =  NULL;
}

/* Public Methods */

GType
sim_directive_get_type (void)
{
  static GType object_type = 0;
 
  if (!object_type)
  {
    static const GTypeInfo type_info = {
              sizeof (SimDirectiveClass),
              NULL,
              NULL,
              (GClassInitFunc) sim_directive_class_init,
              NULL,
              NULL,                       /* class data */
              sizeof (SimDirective),
              0,                          /* number of pre-allocs */
              (GInstanceInitFunc) sim_directive_instance_init,
              NULL                        /* value table */
    };
    
    g_type_init ();
                                                                                                                             
    object_type = g_type_register_static (G_TYPE_OBJECT, "SimDirective", &type_info, 0);
  }

  return object_type;
}

/*
 *
 *
 *
 *
 */
SimDirective*
sim_directive_new (void)
{
  SimDirective *directive = NULL;

  directive = SIM_DIRECTIVE (g_object_new (SIM_TYPE_DIRECTIVE, NULL));

  return directive;
}

/*
 *
 *
 *
 *
 */
gint
sim_directive_get_id (SimDirective   *directive)
{
  g_return_val_if_fail (directive, 0);
  g_return_val_if_fail (SIM_IS_DIRECTIVE (directive), 0);

  return directive->_priv->id;
}

/*
 *
 *
 *
 *
 */
void sim_directive_set_id (SimDirective   *directive,
			   gint            id)
{
  g_return_if_fail (directive);
  g_return_if_fail (SIM_IS_DIRECTIVE (directive));
  g_return_if_fail (id > 0);

  directive->_priv->id = id;
}

/*
 *
 *
 *
 *
 */
gchar*
sim_directive_get_name (SimDirective   *directive)
{
  g_return_val_if_fail (directive, NULL);
  g_return_val_if_fail (SIM_IS_DIRECTIVE (directive), NULL);

  return directive->_priv->name;
}

/*
 *
 *
 *
 *
 */
void sim_directive_set_name (SimDirective   *directive,
			     const gchar    *name)
{
  g_return_if_fail (directive);
  g_return_if_fail (SIM_IS_DIRECTIVE (directive));
  g_return_if_fail (name);

  if (directive->_priv->name)
    g_free (directive->_priv->name);

  directive->_priv->name = g_strdup (name);
}

/*
 *
 *
 *
 *
 */
gint
sim_directive_get_priority (SimDirective   *directive)
{
  g_return_val_if_fail (directive != NULL, 0);
  g_return_val_if_fail (SIM_IS_DIRECTIVE (directive), 0);

  if (directive->_priv->priority < 0)
    return 0;
  if (directive->_priv->priority > 5)
    return 5;

  return directive->_priv->priority;
}

/*
 *
 *
 *
 *
 */
void sim_directive_set_priority (SimDirective   *directive,
				 gint            priority)
{
  g_return_if_fail (directive);
  g_return_if_fail (SIM_IS_DIRECTIVE (directive));

  if (priority < 0)
    directive->_priv->priority = 0;
  else if (priority > 5)
    directive->_priv->priority = 5;
  else
    directive->_priv->priority = priority;
}

/*
 *
 *
 *
 *
 */
GTime
sim_directive_get_time_out (SimDirective   *directive)
{
  g_return_val_if_fail (directive, 0);
  g_return_val_if_fail (SIM_IS_DIRECTIVE (directive), 0);

  return directive->_priv->time_out;
}

/*
 *
 *
 *
 *
 */
void 
sim_directive_set_time_out (SimDirective   *directive,
			    GTime           time_out)
{
  g_return_if_fail (directive);
  g_return_if_fail (SIM_IS_DIRECTIVE (directive));
  g_return_if_fail (time_out >= 0);

  directive->_priv->time_out = time_out;
}

/*
 *
 *
 *
 *
 */
GTime
sim_directive_get_time_last (SimDirective   *directive)
{
  g_return_val_if_fail (directive, 0);
  g_return_val_if_fail (SIM_IS_DIRECTIVE (directive), 0);

  return directive->_priv->time_last;
}

/*
 *
 *
 *
 *
 */
void sim_directive_set_time_last (SimDirective   *directive,
				  GTime           time_last)
{
  g_return_if_fail (directive);
  g_return_if_fail (SIM_IS_DIRECTIVE (directive));
  g_return_if_fail (time_last >= 0);

  directive->_priv->time_out = time_last;
}

/*
 *
 *
 *
 *
 */
GNode*
sim_directive_get_root_node (SimDirective  *directive)
{
  g_return_val_if_fail (directive, NULL);
  g_return_val_if_fail (SIM_IS_DIRECTIVE (directive), NULL);

  return directive->_priv->rule_root;
}

/*
 *
 *
 *
 *
 */
void
sim_directive_set_root_node (SimDirective  *directive,
			     GNode         *root_node)
{
  g_return_if_fail (directive);
  g_return_if_fail (SIM_IS_DIRECTIVE (directive));
  g_return_if_fail (root_node);

  directive->_priv->rule_root = root_node;
}

/*
 *
 *
 *
 *
 */
GNode*
sim_directive_get_curr_node (SimDirective  *directive)
{
  g_return_val_if_fail (directive, NULL);
  g_return_val_if_fail (SIM_IS_DIRECTIVE (directive), NULL);

  return directive->_priv->rule_curr;
}

/*
 *
 *
 *
 *
 */
void
sim_directive_set_curr_node (SimDirective  *directive,
			     GNode         *curr_node)
{
  g_return_if_fail (directive);
  g_return_if_fail (SIM_IS_DIRECTIVE (directive));
  g_return_if_fail (curr_node);

  directive->_priv->rule_curr = curr_node;
}

/*
 *
 *
 *
 *
 */
SimRule*
sim_directive_get_root_rule (SimDirective  *directive)
{
  g_return_val_if_fail (directive, NULL);
  g_return_val_if_fail (SIM_IS_DIRECTIVE (directive), NULL);
  g_return_val_if_fail (directive->_priv->rule_root, NULL);

  return (SimRule *) directive->_priv->rule_root->data;
}

/*
 *
 *
 *
 *
 */
SimRule*
sim_directive_get_curr_rule (SimDirective  *directive)
{
  g_return_val_if_fail (directive, NULL);
  g_return_val_if_fail (SIM_IS_DIRECTIVE (directive), NULL);
  g_return_val_if_fail (directive->_priv->rule_curr, NULL);

  return (SimRule *) directive->_priv->rule_curr->data;
}

/*
 *
 *
 *
 *
 */
GTime
sim_directive_get_rule_curr_time_out_max (SimDirective  *directive)
{
  GNode  *node;
  GTime   time_out = 0;

  g_return_val_if_fail (directive, 0);
  g_return_val_if_fail (SIM_IS_DIRECTIVE (directive), 0);
  g_return_val_if_fail (directive->_priv->rule_curr, 0);

  node = directive->_priv->rule_curr->children;

  while (node)
    {
      SimRule *rule = (SimRule *) node->data;
      GTime   time = sim_rule_get_time_out (rule);

      if (!time)
	return 0;

      if (time > time_out)
	time_out = time;

      node = node->next;
    }

  return time_out;
}

/*
 *
 *
 *
 *
 */
gint
sim_directive_get_rule_level (SimDirective   *directive)
{
  g_return_val_if_fail (directive, 0);
  g_return_val_if_fail (SIM_IS_DIRECTIVE (directive), 0);
  g_return_val_if_fail (directive->_priv->rule_curr, 0);

  return g_node_depth (directive->_priv->rule_curr);
}

/*
 *
 *
 *
 */
gboolean
sim_directive_match_by_alert (SimDirective  *directive,
				SimAlert    *alert)
{
  SimRule *rule;
  gboolean match;

  g_return_val_if_fail (directive, FALSE);
  g_return_val_if_fail (SIM_IS_DIRECTIVE (directive), FALSE);
  g_return_val_if_fail (directive->_priv->rule_root, FALSE);
  g_return_val_if_fail (alert, FALSE);
  g_return_val_if_fail (SIM_IS_ALERT (alert), FALSE);

  rule = (SimRule *) directive->_priv->rule_root->data;

  match = sim_rule_match_by_alert (rule, alert);

  return match;
}

/*
 *
 *
 *
 */
gboolean
sim_directive_backlog_match_by_alert (SimDirective  *directive,
				      SimAlert    *alert)
{
  GNode      *node = NULL;
  GNode      *children = NULL;

  g_return_val_if_fail (directive, FALSE);
  g_return_val_if_fail (SIM_IS_DIRECTIVE (directive), FALSE);
  g_return_val_if_fail (!directive->_priv->matched, FALSE);
  g_return_val_if_fail (alert, FALSE);
  g_return_val_if_fail (SIM_IS_ALERT (alert), FALSE);
  g_return_val_if_fail (directive->_priv->rule_curr, FALSE);
  g_return_val_if_fail (SIM_IS_RULE (directive->_priv->rule_curr->data), FALSE);

  node = directive->_priv->rule_curr->children;

  while (node)
    {
      SimRule *rule = (SimRule *) node->data;

      if (sim_rule_match_by_alert (rule, alert))
	{
	  GTime time_last = time (NULL);
	  directive->_priv->rule_curr = node;
	  directive->_priv->time_last = time_last;
	  directive->_priv->time_out = sim_directive_get_rule_curr_time_out_max (directive);

	  sim_rule_set_alert_data (rule, alert);

	  if (!G_NODE_IS_LEAF (node))
	    {
	      children = node->children;
	      while (children)
		{
		  SimRule *rule_child = (SimRule *) children->data;

		  sim_rule_set_time_last (rule_child, time_last);

		  sim_directive_set_rule_vars (directive, children);
		  children = children->next;
		}
	    }
	  else
	    {
	      directive->_priv->matched = TRUE;
	    }

	  return TRUE;
	}
      node = node->next;
    }

  return FALSE;
}

/*
 *
 *
 *
 */
gboolean
sim_directive_backlog_match_by_not (SimDirective  *directive)
{
  GNode      *node = NULL;
  GNode      *children = NULL;
  GTimeVal    curr_time;

  g_return_val_if_fail (directive, FALSE);
  g_return_val_if_fail (SIM_IS_DIRECTIVE (directive), FALSE);

  g_get_current_time (&curr_time);

  node = directive->_priv->rule_curr->children;

  while (node)
    {
      SimRule *rule = (SimRule *) node->data;
      
      if ((sim_rule_is_time_out (rule)) && (sim_rule_get_not (rule)) && (!sim_rule_is_not_invalid (rule))) 
	{
	  directive->_priv->rule_curr = node;
	  directive->_priv->time_last = curr_time.tv_sec;
	  directive->_priv->time_out = sim_directive_get_rule_curr_time_out_max (directive);

	  if (!G_NODE_IS_LEAF (node))
	    {
	      children = node->children;
	      while (children)
		{
		  sim_directive_set_rule_vars (directive, children);
		  children = children->next;
		}
	    }
	  else
	    {
	      directive->_priv->matched = TRUE;
	    }
	  
	  return TRUE;
	}
      node = node->next;
    }

  return FALSE;
}

/*
 *
 *
 *
 */
void
sim_directive_set_rule_vars (SimDirective     *directive,
			     GNode            *node)
{
  SimRule    *rule;
  SimRule    *rule_up;
  GNode      *node_up;
  GList      *vars;
  GInetAddr  *ia;
  gint        port;

  g_return_if_fail (directive);
  g_return_if_fail (SIM_IS_DIRECTIVE (directive));
  g_return_if_fail (node);
  g_return_if_fail (g_node_depth (node) > 1);

  rule = (SimRule *) node->data;
  vars = sim_rule_get_vars (rule);

  while (vars)
    {
      SimRuleVar *var = (SimRuleVar *) vars->data;

      node_up = sim_directive_get_node_branch_by_level (directive, node, var->level);
      if (!node_up)
	{
	  vars = vars->next;
	  continue;
	}

      rule_up = (SimRule *) node_up->data;

      switch (var->type)
	{
	case SIM_RULE_VAR_SRC_IA:
	  ia = sim_rule_get_src_ia (rule_up);

	  switch (var->attr)
	    {
	    case SIM_RULE_VAR_SRC_IA:
	      sim_rule_append_src_ia (rule, gnet_inetaddr_clone (ia));
	      break;
	    case SIM_RULE_VAR_DST_IA:
	      sim_rule_append_dst_ia (rule, gnet_inetaddr_clone (ia));
	      break;
	    default:
	      break;
	    }
	  break;

	case SIM_RULE_VAR_DST_IA:
	  ia = sim_rule_get_dst_ia (rule_up);

	  switch (var->attr)
	    {
	    case SIM_RULE_VAR_SRC_IA:
	      sim_rule_append_src_ia (rule, gnet_inetaddr_clone (ia));
	      break;
	    case SIM_RULE_VAR_DST_IA:
	      sim_rule_append_dst_ia (rule, gnet_inetaddr_clone (ia));
	      break;
	    default:
	      break;
	    }
	  break;

	case SIM_RULE_VAR_SRC_PORT:
	  port = sim_rule_get_src_port (rule_up);

	  switch (var->attr)
	    {
	    case SIM_RULE_VAR_SRC_PORT:
	      sim_rule_append_src_port (rule, port);
	      break;
	    case SIM_RULE_VAR_DST_PORT:
	      sim_rule_append_dst_port (rule, port);
	      break;
	    default:
	      break;
	    }
	  break;

	case SIM_RULE_VAR_DST_PORT:
	  port = sim_rule_get_dst_port (rule_up);

	  switch (var->attr)
	    {
	    case SIM_RULE_VAR_SRC_PORT:
	      sim_rule_append_src_port (rule, port);
	      break;
	    case SIM_RULE_VAR_DST_PORT:
	      sim_rule_append_dst_port (rule, port);
	      break;
	    default:
	      break;
	    }
	  break;

	default:
	  break;
	}

      vars = vars->next;
    }
}

/*
 *
 *
 *
 */
GNode*
sim_directive_get_node_branch_by_level (SimDirective     *directive,
					GNode            *node,
					gint              level)
{
  GNode  *ret;
  gint    up_level;
  gint    i;

  g_return_val_if_fail (directive, NULL);
  g_return_val_if_fail (SIM_IS_DIRECTIVE (directive), NULL);
  g_return_val_if_fail (node, NULL);
  
  up_level = g_node_depth (node) - level;
  if (up_level < 1)
    return NULL;

  ret = node;
  for (i = 0; i < up_level; i++)
    {
      ret = ret->parent;
    }
  
  return ret;
}

/*
 *
 *
 *
 */
void
sim_directive_set_matched (SimDirective     *directive,
			   gboolean          matched)
{
  g_return_if_fail (directive);
  g_return_if_fail (SIM_IS_DIRECTIVE (directive));

  directive->_priv->matched = matched;
}


/*
 *
 *
 *
 */
gboolean
sim_directive_get_matched (SimDirective     *directive)
{
  g_return_val_if_fail (directive, FALSE);
  g_return_val_if_fail (SIM_IS_DIRECTIVE (directive), FALSE);

  return directive->_priv->matched;
}

/**
 * sim_directive_is_time_out:
 * @directive: a #SimDirective.
 *
 * Look if the #SimDirective is time out
 *
 * Returns: TRUE if is time out, FALSE otherwise.
 */
gboolean
sim_directive_is_time_out (SimDirective     *directive)
{
  g_return_val_if_fail (directive, FALSE);
  g_return_val_if_fail (SIM_IS_DIRECTIVE (directive), FALSE);
  g_return_val_if_fail (!directive->_priv->matched, FALSE);

  if ((!directive->_priv->time_out) || (!directive->_priv->time_last))
    return FALSE;

  if (time (NULL) > (directive->_priv->time_last + directive->_priv->time_out))
    return TRUE;

  return FALSE;
}

/*
 *
 *
 *
 */
GNode*
sim_directive_node_data_clone (GNode *node)
{
  SimRule  *new_rule;
  GNode    *new_node;
  GNode    *child;

  g_return_val_if_fail (node, NULL);
  g_return_val_if_fail (node->data, NULL);
  g_return_val_if_fail (SIM_IS_RULE (node->data), NULL);

  new_rule = sim_rule_clone (SIM_RULE (node->data));
  new_node = g_node_new (new_rule);
  
  for (child = g_node_last_child (node); child; child = child->prev)
    g_node_prepend (new_node, sim_directive_node_data_clone (child));
   
  return new_node;
}

/*
 *
 *
 *
 */
void
sim_directive_node_data_destroy (GNode *node)
{
  GNode   *child;
  
  g_return_if_fail (node);
  g_return_if_fail (node->data);
  g_return_if_fail (SIM_IS_RULE (node->data));

  g_object_unref (SIM_RULE (node->data));
  
  for (child = g_node_last_child (node); child; child = child->prev)
    sim_directive_node_data_destroy (child);
}

/*
 *
 *
 *
 */
SimDirective*
sim_directive_clone (SimDirective     *directive)
{
  SimDirective     *new_directive;
  GTimeVal          curr_time;
  GList            *list;

  g_return_val_if_fail (directive, NULL);
  g_return_val_if_fail (SIM_IS_DIRECTIVE (directive), NULL);

  g_get_current_time (&curr_time);

  new_directive = SIM_DIRECTIVE (g_object_new (SIM_TYPE_DIRECTIVE, NULL));

  new_directive->_priv->id = directive->_priv->id;
  new_directive->_priv->name = g_strdup (directive->_priv->name);
  new_directive->_priv->priority = directive->_priv->priority;

  new_directive->_priv->rule_root = sim_directive_node_data_clone (directive->_priv->rule_root);
  new_directive->_priv->rule_curr = new_directive->_priv->rule_root;

  new_directive->_priv->time_out = directive->_priv->time_out;
  new_directive->_priv->time = ((gint64) curr_time.tv_sec * (gint64) G_USEC_PER_SEC) + (gint64) curr_time.tv_usec;
  new_directive->_priv->time_last = curr_time.tv_sec;
  new_directive->_priv->time_out = sim_directive_get_rule_curr_time_out_max (new_directive);

  new_directive->_priv->matched = directive->_priv->matched;

  return new_directive;
}

/*
 *
 *
 *
 */
gchar*
sim_directive_backlog_get_insert_clause (SimDirective *directive)
{
  SimRule  *rule;
  gchar    *query;

  g_return_val_if_fail (directive, NULL);
  g_return_val_if_fail (SIM_IS_DIRECTIVE (directive), NULL);
  g_return_val_if_fail (directive->_priv->rule_curr, NULL);
  
  rule = (SimRule *) directive->_priv->rule_curr->data;

  query = g_strdup_printf ("INSERT INTO backlog "
			   "(utime, id, name, rule_level, rule_type, rule_name, "
			   "occurrence, time_out, matched, plugin_id, plugin_sid, "
			   "src_ip, dst_ip, src_port, dst_port, condition, value, "
			   "time_interval, absolute, priority, reliability) "
			   "VALUES (%lld, %d, '%s', %d, %d, '%s', %d, %d, %d, %d, %d, "
			   "%u, %u, %d, %d, %d, '%s', %d, %d, %d, %d)",
			   directive->_priv->time,
			   directive->_priv->id,
			   directive->_priv->name,
			   sim_directive_get_rule_level (directive),
			   rule->type,
			   sim_rule_get_name (rule),
			   sim_rule_get_occurrence (rule),
			   directive->_priv->time_out,
			   directive->_priv->matched,
			   sim_rule_get_plugin_id (rule),
			   sim_rule_get_plugin_sid (rule),
			   (sim_rule_get_src_ia (rule)) ? sim_inetaddr_ntohl (sim_rule_get_src_ia (rule)) : -1,
			   (sim_rule_get_dst_ia (rule)) ? sim_inetaddr_ntohl (sim_rule_get_dst_ia (rule)) : -1,
			   sim_rule_get_src_port (rule),
			   sim_rule_get_dst_port (rule),
			   sim_rule_get_condition (rule),
			   sim_rule_get_value (rule),
			   sim_rule_get_interval (rule),
			   sim_rule_get_absolute (rule),
			   sim_rule_get_priority (rule),
			   sim_rule_get_reliability (rule));

  return query;
}

/*
 *
 *
 *
 */
gchar*
sim_directive_backlog_get_update_clause (SimDirective *directive)
{
  SimRule  *rule;
  gchar *query;

  g_return_val_if_fail (directive, NULL);
  g_return_val_if_fail (SIM_IS_DIRECTIVE (directive), NULL);
  g_return_val_if_fail (directive->_priv->rule_curr, NULL);

  rule = (SimRule *) directive->_priv->rule_curr->data;

  query = g_strdup_printf ("UPDATE backlog SET rule_level = %d, rule_type = %d, rule_name = '%s', "
			   "occurrence = %d, time_out = %d, matched = %d, plugin_id = %d, plugin_sid = %d, "
			   "condition = %d, value = '%s', time_interval = %d, absolute = %d, "
			   "priority = %d, reliability = %d "
			   "WHERE utime = %lld AND id = %d",
			   sim_directive_get_rule_level (directive),
			   rule->type,
			   sim_rule_get_name (rule),
			   sim_rule_get_occurrence (rule),
			   directive->_priv->time_out,
			   directive->_priv->matched,
			   sim_rule_get_plugin_id (rule),
			   sim_rule_get_plugin_sid (rule),
			   sim_rule_get_condition (rule),
			   sim_rule_get_value (rule),
			   sim_rule_get_interval (rule),
			   sim_rule_get_absolute (rule),
			   sim_rule_get_priority (rule),
			   sim_rule_get_reliability (rule),
			   directive->_priv->time,
			   directive->_priv->id);

  return query;
}

/*
 *
 *
 *
 */
gchar*
sim_directive_backlog_get_delete_clause (SimDirective *directive)
{
  gchar *query;

  g_return_val_if_fail (directive, NULL);
  g_return_val_if_fail (SIM_IS_DIRECTIVE (directive), NULL);
  g_return_val_if_fail (directive->_priv->rule_curr, NULL);

  query = g_strdup_printf ("DELETE FROM backlog WHERE utime = %lld AND id = %d",
			   directive->_priv->time,
			   directive->_priv->id);


  return query;
}

void
sim_directive_print (SimDirective  *directive)
{
  g_return_if_fail (directive);
  g_return_if_fail (SIM_IS_DIRECTIVE (directive));

  g_print ("DIRECTIVE: name=\"%s\"\n", directive->_priv->name);
}
