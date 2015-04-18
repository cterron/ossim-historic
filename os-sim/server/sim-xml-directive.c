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

#include <sim-util.h>
#include <sim-net.h>
#include <sim-xml-directive.h>

struct _SimXmlDirectivePrivate {
  SimContainer  *container;

  GList    *directives;
};

#define OBJECT_DIRECTIVES       "directives"
#define OBJECT_DIRECTIVE        "directive"
#define OBJECT_RULE             "rule"
#define OBJECT_RULES            "rules"
#define OBJECT_ACTION           "action"
#define OBJECT_ACTIONS          "actions"

#define PROPERTY_ID             "id"
#define PROPERTY_NAME           "name"
#define PROPERTY_TIME_OUT       "time_out"
#define PROPERTY_SRC_IP         "from"
#define PROPERTY_DST_IP         "to"
#define PROPERTY_SRC_PORT       "port_from"
#define PROPERTY_DST_PORT       "port_to"
#define PROPERTY_PLUGIN         "plugin"
#define PROPERTY_TPLUGIN        "tplugin"
#define PROPERTY_ON             "on"

static void sim_xml_directive_class_init (SimXmlDirectiveClass *klass);
static void sim_xml_directive_init       (SimXmlDirective *xmldirect, SimXmlDirectiveClass *klass);
static void sim_xml_directive_finalize   (GObject *object);

/*
 * SimXmlDirective object signals
 */
enum {
  SIM_XML_DIRECTIVE_CHANGED,
  SIM_XML_DIRECTIVE_LAST_SIGNAL
};

static gint xmldirect_signals[SIM_XML_DIRECTIVE_LAST_SIGNAL] = { 0, };
static GObjectClass *parent_class = NULL;

/*
 * SimXmlDirective class interface
 */

static void
sim_xml_directive_class_init (SimXmlDirectiveClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  
  parent_class = g_type_class_peek_parent (klass);
  
  xmldirect_signals[SIM_XML_DIRECTIVE_CHANGED] =
    g_signal_new ("changed",
		  G_TYPE_FROM_CLASS (object_class),
		  G_SIGNAL_RUN_LAST,
		  G_STRUCT_OFFSET (SimXmlDirectiveClass, changed),
		  NULL, NULL,
		  g_cclosure_marshal_VOID__VOID,
		  G_TYPE_NONE, 0);
  
  object_class->finalize = sim_xml_directive_finalize;
  klass->changed = NULL;
}

static void
sim_xml_directive_init (SimXmlDirective *xmldirect, SimXmlDirectiveClass *klass)
{
  g_return_if_fail (SIM_IS_XML_DIRECTIVE (xmldirect));
  
  /* allocate private structure */
  xmldirect->_priv = g_new0 (SimXmlDirectivePrivate, 1);
  xmldirect->_priv->directives = NULL;
}

static void
sim_xml_directive_finalize (GObject *object)
{
  SimXmlDirective *xmldirect = (SimXmlDirective *) object;
  
  g_return_if_fail (SIM_IS_XML_DIRECTIVE (xmldirect));
  
  /* chain to parent class */
  parent_class->finalize (object);
}

GType
sim_xml_directive_get_type (void)
{
  static GType type = 0;
  
  if (!type) {
    static const GTypeInfo info = {
      sizeof (SimXmlDirectiveClass),
      (GBaseInitFunc) NULL,
      (GBaseFinalizeFunc) NULL,
      (GClassInitFunc) sim_xml_directive_class_init,
      NULL,
      NULL,
      sizeof (SimXmlDirective),
      0,
      (GInstanceInitFunc) sim_xml_directive_init
    };
    type = g_type_register_static (G_TYPE_OBJECT,
				   "SimXmlDirective",
				   &info, 0);
  }
  return type;
}

/**
 * sim_xml_directive_new
 *
 * Creates a new #SimXmlDirective object, which can be used to describe
 * a directive which will then be loaded by a provider to create its
 * defined structure
 */
SimXmlDirective *
sim_xml_directive_new (void)
{
  SimXmlDirective *xmldirect;
  
  xmldirect = g_object_new (SIM_TYPE_XML_DIRECTIVE, NULL);
  return xmldirect;
}

/**
 * sim_xml_directive_new_from_file
 */
