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

#include "sim-inet.h"
#include <config.h>
#include <string.h>

struct _SimXmlDirectivePrivate {
  SimContainer  *container;

  GList    *directives;
  GList    *groups;
};

#define OBJECT_DIRECTIVES       "directives"
#define OBJECT_DIRECTIVE        "directive"
#define OBJECT_RULE             "rule"
#define OBJECT_RULES            "rules"
#define OBJECT_ACTION           "action"
#define OBJECT_ACTIONS          "actions"

#define PROPERTY_ID             "id"
#define PROPERTY_NAME           "name"
#define PROPERTY_STICKY         "sticky"
#define PROPERTY_STICKY_DIFFERENT	"sticky_different"
#define PROPERTY_NOT			"not"
#define PROPERTY_TYPE           "type"
#define PROPERTY_PRIORITY       "priority"
#define PROPERTY_RELIABILITY    "reliability"
#define PROPERTY_REL_ABS        "rel_abs"
#define PROPERTY_CONDITION      "condition"
#define PROPERTY_VALUE          "value"
#define PROPERTY_INTERVAL       "interval"
#define PROPERTY_ABSOLUTE       "absolute"
#define PROPERTY_TIME_OUT       "time_out"
#define PROPERTY_OCCURRENCE     "occurrence"
#define PROPERTY_SRC_IP         "from"
#define PROPERTY_DST_IP         "to"
#define PROPERTY_SRC_PORT       "port_from"
#define PROPERTY_DST_PORT       "port_to"
#define PROPERTY_PROTOCOL       "protocol"
#define PROPERTY_PLUGIN_ID			"plugin_id"
#define PROPERTY_PLUGIN_SID			"plugin_sid"
#define PROPERTY_SENSOR					"sensor"

#define OBJECT_GROUPS			"groups"
#define OBJECT_GROUP			"group"
#define OBJECT_APPEND_DIRECTIVE		"append-directive"
#define PROPERTY_DIRECTIVE_ID		"directive_id"


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


gboolean 
sim_xml_directive_new_groups_from_node (SimXmlDirective	*xmldirect,
					xmlNodePtr	node);
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
  xmldirect->_priv->groups = NULL;
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

/*
 *
 *
 *
 *
 */
SimDirective*
find_directive (SimXmlDirective	*xmldirect,
		gint		id)
{
  GList *list;

  g_return_val_if_fail (xmldirect, NULL);
  g_return_val_if_fail (SIM_IS_XML_DIRECTIVE (xmldirect), NULL);
  
  if (!id)
    return NULL;

  list = xmldirect->_priv->directives;
  while (list)
    {
      SimDirective *directive = (SimDirective *) list->data;
      gint cmp = sim_directive_get_id (directive);

      if (cmp == id)
	return directive;

      list = list->next;
    }

  return NULL;
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
  SimXmlDirective *xmldirect;	//here will be stored all the directives, and all the groups
  gchar *body;
  xmlDocPtr doc;
  xmlNodePtr root;
  xmlNodePtr node;
  GList			*list;
  GList			*ids;
  
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
  doc = xmlParseMemory (body, strlen ((char *) body));
  g_free (body);
  
  if (!doc) {
    g_message ("Could not parse file at %s", file);
    return NULL;
  }
  
  xmldirect = g_object_new (SIM_TYPE_XML_DIRECTIVE, NULL);
  xmldirect->_priv->container = container;

  /* parse the file */
  root = xmlDocGetRootElement (doc);				//we need to know the first element in the tree
  if (strcmp ((gchar *) root->name, OBJECT_DIRECTIVES)) 
	{
    g_message ("Invalid XML directive file '%s'", file);
    g_object_unref (G_OBJECT (xmldirect));
    return NULL;
  }

  node = root->xmlChildrenNode;
  while (node) //while 
	{
    if (!strcmp ((gchar *) node->name, OBJECT_DIRECTIVE))		//parse each one of the directives and store it in xmldirect	
      sim_xml_directive_new_directive_from_node (xmldirect, node);

    if (!strcmp ((gchar *) node->name, OBJECT_GROUPS))
      sim_xml_directive_new_groups_from_node (xmldirect, node); // the same with directive groups

    node = node->next;
  }

	//now we have all the directives, and all the groups. But is needed to tell to each directive if it's is inside
	//a group
	list = xmldirect->_priv->groups;
  while (list)
  {
    SimDirectiveGroup *group = (SimDirectiveGroup *) list->data;
    GList *ids = sim_directive_group_get_ids (group);

    while (ids)
		{
		  gint id = GPOINTER_TO_INT (ids->data);
			SimDirective *directive= find_directive (xmldirect, id);

		  if (directive)
		    sim_directive_append_group (directive, group);

			ids = ids->next;
		}

    list = list->next;
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
  gchar			*xml;
  gboolean	result;
  
  g_return_val_if_fail (SIM_IS_XML_DIRECTIVE (xmldirect), FALSE);
  
  xml = sim_xml_directive_to_string (xmldirect);
  if (xml) 
	{
    result = sim_file_save (file, xml, strlen ((char *) xml));
    g_free (xml);
  }
	else
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
  doc = xmlNewDoc ((xmlChar *) "1.0");
  root = xmlNewDocNode (doc, NULL, (xmlChar *) OBJECT_DIRECTIVES, NULL);
  xmlDocSetRootElement (doc, root);
  
  /* save to memory */
  xmlDocDumpMemory (doc, &xml, &size);
  xmlFreeDoc (doc);
  if (!xml) {
    g_message ("Could not dump XML file to memory");
    return NULL;
  }
  
  retval = g_strdup ((gchar *) xml);
  free (xml);
  
  return retval;
}

/*
 *
 * Parameter node is the same that a single directive inside the directives.xml. Its needed to extract
 * all the data from node and insert it into a SimDirective object to be
 * able to return it.
 *
 * http://xmlsoft.org/html/libxml-tree.html#xmlNode
 *
 * Returns NULL on error and don't load the directive at all.
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
  gchar         *value = NULL;
  gint           priority;
  gint           id;

  g_return_val_if_fail (xmldirect != NULL, NULL);
  g_return_val_if_fail (SIM_IS_XML_DIRECTIVE (xmldirect), NULL);
  g_return_val_if_fail (node != NULL, NULL);

  if (strcmp ((gchar *) node->name, (gchar *) OBJECT_DIRECTIVE))
    {
      g_message ("Invalid directive node %s", node->name);
      return NULL;
    }

  id = atoi( (char * ) (xmlGetProp (node, (xmlChar *) PROPERTY_ID))); //get the id of that directive 
  name = g_strdup_printf ("directive_event: %s", (char *) xmlGetProp (node, (xmlChar *) PROPERTY_NAME));
  
	g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Loading directive: %d", id);

  if ((value = (gchar *) xmlGetProp (node, (xmlChar *) PROPERTY_PRIORITY)))
  {
    priority= strtol(value, (char **) NULL, 10);
    xmlFree(value);
  } 

  directive = sim_directive_new ();
  sim_directive_set_id (directive, id);
  sim_directive_set_name (directive, name);
  sim_directive_set_priority (directive, priority);

  children = node->xmlChildrenNode; // xmlChildrenNode is a #define to children. It's the same to do node->children
  while (children)   
	{
    if (!strcmp ((gchar *) children->name, OBJECT_RULE))
    {
			rule_root = sim_xml_directive_new_rule_from_node (xmldirect, children, NULL, 1);//pass all the directive to the
																																											//function and (separate && store) it
																																											//into individual rules
			if (!rule_root)
			{
				g_message ("Error: There are a problem in directive: %d. Aborting load of that directive", id);
				return NULL;
			}
    }
    children = children->next;
  }

  /* The time out of the first rule is set to directive time out 
   * if the rule have occurence > 1, otherwise is set to 0.
   */
  if (rule_root)
  {
    SimRule *rule = (SimRule *) rule_root->data;
    gint time_out = sim_rule_get_time_out (rule);
    gint occurrence = sim_rule_get_occurrence (rule);
    if (occurrence > 1)
			sim_directive_set_time_out (directive, time_out);
    else
			sim_directive_set_time_out (directive, 0);
  }
  sim_directive_set_root_node (directive, rule_root);
  
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
  
  if (strcmp ((gchar *) node->name, (gchar *)  OBJECT_ACTION))
    {
      g_message ("Invalid action node %s", node->name);
      return NULL;
    }

  action = sim_action_new ();

  return action;
}

/*
 * Checks all the plugin_sids from a "rule" statment in a directive, and store it in a list in rule->_priv->plugin_sids
 *
 * Returns FALSE on error
 *
 */
static gboolean
sim_xml_directive_set_rule_plugin_sids (SimXmlDirective  *xmldirect, //FIXME: xmldirect is not used in this function
																				SimRule          *rule,
																				gchar            *value)
{
  gchar     **values;
  gchar     **level;
  gint        i;
	gboolean		pluginsid_neg = FALSE; //if the address is negated, this will be YES (just for that sid, not the others).
  gchar		    *token_value; //this will be each one of the strings, between "," and "," from value.

  g_return_val_if_fail (xmldirect != NULL, FALSE);
  g_return_val_if_fail (SIM_IS_XML_DIRECTIVE (xmldirect), FALSE);
  g_return_val_if_fail (rule != NULL, FALSE);
  g_return_val_if_fail (SIM_IS_RULE (rule), FALSE);
  g_return_val_if_fail (value != NULL, FALSE);

  values = g_strsplit (value, SIM_DELIMITER_LIST, 0);				//separate each of the individual plugin_sid delimited with ","
  for (i = 0; values[i] != NULL; i++)
  {
    if (values[i][0] == '!')													//Each plugin_sid could be negated. We'll store it in other place.
    {
      pluginsid_neg = TRUE;
      token_value = values[i]+1;															//removing the "!"...
    }
    else
    {
      pluginsid_neg = FALSE;
      token_value = values[i];
    }

    if (strstr (token_value, SIM_DELIMITER_LEVEL))            //if this token doesn't refers to the 1st rule level...
		{
		  SimRuleVar *var = g_new0 (SimRuleVar, 1);								//here is stored the level to wich this PLUGIN_SID make 
																															//reference and what kind of token is (src_ia, protocol...)

			level = g_strsplit (token_value, SIM_DELIMITER_LEVEL, 0);	//separate the (ie) 1:PLUGIN_SID into 2 tokens

		  var->type = sim_get_rule_var_from_char (level[1]);			//level[1] = PLUGIN_SID
		  var->attr = SIM_RULE_VAR_PLUGIN_SID;
			if (sim_string_is_number (level[0]))
				var->level = atoi(level[0]);
			else
			{
				g_strfreev (level);
				g_free (var);				
				return FALSE;
			}				
	  
		  sim_rule_append_var (rule, var);												//we don't need to call to sim_rule_append_plugin_sid()
		  g_strfreev (level);																			//because we aren't going to store nothing as we will read
		}																													//the plugin_sid from other level.
    else																							
		if (!strcmp (token_value, SIM_IN_ADDR_ANY_CONST)) 
		{
      if (pluginsid_neg) //we can't negate "ANY" plugin_sid!
      {
        g_strfreev (values);
        return FALSE;
      }
		  sim_rule_append_plugin_sid (rule, 0);
		}
    else																											// this token IS the 1st level
		if (sim_string_is_number (token_value))
		{
      g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "ADDING: %s",token_value);

    	if (pluginsid_neg)
				sim_rule_append_plugin_sid_not (rule, atoi(token_value));
      else
				sim_rule_append_plugin_sid (rule, atoi(token_value));
		}
		else
		{
		  g_strfreev (values);
			return FALSE;
		}
  }
  g_strfreev (values);

	return TRUE;
}

