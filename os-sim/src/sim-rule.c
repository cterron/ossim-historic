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


#include "sim-rule.h"
#include "sim-util.h"

#include <time.h>
#include <config.h>

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
  GInetAddr  *sensor;

  gboolean         sticky;
  SimRuleVarType   sticky_different;
  GList           *stickys;

  //This variables are used to store the data from directives. i.e., the src-inets will store 
	//all the inets wich appears in the directives file. But, for example the variable above 
	//"GInetAddr *src_ia" will store the data from event that matches
	GList				*plugin_sids_not;	//gint
	GList				*src_inets_not;	//SimInet object
	GList				*dst_inets_not;
	GList				*src_ports_not;	//gint
	GList				*dst_ports_not;
	GList				*protocols_not;	//gint
	GList				*sensors_not;		//SimSensor

	GList      *vars;										
  GList      *plugin_sids;
  GList      *src_inets; //SimInet 
  GList      *dst_inets;
  GList      *src_ports;
  GList      *dst_ports;
  GList      *protocols;
  GList      *sensors;	//SimSensor
};

static gpointer parent_class = NULL;
static gint sim_rule_signals[LAST_SIGNAL] = { 0 };		//FIXME: There are some classes in OSSIM wich define this, but 
																											//don't use it. It's needed to attach this to sim_rule_impl_finalize
																											//and the other classes.

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
//    sim_rule_print(rule);

  if (rule->_priv->name)
    g_free (rule->_priv->name);

  if (rule->_priv->value)
    g_free (rule->_priv->value);

  if (rule->_priv->src_ia)
    gnet_inetaddr_unref (rule->_priv->src_ia);
  if (rule->_priv->dst_ia)
    gnet_inetaddr_unref (rule->_priv->dst_ia);

  if (rule->_priv->sensor)
    gnet_inetaddr_unref (rule->_priv->sensor);

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

	/* sensors */
  list = rule->_priv->sensors;
  while (list)
    {
      SimSensor *sensor = (SimSensor *) list->data;
      g_object_unref (sensor);
      list = list->next;
    }
  g_list_free (rule->_priv->sensors);

  /* protocols */
  g_list_free (rule->_priv->protocols);

  /* stickys */
  g_list_free (rule->_priv->stickys);

	//not's:
	// !src ips 
	list = rule->_priv->src_inets_not; 
	while (list) 
	{ 
		SimInet *inet = (SimInet *) list->data; 
		g_object_unref (inet); 
		list = list->next; 
	} 
	g_list_free (rule->_priv->src_inets_not); 

	// !dst ips 
	list = rule->_priv->dst_inets_not; 
	while (list) 
	{ 
		SimInet *inet = (SimInet *) list->data; 
		g_object_unref (inet); 
		list = list->next; 
	} 
	g_list_free (rule->_priv->dst_inets_not); 

	// !plugin_sids 
	list = rule->_priv->plugin_sids_not; 
	while (list) 
	{ 
		gint *plugin_sid = (int *) list->data; 
		g_free (plugin_sid); 
		list = list->next; 
	} 
	g_list_free (rule->_priv->plugin_sids_not); 
 
	// !src ports
	list = rule->_priv->src_ports_not; 
	while (list) 
	{ 
		gint *port = (int *) list->data; 
		g_free (port); 
		list = list->next; 
	} 
	g_list_free (rule->_priv->src_ports_not); 
 
 	// !dst ports
	list = rule->_priv->dst_ports_not; 
	while (list) 
	{ 
		gint *port = (int *) list->data; 
		g_free (port); 
		list = list->next; 
	} 
	g_list_free (rule->_priv->dst_ports_not); 

 	// !protocols
	list = rule->_priv->protocols_not; 
	while (list) 
	{ 
		gint *protocol = (int *) list->data; 
		g_free (protocol); 
		list = list->next; 
	} 
	g_list_free (rule->_priv->sensors_not); 
 
	// !sensors
	list = rule->_priv->sensors_not; 
	while (list) 
	{ 
		SimInet *inet = (SimInet *) list->data; 
		g_object_unref (inet); 
		list = list->next; 
	} 
	g_list_free (rule->_priv->sensors_not); 
 

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

//  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_rule_instance_init");

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
  rule->_priv->sensor = NULL;

  rule->_priv->sticky = FALSE;
  rule->_priv->sticky_different = SIM_RULE_VAR_NONE;
  rule->_priv->stickys = NULL;

  rule->_priv->plugin_sids_not = NULL;
  rule->_priv->src_inets_not = NULL;
  rule->_priv->dst_inets_not = NULL;
  rule->_priv->src_ports_not = NULL;
  rule->_priv->dst_ports_not = NULL;
  rule->_priv->protocols_not = NULL;

  rule->_priv->vars = NULL;
  rule->_priv->plugin_sids = NULL;
  rule->_priv->src_inets = NULL;
  rule->_priv->dst_inets = NULL;
  rule->_priv->src_ports = NULL;
  rule->_priv->dst_ports = NULL;
  rule->_priv->protocols = NULL;
  rule->_priv->sensors = NULL;
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

  if (rule->_priv->reliability <= 0)
    return 0;
  if (rule->_priv->reliability >= 10)
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
 *	FIXME: Not used anywhere
 */
gint
sim_rule_get_count (SimRule   *rule)
{
  g_return_val_if_fail (rule, 0);
  g_return_val_if_fail (SIM_IS_RULE (rule), 0);

  return rule->_priv->count_occu;
}

/*
 *	FIXME: Not used anywhere
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
 */
GInetAddr*
sim_rule_get_sensor (SimRule   *rule)
{
  g_return_val_if_fail (rule, NULL);
  g_return_val_if_fail (SIM_IS_RULE (rule), NULL);

  return rule->_priv->sensor;
}

/*
 *
 *
 */
void sim_rule_set_sensor (SimRule    *rule,
												  GInetAddr  *sensor)
{
  g_return_if_fail (rule);
  g_return_if_fail (SIM_IS_RULE (rule));
  g_return_if_fail (sensor);

  if (rule->_priv->sensor)
    gnet_inetaddr_unref (rule->_priv->sensor);

  rule->_priv->sensor = sensor;;
}


/*
 * Append a single plugin_sid to a GList in a SimRule
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
 * Append a sensor to the list of sensors inside the rule. 
 * This is NOT the same that the single sensor wich appears in the SimRule.
 */
void
sim_rule_append_sensor (SimRule    *rule,
												 SimSensor  *sensor)
{
  g_return_if_fail (rule);
  g_return_if_fail (SIM_IS_RULE (rule));
  g_return_if_fail (sensor);
  g_return_if_fail (SIM_IS_SENSOR (sensor));

  rule->_priv->sensors = g_list_append (rule->_priv->sensors, sensor);
}

/*
 *
 *
 *
 *
 */
void
sim_rule_remove_sensor	 (SimRule    *rule,
													SimSensor  *sensor)
{
  g_return_if_fail (rule);
  g_return_if_fail (SIM_IS_RULE (rule));
  g_return_if_fail (sensor);
  g_return_if_fail (SIM_IS_SENSOR (sensor));

  rule->_priv->sensors = g_list_remove (rule->_priv->sensors, sensor);
}

/*
 *
 *
 *
 *
 */
GList*
sim_rule_get_sensors (SimRule   *rule)
{
  g_return_val_if_fail (rule, NULL);
  g_return_val_if_fail (SIM_IS_RULE (rule), NULL);

  return rule->_priv->sensors;
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
 * Inside var there are the kind of event (src_ip, protocol, plugin_sid or whatever) and the level to wich is 
 *referencing. i.e. if in a directive appears 1:SRC_IP that info is inside the var
 *
 */
GList*
sim_rule_get_vars (SimRule     *rule)
{
  g_return_val_if_fail (rule, NULL);
  g_return_val_if_fail (SIM_IS_RULE (rule), NULL);

  return rule->_priv->vars;		//SimRuleVar
}


//Append all the Not elements (defined with "!") into GList's in the rule
/*
 * 
 */
void 
sim_rule_append_src_inet_not (SimRule *rule, 
															SimInet *src_inet) 
{
	g_return_if_fail (rule); 
	g_return_if_fail (SIM_IS_RULE (rule)); 
	g_return_if_fail (src_inet); 
	g_return_if_fail (SIM_IS_INET (src_inet)); 

	rule->_priv->src_inets_not = g_list_append (rule->_priv->src_inets_not, src_inet); 
}
 
/*
 * 
 */
void 
sim_rule_append_dst_inet_not (SimRule *rule, 
															SimInet *dst_inet) 
{
	g_return_if_fail (rule); 
	g_return_if_fail (SIM_IS_RULE (rule)); 
	g_return_if_fail (dst_inet); 
	g_return_if_fail (SIM_IS_INET (dst_inet)); 

	rule->_priv->dst_inets_not = g_list_append (rule->_priv->dst_inets_not, dst_inet); 
}

/*
 * 
 */
void 
sim_rule_append_src_port_not (SimRule *rule, 
															gint	*src_port) 
{
	g_return_if_fail (rule); 
	g_return_if_fail (SIM_IS_RULE (rule)); 
	g_return_if_fail (src_port); 

	rule->_priv->src_ports_not = g_list_append (rule->_priv->src_ports_not, src_port); 
}

/*
 * 
 */
void 
sim_rule_append_dst_port_not (SimRule *rule, 
															gint	*dst_port) 
{
	g_return_if_fail (rule); 
	g_return_if_fail (SIM_IS_RULE (rule)); 
	g_return_if_fail (dst_port); 

	rule->_priv->dst_ports_not = g_list_append (rule->_priv->dst_ports_not, dst_port); 
}

/*
 * 
 */
void 
sim_rule_append_plugin_sid_not (SimRule *rule, 
																gint plugin_sid) 
{
	g_return_if_fail (rule); 
	g_return_if_fail (SIM_IS_RULE (rule)); 
	g_return_if_fail (plugin_sid); 

	rule->_priv->plugin_sids_not = g_list_append (rule->_priv->plugin_sids_not, GINT_TO_POINTER (plugin_sid)); 
}
/*
 * 
 */
void 
sim_rule_append_protocol_not (SimRule *rule, 
															gint *protocol) 
{
	g_return_if_fail (rule); 
	g_return_if_fail (SIM_IS_RULE (rule)); 
	g_return_if_fail (protocol); 

	rule->_priv->protocols_not = g_list_append (rule->_priv->protocols_not, protocol); 
}
/*
 * 
 */
void 
sim_rule_append_sensor_not (SimRule *rule, 
														SimSensor *sensor) 
{
	g_return_if_fail (rule); 
	g_return_if_fail (SIM_IS_RULE (rule)); 
	g_return_if_fail (sensor); 
	g_return_if_fail (SIM_IS_SENSOR (sensor)); 

	rule->_priv->sensors_not = g_list_append (rule->_priv->sensors_not, sensor); 
}

//The following functions remove the not ("!") elements in the rule
/*
 * 
 */
void 
sim_rule_remove_src_inet_not (SimRule *rule, 
															SimInet *src_inet) 
{
	g_return_if_fail (rule); 
	g_return_if_fail (SIM_IS_RULE (rule)); 
	g_return_if_fail (src_inet); 
	g_return_if_fail (SIM_IS_INET (src_inet)); 

	rule->_priv->src_inets_not = g_list_remove (rule->_priv->src_inets_not, src_inet); 
}
 
/*
 * 
 */
void 
sim_rule_remove_dst_inet_not (SimRule *rule, 
															SimInet *dst_inet) 
{
	g_return_if_fail (rule); 
	g_return_if_fail (SIM_IS_RULE (rule)); 
	g_return_if_fail (dst_inet); 
	g_return_if_fail (SIM_IS_INET (dst_inet)); 

	rule->_priv->dst_inets_not = g_list_remove (rule->_priv->dst_inets_not, dst_inet); 
}

/*
 * 
 */
void 
sim_rule_remove_src_port_not (SimRule *rule, 
															gint	*src_port) 
{
	g_return_if_fail (rule); 
	g_return_if_fail (SIM_IS_RULE (rule)); 
	g_return_if_fail (src_port); 

	rule->_priv->src_ports_not = g_list_remove (rule->_priv->src_ports_not, src_port); 
}

/*
 * 
 */
void 
sim_rule_remove_dst_port_not (SimRule *rule, 
															gint	*dst_port) 
{
	g_return_if_fail (rule); 
	g_return_if_fail (SIM_IS_RULE (rule)); 
	g_return_if_fail (dst_port); 

	rule->_priv->dst_ports_not = g_list_remove (rule->_priv->dst_ports_not, dst_port); 
}

/*
 * 
 */
void 
sim_rule_remove_plugin_sid_not (SimRule *rule, 
																gint *plugin_sid) 
{
	g_return_if_fail (rule); 
	g_return_if_fail (SIM_IS_RULE (rule)); 
	g_return_if_fail (plugin_sid); 

	rule->_priv->plugin_sids_not = g_list_remove (rule->_priv->plugin_sids_not, plugin_sid); 
}
/*
 * 
 */
void 
sim_rule_remove_protocol_not (SimRule *rule, 
															gint *protocol) 
{
	g_return_if_fail (rule); 
	g_return_if_fail (SIM_IS_RULE (rule)); 
	g_return_if_fail (protocol); 

	rule->_priv->protocols_not = g_list_remove (rule->_priv->protocols_not, protocol); 
}
/*
 * 
 */
void 
sim_rule_remove_sensor_not (SimRule *rule, 
														SimSensor *sensor) 
{
	g_return_if_fail (rule); 
	g_return_if_fail (SIM_IS_RULE (rule)); 
	g_return_if_fail (sensor); 
	g_return_if_fail (SIM_IS_SENSOR (sensor)); 

	rule->_priv->sensors_not = g_list_remove (rule->_priv->sensors_not, sensor); 
}

//get the GList with the elements defined like "!" in the rule, the negated elements. 

/*
 *
 */
GList* 
sim_rule_get_src_inets_not (SimRule *rule) 
{ 
 g_return_val_if_fail (rule, NULL); 
 g_return_val_if_fail (SIM_IS_RULE (rule), NULL); 
 
 return rule->_priv->src_inets_not; 
} 

/*
 *
 */
GList* 
sim_rule_get_dst_inets_not (SimRule *rule) 
{ 
 g_return_val_if_fail (rule, NULL); 
 g_return_val_if_fail (SIM_IS_RULE (rule), NULL); 
 
 return rule->_priv->dst_inets_not; 
} 

/*
 *
 */
GList* 
sim_rule_get_src_ports_not (SimRule *rule) 
{ 
 g_return_val_if_fail (rule, NULL); 
 g_return_val_if_fail (SIM_IS_RULE (rule), NULL); 
 
 return rule->_priv->src_ports_not; 
} 

/*
 *
 */
GList* 
sim_rule_get_dst_ports_not (SimRule *rule) 
{ 
 g_return_val_if_fail (rule, NULL); 
 g_return_val_if_fail (SIM_IS_RULE (rule), NULL); 
 
 return rule->_priv->dst_ports_not; 
} 

/*
 *
 */
GList* 
sim_rule_get_plugin_sids_not (SimRule *rule) 
{ 
 g_return_val_if_fail (rule, NULL); 
 g_return_val_if_fail (SIM_IS_RULE (rule), NULL); 
 
 return rule->_priv->plugin_sids_not; 
} 

/*
 *
 */
GList* 
sim_rule_get_protocols_not (SimRule *rule) 
{ 
 g_return_val_if_fail (rule, NULL); 
 g_return_val_if_fail (SIM_IS_RULE (rule), NULL); 
 
 return rule->_priv->protocols_not; 
} 

/*
 *
 */
GList* 
sim_rule_get_sensors_not (SimRule *rule) 
{ 
 g_return_val_if_fail (rule, NULL); 
 g_return_val_if_fail (SIM_IS_RULE (rule), NULL); 
 
 return rule->_priv->sensors_not; 
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
  new_rule->_priv->sensor = (rule->_priv->sensor) ? gnet_inetaddr_clone (rule->_priv->sensor) : NULL;

  new_rule->_priv->condition = rule->_priv->condition;
  new_rule->_priv->value = (rule->_priv->value) ? g_strdup (rule->_priv->value) : NULL;
  new_rule->_priv->interval = rule->_priv->interval;

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

  /* sensors */
  list = rule->_priv->sensors;
  while (list)
    {
      SimSensor *sensor = (SimSensor *) list->data;
      new_rule->_priv->sensors = g_list_append (new_rule->_priv->sensors, sim_sensor_clone (sensor));
      list = list->next;
    }

	//"Not" elements:

 // src ips not 
	list = rule->_priv->src_inets_not; 
	while (list) 
	{ 
		SimInet *inet = (SimInet *) list->data; 
		new_rule->_priv->src_inets_not = g_list_append (new_rule->_priv->src_inets_not, sim_inet_clone (inet)); 
		list = list->next; 
	} 

  // dst ips not 
	list = rule->_priv->dst_inets_not; 
	while (list) 
	{ 
		SimInet *inet = (SimInet *) list->data; 
		new_rule->_priv->dst_inets_not = g_list_append (new_rule->_priv->dst_inets_not, sim_inet_clone (inet)); 
		list = list->next; 
	} 

  // src ports not 
	list = rule->_priv->src_ports_not; 
	while (list) 
	{ 
    gint port = GPOINTER_TO_INT (list->data);	//extracts the integer from the pointer
		new_rule->_priv->src_ports_not = g_list_append (new_rule->_priv->src_ports_not, GINT_TO_POINTER (port)); 
		list = list->next; 
	}
 
  // dst ports not 
	list = rule->_priv->dst_ports_not; 
	while (list) 
	{ 
    gint port = GPOINTER_TO_INT (list->data);	
		new_rule->_priv->dst_ports_not = g_list_append (new_rule->_priv->dst_ports_not, GINT_TO_POINTER (port)); 
		list = list->next; 
	} 

  // plugin_sids not 
	list = rule->_priv->plugin_sids_not; 
	while (list) 
	{ 
    gint plugin_sid = GPOINTER_TO_INT (list->data);	
		new_rule->_priv->plugin_sids_not = g_list_append (new_rule->_priv->plugin_sids_not, GINT_TO_POINTER (plugin_sid)); 
		list = list->next; 
	} 

  // protocols not 
	list = rule->_priv->protocols_not; 
	while (list) 
	{ 
    gint protocol = GPOINTER_TO_INT (list->data);	
		new_rule->_priv->protocols_not = g_list_append (new_rule->_priv->protocols_not, GINT_TO_POINTER (protocol)); 
		list = list->next; 
	} 
  // sensors not 
	list = rule->_priv->sensors_not; 
	while (list) 
	{ 
		SimSensor *sensor = (SimSensor *) list->data; 
		new_rule->_priv->sensors_not = g_list_append (new_rule->_priv->sensors_not, sim_sensor_clone (sensor)); 
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
 * returns TRUE if the "not_invalid" inside the rule is not active;
 * :) Traduction: if the rule has a "not" in it, not invalid will be put to not in the 
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

/*
 *
 * check if "val" variable is inside the GList. GList is a guint list.
 *
 */
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
sim_rule_match_by_event (SimRule      *rule,
												 SimEvent     *event)
{ 
  GList      *list = NULL;
  gboolean    match;
		
  g_return_val_if_fail (rule, FALSE);
  g_return_val_if_fail (SIM_IS_RULE (rule), FALSE);
  g_return_val_if_fail (rule->type != SIM_RULE_TYPE_NONE, FALSE);
  g_return_val_if_fail (rule->_priv->plugin_id > 0, FALSE);
  g_return_val_if_fail (event, FALSE);
  g_return_val_if_fail (SIM_IS_EVENT (event), FALSE);
  g_return_val_if_fail (event->type != SIM_EVENT_TYPE_NONE, FALSE);
  g_return_val_if_fail (event->plugin_id > 0, FALSE);
  g_return_val_if_fail (event->plugin_sid > 0, FALSE);
  g_return_val_if_fail (event->src_ia, FALSE);

//	sim_rule_print (rule);
	gchar *lala = sim_event_to_string(event);
  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_rule_match_by_event: printing event: %s",lala);
			
	match = TRUE;
	
  /* Time Out */
  if ((sim_rule_is_time_out (rule)) && (rule->_priv->level > 1))
    return FALSE;

  /* Match Type */
  if (rule->type != event->type)
    return FALSE;

  /* Match Plugin ID */
  if (rule->_priv->plugin_id != event->plugin_id)
    return FALSE;

	//Match "Not" fields. 
	//Here we will check if the event matches with some of the "!" fields. If some field make match, and the directive
	//has the "!" modificator, then this will return FALSE.
	
	// Match !src_ia 
	if (rule->_priv->src_inets_not) 
	{ 
		SimInet *inet = sim_inet_new_from_ginetaddr (event->src_ia); 
		list = rule->_priv->src_inets_not; 
		while (list) 
		{ 
			SimInet *cmp_inet = (SimInet *) list->data; 

			if (sim_inet_has_inet (cmp_inet, inet)) 
			{ 
				g_object_unref (inet); 
				return FALSE; 
			} 
			list = list->next; 
		} 
		g_object_unref (inet); 
	} 
 
	// Match !dst_ia 
	if (rule->_priv->dst_inets_not) 
	{ 
		SimInet *inet = sim_inet_new_from_ginetaddr (event->dst_ia); 
		list = rule->_priv->dst_inets_not; 
		while (list) 
		{ 
			SimInet *cmp_inet = (SimInet *) list->data; 

			if (sim_inet_has_inet (cmp_inet, inet)) 
			{ 
				g_object_unref (inet); 
				return FALSE; 
			} 
			list = list->next; 
		} 
		g_object_unref (inet); 
	} 

	// Match !src ports
	if (rule->_priv->src_ports_not) 
	{ 
		list = rule->_priv->src_ports_not; 
		while (list) 
		{ 
			if (event->src_port == GPOINTER_TO_INT (list->data)) //if the ports match, as this is negated, the rule doesn't match 
				return FALSE; 
			list = list->next; 
		} 
	} 

 	// Match !dst ports
	if (rule->_priv->dst_ports_not) 
	{ 
		list = rule->_priv->dst_ports_not; 
		while (list) 
		{ 
			if (event->dst_port == GPOINTER_TO_INT (list->data)) 
				return FALSE; 
			list = list->next; 
		} 
	} 

 	// Match !plugin_sids
	if (rule->_priv->plugin_sids_not) 
	{ 
		list = rule->_priv->plugin_sids_not; 
		while (list) 
		{ 
			if (event->plugin_sid == GPOINTER_TO_INT (list->data)) 
				return FALSE; 
			list = list->next; 
		} 
	} 
 
 	// Match !protocols
	if (rule->_priv->protocols_not) 
	{ 
		list = rule->_priv->protocols_not; 
		while (list) 
		{ 
			if (event->protocol == GPOINTER_TO_INT (list->data))//list->data is a pointer to SimProtocolType, more or less an int
				return FALSE; 
			list = list->next; 
		} 
	} 

  // Match !sensor
  if (rule->_priv->sensors_not)
  {
    GInetAddr *sensor_ia = gnet_inetaddr_new_nonblock (event->sensor, 0);
    list = rule->_priv->sensors_not;
    while (list)
    {
      SimSensor *cmp_sensor = (SimSensor *) list->data;

      if (gnet_inetaddr_noport_equal (sim_sensor_get_ia(cmp_sensor), sensor_ia))
      {
        g_object_unref (sensor_ia);
        return FALSE;
      }
      list = list->next;
    }
    g_object_unref (sensor_ia);
  }
 
	//match the non-negated elements.
	
  /* Match Plugin SIDs */
  if (rule->_priv->plugin_sids)
  {
    match = FALSE;
    list = rule->_priv->plugin_sids;
    while (list)
		{
	  	gint plugin_sid = GPOINTER_TO_INT (list->data);
	  
		  if ((!plugin_sid) || (plugin_sid == event->plugin_sid))
	    {
	      match = TRUE;
	      break;
	    }
	  
		  list = list->next;
		}
  }
	if (!match)
		return FALSE;

  /* Match src_ia */
  if (rule->_priv->src_inets)
  {
    SimInet *inet = sim_inet_new_from_ginetaddr (event->src_ia); //take the event src ip nd check if it belongs to the rule	
    match = FALSE;
    list = rule->_priv->src_inets;
    while (list)
		{
		  SimInet *cmp_inet = (SimInet *) list->data; //each rule can handle multiple src's
	  
		  if ((sim_inet_is_reserved (cmp_inet)) || 
		      (sim_inet_has_inet (cmp_inet, inet)))
	    {
	      match = TRUE;
	      break;
	    }
	  
	  	list = list->next;
		}
    g_object_unref (inet);
  }
	if (!match)
		return FALSE;

  /* Find dst_ia */
  if ((rule->_priv->dst_inets) && (event->dst_ia))
  {
    SimInet *inet = sim_inet_new_from_ginetaddr (event->dst_ia);
    match = FALSE;
    list = rule->_priv->dst_inets;
    while (list)
		{
		  SimInet *cmp_inet = (SimInet *) list->data;
	  
	  	if ((sim_inet_is_reserved (cmp_inet)) || 
	      (sim_inet_has_inet (cmp_inet, inet)))
	    {
	      match = TRUE;
	      break;
	    }
	  
		  list = list->next;
		}
    g_object_unref (inet);
  }
	if (!match)
		return FALSE;

  /* Find src_port */
  if (rule->_priv->src_ports)
  {
    match = FALSE;
    list = rule->_priv->src_ports;
    while (list)
		{
	  	gint cmp_port = GPOINTER_TO_INT (list->data);
	  
		  if ((!cmp_port) || (cmp_port == event->src_port))
	    {
	      match = TRUE;
	      break;
	    }
	  
		  list = list->next;
		}
  }
	if (!match)
		return FALSE;

  /* Find dst_port */
  if (rule->_priv->dst_ports)
  {
  	match = FALSE;
    list = rule->_priv->dst_ports;
    while (list)
		{
		  gint cmp_port = GPOINTER_TO_INT (list->data);
	  
	  	if ((!cmp_port) || (cmp_port == event->dst_port))
	    {
	      match = TRUE;
	      break;
	    }
	  
		  list = list->next;
		}
  }
	if (!match)
		return FALSE;

  /* Protocols */
  if (rule->_priv->protocols)
  {
    match = FALSE;
    list = rule->_priv->protocols;
    while (list)
		{
	  	SimProtocolType cmp_prot = GPOINTER_TO_INT (list->data);
	  
		  if ((!cmp_prot) || (cmp_prot == event->protocol))
	    {
	      match = TRUE;
	      break;
	    }
	  
	  	list = list->next;
		}
  }
	if (!match)
		return FALSE;

  /* Match sensor */
  if (rule->_priv->sensors)
  {
    match = FALSE;
    list = rule->_priv->sensors;
		gchar *tmp;
    while (list)
    {
      SimSensor *cmp_sensor = (SimSensor *) list->data; //each rule can handle multiple sensors
			tmp = gnet_inetaddr_get_canonical_name (sim_sensor_get_ia(cmp_sensor));	//get the dotted decimal name.
		  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_rule_match_by_event: %s - %s",tmp, event->sensor);

			if ((!strcmp ("0.0.0.0", tmp)) ||
					(!strcmp (event->sensor, tmp)))
				{
					match = TRUE;
					break;
				}
      list = list->next;
    }
  }
	if (!match)
		return FALSE;


	
  /* Match Condition (Only monitor events)*/
  if ((rule->_priv->condition != SIM_CONDITION_TYPE_NONE) &&
      (event->condition != SIM_CONDITION_TYPE_NONE))
  {
    if (rule->_priv->condition != event->condition)
			return FALSE;

    /* Match Value */
    if ((rule->_priv->value) && (event->value))
		{
			//The event->value must be the same than rule->_priv->value to match. When we ask to
			//an agent a watch_rule, is the agent who compares and test if it's the real value.
			//Then, the agent will return to us an event with the same value that we send to him
			//so we know them that our question has matched
		  if (g_ascii_strcasecmp (rule->_priv->value, event->value))
	  	  return FALSE;
		}
  }

  /* If rule is sticky */
  if (rule->_priv->sticky)
    event->sticky = TRUE;

  if ((rule->_priv->occurrence > 1) && (rule->_priv->sticky_different))
  {
    guint32 val;

		//sticky_different can be assigned only to a single variable
    switch (rule->_priv->sticky_different)
		{
			case SIM_RULE_VAR_PLUGIN_SID:
						  val = (guint32) event->plugin_sid; //if we find the plugin_sid from the event inside the stickys list, it returns false because it means that it belongs to another directive
						  if (find_guint32_value (rule->_priv->stickys, val))
	    					return FALSE;
					  	rule->_priv->stickys = g_list_append (rule->_priv->stickys, GINT_TO_POINTER (val));
							break;
			case SIM_RULE_VAR_SRC_IA:
							val = (guint32) sim_inetaddr_ntohl (event->src_ia);
							if (find_guint32_value (rule->_priv->stickys, val))
								return FALSE;
							rule->_priv->stickys = g_list_append (rule->_priv->stickys, GINT_TO_POINTER (val));
							break;
			case SIM_RULE_VAR_DST_IA:
							val = (guint32) sim_inetaddr_ntohl (event->dst_ia);
							if (find_guint32_value (rule->_priv->stickys, val))
								return FALSE;
							rule->_priv->stickys = g_list_append (rule->_priv->stickys, GINT_TO_POINTER (val));
							break;
			case SIM_RULE_VAR_SRC_PORT:
							val = (guint32) event->src_port;
							if (find_guint32_value (rule->_priv->stickys, val))
								return FALSE;
							rule->_priv->stickys = g_list_append (rule->_priv->stickys, GINT_TO_POINTER (val));
							break;
			case SIM_RULE_VAR_DST_PORT:
							val = (guint32) event->dst_port;
							if (find_guint32_value (rule->_priv->stickys, val))
								return FALSE;
							rule->_priv->stickys = g_list_append (rule->_priv->stickys, GINT_TO_POINTER (val));
							break;
			case SIM_RULE_VAR_PROTOCOL:
							val = (guint32) event->protocol;
							if (find_guint32_value (rule->_priv->stickys, val))
								return FALSE;
							rule->_priv->stickys = g_list_append (rule->_priv->stickys, GINT_TO_POINTER (val));
							break;
			case SIM_RULE_VAR_SENSOR:
							val = (guint32) sim_ipchar_2_ulong (event->sensor);
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

    event->level = rule->_priv->level;
    event->match = TRUE;
    if (rule->_priv->occurrence != rule->_priv->count_occu)
		{
	  	rule->_priv->count_occu++;
		  event->count = rule->_priv->count_occu - 1;
		  return FALSE;
		}
    else
		{
	  	event->count = rule->_priv->occurrence;
		  rule->_priv->count_occu = 1;
		}
  }
  else
    event->count = 1;

  /* Not */
	//If the rule is enterely negated, and after all the checks it matches, we have to return false.
  if (rule->_priv->not)
    {
      rule->_priv->not_invalid = TRUE; //I have to check this statment
      return FALSE;
    }

  event->level = rule->_priv->level;
  event->match = TRUE;
  return TRUE;
}

/*
 *
 *
 *
 *
 */
void
sim_rule_set_event_data (SimRule      *rule,
												 SimEvent     *event)
{
  g_return_if_fail (rule);
  g_return_if_fail (SIM_IS_RULE (rule));
  g_return_if_fail (event);
  g_return_if_fail (SIM_IS_EVENT (event));

  rule->_priv->plugin_sid = event->plugin_sid;

	g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_rule_set_event_data: src_ia: %s", gnet_inetaddr_get_canonical_name(event->src_ia));
	g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_rule_set_event_data: dst_ia: %s", gnet_inetaddr_get_canonical_name(event->dst_ia));
	g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_rule_set_event_data: sensor: %s", event->sensor);
	 
  if (gnet_inetaddr_get_canonical_name(event->src_ia) && gnet_inetaddr_get_canonical_name(event->dst_ia))
  {
    rule->_priv->src_ia = (event->src_ia) ? gnet_inetaddr_clone (event->src_ia) : NULL;
    rule->_priv->dst_ia = (event->dst_ia) ? gnet_inetaddr_clone (event->dst_ia) : NULL;
    rule->_priv->src_port = event->src_port;
    rule->_priv->dst_port = event->dst_port;
    rule->_priv->protocol = event->protocol;
    rule->_priv->sensor = (event->sensor) ? gnet_inetaddr_new_nonblock (event->sensor, 0) : NULL;
  }
  else
    g_message("Error: The src or dst of an event is wrong");
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
    rule->_priv->src_ia = gnet_inetaddr_clone (rule->_priv->src_inets->data);//FIXME: I think that this don't do what its supposed to..

  if ((rule->_priv->dst_inets) && (rule->_priv->dst_inets->data))
    rule->_priv->dst_ia = gnet_inetaddr_clone (rule->_priv->dst_inets->data);
  if ((rule->_priv->src_ports) && (rule->_priv->src_ports->data))
    rule->_priv->src_port = GPOINTER_TO_INT (rule->_priv->src_ports->data);
  if ((rule->_priv->dst_ports) && (rule->_priv->dst_ports->data))
    rule->_priv->dst_port = GPOINTER_TO_INT (rule->_priv->dst_ports->data);

  if ((rule->_priv->sensors) && (rule->_priv->sensors->data))
    rule->_priv->sensor =  gnet_inetaddr_clone (rule->_priv->sensors->data);//FIXME: wrrrronggggg
}

/*
 *
 * This function is just for debugging, it's not needed to call it from anywhere.
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

  g_message ("Rule: ");
  g_message ("sticky=%d ", rule->_priv->sticky);
  g_message ("not=%d ", rule->_priv->not);
  g_message ("name=%s ", rule->_priv->name);
  g_message ("level=%d ", rule->_priv->level);
  g_message ("priority=%d ", rule->_priv->priority);
  g_message ("reliability=%d ", rule->_priv->reliability);
  g_message ("time_out=%d ", rule->_priv->time_out);
  g_message ("occurrence=%d ", rule->_priv->occurrence);
  g_message ("plugin_id=%d ", rule->_priv->plugin_id);
  g_message ("plugin_sid=%d ", g_list_length (rule->_priv->plugin_sids));
  g_message ("src_inets=%d ", g_list_length (rule->_priv->src_inets));
  g_message ("\nplugin_sids_not=%d ", g_list_length (rule->_priv->plugin_sids_not));
  list = rule->_priv->src_inets;
  while (list)
    {
      SimInet *ia = (SimInet *) list->data;
      ip = sim_inet_ntop (ia);
      g_message (" %s ", ip);
      g_free (ip);
      list = list->next;
    }

	g_message ("dst_inets=%d ", g_list_length (rule->_priv->dst_inets));
  list = rule->_priv->dst_inets;
  while (list)
    {
      SimInet *ia = (SimInet *) list->data;
      ip = sim_inet_ntop (ia);
      g_message (" %s ", ip);
      g_free (ip);
      list = list->next;
    }
  g_message ("src_ports=%d ", g_list_length (rule->_priv->src_ports));
  list = rule->_priv->src_ports;
  while (list)
    {
      gint port = GPOINTER_TO_INT (list->data);
      g_message (" %d ", port);
      list = list->next;
    }
  g_message ("dst_ports=%d ", g_list_length (rule->_priv->dst_ports));
  list = rule->_priv->dst_ports;
  while (list)
    {
      gint port = GPOINTER_TO_INT (list->data);
      g_message (" %d ", port);
      list = list->next;
    }
  g_message ("sensors=%d ", g_list_length (rule->_priv->sensors));
  list = rule->_priv->sensors;
  while (list)
    {
			SimSensor *sensor = (SimSensor *) list->data;
      g_message (" %s ", sim_sensor_get_name(sensor));
      list = list->next;
    }
 g_message ("vars=%d ", g_list_length (rule->_priv->vars));
  if (rule->_priv->src_ia)
    {
      ip = gnet_inetaddr_get_canonical_name (rule->_priv->src_ia);
      g_message ("src_ia=%s ", ip);
      g_free (ip);
    }
  if (rule->_priv->dst_ia)
    {
      ip = gnet_inetaddr_get_canonical_name (rule->_priv->dst_ia);
      g_message ("dst_ia=%s ", ip);
      g_free (ip);
    }
   if (rule->_priv->sensor)
    {
      ip = gnet_inetaddr_get_canonical_name (rule->_priv->dst_ia);
      g_message ("sensor=%s ", ip);
      g_free (ip);
    }
 g_message ("src_port=%d ", rule->_priv->src_port);
  g_message ("dst_port=%d ", rule->_priv->dst_port);

  g_message ("src_inets_not=%d ", g_list_length (rule->_priv->src_inets_not));
	list = rule->_priv->src_inets_not;
  while (list)
    {
      SimInet *ia = (SimInet *) list->data;
      ip = sim_inet_ntop (ia);
      g_message (" %s ", ip);
      g_free (ip);
      list = list->next;
    }

  g_message ("dst_inets_not=%d ", g_list_length (rule->_priv->dst_inets_not));
	list = rule->_priv->dst_inets_not;
  while (list)
    {
      SimInet *ia = (SimInet *) list->data;
      ip = sim_inet_ntop (ia);
      g_message (" %s ", ip);
      g_free (ip);
      list = list->next;
    }

	g_message ("src_ports_not=%d ", g_list_length (rule->_priv->src_ports_not));
	list = rule->_priv->src_ports_not;
  while (list)
    {
      g_message (" %d ", GPOINTER_TO_INT (list->data));
      list = list->next;
    }

	g_message ("sAAAAAAAAAAAAA");
	g_message ("dst_ports_not=%d ", g_list_length (rule->_priv->dst_ports_not));
	list = rule->_priv->dst_ports_not;
  while (list)
    {
      g_message (" %d ", GPOINTER_TO_INT (list->data));
      list = list->next;
    }

	g_message ("protocols_not=%d ", g_list_length (rule->_priv->protocols_not));
	list = rule->_priv->protocols_not;
  while (list)
    {
      g_message (" %d ", GPOINTER_TO_INT (list->data));
      list = list->next;
    }

  g_message ("plugin_sids_not=%d ", g_list_length (rule->_priv->plugin_sids_not));
	list = rule->_priv->plugin_sids_not;
  while (list)
    {
      g_message (" %d ", GPOINTER_TO_INT (list->data));
      list = list->next;
    }


	g_message ("sensors_not=%d ", g_list_length (rule->_priv->sensors_not));
	list = rule->_priv->sensors_not;
  while (list)
    {
      SimSensor *sensor = (SimSensor *) list->data;
      GInetAddr *sensor_ia = sim_sensor_get_ia (sensor);
      g_message (" %s ", gnet_inetaddr_get_canonical_name(sensor_ia));
      list = list->next;
    }

  g_message ("\n");
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

  return g_string_free (str, FALSE); //liberate the GString object, but not the string itself so we can return it.
}

// vim: set tabstop=2:

