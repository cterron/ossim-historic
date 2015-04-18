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

#include "sim-rule.h"

enum
{
  DESTROY,
  LAST_SIGNAL
};

struct _SimRulePrivate {
  gchar      *name;
  gint        level;

  gint        timer;
  gint        occurence;
  gint        value;

  GInetAddr  *src_ia;
  GInetAddr  *dst_ia;
  gint        src_port;
  gint        dst_port;
  gint        plugin;
  gint        tplugin;

  GList      *actions;
  GList      *vars;
  GList      *src_ias;
  GList      *dst_ias;
  GList      *src_ports;
  GList      *dst_ports;
};

static gpointer parent_class = NULL;
static gint sim_rule_signals[LAST_SIGNAL] = { 0 };

/* GType Functions */

static void 
sim_rule_impl_dispose (GObject  *gobject)
{
  G_OBJECT_CLASS (parent_class)->dispose (gobject);
}

static void 
sim_rule_impl_finalize (GObject  *gobject)
{
  SimRule *rule = SIM_RULE (gobject);
  GList   *list;

  g_free (rule->_priv->name);

  if (rule->_priv->src_ia)
    gnet_inetaddr_unref (rule->_priv->src_ia);
  if (rule->_priv->dst_ia)
    gnet_inetaddr_unref (rule->_priv->dst_ia);

  /* Actions */
  list = rule->_priv->actions;
  while (list)
    {
      SimAction *action = (SimAction *) list->data;
      g_object_unref (action);
      list = list->next;
    }
  g_list_free (rule->_priv->actions);

  /* vars */
  list = rule->_priv->vars;
  while (list)
    {
      SimRuleVar *rule_var = (SimRuleVar *) list->data;
      g_free (rule_var);
      list = list->next;
    }
  g_list_free (rule->_priv->vars);

  /* src ips */
  list = rule->_priv->src_ias;
  while (list)
    {
      GInetAddr *ia = (GInetAddr *) list->data;
      gnet_inetaddr_unref (ia);
      list = list->next;
    }
  g_list_free (rule->_priv->src_ias);

  /* dst ips */
  list = rule->_priv->dst_ias;
  while (list)
    {
      GInetAddr *ia = (GInetAddr *) list->data;
      gnet_inetaddr_unref (ia);
      list = list->next;
    }
  g_list_free (rule->_priv->dst_ias);

  /* src ports */
  list = rule->_priv->src_ports;
  while (list)
    {
      rule->_priv->src_ports = g_list_remove (rule->_priv->src_ports, list->data);
      list = list->next;
    }
  g_list_free (rule->_priv->src_ports);
 
  /* dst ports */
  list = rule->_priv->dst_ports;
  while (list)
    {
      rule->_priv->dst_ports = g_list_remove (rule->_priv->dst_ports, list->data); 
      list = list->next;
    }
  g_list_free (rule->_priv->dst_ports);

  G_OBJECT_CLASS (parent_class)->finalize (gobject);
}

static void
sim_rule_class_init (SimRuleClass * class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  parent_class = g_type_class_ref (G_TYPE_OBJECT);

  object_class->dispose = sim_rule_impl_dispose;
  object_class->finalize = sim_rule_impl_finalize;
}

static void
sim_rule_instance_init (SimRule *rule)
{
  rule->_priv = g_new0 (SimRulePrivate, 1);

  rule->type = SIM_RULE_TYPE_NONE;

  rule->_priv->name = NULL;
  rule->_priv->level = 0;

  rule->_priv->timer = 0;
  rule->_priv->occurence = 0;
  rule->_priv->value = 0;

  rule->_priv->plugin = 0;
  rule->_priv->tplugin = 0;
  rule->_priv->src_ia = NULL;
  rule->_priv->dst_ia = NULL;
  rule->_priv->src_port = 0;
  rule->_priv->dst_port = 0;

  rule->_priv->actions = NULL;
  rule->_priv->vars = NULL;
  rule->_priv->src_ias = NULL;
  rule->_priv->dst_ias = NULL;
  rule->_priv->src_ports = NULL;
  rule->_priv->dst_ports = NULL;
}

/* Public Methods */