/*
 *
 *
 *
 *
 */
static gboolean
sim_xml_directive_set_rule_src_ips (SimXmlDirective  *xmldirect,	//FIXME: xmldirect is used just to get the container
																    SimRule          *rule,				//so the right way is to pass just the container.
																    gchar            *value)
{
  SimContainer  *container;
  SimNet        *net;
  gchar		      **values;
  gchar			    *token_value; //this will be each one of the strings, between "," and "," from value.
	gboolean			addr_neg;	//if the address is negated, this will be YES (just for that address, not the others).
  gchar				  **level;
  gint          i;

  g_return_val_if_fail (xmldirect != NULL, FALSE);
  g_return_val_if_fail (SIM_IS_XML_DIRECTIVE (xmldirect), FALSE);
  g_return_val_if_fail (rule != NULL, FALSE);
  g_return_val_if_fail (SIM_IS_RULE (rule), FALSE);
  g_return_val_if_fail (value != NULL, FALSE);

  container = xmldirect->_priv->container;

  values = g_strsplit (value, SIM_DELIMITER_LIST, 0);						//split into ","...
  for (i = 0; values[i] != NULL; i++)
  {
    if (values[i][0] == '!')													
		{
			addr_neg = TRUE;
			token_value = values[i]+1;	
		}
		else
		{
			addr_neg = FALSE;
			token_value = values[i];
		}
			
    if (strstr (token_value, SIM_DELIMITER_LEVEL))								//if this isn't the first level...
		{
		  SimRuleVar *var = g_new0 (SimRuleVar, 1);

		  level = g_strsplit (token_value, SIM_DELIMITER_LEVEL, 0);

		  var->type = sim_get_rule_var_from_char (level[1]);
		  var->attr = SIM_RULE_VAR_SRC_IA;
      if (sim_string_is_number (level[0]))
        var->level = atoi(level[0]);
      else
      {
        g_strfreev (level);
        g_free (var);
        return FALSE;
      }
	  
		  sim_rule_append_var (rule, var);

			g_strfreev (level);
		}
    else
		if (!strcmp (token_value, SIM_IN_ADDR_ANY_CONST))
		{
			if (addr_neg)	//we can't negate "ANY" address!
			{
			  g_strfreev (values);
				return FALSE;
			}
		  gchar *ip = g_strdup (SIM_IN_ADDR_ANY_IP_STR);
			sim_rule_append_src_inet (rule, sim_inet_new (ip));			
		  g_free (ip);
		}
    else 
		if (!strcmp (token_value, SIM_HOME_NET_CONST))						//usually, "HOME_NET"
    {
      /* load all nets as source
         Todo: load only those flagged as "internal". 
      */
      GList *nets = sim_container_get_nets(container);
      while (nets)
      {
	      SimNet *net = (SimNet *) nets->data;
		    GList *inets = sim_net_get_inets (net); 						//all the nets in src Policy
			  while(inets)
				{
          SimInet *inet = (SimInet *) inets->data;
					if (addr_neg)	//if the address inside the rule is negated, the we have to add it to the src_inet_not Glist
	          sim_rule_append_src_inet_not (rule, inet);
					else
						sim_rule_append_src_inet (rule, inet);
          inets = inets->next;
        }
        g_list_free(inets);
        nets = nets->next;
      }
      g_list_free(nets);
    }
    else																											// ossim acepts too network names defined in Policy.
		{
		  net = (SimNet *) sim_container_get_net_by_name (container, token_value);
			if (net)
	    {
	      GList *inets = sim_net_get_inets (net);
				if (inets)
		      while (inets)
					{
					  SimInet *inet = (SimInet *) inets->data;
	          if (addr_neg)
  	          sim_rule_append_src_inet_not (rule, inet);
    	      else
      	      sim_rule_append_src_inet (rule, inet);
					  inets = inets->next;
					}
				else
					return FALSE;
			}
		  else																										//and of course, we accept a single network.
	    {
	      GList *inets = sim_get_inets (token_value);
				if (inets)
		      while (inets)
					{
						SimInet *inet = (SimInet *) inets->data;
	          if (addr_neg)
  	          sim_rule_append_src_inet_not (rule, inet);
    	      else
      	      sim_rule_append_src_inet (rule, inet);
					  inets = inets->next;
					}
				else
					return FALSE;
	    }
		}
  }

  g_strfreev (values);
	return TRUE;
}

/*
 *
 *
 *
 *
 */
static gboolean
sim_xml_directive_set_rule_dst_ips (SimXmlDirective  *xmldirect,
																    SimRule          *rule,
																    gchar            *value)
{
  SimContainer  *container;
  SimNet     *net;
  gchar     	**values;
	gchar				*token_value;
	gboolean 		addr_neg;
  gchar     	**level;
  gint        i;

  g_return_val_if_fail (xmldirect != NULL, FALSE);
  g_return_val_if_fail (SIM_IS_XML_DIRECTIVE (xmldirect), FALSE);
  g_return_val_if_fail (rule != NULL, FALSE);
  g_return_val_if_fail (SIM_IS_RULE (rule), FALSE);
  g_return_val_if_fail (value != NULL, FALSE);

  container = xmldirect->_priv->container;

  values = g_strsplit (value, SIM_DELIMITER_LIST, 0);
  for (i = 0; values[i] != NULL; i++)
  {
    if (values[i][0] == '!')													
    {
      addr_neg = TRUE;
      token_value = values[i]+1;
    }
    else
    {
      addr_neg = FALSE;
      token_value = values[i];
    }

    if (strstr (token_value, SIM_DELIMITER_LEVEL))
		{
		  SimRuleVar *var = g_new0 (SimRuleVar, 1);

	  	level = g_strsplit (token_value, SIM_DELIMITER_LEVEL, 0);

		  var->type = sim_get_rule_var_from_char (level[1]);
		  var->attr = SIM_RULE_VAR_DST_IA;
      if (sim_string_is_number (level[0]))
        var->level = atoi(level[0]);
      else
      {
        g_strfreev (level);
        g_free (var);
        return FALSE;
      }
	  
		  sim_rule_append_var (rule, var);
	  	g_strfreev (level);
		}
    else
		if (!strcmp (token_value, SIM_IN_ADDR_ANY_CONST))
		{
			if (addr_neg)	//we can't negate "ANY" address!
			{
			  g_strfreev (values);
				return FALSE;
			}
		  gchar *ip = g_strdup (SIM_IN_ADDR_ANY_IP_STR);
      sim_rule_append_dst_inet (rule, sim_inet_new (ip));
		  g_free (ip);
		}
    else
		if (!strcmp (token_value, SIM_HOME_NET_CONST))
    {
      /* load all nets as destination
         Todo: load only those flagged as "internal". 
      */
      GList *nets = sim_container_get_nets(container);
      while(nets)
      {
    	  SimNet *net = (SimNet *) nets->data;
        GList *inets = sim_net_get_inets (net);
        while(inets)
				{
          SimInet *inet = (SimInet *) inets->data;
          if (addr_neg)
            sim_rule_append_dst_inet_not (rule, inet);
          else
            sim_rule_append_dst_inet (rule, inet);
          inets = inets->next;
        }
        g_list_free(inets);
        nets = nets->next;
      }
      g_list_free(nets);
    }
    else
		{
	  	net = (SimNet *) sim_container_get_net_by_name (container, token_value);
		  if (net)
	    {
	      GList *inets = sim_net_get_inets (net);
				if (inets)
		      while (inets)
					{
			  		SimInet *inet = (SimInet *) inets->data;
	          if (addr_neg)
  	          sim_rule_append_dst_inet_not (rule, inet);
    	      else
      	      sim_rule_append_dst_inet (rule, inet);
				  	inets = inets->next;
					}
				else
					return FALSE;
	    }
	  	else
	    {
	      GList *inets = sim_get_inets (token_value);
				if (inets)
		      while (inets)
					{
					  SimInet *inet = (SimInet *) inets->data;
	          if (addr_neg)
  	          sim_rule_append_dst_inet_not (rule, inet);
    	      else
      	      sim_rule_append_dst_inet (rule, inet);
		  		  inets = inets->next;
					}
				else
					return FALSE;
	    }
		}
  }
  g_strfreev (values);
	return TRUE;
}

/*
 *
 *
 *
 *
 */
static gboolean
sim_xml_directive_set_rule_src_ports (SimXmlDirective  *xmldirect,
																      SimRule          *rule,
																      gchar            *value)
{
  SimContainer  *container;
  SimNet     *net;
  GList      *hosts;
  gchar     **values;
  gchar     **level;
  gchar     **range;
  gchar      *host;
  gint        i;
	gchar 		*token_value;
	gboolean	port_neg;

  g_return_val_if_fail (xmldirect != NULL, FALSE);
  g_return_val_if_fail (SIM_IS_XML_DIRECTIVE (xmldirect), FALSE);
  g_return_val_if_fail (rule != NULL, FALSE);
  g_return_val_if_fail (SIM_IS_RULE (rule), FALSE);
  g_return_val_if_fail (value != NULL, FALSE);

  container = xmldirect->_priv->container;

  values = g_strsplit (value, SIM_DELIMITER_LIST, 0);
  for (i = 0; values[i] != NULL; i++)
  {
    if (values[i][0] == '!')													
    {
      port_neg = TRUE;
      token_value = values[i]+1;
    }
    else
    {
      port_neg = FALSE;
      token_value = values[i];
    }

    if (strstr (token_value, SIM_DELIMITER_LEVEL))
		{
		  SimRuleVar *var = g_new0 (SimRuleVar, 1);
		  level = g_strsplit (token_value, SIM_DELIMITER_LEVEL, 0);

		  var->type = sim_get_rule_var_from_char (level[1]);
	  	var->attr = SIM_RULE_VAR_SRC_PORT;
      if (sim_string_is_number (level[0]))
        var->level = atoi(level[0]);
      else
      {
        g_strfreev (level);
        g_strfreev (values);
        g_free (var);
        return FALSE;
      }

		  sim_rule_append_var (rule, var);

	  	g_strfreev (level);
		}
    else
		if (!strcmp (token_value,  SIM_IN_ADDR_ANY_CONST))
		{
      if (port_neg) //we can't negate "ANY" port!
      {
        g_strfreev (values);
        return FALSE;
      }
		  sim_rule_append_src_port (rule, 0);
		}
    else
		if (strstr (token_value, SIM_DELIMITER_RANGE))							//multiple ports in a range. "1-5"
    {
      gint start, end, j = 0;

      range = g_strsplit (token_value, SIM_DELIMITER_RANGE, 0);

			if (!sim_string_is_number (range[0]) || !sim_string_is_number (range[1]))
			{
				g_strfreev (range);
				g_strfreev (values);
				return FALSE;
			}

      start = atoi(range[0]);
      end   = atoi(range[1]);

      for(j=start;j<=end;j++)
			{
				if (port_neg)			//if ports are !1-5, all the ports in that range will be negated.
				  sim_rule_append_src_port_not (rule, j);
				else
				  sim_rule_append_src_port (rule, j);
      }
      g_strfreev (range); 
    }
    else																									//just one port
		{
      if (sim_string_is_number (token_value))
			{
				if (port_neg)			
				  sim_rule_append_src_port_not (rule, atoi (token_value));
				else
				  sim_rule_append_src_port (rule, atoi (token_value));
			}
      else
			{
				g_strfreev (values);
        return FALSE;
			}
		}
  }
  g_strfreev (values);
	return TRUE;
}