SimXmlDirective*
sim_xml_directive_new_from_file (SimContainer *container,
				 const gchar *file)
{
  SimXmlDirective *xmldirect;
  gchar *body;
  xmlDocPtr doc;
  xmlNodePtr root;
  xmlNodePtr node;
  
  g_return_if_fail (container != NULL);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_val_if_fail (file != NULL, NULL);
  
  /* load the file from the given FILE */
  body = sim_file_load (file);
  if (!body) {
    g_message ("Could not load file at %s", file);
    return NULL;
  }
  
  /* parse the loaded XML file */
  doc = xmlParseMemory (body, strlen (body));
  g_free (body);
  
  if (!doc) {
    g_message ("Could not parse file at %s", file);
    return NULL;
  }
  
  xmldirect = g_object_new (SIM_TYPE_XML_DIRECTIVE, NULL);
  xmldirect->_priv->container = container;

  /* parse the file */
  root = xmlDocGetRootElement (doc);
  if (strcmp (root->name, OBJECT_DIRECTIVES)) {
    g_message ("Invalid XML directive file '%s'", file);
    g_object_unref (G_OBJECT (xmldirect));
    return NULL;
  }

  node = root->xmlChildrenNode;
  while (node) {

    if (!strcmp (node->name, OBJECT_DIRECTIVE))
      sim_xml_directive_new_directive_from_node (xmldirect, node);
    else
      g_message ("Invalid directive element");

    node = node->next;
  }

  return xmldirect;
}

/**
 *
 *
 *
 */
void
sim_xml_directive_set_container (SimXmlDirective * xmldirect,
			      SimContainer *container)
{
  g_return_if_fail (xmldirect != NULL);
  g_return_if_fail (SIM_IS_XML_DIRECTIVE (xmldirect));
  g_return_if_fail (container != NULL);
  g_return_if_fail (SIM_IS_CONTAINER (container));

  xmldirect->_priv->container = container;
}



/**
 * sim_xml_directive_changed
 * @xmldirect: XML directive
 *
 * Emit the "changed" signal for the given XML directive
 */
void
sim_xml_directive_changed (SimXmlDirective * xmldirect)
{
  g_return_if_fail (SIM_IS_XML_DIRECTIVE (xmldirect));
  g_signal_emit (G_OBJECT (xmldirect),
		 xmldirect_signals[SIM_XML_DIRECTIVE_CHANGED],
		 0);
}

/**
 * sim_xml_directive_reload
 * @xmldirect: XML directive.
 *
 * Reload the given XML directive from its original place, discarding
 * all changes that may have happened.
 */
void
sim_xml_directive_reload (SimXmlDirective *xmldirect)
{
  /* FIXME: implement */
}

/**
 * sim_xml_directive_save
 * @xmldirect: XML directive.
 * @file: FILE to save the XML directive to.
 *
 * Save the given XML directive to disk.
 */
gboolean
sim_xml_directive_save (SimXmlDirective *xmldirect, const gchar *file)
{
  gchar*xml;
  gboolean result;
  
  g_return_val_if_fail (SIM_IS_XML_DIRECTIVE (xmldirect), FALSE);
  
  xml = sim_xml_directive_to_string (xmldirect);
  if (xml) {
    result = sim_file_save (file, xml, strlen (xml));
    g_free (xml);
  } else
    result = FALSE;

  return result;
}

/**
 * sim_xml_directive_to_string
 * @xmldirect: a #SimXmlDirective object.
 *
 * Get the given XML directive contents as a XML string.
 *
 * Returns: the XML string representing the structure and contents of the
 * given #SimXmlDirective object. The returned value must be freed when no
 * longer needed.
 */
gchar *
sim_xml_directive_to_string (SimXmlDirective *xmldirect)
{
  xmlDocPtr doc;
  xmlNodePtr root;
  xmlNodePtr tables_node = NULL;
  GList *list, *l;
  xmlChar *xml;
  gint size;
  gchar *retval;
  
  g_return_val_if_fail (SIM_IS_XML_DIRECTIVE (xmldirect), NULL);
  
  /* create the top node */
  doc = xmlNewDoc ("1.0");
  root = xmlNewDocNode (doc, NULL, OBJECT_DIRECTIVES, NULL);
  xmlDocSetRootElement (doc, root);
  
  /* save to memory */
  xmlDocDumpMemory (doc, &xml, &size);
  xmlFreeDoc (doc);
  if (!xml) {
    g_message ("Could not dump XML file to memory");
    return NULL;
  }
  
  retval = g_strdup (xml);
  free (xml);
  
  return retval;
}

/*
 *
 *
 *
 *
 */