GType
sim_rule_get_type (void)
{
  static GType object_type = 0;
 
  if (!object_type)
  {
    static const GTypeInfo type_info = {
              sizeof (SimRuleClass),
              NULL,
              NULL,
              (GClassInitFunc) sim_rule_class_init,
              NULL,
              NULL,                       /* class data */
              sizeof (SimRule),
              0,                          /* number of pre-allocs */
              (GInstanceInitFunc) sim_rule_instance_init,
              NULL                        /* value table */
    };
    
    g_type_init ();
                                                                                                                             
    object_type = g_type_register_static (G_TYPE_OBJECT, "SimRule", &type_info, 0);
  }

  return object_type;
}

/*
 *
 *
 *
 *
 */
SimRule*
sim_rule_new (void)
{
  SimRule *rule = NULL;

  rule = SIM_RULE (g_object_new (SIM_TYPE_RULE, NULL));

  return rule;
}

/*
 *
 *
 *
 *
 */
gint
sim_rule_get_level (SimRule   *rule)
{
  g_return_val_if_fail (rule != NULL, 0);
  g_return_val_if_fail (SIM_IS_RULE (rule), 0);

  return rule->_priv->level;
}

/*
 *
 *
 *
 *
 */
void sim_rule_set_level (SimRule   *rule,
			 gint       level)
{
  g_return_if_fail (rule != NULL);
  g_return_if_fail (SIM_IS_RULE (rule));
  g_return_if_fail (level > 0);

  rule->_priv->level = level;
}

/*
 *
 *
 *
 *
 */
gint
sim_rule_get_plugin (SimRule   *rule)
{
  g_return_val_if_fail (rule != NULL, 0);
  g_return_val_if_fail (SIM_IS_RULE (rule), 0);

  return rule->_priv->plugin;
}

/*
 *
 *
 *
 *
 */
void sim_rule_set_plugin (SimRule   *rule,
			 gint       plugin)
{
  g_return_if_fail (rule != NULL);
  g_return_if_fail (SIM_IS_RULE (rule));
  g_return_if_fail (plugin >= 0);

  rule->_priv->plugin = plugin;
}

/*
 *
 *
 *
 *
 */
gint
sim_rule_get_tplugin (SimRule   *rule)
{
  g_return_val_if_fail (rule != NULL, 0);
  g_return_val_if_fail (SIM_IS_RULE (rule), 0);

  return rule->_priv->tplugin;
}

/*
 *
 *
 *
 *
 */
void sim_rule_set_tplugin (SimRule   *rule,
			 gint       tplugin)
{
  g_return_if_fail (rule != NULL);
  g_return_if_fail (SIM_IS_RULE (rule));
  g_return_if_fail (tplugin >= 0);

  rule->_priv->tplugin = tplugin;
}

/*
 *
 *
 *
 *
 */
gchar*
sim_rule_get_name (SimRule   *rule)
{
  g_return_val_if_fail (rule != NULL, NULL);
  g_return_val_if_fail (SIM_IS_RULE (rule), NULL);

  return rule->_priv->name;
}

/*
 *
 *
 *
 *
 */
void sim_rule_set_name (SimRule   *rule,
			gchar     *name)
{
  g_return_if_fail (rule != NULL);
  g_return_if_fail (SIM_IS_RULE (rule));
  g_return_if_fail (name != NULL);

  rule->_priv->name = name;
}

/*
 *
 *
 *
 *
 */
GInetAddr*
sim_rule_get_src_ia (SimRule   *rule)
{
  g_return_val_if_fail (rule != NULL, NULL);
  g_return_val_if_fail (SIM_IS_RULE (rule), NULL);

  return rule->_priv->src_ia;
}

/*
 *
 *
 *
 *
 */
void sim_rule_set_src_ia (SimRule    *rule,
			  GInetAddr  *src_ia)
{
  g_return_if_fail (rule != NULL);
  g_return_if_fail (SIM_IS_RULE (rule));
  g_return_if_fail (src_ia != NULL);

  rule->_priv->src_ia = src_ia;
}

/*
 *
 *
 *
 *
 */
GInetAddr*
sim_rule_get_dst_ia (SimRule   *rule)
{
  g_return_val_if_fail (rule != NULL, NULL);
  g_return_val_if_fail (SIM_IS_RULE (rule), NULL);

  return rule->_priv->dst_ia;
}

/*
 *
 *
 *
 *
 */
void sim_rule_set_dst_ia (SimRule    *rule,
			  GInetAddr  *dst_ia)
{
  g_return_if_fail (rule != NULL);
  g_return_if_fail (SIM_IS_RULE (rule));
  g_return_if_fail (dst_ia != NULL);

  rule->_priv->dst_ia = dst_ia;
}

/*
 *
 *
 *
 *
 */
gint
sim_rule_get_src_port (SimRule   *rule)
{
  g_return_val_if_fail (rule != NULL, 0);
  g_return_val_if_fail (SIM_IS_RULE (rule), 0);

  return rule->_priv->src_port;
}

/*
 *
 *
 *
 *
 */
void sim_rule_set_src_port (SimRule   *rule,
			    gint       src_port)
{
  g_return_if_fail (rule != NULL);
  g_return_if_fail (SIM_IS_RULE (rule));
  g_return_if_fail (src_port >= 0);

  rule->_priv->src_port = src_port;
}

/*
 *
 *
 *
 *
 */
gint
sim_rule_get_dst_port (SimRule   *rule)
{
  g_return_val_if_fail (rule != NULL, 0);
  g_return_val_if_fail (SIM_IS_RULE (rule), 0);

  return rule->_priv->dst_port;
}

/*
 *
 *
 *
 *
 */
void sim_rule_set_dst_port (SimRule   *rule,
			    gint       dst_port)
{
  g_return_if_fail (rule != NULL);
  g_return_if_fail (SIM_IS_RULE (rule));
  g_return_if_fail (dst_port > -1);

  rule->_priv->dst_port = dst_port;
}

/*
 *
 *
 *
 *
 */
void
sim_rule_append_action (SimRule     *rule,
			SimAction   *action)
{
  g_return_if_fail (rule != NULL);
  g_return_if_fail (SIM_IS_RULE (rule));
  g_return_if_fail (action != NULL);
  g_return_if_fail (SIM_IS_ACTION (action));

  rule->_priv->actions = g_list_append (rule->_priv->actions, action);
}

/*
 *
 *
 *
 *
 */
void
sim_rule_remove_action (SimRule     *rule,
			SimAction   *action)
{
  g_return_if_fail (rule != NULL);
  g_return_if_fail (SIM_IS_RULE (rule));
  g_return_if_fail (action != NULL);
  g_return_if_fail (SIM_IS_ACTION (action));

  rule->_priv->actions = g_list_remove (rule->_priv->actions, action);
}

/*
 *
 *
 *
 *
 */
GList*
sim_rule_get_actions (SimRule     *rule)
{
  g_return_val_if_fail (rule != NULL, NULL);
  g_return_val_if_fail (SIM_IS_RULE (rule), NULL);

  return rule->_priv->actions;
}

/*
 *
 *
 *
 *
 */
void
sim_rule_append_src_ia (SimRule    *rule,
			GInetAddr  *src_ia)
{
  g_return_if_fail (rule != NULL);
  g_return_if_fail (SIM_IS_RULE (rule));

  rule->_priv->src_ias = g_list_append (rule->_priv->src_ias, src_ia);
}

/*
 *
 *
 *
 *
 */
void
sim_rule_remove_src_ia (SimRule    *rule,
			GInetAddr  *src_ia)
{
  g_return_if_fail (rule != NULL);
  g_return_if_fail (SIM_IS_RULE (rule));

  rule->_priv->src_ias = g_list_remove (rule->_priv->src_ias, src_ia);
}

/*
 *
 *
 *
 *
 */
GList*
sim_rule_get_src_ias (SimRule   *rule)
{
  g_return_val_if_fail (rule != NULL, NULL);
  g_return_val_if_fail (SIM_IS_RULE (rule), NULL);

  return rule->_priv->src_ias;
}

/*
 *
 *
 *
 *
 */