/*
 *
 *
 *
 *
 */
static gboolean
sim_xml_directive_set_rule_dst_ports (SimXmlDirective  *xmldirect,
																      SimRule          *rule,
																      gchar            *value)
{
  SimContainer  *container;
  SimNet     *net;
  GList      *hosts;
  gchar     **values;
  gchar     **level;
  gchar     **range;
  gchar      *host;
  gint        i;
	gchar 		*token_value;
	gboolean	port_neg;

  g_return_val_if_fail (xmldirect != NULL, FALSE);
  g_return_val_if_fail (SIM_IS_XML_DIRECTIVE (xmldirect), FALSE);
  g_return_val_if_fail (rule != NULL, FALSE);
  g_return_val_if_fail (SIM_IS_RULE (rule), FALSE);
  g_return_val_if_fail (value != NULL, FALSE);

  container = xmldirect->_priv->container;

  values = g_strsplit (value, SIM_DELIMITER_LIST, 0);
  for (i = 0; values[i] != NULL; i++)
  {
    if (values[i][0] == '!')													
    {
      port_neg = TRUE;
      token_value = values[i]+1;
    }
    else
    {
      port_neg = FALSE;
      token_value = values[i];
    }

    if (strstr (token_value, SIM_DELIMITER_LEVEL))
		{
		  SimRuleVar *var = g_new0 (SimRuleVar, 1);

	  	level = g_strsplit (token_value, SIM_DELIMITER_LEVEL, 0);

		  var->type = sim_get_rule_var_from_char (level[1]);
		  var->attr = SIM_RULE_VAR_DST_PORT;
      if (sim_string_is_number (level[0]))
        var->level = atoi(level[0]);
      else
      {
        g_strfreev (level);
        g_free (var);
        g_strfreev (values);
        return FALSE;
      }
	  
		  sim_rule_append_var (rule, var);

		  g_strfreev (level);
		}
    else
		if (!strcmp (token_value, SIM_IN_ADDR_ANY_CONST))
		{
      if (port_neg) //we can't negate "ANY" port!
      {
        g_strfreev (values);
        return FALSE;
      }
      sim_rule_append_dst_port (rule, 0);
		}
    else
		if (strstr (token_value, SIM_DELIMITER_RANGE))
    {
      gint start, end, j = 0;

      range = g_strsplit (token_value, SIM_DELIMITER_RANGE, 0);
      if (!sim_string_is_number (range[0]) || !sim_string_is_number (range[1]))
      {
        g_strfreev (range);
        g_strfreev (values);
        return FALSE;
      }

      start = atoi(range[0]);
      end   = atoi(range[1]);

      for(j=start;j<=end;j++)
			{
        if (port_neg)     //if ports are ie. !1-5, all the ports in that range will be negated.
          sim_rule_append_dst_port_not (rule, j);
        else
          sim_rule_append_dst_port (rule, j);
      }
      g_strfreev (range); 
    }
    else
		{
			if (sim_string_is_number (token_value))
			{
        if (port_neg)
          sim_rule_append_dst_port_not (rule, atoi (token_value));
        else
          sim_rule_append_dst_port (rule, atoi (token_value));
			}
			else
			{
  			g_strfreev (values);
				return FALSE;
			}
		}
  }
  g_strfreev (values);
	return TRUE;
}

/*
 *
 *
 *
 *
 */
static gboolean
sim_xml_directive_set_rule_protocol (SimXmlDirective  *xmldirect,		//FIXME: xmldirect not needed here
																     SimRule          *rule,
																     gchar            *value)
{
  gchar     **values;
  gchar     **level;
  gint        i;
	gchar 		*token_value;
	gboolean	proto_neg;

  g_return_val_if_fail (xmldirect != NULL, FALSE);
  g_return_val_if_fail (SIM_IS_XML_DIRECTIVE (xmldirect), FALSE);
  g_return_val_if_fail (rule != NULL, FALSE);
  g_return_val_if_fail (SIM_IS_RULE (rule), FALSE);
  g_return_val_if_fail (value != NULL, FALSE);

  values = g_strsplit (value, SIM_DELIMITER_LIST, 0);
  for (i = 0; values[i] != NULL; i++)
  {
    if (values[i][0] == '!')													
    {
      proto_neg = TRUE;
      token_value = values[i]+1;
    }
    else
    {
      proto_neg = FALSE;
      token_value = values[i];
    }

    if (strstr (token_value, SIM_DELIMITER_LEVEL))
		{
		  SimRuleVar *var = g_new0 (SimRuleVar, 1);

		  level = g_strsplit (token_value, SIM_DELIMITER_LEVEL, 0);

		  var->type = sim_get_rule_var_from_char (level[1]);
		  var->attr = SIM_RULE_VAR_PROTOCOL;
      if (sim_string_is_number (level[0]))
        var->level = atoi(level[0]);
      else
      {
        g_strfreev (level);
			  g_strfreev (values);
        g_free (var);
        return FALSE;
      }
	  
		  sim_rule_append_var (rule, var);

	  	g_strfreev (level);
		}
    else
		if (!strcmp (token_value, SIM_IN_ADDR_ANY_CONST)) 
		{
    	if (proto_neg) //we can't negate "ANY" protocol
      {
        g_strfreev (values);
        return FALSE;
      }
      sim_rule_append_protocol (rule, 0);
		}
    else
		{
      if (sim_string_is_number (token_value))
      {
				if (proto_neg)
					sim_rule_append_protocol_not (rule, atoi (token_value));
				else
					sim_rule_append_protocol (rule, atoi (token_value));
			}
      else
      {
				int proto = sim_protocol_get_type_from_str (token_value);
				if (proto  != SIM_PROTOCOL_TYPE_NONE)
				{
					if (proto_neg)
			      sim_rule_append_protocol_not (rule, proto );
					else
						sim_rule_append_protocol (rule, proto);
				}
				else
				{
				  g_strfreev (values);
					return FALSE;
				}
      }
		}
  }
  g_strfreev (values);
	return TRUE;
}

