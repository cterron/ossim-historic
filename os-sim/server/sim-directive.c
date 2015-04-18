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

enum 
{
  DESTROY,
  LAST_SIGNAL
};

struct _SimDirectivePrivate {
  gint       id;
  gchar     *name;

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

  g_free (directive->_priv->name);

  sim_directive_node_data_destroy (directive->_priv->rule_root);
  g_node_destroy (directive->_priv->rule_root);

  list = directive->_priv->actions;
  while (list)
    {
      SimAction *action = (SimAction *) list->data;
      g_object_unref (action);
      list = list->next;
    }
  g_list_free (directive->_priv->actions);
  
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
  g_return_val_if_fail (directive != NULL, 0);
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
  g_return_if_fail (directive != NULL);
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
  g_return_val_if_fail (directive != NULL, NULL);
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
			     gchar          *name)
{
  g_return_if_fail (directive != NULL);
  g_return_if_fail (SIM_IS_DIRECTIVE (directive));
  g_return_if_fail (name != NULL);

  directive->_priv->name = name;
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
void sim_directive_set_time_out (SimDirective   *directive,
				GTime           time_out)
{
  g_return_if_fail (directive);
  g_return_if_fail (SIM_IS_DIRECTIVE (directive));
  g_return_if_fail (time_out > 0);

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
  g_return_if_fail (time_last > 0);

  directive->_priv->time_out = time_last;
}

/*
 *
 *
 *
 *
 */
GNode*
sim_directive_get_rule_root (SimDirective  *directive)
{
  g_return_val_if_fail (directive != NULL, NULL);
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
sim_directive_set_rule_root (SimDirective  *directive,
			     GNode         *rule_root)
{
  g_return_if_fail (directive != NULL);
  g_return_if_fail (SIM_IS_DIRECTIVE (directive));
  g_return_if_fail (rule_root != NULL);

  directive->_priv->rule_root = rule_root;
}

/*
 *
 *
 *
 *
 */
GNode*
sim_directive_get_rule_curr (SimDirective  *directive)
{
  g_return_val_if_fail (directive != NULL, NULL);
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
sim_directive_set_rule_curr (SimDirective  *directive,
			     GNode         *rule_curr)
{
  g_return_if_fail (directive != NULL);
  g_return_if_fail (SIM_IS_DIRECTIVE (directive));
  g_return_if_fail (rule_curr != NULL);

  directive->_priv->rule_curr = rule_curr;
}

/*
 *
 *
 *
 *
 */
void
sim_directive_append_action (SimDirective     *directive,
			     SimAction        *action)
{
  g_return_if_fail (directive != NULL);
  g_return_if_fail (SIM_IS_DIRECTIVE (directive));
  g_return_if_fail (action != NULL);
  g_return_if_fail (SIM_IS_ACTION (action));

  directive->_priv->actions = g_list_append (directive->_priv->actions, action);
}

/*
 *
 *
 *
 *
 */
void
sim_directive_remove_action (SimDirective     *directive,
			     SimAction        *action)
{
  g_return_if_fail (directive != NULL);
  g_return_if_fail (SIM_IS_DIRECTIVE (directive));
  g_return_if_fail (action != NULL);
  g_return_if_fail (SIM_IS_ACTION (action));

  directive->_priv->actions = g_list_remove (directive->_priv->actions, action);
}

/*
 *
 *
 *
 *
 */
GList*
sim_directive_get_actions (SimDirective     *directive)
{
  g_return_val_if_fail (directive != NULL, NULL);
  g_return_val_if_fail (SIM_IS_DIRECTIVE (directive), NULL);

  return directive->_priv->actions;
}

/*
 *
 *
 *
 *
 */
gint
sim_directive_get_level (SimDirective   *directive)
{
  g_return_val_if_fail (directive != NULL, 0);
  g_return_val_if_fail (SIM_IS_DIRECTIVE (directive), 0);

  return g_node_depth (directive->_priv->rule_curr);
}

/*
 *
 *
 *
 */
gboolean
sim_directive_match_rule_root_by_message (SimDirective  *directive,
					  SimMessage    *message)
{
  SimRule *rule;

  g_return_val_if_fail (directive != NULL, FALSE);
  g_return_val_if_fail (SIM_IS_DIRECTIVE (directive), FALSE);
  g_return_val_if_fail (message != NULL, FALSE);
  g_return_val_if_fail (SIM_IS_MESSAGE (message), FALSE);
  g_return_val_if_fail (directive->_priv->rule_root != NULL, FALSE);

  rule = (SimRule *) directive->_priv->rule_root->data;

  return sim_rule_match_by_message (rule, message);
}

/*
 *
 *
 *
 */
gboolean
sim_directive_match_rule_by_message (SimDirective  *directive,
				     SimMessage    *message)
{
  GNode *node = NULL;
  GNode *children = NULL;
  GTimeVal          curr_time;

  g_return_val_if_fail (directive != NULL, FALSE);
  g_return_val_if_fail (SIM_IS_DIRECTIVE (directive), FALSE);
  g_return_val_if_fail (message != NULL, FALSE);
  g_return_val_if_fail (SIM_IS_MESSAGE (message), FALSE);
  g_return_val_if_fail (directive->_priv->rule_curr != NULL, FALSE);

  node = directive->_priv->rule_curr->children;

  while (node)
    {
      SimRule *rule = (SimRule *) node->data;

      if (sim_rule_match_by_message (rule, message))
	{
	  g_get_current_time (&curr_time);

	  directive->_priv->rule_curr = node;
	  directive->_priv->time_last = curr_time.tv_sec;

	  sim_rule_set_message_data (rule, message);

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

  g_return_if_fail (directive != NULL);
  g_return_if_fail (SIM_IS_DIRECTIVE (directive));
  g_return_if_fail (node != NULL);
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

  g_return_val_if_fail (directive != NULL, NULL);
  g_return_val_if_fail (SIM_IS_DIRECTIVE (directive), NULL);
  g_return_val_if_fail (node != NULL, NULL);
  
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
gboolean
sim_directive_matched (SimDirective     *directive)
{
  g_return_val_if_fail (directive != NULL, FALSE);
  g_return_val_if_fail (SIM_IS_DIRECTIVE (directive), FALSE);

  return directive->_priv->matched;
}

/*
 *
 *
 *
 */
GNode*
sim_directive_node_data_clone (GNode *node)
{
  GNode *new_node = NULL;

  if (node)
    {
      GNode   *child;

      SimRule *new_rule = sim_rule_clone ((SimRule *) node->data);

      new_node = g_node_new (new_rule);
       
      for (child = g_node_last_child (node); child; child = child->prev)
        g_node_prepend (new_node, sim_directive_node_data_clone (child));
    }
   
  return new_node;
}

/*
 *
 *
 *
 */
GNode*
sim_directive_node_data_destroy (GNode *node)
{
  GNode *new_node = NULL;

  if (node)
    {
      GNode   *child;

      SimRule *rule = node->data;

      g_object_unref (rule);

      for (child = g_node_last_child (node); child; child = child->prev)
        sim_directive_node_data_destroy (child);
    }
   
  return new_node;
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

  g_return_val_if_fail (directive != NULL, NULL);
  g_return_val_if_fail (SIM_IS_DIRECTIVE (directive), NULL);

  g_get_current_time (&curr_time);

  new_directive = SIM_DIRECTIVE (g_object_new (SIM_TYPE_DIRECTIVE, NULL));

  new_directive->_priv->id = directive->_priv->id;
  new_directive->_priv->name = g_strdup (directive->_priv->name);

  new_directive->_priv->time_out = directive->_priv->time_out;
  new_directive->_priv->time = ((gint64) curr_time.tv_sec * (gint64) G_USEC_PER_SEC) + (gint64) curr_time.tv_usec;
  new_directive->_priv->time_last = curr_time.tv_sec;

  new_directive->_priv->matched = directive->_priv->matched;

  new_directive->_priv->rule_root = sim_directive_node_data_clone (directive->_priv->rule_root);
  new_directive->_priv->rule_curr = new_directive->_priv->rule_root;

  /* Actions */
  list = directive->_priv->actions;
  while (list)
    {
      SimAction *action = (SimAction *) list->data;
      SimAction *new_action = sim_action_clone (action);

      new_directive->_priv->actions = g_list_append (new_directive->_priv->actions, new_action);

      list = list->next;
    }

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
  gchar *query;

  g_return_val_if_fail (directive, NULL);
  g_return_val_if_fail (SIM_IS_DIRECTIVE (directive), NULL);
  g_return_val_if_fail (directive->_priv->rule_curr, NULL);
  
  rule = (SimRule *) directive->_priv->rule_curr->data;

  query = g_strdup_printf ("INSERT INTO backlog VALUES (%lld, %d, '%s', %d, %d, %d, '%s', '%s', '%s', %d, %d, %d, %d)",
			   directive->_priv->time,
			   directive->_priv->id,
			   directive->_priv->name,
			   directive->_priv->time_out,
			   directive->_priv->matched,
			   sim_directive_get_level (directive),
			   sim_rule_get_name (rule),
			   (sim_rule_get_src_ia (rule)) ? gnet_inetaddr_get_canonical_name (sim_rule_get_src_ia (rule)) : NULL,
			   (sim_rule_get_src_ia (rule)) ? gnet_inetaddr_get_canonical_name (sim_rule_get_dst_ia (rule)) : NULL,
			   sim_rule_get_src_port (rule),
			   sim_rule_get_dst_port (rule),
			   sim_rule_get_plugin (rule),
			   sim_rule_get_tplugin (rule));

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

  query = g_strdup_printf ("UPDATE backlog SET matched = %d, level = %d, rule_name = '%s', plugin = %d, tplugin = %d WHERE utime = %lld AND id = %d",
			   directive->_priv->matched,
			   sim_directive_get_level (directive),
			   sim_rule_get_name (rule),
			   sim_rule_get_plugin (rule),
			   sim_rule_get_tplugin (rule),
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