SimDirective*
sim_xml_directive_new_directive_from_node (SimXmlDirective  *xmldirect,
					   xmlNodePtr        node)
{
  SimDirective  *directive;
  SimAction     *action;
  GNode         *rule_root;
  xmlNodePtr     children;
  xmlNodePtr     actions;
  gchar         *name;
  gint           id;
  GTime          timeout;

  g_return_val_if_fail (xmldirect != NULL, NULL);
  g_return_val_if_fail (SIM_IS_XML_DIRECTIVE (xmldirect), NULL);
  g_return_val_if_fail (node != NULL, NULL);

  if (strcmp (node->name, OBJECT_DIRECTIVE))
    {
      g_message ("Invalid directive node %s", node->name);
      return NULL;
    }

  id = atoi (xmlGetProp (node, PROPERTY_ID));
  name = g_strdup (xmlGetProp (node, PROPERTY_NAME));
  timeout = atoi (xmlGetProp (node, PROPERTY_TIME_OUT));

  directive = sim_directive_new ();
  sim_directive_set_id (directive, id);
  sim_directive_set_name (directive, name);
  sim_directive_set_time_out (directive, timeout);

  children = node->xmlChildrenNode;
  while (children) {

    if (!strcmp (children->name, OBJECT_RULE))
      {
	rule_root = sim_xml_directive_new_rule_from_node (xmldirect, children, NULL, 1);
      }
    
    if (!strcmp (children->name, OBJECT_ACTIONS))
      {
	actions = children->children;
	while (actions)
	  {
	    action = sim_xml_directive_new_action_from_node (xmldirect, actions);

	    sim_directive_append_action (directive, action);
	    
	    actions = actions->next;;
	  }
      }

    children = children->next;
  }

  sim_directive_set_rule_root (directive, rule_root);
  
  xmldirect->_priv->directives = g_list_append (xmldirect->_priv->directives, directive);

  return directive;
}


/*
 *
 *
 *
 *
 */
SimAction*
sim_xml_directive_new_action_from_node (SimXmlDirective *xmldirect,
					xmlNodePtr       node)
{
  SimAction  *action;

  g_return_if_fail (xmldirect != NULL);
  g_return_if_fail (SIM_IS_XML_DIRECTIVE (xmldirect));
  g_return_if_fail (node != NULL);
  
  if (strcmp (node->name, OBJECT_ACTION))
    {
      g_message ("Invalid action node %s", node->name);
      return NULL;
    }

  action = sim_action_new ();

  return action;
}

/*
 *
 *
 *
 *
 */
static void
sim_xml_directive_set_rule_src_ips (SimXmlDirective  *xmldirect,
				    SimRule          *rule,
				    gchar            *value)
{
  SimContainer  *container;
  SimNet     *net;
  gchar     **values;
  gchar     **level;
  gint        i;

  g_return_if_fail (xmldirect != NULL);
  g_return_if_fail (SIM_IS_XML_DIRECTIVE (xmldirect));
  g_return_if_fail (rule != NULL);
  g_return_if_fail (SIM_IS_RULE (rule));
  g_return_if_fail (value != NULL);

  container = xmldirect->_priv->container;

  values = g_strsplit (value, SIM_DELIMITER_LIST, 0);
  for (i = 0; values[i] != NULL; i++)
    {
      if (strstr (values[i], SIM_DELIMITER_LEVEL))
	{
	  SimRuleVar *var = g_new0 (SimRuleVar, 1);

	  level = g_strsplit (values[i], SIM_DELIMITER_LEVEL, 0);

	  var->type = sim_get_rule_var_from_char (level[1]);
	  var->attr = SIM_RULE_VAR_SRC_IA;
	  var->level = atoi(level[0]);
	  
	  sim_rule_append_var (rule, var);

	  g_strfreev (level);
	}
      else if (!strcmp (values[i], SIM_IN_ADDR_ANY_CONST))
	{
	  GInetAddr *ia = gnet_inetaddr_new_nonblock (SIM_IN_ADDR_ANY_IP_STR, 0);
	  sim_rule_append_src_ia (rule, ia);
	}
      else
	{
	  net = (SimNet *) sim_container_get_net_by_name (container, values[i]);
	  if (net)
	    {
	      GList *ias = sim_net_get_ias (net);
	      while (ias)
		{
		  GInetAddr *ia = (GInetAddr *) ias->data;

		  sim_rule_append_src_ia (rule, ia);
		  
		  ias = ias->next;
		}
	    }
	  else
	    {
	      GList *ias = sim_get_ias (values[i]);
	      while (ias)
		{
		  GInetAddr *ia = (GInetAddr *) ias->data;
		  
		  sim_rule_append_src_ia (rule, ia);
		  
		  ias = ias->next;
		}
	    }
	}
    }

  g_strfreev (values);
}

/*
 *
 *
 *
 *
 */
static void
sim_xml_directive_set_rule_dst_ips (SimXmlDirective  *xmldirect,
				    SimRule          *rule,
				    gchar            *value)
{
  SimContainer  *container;
  SimNet     *net;
  gchar     **values;
  gchar     **level;
  gint        i;

  g_return_if_fail (xmldirect != NULL);
  g_return_if_fail (SIM_IS_XML_DIRECTIVE (xmldirect));
  g_return_if_fail (rule != NULL);
  g_return_if_fail (SIM_IS_RULE (rule));
  g_return_if_fail (value != NULL);

  container = xmldirect->_priv->container;

  values = g_strsplit (value, SIM_DELIMITER_LIST, 0);
  for (i = 0; values[i] != NULL; i++)
    {
      if (strstr (values[i], SIM_DELIMITER_LEVEL))
	{
	  SimRuleVar *var = g_new0 (SimRuleVar, 1);

	  level = g_strsplit (values[i], SIM_DELIMITER_LEVEL, 0);

	  var->type = sim_get_rule_var_from_char (level[1]);
	  var->attr = SIM_RULE_VAR_DST_IA;
	  var->level = atoi(level[0]);
	  
	  sim_rule_append_var (rule, var);

	  g_strfreev (level);
	}
      else if (!strcmp (values[i], SIM_IN_ADDR_ANY_CONST))
	{
	  GInetAddr *ia = gnet_inetaddr_new_nonblock (SIM_IN_ADDR_ANY_IP_STR, 0);
	  sim_rule_append_dst_ia (rule, ia);
	}
      else
	{
	  net = (SimNet *) sim_container_get_net_by_name (container, values[i]);
	  if (net)
	    {
	      GList *ias = sim_net_get_ias (net);
	      while (ias)
		{
		  GInetAddr *ia = (GInetAddr *) ias->data;

		  sim_rule_append_dst_ia (rule, ia);
		  
		  ias = ias->next;
		}
	    }
	  else
	    {
	      GList *ias = sim_get_ias (values[i]);
	      while (ias)
		{
		  GInetAddr *ia = (GInetAddr *) ias->data;
		  
		  sim_rule_append_dst_ia (rule, ia);
		  
		  ias = ias->next;
		}
	    }
	}
    }

  g_strfreev (values);
}

/*
 *
 *
 *
 *
 */
static void
sim_xml_directive_set_rule_src_ports (SimXmlDirective  *xmldirect,
				      SimRule          *rule,
				      gchar            *value)
{
  SimContainer  *container;
  SimNet     *net;
  GList      *hosts;
  gchar     **values;
  gchar     **level;
  gchar      *host;
  gint        i;

  g_return_if_fail (xmldirect != NULL);
  g_return_if_fail (SIM_IS_XML_DIRECTIVE (xmldirect));
  g_return_if_fail (rule != NULL);
  g_return_if_fail (SIM_IS_RULE (rule));
  g_return_if_fail (value != NULL);

  container = xmldirect->_priv->container;

  values = g_strsplit (value, SIM_DELIMITER_LIST, 0);
  for (i = 0; values[i] != NULL; i++)
    {
      if (strstr (values[i], SIM_DELIMITER_LEVEL))
	{
	  SimRuleVar *var = g_new0 (SimRuleVar, 1);

	  level = g_strsplit (values[i], SIM_DELIMITER_LEVEL, 0);

	  var->type = sim_get_rule_var_from_char (level[1]);
	  var->attr = SIM_RULE_VAR_SRC_PORT;
	  var->level = atoi(level[0]);

	  sim_rule_append_var (rule, var);

	  g_strfreev (level);
	}
      else if (!strcmp (values[i], SIM_IN_ADDR_ANY_CONST))
	{
	  sim_rule_append_src_port (rule, 0);
	}
      else
	{
	  sim_rule_append_src_port (rule, atoi (values[i]));
	}
    }
  g_strfreev (values);
}

/*
 *
 *
 *
 *
 */
static void
sim_xml_directive_set_rule_dst_ports (SimXmlDirective  *xmldirect,
				      SimRule          *rule,
				      gchar            *value)
{
  SimContainer  *container;
  SimNet     *net;
  GList      *hosts;
  gchar     **values;
  gchar     **level;
  gchar      *host;
  gint        i;

  g_return_if_fail (xmldirect != NULL);
  g_return_if_fail (SIM_IS_XML_DIRECTIVE (xmldirect));
  g_return_if_fail (rule != NULL);
  g_return_if_fail (SIM_IS_RULE (rule));
  g_return_if_fail (value != NULL);

  container = xmldirect->_priv->container;

  values = g_strsplit (value, SIM_DELIMITER_LIST, 0);
  for (i = 0; values[i] != NULL; i++)
    {
      if (strstr (values[i], SIM_DELIMITER_LEVEL))
	{
	  SimRuleVar *var = g_new0 (SimRuleVar, 1);

	  level = g_strsplit (values[i], SIM_DELIMITER_LEVEL, 0);

	  var->type = sim_get_rule_var_from_char (level[1]);
	  var->attr = SIM_RULE_VAR_DST_PORT;
	  var->level = atoi(level[0]);
	  
	  sim_rule_append_var (rule, var);

	  g_strfreev (level);
	}
      else if (!strcmp (values[i], SIM_IN_ADDR_ANY_CONST))
	{
	  sim_rule_append_dst_port (rule, 0);
	}
      else
	{
	  sim_rule_append_dst_port (rule, atoi (values[i]));
	}
    }
  g_strfreev (values);
}

/*
 *
 *
 *
 *
 */
GNode*
sim_xml_directive_new_rule_from_node (SimXmlDirective  *xmldirect,
				      xmlNodePtr        node,
				      GNode            *root,
				      gint              level)
{
  SimRule       *rule;
  SimAction     *action;
  GNode         *rule_node;
  GNode         *rule_child;
  xmlNodePtr     children;
  xmlNodePtr     children_rules;
  xmlNodePtr     actions;
  gchar         *name;

  g_return_val_if_fail (xmldirect != NULL, NULL);
  g_return_val_if_fail (SIM_IS_XML_DIRECTIVE (xmldirect), NULL);
  g_return_val_if_fail (node != NULL, NULL);

  if (strcmp (node->name, OBJECT_RULE))
    {
      g_message ("Invalid rule node %s", node->name);
      return NULL;
    }

  name = g_strdup (xmlGetProp (node, PROPERTY_NAME));

  rule = sim_rule_new ();
  sim_rule_set_level (rule, level);
  sim_rule_set_name (rule, name);

  sim_xml_directive_set_rule_src_ips (xmldirect, rule, xmlGetProp (node, PROPERTY_SRC_IP));
  sim_xml_directive_set_rule_dst_ips (xmldirect, rule, xmlGetProp (node, PROPERTY_DST_IP));
  sim_xml_directive_set_rule_src_ports (xmldirect, rule, xmlGetProp (node, PROPERTY_SRC_PORT));
  sim_xml_directive_set_rule_dst_ports (xmldirect, rule, xmlGetProp (node, PROPERTY_DST_PORT));

  sim_rule_set_plugin (rule, atoi (xmlGetProp (node, PROPERTY_PLUGIN)));
  sim_rule_set_tplugin (rule, atoi (xmlGetProp (node, PROPERTY_TPLUGIN)));

  if (!root)
    rule_node = g_node_new (rule);
  else
    rule_node = g_node_append_data (root, rule);

  children = node->xmlChildrenNode;
  while (children) 
    {

      /* Gets Rules Node */
      if (!strcmp (children->name, OBJECT_RULES))
	{
	  children_rules = children->xmlChildrenNode;
	  while (children_rules)
	    {
	      /* Recursive call */
	      sim_xml_directive_new_rule_from_node (xmldirect, children_rules, rule_node, level + 1);
	      
	      children_rules = children_rules->next;
	    }
	}
 
      if (!strcmp (children->name, OBJECT_ACTIONS))
	{
	  actions = children->children;
	  while (actions)
	    {
	      action = sim_xml_directive_new_action_from_node (xmldirect, actions);
	      
	      sim_rule_append_action (rule, action);
	      
	      actions = actions->next;;
	    }
	}

      children = children->next;
    }

  return rule_node;
}

/*
 *
 *
 *
 *
 */
GList*
sim_xml_directive_get_directives (SimXmlDirective *xmldirect)
{
  g_return_val_if_fail (xmldirect != NULL, NULL);
  g_return_val_if_fail (SIM_IS_XML_DIRECTIVE (xmldirect), NULL);

  return xmldirect->_priv->directives;
}
