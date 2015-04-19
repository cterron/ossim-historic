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

#include <time.h>

enum
{
  DESTROY,
  LAST_SIGNAL
};

struct _SimRulePrivate {
  gint        level;
  gchar      *name;
  gboolean    not;
  gboolean    not_invalid;

  gint        priority;
  gint        reliability;
  gboolean    rel_abs;

  GTime       time_out;
  GTime       time_last;
  gint        occurrence;

  SimConditionType   condition;
  gchar             *value;
  gint               interval;
  gboolean           absolute;

  gint        count_occu;

  gint        plugin_id;
  gint        plugin_sid;
  GInetAddr  *src_ia;
  GInetAddr  *dst_ia;
  gint        src_port;
  gint        dst_port;
  SimProtocolType    protocol;

  gboolean         sticky;
  SimRuleVarType   sticky_different;
  GList           *stickys;

  gboolean    plugin_sids_not;
  gboolean    src_ias_not;
  gboolean    dst_ias_not;
  gboolean    src_ports_not;
  gboolean    dst_ports_not;
  gboolean    protocols_not;

  GList      *vars;
  GList      *plugin_sids;
  GList      *src_inets;
  GList      *dst_inets;
  GList      *src_ports;
  GList      *dst_ports;
  GList      *protocols;
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

  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_rule_impl_finalize: Name %s, Level %d", rule->_priv->name, rule->_priv->level);

  if (rule->_priv->name)
    g_free (rule->_priv->name);

  if (rule->_priv->value)
    g_free (rule->_priv->value);

  if (rule->_priv->src_ia)
    gnet_inetaddr_unref (rule->_priv->src_ia);
  if (rule->_priv->dst_ia)
    gnet_inetaddr_unref (rule->_priv->dst_ia);

  /* vars */
  list = rule->_priv->vars;
  while (list)
    {
      SimRuleVar *rule_var = (SimRuleVar *) list->data;
      g_free (rule_var);
      list = list->next;
    }
  g_list_free (rule->_priv->vars);

  /* Plugin Sids */
  g_list_free (rule->_priv->plugin_sids);

  /* src ips */
  list = rule->_priv->src_inets;
  while (list)
    {
      SimInet *inet = (SimInet *) list->data;
      g_object_unref (inet);
      list = list->next;
    }
  g_list_free (rule->_priv->src_inets);

  /* dst ips */
  list = rule->_priv->dst_inets;
  while (list)
    {
      SimInet *inet = (SimInet *) list->data;
      g_object_unref (inet);
      list = list->next;
    }
  g_list_free (rule->_priv->dst_inets);

  /* src ports */
  g_list_free (rule->_priv->src_ports);
 
  /* dst ports */
  g_list_free (rule->_priv->dst_ports);

  /* protocols */
  g_list_free (rule->_priv->protocols);

  /* stickys */
  g_list_free (rule->_priv->stickys);

  g_free (rule->_priv);

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

  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_rule_instance_init");

  rule->type = SIM_RULE_TYPE_NONE;

  rule->_priv->level = 0;
  rule->_priv->name = NULL;
  rule->_priv->not = FALSE;
  rule->_priv->not_invalid = FALSE;

  rule->_priv->priority = 0;
  rule->_priv->reliability = 0;
  rule->_priv->rel_abs = TRUE;

  rule->_priv->condition = SIM_CONDITION_TYPE_NONE;
  rule->_priv->value = NULL;
  rule->_priv->interval = 0;
  rule->_priv->absolute = FALSE;

  rule->_priv->time_out = 0;
  rule->_priv->time_last = 0;
  rule->_priv->occurrence = 1;

  rule->_priv->count_occu = 1;

  rule->_priv->plugin_id = 0;
  rule->_priv->plugin_sid = 0;
  rule->_priv->src_ia = NULL;
  rule->_priv->dst_ia = NULL;
  rule->_priv->src_port = 0;
  rule->_priv->dst_port = 0;
  rule->_priv->protocol = SIM_PROTOCOL_TYPE_NONE;

  rule->_priv->sticky = FALSE;
  rule->_priv->sticky_different = SIM_RULE_VAR_NONE;
  rule->_priv->stickys = NULL;

  rule->_priv->plugin_sids_not = FALSE;
  rule->_priv->src_ias_not = FALSE;
  rule->_priv->dst_ias_not = FALSE;
  rule->_priv->src_ports_not = FALSE;
  rule->_priv->dst_ports_not = FALSE;
  rule->_priv->protocols_not = FALSE;

  rule->_priv->vars = NULL;
  rule->_priv->plugin_sids = NULL;
  rule->_priv->src_inets = NULL;
  rule->_priv->dst_inets = NULL;
  rule->_priv->src_ports = NULL;
  rule->_priv->dst_ports = NULL;
  rule->_priv->protocols = NULL;
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
  SimRule *rule;

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
  g_return_val_if_fail (rule, 0);
  g_return_val_if_fail (SIM_IS_RULE (rule), 0);

  return rule->_priv->level;
}

/*
 *
 *
 *
 *
 */
void
sim_rule_set_level (SimRule   *rule,
		    gint       level)
{
  g_return_if_fail (rule);
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
sim_rule_get_protocol (SimRule   *rule)
{
  g_return_val_if_fail (rule, SIM_PROTOCOL_TYPE_NONE);
  g_return_val_if_fail (SIM_IS_RULE (rule), SIM_PROTOCOL_TYPE_NONE);

  return rule->_priv->protocol;
}

/*
 *
 *
 *
 *
 */
void
sim_rule_set_protocol (SimRule   *rule,
		       gint       protocol)
{
  g_return_if_fail (rule);
  g_return_if_fail (SIM_IS_RULE (rule));

  rule->_priv->protocol = protocol;
}


/*
 *
 *
 *
 *
 */
gboolean
sim_rule_get_not (SimRule   *rule)
{
  g_return_val_if_fail (rule, FALSE);
  g_return_val_if_fail (SIM_IS_RULE (rule), FALSE);

  return rule->_priv->not;
}

/*
 *
 *
 *
 *
 */
void
sim_rule_set_not (SimRule   *rule,
		  gboolean   not)
{
  g_return_if_fail (rule);
  g_return_if_fail (SIM_IS_RULE (rule));

  rule->_priv->not = not;
}

/*
 *
 *
 *
 *
 */
gboolean
sim_rule_get_sticky (SimRule   *rule)
{
  g_return_val_if_fail (rule, FALSE);
  g_return_val_if_fail (SIM_IS_RULE (rule), FALSE);

  return rule->_priv->sticky;
}

/*
 *
 *
 *
 *
 */
void
sim_rule_set_sticky (SimRule   *rule,
		  gboolean   sticky)
{
  g_return_if_fail (rule);
  g_return_if_fail (SIM_IS_RULE (rule));

  rule->_priv->sticky = sticky;
}

/*
 *
 *
 *
 *
 */
SimRuleVarType
sim_rule_get_sticky_different (SimRule   *rule)
{
  g_return_val_if_fail (rule, FALSE);
  g_return_val_if_fail (SIM_IS_RULE (rule), FALSE);

  return rule->_priv->sticky_different;
}

/*
 *
 *
 *
 *
 */
void
sim_rule_set_sticky_different (SimRule         *rule,
			     SimRuleVarType   sticky_different)
{
  g_return_if_fail (rule);
  g_return_if_fail (SIM_IS_RULE (rule));

  rule->_priv->sticky_different = sticky_different;
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
  g_return_val_if_fail (rule, NULL);
  g_return_val_if_fail (SIM_IS_RULE (rule), NULL);

  return rule->_priv->name;
}

/*
 *
 *
 *
 *
 */
void
sim_rule_set_name (SimRule   *rule,
		   const gchar *name)
{
  g_return_if_fail (rule);
  g_return_if_fail (SIM_IS_RULE (rule));
  g_return_if_fail (name);

  if (rule->_priv->name)
    g_free (rule->_priv->name);

  rule->_priv->name = g_strdup (name);
}

/*
 *
 *
 *
 *
 */
gint
sim_rule_get_priority (SimRule   *rule)
{
  g_return_val_if_fail (rule, 0);
  g_return_val_if_fail (SIM_IS_RULE (rule), 0);

  if (rule->_priv->priority < 0)
    return 0;
  if (rule->_priv->priority > 5)
    return 5;

  return rule->_priv->priority;
}

/*
 *
 *
 *
 *
 */
void
sim_rule_set_priority (SimRule   *rule,
		       gint       priority)
{
  g_return_if_fail (rule);
  g_return_if_fail (SIM_IS_RULE (rule));

  if (priority < 0)
    rule->_priv->priority = 0;
  else if (priority > 5)
    rule->_priv->priority = 5;
  else 
    rule->_priv->priority = priority;
}

/*
 *
 *
 *
 *
 */
gint
sim_rule_get_reliability (SimRule   *rule)
{
  g_return_val_if_fail (rule, 0);
  g_return_val_if_fail (SIM_IS_RULE (rule), 0);

  if (rule->_priv->reliability < 0)
    return 0;
  if (rule->_priv->reliability > 10)
    return 10;

  return rule->_priv->reliability;
}

/*
 *
 *
 *
 *
 */
void
sim_rule_set_reliability (SimRule   *rule,
			  gint       reliability)
{
  g_return_if_fail (rule);
  g_return_if_fail (SIM_IS_RULE (rule));

  if (reliability < 0)
    rule->_priv->reliability = 0;
  else if (reliability > 10)
    rule->_priv->reliability = 10;
  else 
    rule->_priv->reliability = reliability;
}

/*
 *
 *
 *
 *
 */
gboolean
sim_rule_get_rel_abs (SimRule   *rule)
{
  g_return_val_if_fail (rule, FALSE);
  g_return_val_if_fail (SIM_IS_RULE (rule), FALSE);
  
  return rule->_priv->rel_abs;
}

/*
 *
 *
 *
 *
 */
void
sim_rule_set_rel_abs (SimRule   *rule,
		      gboolean   rel_abs)
{
  g_return_if_fail (rule);
  g_return_if_fail (SIM_IS_RULE (rule));

  rule->_priv->rel_abs = rel_abs;
}

/*
 *
 *
 *
 *
 */
SimConditionType
sim_rule_get_condition (SimRule   *rule)
{
  g_return_val_if_fail (rule, SIM_CONDITION_TYPE_NONE);
  g_return_val_if_fail (SIM_IS_RULE (rule), SIM_CONDITION_TYPE_NONE);

  return rule->_priv->condition;
}

/*
 *
 *
 *
 *
 */
void
sim_rule_set_condition (SimRule           *rule,
			SimConditionType   condition)
{
  g_return_if_fail (rule);
  g_return_if_fail (SIM_IS_RULE (rule));

  rule->_priv->condition = condition;
}

/*
 *
 *
 *
 *
 */
gchar*
sim_rule_get_value (SimRule   *rule)
{
  g_return_val_if_fail (rule, NULL);
  g_return_val_if_fail (SIM_IS_RULE (rule), NULL);

  return rule->_priv->value;
}

/*
 *
 *
 *
 *
 */
void
sim_rule_set_value (SimRule      *rule,
		    const gchar  *value)
{
  g_return_if_fail (rule);
  g_return_if_fail (SIM_IS_RULE (rule));
  g_return_if_fail (value);

  if (rule->_priv->value)
    g_free (rule->_priv->value);

  rule->_priv->value = g_strdup (value);
}

/*
 *
 *
 *
 *
 */
gint
sim_rule_get_interval (SimRule   *rule)
{
  g_return_val_if_fail (rule, 0);
  g_return_val_if_fail (SIM_IS_RULE (rule), 0);

  return rule->_priv->interval;
}

/*
 *
 *
 *
 *
 */
void
sim_rule_set_interval (SimRule   *rule,
		       gint       interval)
{
  g_return_if_fail (rule);
  g_return_if_fail (SIM_IS_RULE (rule));
  g_return_if_fail (interval >= 0);

  rule->_priv->interval = interval;
}

/*
 *
 *
 *
 *
 */
gboolean
sim_rule_get_absolute (SimRule   *rule)
{
  g_return_val_if_fail (rule, 0);
  g_return_val_if_fail (SIM_IS_RULE (rule), 0);

  return rule->_priv->absolute;
}

/*
 *
 *
 *
 *
 */
void
sim_rule_set_absolute (SimRule   *rule,
		       gboolean   absolute)
{
  g_return_if_fail (rule != NULL);
  g_return_if_fail (SIM_IS_RULE (rule));

  rule->_priv->absolute = absolute;
}

/*
 *
 *
 *
 *
 */
GTime
sim_rule_get_time_out (SimRule   *rule)
{
  g_return_val_if_fail (rule, 0);
  g_return_val_if_fail (SIM_IS_RULE (rule), 0);

  return rule->_priv->time_out;
}

/*
 *
 *
 *
 *
 */
void
sim_rule_set_time_out (SimRule   *rule,
		       GTime      time_out)
{
  g_return_if_fail (rule);
  g_return_if_fail (SIM_IS_RULE (rule));
  g_return_if_fail (time_out >= 0);

  rule->_priv->time_out = time_out;
}

/*
 *
 *
 *
 *
 */
GTime
sim_rule_get_time_last (SimRule   *rule)
{
  g_return_val_if_fail (rule, 0);
  g_return_val_if_fail (SIM_IS_RULE (rule), 0);

  return rule->_priv->time_last;
}

/*
 *
 *
 *
 *
 */
void
sim_rule_set_time_last (SimRule   *rule,
		       GTime      time_last)
{
  g_return_if_fail (rule);
  g_return_if_fail (SIM_IS_RULE (rule));
  g_return_if_fail (time_last >= 0);

  rule->_priv->time_last = time_last;
}

/*
 *
 *
 *
 *
 */
gint
sim_rule_get_occurrence (SimRule   *rule)
{
  g_return_val_if_fail (rule, 0);
  g_return_val_if_fail (SIM_IS_RULE (rule), 0);

  return rule->_priv->occurrence;
}

/*
 *
 *
 *
 *
 */
void
sim_rule_set_occurrence (SimRule   *rule,
			 gint       occurrence)
{
  g_return_if_fail (rule);
  g_return_if_fail (SIM_IS_RULE (rule));
  g_return_if_fail (occurrence > 0);

  rule->_priv->occurrence = occurrence;
}

/*
 *
 *
 *
 *
 */
gint
sim_rule_get_count (SimRule   *rule)
{
  g_return_val_if_fail (rule, 0);
  g_return_val_if_fail (SIM_IS_RULE (rule), 0);

  return rule->_priv->count_occu;
}

/*
 *
 *
 *
 *
 */
void
sim_rule_set_count (SimRule   *rule,
		    gint       count_occu)
{
  g_return_if_fail (rule);
  g_return_if_fail (SIM_IS_RULE (rule));
  g_return_if_fail (count_occu > 0);

  rule->_priv->count_occu = count_occu;
}

/*
 *
 *
 *
 *
 */
gint
sim_rule_get_plugin_id (SimRule   *rule)
{
  g_return_val_if_fail (rule, 0);
  g_return_val_if_fail (SIM_IS_RULE (rule), 0);

  return rule->_priv->plugin_id;
}

/*
 *
 *
 *
 *
 */
void 
sim_rule_set_plugin_id (SimRule   *rule,
			gint       plugin_id)
{
  g_return_if_fail (rule);
  g_return_if_fail (SIM_IS_RULE (rule));
  g_return_if_fail (plugin_id > 0);

  rule->_priv->plugin_id = plugin_id;
}

/*
 *
 *
 *
 *
 */
gint
sim_rule_get_plugin_sid (SimRule   *rule)
{
  g_return_val_if_fail (rule, 0);
  g_return_val_if_fail (SIM_IS_RULE (rule), 0);

  return rule->_priv->plugin_sid;
}

/*
 *
 *
 *
 *
 */
void
sim_rule_set_plugin_sid (SimRule   *rule,
			 gint       plugin_sid)
{
  g_return_if_fail (rule);
  g_return_if_fail (SIM_IS_RULE (rule));
  g_return_if_fail (plugin_sid > 0);

  rule->_priv->plugin_sid = plugin_sid;
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
  g_return_val_if_fail (rule, NULL);
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
  g_return_if_fail (rule);
  g_return_if_fail (SIM_IS_RULE (rule));
  g_return_if_fail (src_ia);

  if (rule->_priv->src_ia)
    gnet_inetaddr_unref (rule->_priv->src_ia);

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
  g_return_val_if_fail (rule, NULL);
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
  g_return_if_fail (rule);
  g_return_if_fail (SIM_IS_RULE (rule));
  g_return_if_fail (dst_ia);

  if (rule->_priv->dst_ia)
    gnet_inetaddr_unref (rule->_priv->dst_ia);

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
  g_return_val_if_fail (rule, 0);
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
  g_return_if_fail (rule);
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
  g_return_val_if_fail (rule, 0);
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
  g_return_if_fail (rule);
  g_return_if_fail (SIM_IS_RULE (rule));
  g_return_if_fail (dst_port >= 0);

  rule->_priv->dst_port = dst_port;
}

/*
 *
 *
 *
 *
 */
void
sim_rule_append_plugin_sid (SimRule   *rule,
			    gint       plugin_sid)
{
  g_return_if_fail (rule);
  g_return_if_fail (SIM_IS_RULE (rule));
  g_return_if_fail (plugin_sid >= 0);

  rule->_priv->plugin_sids = g_list_append (rule->_priv->plugin_sids, GINT_TO_POINTER (plugin_sid));
}

/*
 *
 *
 *
 *
 */
void
sim_rule_remove_plugin_sid (SimRule   *rule,
			    gint       plugin_sid)
{
  g_return_if_fail (rule);
  g_return_if_fail (SIM_IS_RULE (rule));
  g_return_if_fail (plugin_sid >= 0);

  rule->_priv->plugin_sids = g_list_remove (rule->_priv->plugin_sids, GINT_TO_POINTER (plugin_sid));
}

/*
 *
 *
 *
 *
 */
GList*
sim_rule_get_plugin_sids (SimRule   *rule)
{
  g_return_val_if_fail (rule != NULL, NULL);
  g_return_val_if_fail (SIM_IS_RULE (rule), NULL);

  return rule->_priv->plugin_sids;
}

/*
 *
 *
 *
 *
 */
void
sim_rule_append_src_inet (SimRule    *rule,
			  SimInet    *inet)
{
  g_return_if_fail (rule);
  g_return_if_fail (SIM_IS_RULE (rule));
  g_return_if_fail (inet);
  g_return_if_fail (SIM_IS_INET (inet));

  rule->_priv->src_inets = g_list_append (rule->_priv->src_inets, inet);
}

/*
 *
 *
 *
 *
 */
void
sim_rule_remove_src_inet (SimRule    *rule,
			  SimInet    *inet)
{
  g_return_if_fail (rule);
  g_return_if_fail (SIM_IS_RULE (rule));
  g_return_if_fail (inet);
  g_return_if_fail (SIM_IS_INET (inet));

  rule->_priv->src_inets = g_list_remove (rule->_priv->src_inets, inet);
}

/*
 *
 *
 *
 *
 */
GList*
sim_rule_get_src_inets (SimRule   *rule)
{
  g_return_val_if_fail (rule, NULL);
  g_return_val_if_fail (SIM_IS_RULE (rule), NULL);

  return rule->_priv->src_inets;
}

/*
 *
 *
 *
 *
 */
void
sim_rule_append_dst_inet (SimRule    *rule,
			  SimInet    *inet)
{
  g_return_if_fail (rule);
  g_return_if_fail (SIM_IS_RULE (rule));
  g_return_if_fail (inet);
  g_return_if_fail (SIM_IS_INET (inet));

  rule->_priv->dst_inets = g_list_append (rule->_priv->dst_inets, inet);
}

/*
 *
 *
 *
 *
 */
void
sim_rule_remove_dst_inet (SimRule    *rule,
			  SimInet    *inet)
{
  g_return_if_fail (rule);
  g_return_if_fail (SIM_IS_RULE (rule));
  g_return_if_fail (inet);
  g_return_if_fail (SIM_IS_INET (inet));

  rule->_priv->dst_inets = g_list_remove (rule->_priv->dst_inets, inet);
}

/*
 *
 *
 *
 *
 */
GList*
sim_rule_get_dst_inets (SimRule   *rule)
{
  g_return_val_if_fail (rule, NULL);
  g_return_val_if_fail (SIM_IS_RULE (rule), NULL);

  return rule->_priv->dst_inets;
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
  g_return_if_fail (src_port >= 0);

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
  g_return_if_fail (src_port >= 0);

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
  g_return_val_if_fail (rule, NULL);
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
  g_return_if_fail (rule);
  g_return_if_fail (SIM_IS_RULE (rule));
  g_return_if_fail (dst_port >= 0);

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
  g_return_if_fail (rule);
  g_return_if_fail (SIM_IS_RULE (rule));
  g_return_if_fail (dst_port >= 0);

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
  g_return_val_if_fail (rule, NULL);
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
sim_rule_append_protocol (SimRule   *rule,
			  SimProtocolType  protocol)
{
  g_return_if_fail (rule);
  g_return_if_fail (SIM_IS_RULE (rule));

  rule->_priv->protocols = g_list_append (rule->_priv->protocols, GINT_TO_POINTER (protocol));
}

/*
 *
 *
 *
 *
 */
void
sim_rule_remove_protocol (SimRule   *rule,
			  SimProtocolType  protocol)
{
  g_return_if_fail (rule);
  g_return_if_fail (SIM_IS_RULE (rule));

  rule->_priv->protocols = g_list_remove (rule->_priv->protocols, GINT_TO_POINTER (protocol));
}

/*
 *
 *
 *
 *
 */
GList*
sim_rule_get_protocols (SimRule   *rule)
{
  g_return_val_if_fail (rule, NULL);
  g_return_val_if_fail (SIM_IS_RULE (rule), NULL);

  return rule->_priv->protocols;
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
  g_return_if_fail (rule);
  g_return_if_fail (SIM_IS_RULE (rule));
  g_return_if_fail (var);

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
  g_return_val_if_fail (rule, NULL);
  g_return_val_if_fail (SIM_IS_RULE (rule), NULL);

  return rule->_priv->vars;
}

/*
 *
 *
 *
 *
 */
gboolean
sim_rule_get_plugin_sids_not (SimRule   *rule)
{
  g_return_val_if_fail (rule, FALSE);
  g_return_val_if_fail (SIM_IS_RULE (rule), FALSE);

  return rule->_priv->plugin_sids_not;
}

/*
 *
 *
 *
 *
 */
void
sim_rule_set_plugin_sids_not (SimRule   *rule,
			      gboolean   plugin_sids_not)
{
  g_return_if_fail (rule);
  g_return_if_fail (SIM_IS_RULE (rule));
  
  rule->_priv->plugin_sids_not = plugin_sids_not;
}

/*
 *
 *
 *
 *
 */
gboolean
sim_rule_get_src_ias_not (SimRule   *rule)
{
  g_return_val_if_fail (rule, FALSE);
  g_return_val_if_fail (SIM_IS_RULE (rule), FALSE);

  return rule->_priv->src_ias_not;
}

/*
 *
 *
 *
 *
 */
void
sim_rule_set_src_ias_not (SimRule   *rule,
			  gboolean   src_ias_not)
{
  g_return_if_fail (rule);
  g_return_if_fail (SIM_IS_RULE (rule));
  
  rule->_priv->src_ias_not = src_ias_not;
}

/*
 *
 *
 *
 *
 */
gboolean
sim_rule_get_dst_ias_not (SimRule   *rule)
{
  g_return_val_if_fail (rule, FALSE);
  g_return_val_if_fail (SIM_IS_RULE (rule), FALSE);

  return rule->_priv->dst_ias_not;
}

/*
 *
 *
 *
 *
 */
void
sim_rule_set_dst_ias_not (SimRule   *rule,
			  gboolean   dst_ias_not)
{
  g_return_if_fail (rule);
  g_return_if_fail (SIM_IS_RULE (rule));
  
  rule->_priv->dst_ias_not = dst_ias_not;
}

/*
 *
 *
 *
 *
 */
gboolean
sim_rule_get_src_ports_not (SimRule   *rule)
{
  g_return_val_if_fail (rule, FALSE);
  g_return_val_if_fail (SIM_IS_RULE (rule), FALSE);

  return rule->_priv->src_ports_not;
}

/*
 *
 *
 *
 *
 */
void
sim_rule_set_src_ports_not (SimRule   *rule,
			  gboolean   src_ports_not)
{
  g_return_if_fail (rule);
  g_return_if_fail (SIM_IS_RULE (rule));
  
  rule->_priv->src_ports_not = src_ports_not;
}

/*
 *
 *
 *
 *
 */
gboolean
sim_rule_get_dst_ports_not (SimRule   *rule)
{
  g_return_val_if_fail (rule, FALSE);
  g_return_val_if_fail (SIM_IS_RULE (rule), FALSE);

  return rule->_priv->dst_ports_not;
}

/*
 *
 *
 *
 *
 */
void
sim_rule_set_dst_ports_not (SimRule   *rule,
			  gboolean   dst_ports_not)
{
  g_return_if_fail (rule);
  g_return_if_fail (SIM_IS_RULE (rule));
  
  rule->_priv->dst_ports_not = dst_ports_not;
}

/*
 *
 *
 *
 *
 */
gboolean
sim_rule_get_protocols_not (SimRule   *rule)
{
  g_return_val_if_fail (rule, FALSE);
  g_return_val_if_fail (SIM_IS_RULE (rule), FALSE);

  return rule->_priv->protocols_not;
}

/*
 *
 *
 *
 *
 */
void
sim_rule_set_protocols_not (SimRule   *rule,
			    gboolean   protocols_not)
{
  g_return_if_fail (rule);
  g_return_if_fail (SIM_IS_RULE (rule));
  
  rule->_priv->protocols_not = protocols_not;
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
  new_rule->type = rule->type;
  new_rule->_priv->level = rule->_priv->level;
  new_rule->_priv->name = g_strdup (rule->_priv->name);
  new_rule->_priv->not = rule->_priv->not;

  new_rule->_priv->sticky = rule->_priv->sticky;
  new_rule->_priv->sticky_different = rule->_priv->sticky_different;

  new_rule->_priv->priority = rule->_priv->priority;
  new_rule->_priv->reliability = rule->_priv->reliability;
  new_rule->_priv->rel_abs = rule->_priv->rel_abs;

  new_rule->_priv->time_out = rule->_priv->time_out;
  new_rule->_priv->occurrence = rule->_priv->occurrence;

  new_rule->_priv->plugin_id = rule->_priv->plugin_id;
  new_rule->_priv->plugin_sid = rule->_priv->plugin_sid;

  new_rule->_priv->src_ia = (rule->_priv->src_ia) ? gnet_inetaddr_clone (rule->_priv->src_ia) : NULL;
  new_rule->_priv->dst_ia = (rule->_priv->dst_ia) ? gnet_inetaddr_clone (rule->_priv->dst_ia) : NULL;
  new_rule->_priv->src_port = rule->_priv->src_port;
  new_rule->_priv->dst_port = rule->_priv->dst_port;
  new_rule->_priv->protocol = rule->_priv->protocol;

  new_rule->_priv->condition = rule->_priv->condition;
  new_rule->_priv->value = (rule->_priv->value) ? g_strdup (rule->_priv->value) : NULL;
  new_rule->_priv->interval = rule->_priv->interval;

  new_rule->_priv->plugin_sids_not = rule->_priv->plugin_sids_not;
  new_rule->_priv->src_ias_not = rule->_priv->src_ias_not;
  new_rule->_priv->dst_ias_not = rule->_priv->dst_ias_not;
  new_rule->_priv->src_ports_not = rule->_priv->src_ports_not;
  new_rule->_priv->dst_ports_not = rule->_priv->dst_ports_not;
  new_rule->_priv->protocols_not = rule->_priv->protocols_not;

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

  /* Plugin Sids */
  list = rule->_priv->plugin_sids;
  while (list)
    {
      gint plugin_sid = GPOINTER_TO_INT (list->data);
      new_rule->_priv->plugin_sids = g_list_append (new_rule->_priv->plugin_sids, GINT_TO_POINTER (plugin_sid));
      list = list->next;
    }

  /* src ips */
  list = rule->_priv->src_inets;
  while (list)
    {
      SimInet *inet = (SimInet *) list->data;
      new_rule->_priv->src_inets = g_list_append (new_rule->_priv->src_inets, sim_inet_clone (inet));
      list = list->next;
    }

  /* dst ips */
  list = rule->_priv->dst_inets;
  while (list)
    {
      SimInet *inet = (SimInet *) list->data;
      new_rule->_priv->dst_inets = g_list_append (new_rule->_priv->dst_inets, sim_inet_clone (inet));
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

  /* Protocols */
  list = rule->_priv->protocols;
  while (list)
    {
      SimProtocolType protocol = GPOINTER_TO_INT (list->data);
      new_rule->_priv->protocols = g_list_append (new_rule->_priv->protocols, GINT_TO_POINTER (protocol)); 
      list = list->next;
    }

  return new_rule;
}

/*
 *
 *
 */
gint
sim_rule_get_reliability_relative (GNode   *rule_node)
{
  GNode   *node;
  SimRule *rule;
  gint     rel = 0;

  g_return_val_if_fail (rule_node, 0);

  node = rule_node;
  while (node)
    {
      SimRule *rule = (SimRule *) node->data;

      rel += rule->_priv->reliability;
      node = node->parent;
    }

  return rel;
}

/*
 *
 *
 *
 */
gboolean
sim_rule_is_not_invalid (SimRule      *rule)
{
  g_return_val_if_fail (rule, FALSE);
  g_return_val_if_fail (SIM_IS_RULE (rule), FALSE);

  return rule->_priv->not_invalid;
}

/**
 * sim_rule_is_time_out:
 * @rule: a #SimRule.
 *
 * Look if a #SimRule is time out.
 *
 * Return: TRUE if is time out, FALSE otherwise.
 */
gboolean 
sim_rule_is_time_out (SimRule      *rule)
{
  g_return_val_if_fail (rule, FALSE);
  g_return_val_if_fail (SIM_IS_RULE (rule), FALSE);

  if ((!rule->_priv->time_out) || (!rule->_priv->time_last))
    return FALSE;

  if (rule->_priv->level == 1)
    {
      if ((rule->_priv->occurrence > 1) && 
	  (time (NULL) > (rule->_priv->time_last + rule->_priv->time_out)))
	{
	  rule->_priv->time_last = 0;
	  rule->_priv->count_occu = 1;
	  return TRUE;
	}
    }
  else
    {
      if (time (NULL) > (rule->_priv->time_last + rule->_priv->time_out))
	return TRUE;
    }

  return FALSE;
}

gboolean
find_guint32_value (GList      *values,
		    guint32     val)
{
  GList *list;

  if (!values)
    return FALSE;

  list = values;
  while (list)
    {
      guint32 cmp = GPOINTER_TO_INT (list->data);

      if (cmp == val)
	return TRUE;

      list = list->next;
    }

  return FALSE;
}

/*
 *
 *
 *
 *
 */
gboolean
sim_rule_match_by_alert (SimRule      *rule,
			 SimAlert     *alert)
{ 
  GList      *list = NULL;
  gboolean    match;

  g_return_val_if_fail (rule, FALSE);
  g_return_val_if_fail (SIM_IS_RULE (rule), FALSE);
  g_return_val_if_fail (rule->type != SIM_RULE_TYPE_NONE, FALSE);
  g_return_val_if_fail (rule->_priv->plugin_id > 0, FALSE);
  g_return_val_if_fail (alert, FALSE);
  g_return_val_if_fail (SIM_IS_ALERT (alert), FALSE);
  g_return_val_if_fail (alert->type != SIM_ALERT_TYPE_NONE, FALSE);
  g_return_val_if_fail (alert->plugin_id > 0, FALSE);
  g_return_val_if_fail (alert->plugin_sid > 0, FALSE);
  g_return_val_if_fail (alert->src_ia, FALSE);

  /* Time Out */
  if ((sim_rule_is_time_out (rule)) && (rule->_priv->level > 1))
    return FALSE;

  /* Match Type */
  if (rule->type != alert->type)
    return FALSE;

  /* Match Plugin ID */
  if (rule->_priv->plugin_id != alert->plugin_id)
    return FALSE;

  /* Match Plugin SIDs */
  if (rule->_priv->plugin_sids)
    {
      match = FALSE;
      list = rule->_priv->plugin_sids;
      while (list)
	{
	  gint plugin_sid = GPOINTER_TO_INT (list->data);
	  
	  if ((!plugin_sid) || (plugin_sid == alert->plugin_sid))
	    {
	      match = TRUE;
	      break;
	    }
	  
	  list = list->next;
	}
      if (match)
	{
	  if (rule->_priv->plugin_sids_not)
	    return FALSE;
	}
      else
	{
	  if (!rule->_priv->plugin_sids_not)
	    return FALSE;
	}
    }

  /* Match src_ia */
  if (rule->_priv->src_inets)
    {
      SimInet *inet = sim_inet_new_from_ginetaddr (alert->src_ia);
      match = FALSE;
      list = rule->_priv->src_inets;
      while (list)
	{
	  SimInet *cmp_inet = (SimInet *) list->data;
	  
	  if ((sim_inet_is_reserved (cmp_inet)) || 
	      (sim_inet_has_inet (inet, cmp_inet)))
	    {
	      match = TRUE;
	      break;
	    }
	  
	  list = list->next;
	}
      g_object_unref (inet);
      if (match)
	{
	  if (rule->_priv->src_ias_not)
	    return FALSE;
	}
      else
	{
	  if (!rule->_priv->src_ias_not)
	    return FALSE;
	}
    }

  /* Find dst_ia */
  if ((rule->_priv->dst_inets) && (alert->dst_ia))
    {
      SimInet *inet = sim_inet_new_from_ginetaddr (alert->dst_ia);
      match = FALSE;
      list = rule->_priv->dst_inets;
      while (list)
	{
	  SimInet *cmp_inet = (SimInet *) list->data;
	  
	  if ((sim_inet_is_reserved (cmp_inet)) || 
	      (sim_inet_has_inet (inet, cmp_inet)))
	    {
	      match = TRUE;
	      break;
	    }
	  
	  list = list->next;
	}
      g_object_unref (inet);
      if (match)
	{
	  if (rule->_priv->dst_ias_not)
	    return FALSE;
	}
      else
	{
	  if (!rule->_priv->dst_ias_not)
	    return FALSE;
	}
    }

  /* Find src_port */
  if (rule->_priv->src_ports)
    {
      match = FALSE;
      list = rule->_priv->src_ports;
      while (list)
	{
	  gint cmp_port = GPOINTER_TO_INT (list->data);
	  
	  if ((!cmp_port) || (cmp_port == alert->src_port))
	    {
	      match = TRUE;
	      break;
	    }
	  
	  list = list->next;
	}
      if (match)
	{
	  if (rule->_priv->src_ports_not)
	    return FALSE;
	}
      else
	{
	  if (!rule->_priv->src_ports_not)
	    return FALSE;
	}
    }

  /* Find dst_port */
  if (rule->_priv->dst_ports)
    {
      match = FALSE;
      list = rule->_priv->dst_ports;
      while (list)
	{
	  gint cmp_port = GPOINTER_TO_INT (list->data);
	  
	  if ((!cmp_port) || (cmp_port == alert->dst_port))
	    {
	      match = TRUE;
	      break;
	    }
	  
	  list = list->next;
	}
      if (match)
	{
	  if (rule->_priv->dst_ports_not)
	    return FALSE;
	}
      else
	{
	  if (!rule->_priv->dst_ports_not)
	    return FALSE;
	}
    }

  /* Protocols */
  if (rule->_priv->protocols)
    {
      match = FALSE;
      list = rule->_priv->protocols;
      while (list)
	{
	  SimProtocolType cmp_prot = GPOINTER_TO_INT (list->data);
	  
	  if ((!cmp_prot) || (cmp_prot == alert->protocol))
	    {
	      match = TRUE;
	      break;
	    }
	  
	  list = list->next;
	}
      if (match)
	{
	  if (rule->_priv->protocols_not)
	    return FALSE;
	}
      else
	{
	  if (!rule->_priv->protocols_not)
	    return FALSE;
	}
    }

  /* Match Condition */
  if ((rule->_priv->condition != SIM_CONDITION_TYPE_NONE) &&
      (alert->condition != SIM_CONDITION_TYPE_NONE))
    {
      if (rule->_priv->condition != alert->condition)
	return FALSE;

      /* Match Value */
      if ((rule->_priv->value) && (alert->value))
	{
	  if (g_ascii_strcasecmp (rule->_priv->value, alert->value))
	    return FALSE;
	}
    }

  /* If rule is sticky */
  if (rule->_priv->sticky)
    alert->sticky = TRUE;

  if ((rule->_priv->occurrence > 1) && (rule->_priv->sticky_different))
    {
      guint32 val;
      switch (rule->_priv->sticky_different)
	{
	case SIM_RULE_VAR_PLUGIN_SID:
	  val = (guint32) alert->plugin_sid;
	  if (find_guint32_value (rule->_priv->stickys, val))
	    return FALSE;
	  rule->_priv->stickys = g_list_append (rule->_priv->stickys, GINT_TO_POINTER (val));
	  break;
	case SIM_RULE_VAR_SRC_IA:
	  val = (guint32) sim_inetaddr_ntohl (alert->src_ia);
	  if (find_guint32_value (rule->_priv->stickys, val))
	    return FALSE;
	  rule->_priv->stickys = g_list_append (rule->_priv->stickys, GINT_TO_POINTER (val));
	  break;
	case SIM_RULE_VAR_DST_IA:
	  val = (guint32) sim_inetaddr_ntohl (alert->dst_ia);
	  if (find_guint32_value (rule->_priv->stickys, val))
	    return FALSE;
	  rule->_priv->stickys = g_list_append (rule->_priv->stickys, GINT_TO_POINTER (val));
	  break;
	case SIM_RULE_VAR_SRC_PORT:
	  val = (guint32) alert->src_port;
	  if (find_guint32_value (rule->_priv->stickys, val))
	    return FALSE;
	  rule->_priv->stickys = g_list_append (rule->_priv->stickys, GINT_TO_POINTER (val));
	  break;
	case SIM_RULE_VAR_DST_PORT:
	  val = (guint32) alert->dst_port;
	  if (find_guint32_value (rule->_priv->stickys, val))
	    return FALSE;
	  rule->_priv->stickys = g_list_append (rule->_priv->stickys, GINT_TO_POINTER (val));
	  break;
	case SIM_RULE_VAR_PROTOCOL:
	  val = (guint32) alert->protocol;
	  if (find_guint32_value (rule->_priv->stickys, val))
	    return FALSE;
	  rule->_priv->stickys = g_list_append (rule->_priv->stickys, GINT_TO_POINTER (val));
	  break;
	default:
	  break;
	}
    }

  /* Match Occurrence */
  if (rule->_priv->occurrence > 1)
    {
      if ((rule->_priv->time_out) && (!rule->_priv->time_last))
	rule->_priv->time_last = time (NULL);

      alert->level = rule->_priv->level;
      alert->match = TRUE;
      if (rule->_priv->occurrence != rule->_priv->count_occu)
	{
	  rule->_priv->count_occu++;
	  alert->count = rule->_priv->count_occu - 1;
	  return FALSE;
	}
      else
	{
	  alert->count = rule->_priv->occurrence;
	  rule->_priv->count_occu = 1;
	}
    }
  else
    alert->count = 1;

  /* Not */
  if (rule->_priv->not)
    {
      rule->_priv->not_invalid = TRUE;
      return FALSE;
    }

  alert->level = rule->_priv->level;
  alert->match = TRUE;
  return TRUE;
}

/*
 *
 *
 *
 *
 */
void
sim_rule_set_alert_data (SimRule      *rule,
			 SimAlert     *alert)
{
  g_return_if_fail (rule);
  g_return_if_fail (SIM_IS_RULE (rule));
  g_return_if_fail (alert);
  g_return_if_fail (SIM_IS_ALERT (alert));

  rule->_priv->plugin_sid = alert->plugin_sid;
  rule->_priv->src_ia = (alert->src_ia) ? gnet_inetaddr_clone (alert->src_ia) : NULL;
  rule->_priv->dst_ia = (alert->dst_ia) ? gnet_inetaddr_clone (alert->dst_ia) : NULL;
  rule->_priv->src_port = alert->src_port;
  rule->_priv->dst_port = alert->dst_port;
  rule->_priv->protocol = alert->protocol;
}

/*
 *
 *
 *
 *
 */
void
sim_rule_set_not_data (SimRule      *rule)
{
  g_return_if_fail (rule);
  g_return_if_fail (SIM_IS_RULE (rule));

  if ((rule->_priv->plugin_sids) && (rule->_priv->plugin_sids->data))
    rule->_priv->plugin_sid = GPOINTER_TO_INT (rule->_priv->plugin_sids->data);
  if ((rule->_priv->src_inets) && (rule->_priv->src_inets->data))
    rule->_priv->src_ia = gnet_inetaddr_clone (rule->_priv->src_inets->data);
  if ((rule->_priv->dst_inets) && (rule->_priv->dst_inets->data))
    rule->_priv->dst_ia = gnet_inetaddr_clone (rule->_priv->dst_inets->data);
  if ((rule->_priv->src_ports) && (rule->_priv->src_ports->data))
    rule->_priv->src_port = GPOINTER_TO_INT (rule->_priv->src_ports->data);
  if ((rule->_priv->dst_ports) && (rule->_priv->dst_ports->data))
    rule->_priv->dst_port = GPOINTER_TO_INT (rule->_priv->dst_ports->data);
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
  GList *list;
  gchar  *ip;

  g_return_if_fail (rule);
  g_return_if_fail (SIM_IS_RULE (rule));

  g_print ("Rule: ");
  g_print ("sticky=%d ", rule->_priv->sticky);
  g_print ("not=%d ", rule->_priv->not);
  g_print ("name=%s ", rule->_priv->name);
  g_print ("level=%d ", rule->_priv->level);
  g_print ("priority=%d ", rule->_priv->priority);
  g_print ("reliability=%d ", rule->_priv->reliability);
  g_print ("time_out=%d ", rule->_priv->time_out);
  g_print ("occurrence=%d ", rule->_priv->occurrence);
  g_print ("plugin_id=%d ", rule->_priv->plugin_id);
  g_print ("plugin_sid=%d ", g_list_length (rule->_priv->plugin_sids));
  g_print ("src_inets=%d ", g_list_length (rule->_priv->src_inets));
  list = rule->_priv->src_inets;
  while (list)
    {
      SimInet *ia = (SimInet *) list->data;
      ip = sim_inet_ntop (ia);
      g_print (" %s ", ip);
      g_free (ip);
      list = list->next;
    }
  g_print ("dst_inets=%d ", g_list_length (rule->_priv->dst_inets));
  list = rule->_priv->dst_inets;
  while (list)
    {
      SimInet *ia = (SimInet *) list->data;
      ip = sim_inet_ntop (ia);
      g_print (" %s ", ip);
      g_free (ip);
      list = list->next;
    }
  g_print ("src_ports=%d ", g_list_length (rule->_priv->src_ports));
  list = rule->_priv->src_ports;
  while (list)
    {
      gint port = GPOINTER_TO_INT (list->data);
      g_print (" %d ", port);
      list = list->next;
    }
  g_print ("dst_ports=%d ", g_list_length (rule->_priv->dst_ports));
  list = rule->_priv->dst_ports;
  while (list)
    {
      gint port = GPOINTER_TO_INT (list->data);
      g_print (" %d ", port);
      list = list->next;
    }
  g_print ("vars=%d ", g_list_length (rule->_priv->vars));
  if (rule->_priv->src_ia)
    {
      ip = gnet_inetaddr_get_canonical_name (rule->_priv->src_ia);
      g_print ("src_ia=%s ", ip);
      g_free (ip);
    }
  if (rule->_priv->dst_ia)
    {
      ip = gnet_inetaddr_get_canonical_name (rule->_priv->dst_ia);
      g_print ("dst_ia=%s ", ip);
      g_free (ip);
    }
  g_print ("src_port=%d ", rule->_priv->src_port);
  g_print ("dst_port=%d ", rule->_priv->dst_port);
  g_print ("\n");
}

/*
 *
 *
 *
 */
gchar*
sim_rule_to_string (SimRule      *rule)
{
  GString  *str;
  gchar    *src_name;
  gchar    *dst_name;
  gchar     timestamp[TIMEBUF_SIZE];

  g_return_val_if_fail (rule, NULL);
  g_return_val_if_fail (SIM_IS_RULE (rule), NULL);

  strftime (timestamp, TIMEBUF_SIZE, "%Y-%m-%d %H:%M:%S", localtime ((time_t *) &rule->_priv->time_last));

  src_name = (rule->_priv->src_ia) ? gnet_inetaddr_get_canonical_name (rule->_priv->src_ia) : NULL;
  dst_name = (rule->_priv->dst_ia) ? gnet_inetaddr_get_canonical_name (rule->_priv->dst_ia) : NULL;

  str = g_string_new ("Rule");
  g_string_append_printf (str, " %d [%s]", rule->_priv->level, timestamp);
  g_string_append_printf (str, " [%d:%d]", rule->_priv->plugin_id, rule->_priv->plugin_sid);
  g_string_append_printf (str, " [Rel:%s%d]", (rule->_priv->rel_abs) ? " " : " +", rule->_priv->reliability);
  g_string_append_printf (str, " %s:%d", src_name, rule->_priv->src_port);

  if (rule->_priv->dst_ia)
    g_string_append_printf (str, " -> %s:%d\n", dst_name, rule->_priv->dst_port);

  if (src_name) g_free (src_name);
  if (dst_name) g_free (dst_name);

  return g_string_free (str, FALSE);
}