void
sim_rule_append_dst_ia (SimRule    *rule,
			GInetAddr  *dst_ia)
{
  g_return_if_fail (rule != NULL);
  g_return_if_fail (SIM_IS_RULE (rule));

  rule->_priv->dst_ias = g_list_append (rule->_priv->dst_ias, dst_ia);
}

/*
 *
 *
 *
 *
 */
void
sim_rule_remove_dst_ia (SimRule    *rule,
			GInetAddr  *dst_ia)
{
  g_return_if_fail (rule);
  g_return_if_fail (SIM_IS_RULE (rule));
  g_return_if_fail (dst_ia);

  rule->_priv->dst_ias = g_list_remove (rule->_priv->dst_ias, dst_ia);
}

/*
 *
 *
 *
 *
 */
GList*
sim_rule_get_dst_ias (SimRule   *rule)
{
  g_return_val_if_fail (rule, NULL);
  g_return_val_if_fail (SIM_IS_RULE (rule), NULL);

  return rule->_priv->dst_ias;
}

/*
 *
 *
 *
 *
 */
void
sim_rule_append_src_port (SimRule   *rule,
			  gint       src_port)
{
  g_return_if_fail (rule);
  g_return_if_fail (SIM_IS_RULE (rule));

  rule->_priv->src_ports = g_list_append (rule->_priv->src_ports, GINT_TO_POINTER (src_port));
}

/*
 *
 *
 *
 *
 */
void
sim_rule_remove_src_port (SimRule   *rule,
			  gint       src_port)
{
  g_return_if_fail (rule);
  g_return_if_fail (SIM_IS_RULE (rule));

  rule->_priv->src_ports = g_list_remove (rule->_priv->src_ports, GINT_TO_POINTER (src_port));
}

/*
 *
 *
 *
 *
 */
GList*
sim_rule_get_src_ports (SimRule   *rule)
{
  g_return_val_if_fail (rule != NULL, NULL);
  g_return_val_if_fail (SIM_IS_RULE (rule), NULL);

  return rule->_priv->src_ports;
}

/*
 *
 *
 *
 *
 */
void
sim_rule_append_dst_port (SimRule   *rule,
			  gint       dst_port)
{
  g_return_if_fail (rule != NULL);
  g_return_if_fail (SIM_IS_RULE (rule));

  rule->_priv->dst_ports = g_list_append (rule->_priv->dst_ports, GINT_TO_POINTER (dst_port));
}

/*
 *
 *
 *
 *
 */
void
sim_rule_remove_dst_port (SimRule   *rule,
			  gint       dst_port)
{
  g_return_if_fail (rule != NULL);
  g_return_if_fail (SIM_IS_RULE (rule));

  rule->_priv->dst_ports = g_list_remove (rule->_priv->dst_ports, GINT_TO_POINTER (dst_port));
}

/*
 *
 *
 *
 *
 */
GList*
sim_rule_get_dst_ports (SimRule   *rule)
{
  g_return_val_if_fail (rule != NULL, NULL);
  g_return_val_if_fail (SIM_IS_RULE (rule), NULL);

  return rule->_priv->dst_ports;
}

/*
 *
 *
 *
 *
 */
void
sim_rule_append_var (SimRule         *rule,
		     SimRuleVar      *var)
{
  g_return_if_fail (rule != NULL);
  g_return_if_fail (SIM_IS_RULE (rule));

  rule->_priv->vars = g_list_append (rule->_priv->vars, var);  
}

/*
 *
 *
 *
 *
 */
GList*
sim_rule_get_vars (SimRule     *rule)
{
  g_return_val_if_fail (rule != NULL, NULL);
  g_return_val_if_fail (SIM_IS_RULE (rule), NULL);
  
  return rule->_priv->vars;
}

/*
 *
 *
 *
 *
 */
SimRule*
sim_rule_clone (SimRule     *rule)
{
  SimRule     *new_rule;
  GList       *list;

  g_return_val_if_fail (rule != NULL, FALSE);
  g_return_val_if_fail (SIM_IS_RULE (rule), FALSE);

  new_rule = SIM_RULE (g_object_new (SIM_TYPE_RULE, NULL));

  new_rule->_priv->name = g_strdup (rule->_priv->name);
  new_rule->_priv->level = rule->_priv->level;

  new_rule->_priv->timer = rule->_priv->timer;
  new_rule->_priv->occurence = rule->_priv->occurence;
  new_rule->_priv->value = rule->_priv->value;

  new_rule->_priv->plugin = rule->_priv->plugin;
  new_rule->_priv->tplugin = rule->_priv->tplugin;
  if (rule->_priv->src_ia) new_rule->_priv->src_ia = gnet_inetaddr_clone (rule->_priv->src_ia);
  if (rule->_priv->dst_ia) new_rule->_priv->dst_ia = gnet_inetaddr_clone (rule->_priv->dst_ia);
  new_rule->_priv->src_port = rule->_priv->src_port;
  new_rule->_priv->dst_port = rule->_priv->dst_port;

  /* Actions */
  list = rule->_priv->actions;
  while (list)
    {
      SimAction *action = (SimAction *) list->data;
      SimAction *new_action = sim_action_clone (action);

      new_rule->_priv->actions = g_list_append (new_rule->_priv->actions, new_action);

      list = list->next;
    }


  /* vars */
  list = rule->_priv->vars;
  while (list)
    {
      SimRuleVar *rule_var = (SimRuleVar *) list->data;

      SimRuleVar  *new_rule_var = g_new0 (SimRuleVar, 1);
      new_rule_var->type = rule_var->type;
      new_rule_var->attr = rule_var->attr;
      new_rule_var->level = rule_var->level;

      new_rule->_priv->vars = g_list_append (new_rule->_priv->vars, new_rule_var);
      list = list->next;
    }

  /* src ips */
  list = rule->_priv->src_ias;
  while (list)
    {
      GInetAddr *ia = (GInetAddr *) list->data;
      new_rule->_priv->src_ias = g_list_append (new_rule->_priv->src_ias, gnet_inetaddr_clone (ia));
      list = list->next;
    }

  /* dst ips */
  list = rule->_priv->dst_ias;
  while (list)
    {
      GInetAddr *ia = (GInetAddr *) list->data;
      new_rule->_priv->dst_ias = g_list_append (new_rule->_priv->dst_ias, gnet_inetaddr_clone (ia));
      list = list->next;
    }

  /* src ports */
  list = rule->_priv->src_ports;
  while (list)
    {
      gint port = GPOINTER_TO_INT (list->data);
      new_rule->_priv->src_ports = g_list_append (new_rule->_priv->src_ports, GINT_TO_POINTER (port));
      list = list->next;
    }
 
  /* dst ports */
  list = rule->_priv->dst_ports;
  while (list)
    {
      gint port = GPOINTER_TO_INT (list->data);
      new_rule->_priv->dst_ports = g_list_append (new_rule->_priv->dst_ports, GINT_TO_POINTER (port)); 
      list = list->next;
    }

  return new_rule;
}

/*
 *
 *
 *
 *
 */
gboolean
sim_rule_match_by_message (SimRule      *rule,
			   SimMessage   *message)
{
  GList      *list = NULL;
  gboolean    match;
  GInetAddr  *src_ia;
  GInetAddr  *dst_ia;
  gint        src_port;
  gint        dst_port;
  gint        plugin;
  gint        tplugin;

  g_return_val_if_fail (rule != NULL, FALSE);
  g_return_val_if_fail (SIM_IS_RULE (rule), FALSE);
  g_return_val_if_fail (message != NULL, FALSE);
  g_return_val_if_fail (SIM_IS_MESSAGE (message), FALSE);

  src_ia = sim_message_get_src_ia (message);
  dst_ia = sim_message_get_dst_ia (message);
  src_port = sim_message_get_src_port (message);
  dst_port = sim_message_get_dst_port (message);
  plugin = sim_message_get_plugin (message);
  tplugin = sim_message_get_tplugin (message);

  if (rule->_priv->plugin != plugin)
    return FALSE;

  if (rule->_priv->tplugin != tplugin)
    return FALSE;

  /* Find src_ia */
  match = FALSE;
  list = rule->_priv->src_ias;
  while (list)
    {
      GInetAddr *cmp_ia = (GInetAddr *) list->data;
      
      if ((gnet_inetaddr_is_reserved (cmp_ia)) || (gnet_inetaddr_noport_equal (src_ia, cmp_ia)))
	{
	  match = TRUE;
	  break;
	}

      list = list->next;
    }
  if (!match)
    return FALSE;

  /* Find dst_ia */
  match = FALSE;
  list = rule->_priv->dst_ias;
  while (list)
    {
      GInetAddr *cmp_ia = (GInetAddr *) list->data;

      if ((gnet_inetaddr_is_reserved (cmp_ia)) || (gnet_inetaddr_noport_equal (dst_ia, cmp_ia)))
	{
	  match = TRUE;
	  break;
	}

      list = list->next;
    }
  if (!match)
    return FALSE;

  /* Find src_port */
  match = FALSE;
  list = rule->_priv->src_ports;
  while (list)
    {
      gint cmp_port = GPOINTER_TO_INT (list->data);

      if ((cmp_port == 0) || (cmp_port == src_port))
	{
	  match = TRUE;
	  break;
	}

      list = list->next;
    }
  if (!match)
    return FALSE;

 /* Find dst_port */
  match = FALSE;
  list = rule->_priv->dst_ports;
  while (list)
    {
      gint cmp_port = GPOINTER_TO_INT (list->data);

      if ((cmp_port == 0) || (cmp_port == dst_port))
	{
	  match = TRUE;
	  break;
	}

      list = list->next;
    }
  if (!match)
    return FALSE;

  return TRUE;
}

/*
 *
 *
 *
 *
 */
void
sim_rule_set_message_data (SimRule      *rule,
			   SimMessage   *message)
{
  GInetAddr  *src_ia;
  GInetAddr  *dst_ia;
  gint        src_port;
  gint        dst_port;
  gint        plugin;
  gint        tplugin;

  g_return_if_fail (rule != NULL);
  g_return_if_fail (SIM_IS_RULE (rule));
  g_return_if_fail (message != NULL);
  g_return_if_fail (SIM_IS_MESSAGE (message));

  src_ia = sim_message_get_src_ia (message);
  dst_ia = sim_message_get_dst_ia (message);
  src_port = sim_message_get_src_port (message);
  dst_port = sim_message_get_dst_port (message);
  plugin = sim_message_get_plugin (message);
  tplugin = sim_message_get_tplugin (message);

  if (src_ia) rule->_priv->src_ia = gnet_inetaddr_clone (src_ia);
  if (dst_ia) rule->_priv->dst_ia = gnet_inetaddr_clone (dst_ia);
  rule->_priv->src_port = src_port;
  rule->_priv->dst_port = dst_port;
  rule->_priv->plugin = plugin;
  rule->_priv->tplugin = tplugin;
}

/*
 *
 *
 *
 *
 */
void
sim_rule_print (SimRule      *rule)
{
  g_print ("Rule: ");
  g_print ("name=%s ", rule->_priv->name);
  g_print ("level=%d ", rule->_priv->level);
  g_print ("timer=%d ", rule->_priv->timer);
  g_print ("ocurrence=%d ", rule->_priv->occurence);
  g_print ("value=%d ", rule->_priv->value);
  g_print ("vars=%d ", g_list_length (rule->_priv->vars));
  g_print ("src_ias=%d ", g_list_length (rule->_priv->src_ias));
  g_print ("dst_ias=%d ", g_list_length (rule->_priv->dst_ias));
  g_print ("src_ports=%d ", g_list_length (rule->_priv->src_ports));
  g_print ("dst_ports=%d ", g_list_length (rule->_priv->dst_ports));
  g_print ("src_ia=%s ", (rule->_priv->src_ia) ? gnet_inetaddr_get_canonical_name (rule->_priv->src_ia) : NULL);
  g_print ("dst_ia=%s ", (rule->_priv->src_ia) ? gnet_inetaddr_get_canonical_name (rule->_priv->dst_ia) : NULL);
  g_print ("src_port=%d ", rule->_priv->src_port);
  g_print ("dst_port=%d ", rule->_priv->dst_port);
  g_print ("plugin=%d ", rule->_priv->plugin);
  g_print ("tplugin=%d ", rule->_priv->tplugin);
  g_print ("\n");
}