/*
 *
 *
 *
 *
 */
static gboolean
sim_xml_directive_set_rule_sensors (SimContainer	  *container,	
																    SimRule          *rule,				
																    gchar            *value)
{
  SimNet        *net;
  SimSensor     *sensor;
  gchar        **values;
  gchar        **level;
  gint           i;
  gchar     *token_value;
  gboolean  sensor_neg;

  g_return_val_if_fail (container != NULL, FALSE);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), FALSE);
  g_return_val_if_fail (rule != NULL, FALSE);
  g_return_val_if_fail (SIM_IS_RULE (rule), FALSE);
  g_return_val_if_fail (value != NULL, FALSE);

  values = g_strsplit (value, SIM_DELIMITER_LIST, 0);						//split into ","...
  for (i = 0; values[i] != NULL; i++)
  {
    if (values[i][0] == '!')													
    {
      sensor_neg = TRUE;
      token_value = values[i]+1;
    }
    else
    {
      sensor_neg = FALSE;
      token_value = values[i];
    }

    if (strstr (token_value, SIM_DELIMITER_LEVEL))								//if this isn't the first level...
		{
		  SimRuleVar *var = g_new0 (SimRuleVar, 1);

		  level = g_strsplit (token_value, SIM_DELIMITER_LEVEL, 0);

		  var->type = sim_get_rule_var_from_char (level[1]);
		  var->attr = SIM_RULE_VAR_SENSOR;
      if (sim_string_is_number (level[0]))
        var->level = atoi(level[0]);
      else
      {
        g_strfreev (level);
  			g_strfreev (values);
        g_free (var);
        return FALSE;
      }
	  
		  sim_rule_append_var (rule, var);

			g_strfreev (level);
		}
    else
		if (!strcmp (token_value, SIM_IN_ADDR_ANY_CONST))
		{
      if (sensor_neg) //we can't negate "ANY" sensor
      {
        g_strfreev (values);
        return FALSE;
      }
		  gchar *ip = g_strdup (SIM_IN_ADDR_ANY_IP_STR);
			sim_rule_append_sensor (rule, sim_sensor_new_from_hostname (ip));
		  g_free (ip);
		}
    else																											// ossim accepts too sensor names defined in policy
		{
		  sensor = (SimSensor *) sim_container_get_sensor_by_name (container, token_value);
			if (sensor)
	    {
				if (sensor_neg)
					sim_rule_append_sensor_not (rule, sensor);
				else
					sim_rule_append_sensor (rule, sensor);
			}
		  else																										//and of course, we accept a single sensor
	    {
				sensor = sim_sensor_new_from_hostname (token_value);
				if (sensor)
				{
	        if (sensor_neg)
  	        sim_rule_append_sensor_not (rule, sensor);
    	    else
      	    sim_rule_append_sensor (rule, sensor);
				}
				else
				{
  				g_strfreev (values);
					return FALSE;					
				}
	    }
		}
  }

  g_strfreev (values);
	return TRUE;
}


/*
 * Create a GNode element
 *
 * Returns NULL on error.
 *
 * GNode *root: first  time this should be called with NULL. after that, recursively 
 * it will pass the pointer to the node.
 */
GNode*
sim_xml_directive_new_rule_from_node (SimXmlDirective  *xmldirect,
																      xmlNodePtr        node,
																      GNode            *root,
																      gint              level)
{
  SimRuleType    type = SIM_RULE_TYPE_NONE;
  SimRule       *rule;
  SimAction     *action;
  GNode         *rule_node;
  GNode         *rule_child;
  xmlNodePtr     children;
  xmlNodePtr     children_rules;
  xmlNodePtr     actions;
  gchar         *value = NULL;
  gchar         *name = NULL;
  SimConditionType   condition = SIM_CONDITION_TYPE_NONE;
  gchar         *par_value = NULL;
  gint           interval = 0;
  gboolean       absolute = FALSE;
  gboolean       sticky = FALSE;
  gint           sticky_different = SIM_RULE_VAR_NONE;
  gboolean       not = FALSE;
  gint           priority = 1;
  gint           reliability = 1;
  gboolean       rel_abs = TRUE;
  gint           time_out = 0;
  gint           occurrence = 1;
  gint           plugin = 0;
  gint           tplugin = 0;

  g_return_val_if_fail (xmldirect != NULL, NULL);
  g_return_val_if_fail (SIM_IS_XML_DIRECTIVE (xmldirect), NULL);
  g_return_val_if_fail (node != NULL, NULL);

  if (strcmp ((gchar *) node->name, OBJECT_RULE)) //This must be a "rule" always.
  {
    g_message ("Invalid rule node %s", node->name);
    return NULL;
  }
 
	//now we're going to extract all the data from the node and store it into variables so we can later return it.
	//This node can be all the rules inside directive (the first time it enters in this function) or just an 
	//internal "rule" thanks to the recursive programming
  if ((value = (gchar *) xmlGetProp (node, (xmlChar *) PROPERTY_TYPE)))
  {
    if (!g_ascii_strcasecmp (value, "detector"))
			type = SIM_RULE_TYPE_DETECTOR;
    else 
		if (!g_ascii_strcasecmp (value, "monitor"))
			type = SIM_RULE_TYPE_MONITOR;
    else
			type = SIM_RULE_TYPE_NONE;

      xmlFree(value);
  }
	
  if ((value = (gchar *) xmlGetProp (node, (xmlChar *) PROPERTY_STICKY)))
  {
    if (!g_ascii_strcasecmp (value, "TRUE"))
			sticky = TRUE;
    xmlFree(value);
  } 
  if ((value = (gchar *) xmlGetProp (node, (xmlChar *) PROPERTY_STICKY_DIFFERENT)))
  {
    sticky_different = sim_get_rule_var_from_char (value);
    xmlFree(value);
  } 
  if ((value = (gchar *) xmlGetProp (node, (xmlChar *) PROPERTY_NOT)))
  {
    if (!g_ascii_strcasecmp (value, "TRUE"))
			not = TRUE;
    xmlFree(value);
  } 
  if ((value = (gchar *) xmlGetProp (node, (xmlChar *) PROPERTY_NAME)))
  { 
    name = g_strdup (value);
    xmlFree(value);
  }
  if ((value = (gchar *) xmlGetProp (node, (xmlChar *) PROPERTY_PRIORITY)))
  {
 		if (sim_string_is_number (value))
	    priority= strtol(value, (char **) NULL, 10);
		else
		{
    	xmlFree(value);
			g_message("Error. there are a problem in the Priority field");
			return NULL;
		}
    xmlFree(value);
  } 
		
	if ((value = (gchar *) xmlGetProp (node, (xmlChar *) PROPERTY_RELIABILITY)))
  {
		gboolean aux=TRUE;
		gchar *tempi = value;	//we don't wan't to loose the pointer.....	
		if (value[0] == '+')
		{
			rel_abs = FALSE;
			value++;		// ++ to the pointer so now "value" points to the number string and we can check it.
   		if (sim_string_is_number (value))
	 	  	reliability = atoi(value);
			else
				aux=FALSE;			
		}
		else
		{
   		if (sim_string_is_number (value))
      	reliability = atoi(value);
			else
				aux=FALSE;
		}
		value = tempi;
		xmlFree(value);
		if (aux == FALSE)
		{
			g_message("Error. there are a problem in the Reliability field");
			return NULL;
		}
	}

	if ((value = (gchar *) xmlGetProp (node, (xmlChar *) PROPERTY_CONDITION)))
  { 
    condition = sim_condition_get_type_from_str (value);
    xmlFree(value);
  }
  if ((value = (gchar *) xmlGetProp (node, (xmlChar *) PROPERTY_VALUE)))
  { 
    par_value = g_strdup (value);
    xmlFree(value);
  } 
	if ((value = (gchar *) xmlGetProp (node, (xmlChar *) PROPERTY_INTERVAL)))
  {
    if (sim_string_is_number (value))
		{
	    interval = strtol(value, (char **) NULL, 10);
	    xmlFree(value);
		}
		else
		{
	    xmlFree(value);
			g_message("Error. there are a problem in the Interval field");
			return NULL;
		}
  }
  if ((value = (gchar *) xmlGetProp (node, (xmlChar *) PROPERTY_ABSOLUTE)))
  {
    if (!g_ascii_strcasecmp (value, "TRUE"))
			absolute = TRUE;
    xmlFree(value);
  } 
  if ((value = (gchar *) xmlGetProp (node, (xmlChar *) PROPERTY_TIME_OUT)))
  {
    if (sim_string_is_number (value))
    {
      time_out = strtol(value, (char **) NULL, 10);
      xmlFree(value);
    }
    else
    {
      xmlFree(value);
			g_message("Error. there are a problem in the 'Absolute' field");
      return NULL;
    }
  }
	
  if ((value = (gchar *) xmlGetProp (node, (xmlChar *) PROPERTY_OCCURRENCE)))
  {
    if (sim_string_is_number (value))
    {
      occurrence = strtol(value, (char **) NULL, 10);
      xmlFree(value);
    }
    else
    {
      xmlFree(value);
			g_message("Error. there are a problem in the Occurrence field");
      return NULL;
    }
  }
	
  if ((value = (gchar *) xmlGetProp (node, (xmlChar *) PROPERTY_PLUGIN_ID)))
  {
    if (sim_string_is_number (value))
    {
      plugin = strtol(value, (char **) NULL, 10);
      xmlFree(value);
    }
    else
    {
		  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Error: plugin-id: %s",value);
			xmlFree(value);
			g_message("Error. there are a problem in the Plugin_id field");
      return NULL;
    }
  }

  rule = sim_rule_new ();
  rule->type = type;
  if (sticky) 
		sim_rule_set_sticky (rule, sticky);
  if (sticky_different) 
		sim_rule_set_sticky_different (rule, sticky_different);
  if (not) 
		sim_rule_set_not (rule, not);
  sim_rule_set_level (rule, level);
  sim_rule_set_name (rule, name);
  sim_rule_set_priority (rule, priority);
  sim_rule_set_reliability (rule, reliability);
  sim_rule_set_rel_abs (rule, rel_abs);
  sim_rule_set_condition (rule, condition);
  if (par_value) 
		sim_rule_set_value (rule, par_value);
  if (interval > 0) 
		sim_rule_set_interval (rule, interval);
  if (absolute) 
		sim_rule_set_absolute (rule, absolute);
  sim_rule_set_time_out (rule, time_out);
  sim_rule_set_occurrence (rule, occurrence);
  sim_rule_set_plugin_id (rule, plugin);

	//at this moment, "rule" variable has some properties, and we continue filling it.
	//Now, we have to fill the properties that can handle multiple variables, like sids, or src_ips ie.
	if ((value = (gchar *) xmlGetProp (node, (xmlChar *) PROPERTY_PLUGIN_SID)))
  {
    if (sim_xml_directive_set_rule_plugin_sids (xmldirect, rule, value)) //FIXME: xmldirect is not needed in the function
			xmlFree(value);
		else
		{
			g_message("Error. there are a problem in the Plugin_sid field");
			return NULL;			
		}
  }
  if ((value = (gchar *) xmlGetProp (node, (xmlChar *) PROPERTY_SRC_IP)))
  {
    if (sim_xml_directive_set_rule_src_ips (xmldirect, rule, value))
	    xmlFree(value);
		else
		{
			g_message("Error. there are a problem in the src_ip field");
			return NULL;			
		}
  }
  if ((value = (gchar *) xmlGetProp (node, (xmlChar *) PROPERTY_DST_IP)))
  {
    if (sim_xml_directive_set_rule_dst_ips (xmldirect, rule, value))
			xmlFree(value);
		else
		{
			g_message("Error. there are a problem in the dst_ip field");
			return NULL;			
		}
  }
  if ((value = (gchar *) xmlGetProp (node, (xmlChar *) PROPERTY_SRC_PORT)))
  {
			g_message("VALUE SRC_PORTS: %s",value);
    if (sim_xml_directive_set_rule_src_ports (xmldirect, rule, value))
		  xmlFree(value);
		else
		{
			g_message("Error. there are a problem in the src_port field");
			return NULL;		
		}
  }

  if ((value = (gchar *) xmlGetProp (node, (xmlChar *) PROPERTY_DST_PORT)))
  {
			g_message("VALUE DST_PORTS: %s",value);
    if (sim_xml_directive_set_rule_dst_ports (xmldirect, rule, value))
	    xmlFree(value);		
		else
		{
			g_message("Error. there are a problem in the dst_port field");
		  return NULL;				
		}
  }
			g_message("---------------------");
  sim_rule_print(rule);
			g_message("---------------------");

  if ((value = (gchar *) xmlGetProp (node, (xmlChar *) PROPERTY_PROTOCOL)))
  {
    if (sim_xml_directive_set_rule_protocol (xmldirect, rule, value))
	    xmlFree(value);
    else
		{
			g_message("Error. there are a problem in the Protocol field");
		  return NULL;		
		}
  }

	if ((value = (gchar *) xmlGetProp (node, (xmlChar *) PROPERTY_SENSOR)))
  {
    if (sim_xml_directive_set_rule_sensors (xmldirect->_priv->container, rule, value))
      xmlFree(value);
    else
		{
			g_message("Error. there are a problem in the Sensor field");
      return NULL;
		}
  }
	
  if (!root)												//ok, this is the first  rule, the root node...
    rule_node = g_node_new (rule);	//..so we have to create the first GNode. 
  else
    rule_node = g_node_append_data (root, rule);	//if it's a child node, we append it to the root.

  children = node->xmlChildrenNode;
  while (children) 									//if the node has more nodes (rules), we have to do the same than this function again
  {																	//so we can call this recursively.
    /* Gets Rules Node */
    if (!strcmp ((gchar *) children->name, OBJECT_RULES))
		{
		  children_rules = children->xmlChildrenNode;
			while (children_rules)
	    {
	      /* Recursive call */
	      if (!strcmp ((gchar *) children_rules->name, OBJECT_RULE)) 
				{
					sim_xml_directive_new_rule_from_node (xmldirect, children_rules, rule_node, level + 1); 
	      }

	      children_rules = children_rules->next;
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


/*
 *
 *
 *
 *
 */
void
sim_xml_directive_new_append_directive_from_node (SimXmlDirective	*xmldirect,
																								  xmlNodePtr		node,
																								  SimDirectiveGroup	*group)
{
  xmlChar	*value;
  gint		id;

  if (strcmp ((gchar *) node->name, OBJECT_APPEND_DIRECTIVE))
  {
    g_message ("Invalid append directive node %s", node->name);
		return;
  }
  if ((value = xmlGetProp (node, (xmlChar *) PROPERTY_DIRECTIVE_ID)))
  {
		if (sim_string_is_number ((gchar *)value))
		{
	    id = strtol((gchar *) value, (char **) NULL, 10);
		  sim_directive_group_append_id (group, id);
		}
		else
			g_message("There are an error in directive groups. The directives ID may be wrong");
		xmlFree(value);
  }
}


/*
 *
 *
 *
 *
 */
gboolean
sim_xml_directive_new_group_from_node (SimXmlDirective	*xmldirect,
																       xmlNodePtr	node)
{
  SimDirectiveGroup	*group;
  xmlNodePtr		children;
  xmlChar		*value;

  if (strcmp ((gchar *) node->name, OBJECT_GROUP))
  {
    g_message ("Invalid group node %s", node->name);
		return FALSE;
  }

  group = sim_directive_group_new ();

  if ((value = xmlGetProp (node, (xmlChar *) PROPERTY_NAME)))
  {
    gchar *name = g_strdup ((gchar *) value);
    sim_directive_group_set_name (group, name);
    xmlFree(value);
  } 

  if ((value = xmlGetProp (node, (xmlChar *) PROPERTY_STICKY)))
  {
    if (!g_ascii_strcasecmp ((gchar *) value, "true"))
			sim_directive_group_set_sticky (group, TRUE);
    else
			sim_directive_group_set_sticky (group, FALSE);
    xmlFree(value);
  }

  children = node->xmlChildrenNode;
  while (children)
  {
    if (!strcmp ((gchar *) children->name, OBJECT_APPEND_DIRECTIVE))
			sim_xml_directive_new_append_directive_from_node (xmldirect, children, group);
  
    children = children->next;
  }

  xmldirect->_priv->groups = g_list_append (xmldirect->_priv->groups, group);
}


/*
 *
 *
 *
 */
gboolean 
sim_xml_directive_new_groups_from_node (SimXmlDirective	*xmldirect,
																				xmlNodePtr	node)
{
  xmlNodePtr  children;

  g_return_if_fail (xmldirect);
  g_return_if_fail (SIM_IS_XML_DIRECTIVE (xmldirect));

  if (strcmp ((gchar *) node->name, OBJECT_GROUPS))
  {
    g_message ("Invalid groups node %s", node->name);
		return FALSE;
  }

  children = node->xmlChildrenNode;
  while (children)
  {
    if (!strcmp ((gchar *) children->name, OBJECT_GROUP))
			sim_xml_directive_new_group_from_node (xmldirect, children);
     
    children = children->next;
  }

}
// vim: set tabstop=2:
