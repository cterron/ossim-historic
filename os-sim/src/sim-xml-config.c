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
#include <sim-xml-config.h>

struct _SimXmlConfigPrivate {
  SimConfig     *config;
};

#define OBJECT_CONFIG           "config"
#define OBJECT_LOG              "log"
#define OBJECT_SENSOR           "sensor"
#define OBJECT_DATASOURCES      "datasources"
#define OBJECT_DATASOURCE       "datasource"
#define OBJECT_DIRECTIVE        "directive"
#define OBJECT_SCHEDULER        "scheduler"
#define OBJECT_SERVER           "server"
#define OBJECT_RSERVERS         "rservers"
#define OBJECT_RSERVER          "rserver"
#define OBJECT_NOTIFIES         "notifies"
#define OBJECT_NOTIFY           "notify"
#define OBJECT_SMTP             "smtp"

#define PROPERTY_NAME           "name"
#define PROPERTY_IP             "ip"
#define PROPERTY_INTERFACE      "interface"
#define PROPERTY_FILENAME       "filename"
#define PROPERTY_PROVIDER       "provider"
#define PROPERTY_DSN            "dsn"
#define PROPERTY_INTERVAL       "interval"
#define PROPERTY_PORT           "port"
#define PROPERTY_EMAILS         "emails"
#define PROPERTY_ALARM_RISKS    "alarm_risks"
#define PROPERTY_HOST           "host"
#define PROPERTY_PROGRAM        "program"
#define PROPERTY_RESEND        "resend"


static void sim_xml_config_class_init (SimXmlConfigClass *klass);
static void sim_xml_config_init       (SimXmlConfig *xmlconfig, SimXmlConfigClass *klass);
static void sim_xml_config_finalize   (GObject *object);

/*
 * SimXmlConfig object signals
 */
enum {
  SIM_XML_CONFIG_CHANGED,
  SIM_XML_CONFIG_LAST_SIGNAL
};

static gint xmlconfig_signals[SIM_XML_CONFIG_LAST_SIGNAL] = { 0, };
static GObjectClass *parent_class = NULL;

/*
 * SimXmlConfig class interface
 */

static void
sim_xml_config_class_init (SimXmlConfigClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  
  parent_class = g_type_class_peek_parent (klass);
  
  xmlconfig_signals[SIM_XML_CONFIG_CHANGED] =
    g_signal_new ("changed",
		  G_TYPE_FROM_CLASS (object_class),
		  G_SIGNAL_RUN_LAST,
		  G_STRUCT_OFFSET (SimXmlConfigClass, changed),
		  NULL, NULL,
		  g_cclosure_marshal_VOID__VOID,
		  G_TYPE_NONE, 0);
  
  object_class->finalize = sim_xml_config_finalize;
  klass->changed = NULL;
}

static void
sim_xml_config_init (SimXmlConfig *xmlconfig, SimXmlConfigClass *klass)
{
  g_return_if_fail (SIM_IS_XML_CONFIG (xmlconfig));
  
  /* allocate private structure */
  xmlconfig->_priv = g_new0 (SimXmlConfigPrivate, 1);
}

static void
sim_xml_config_finalize (GObject *object)
{
  SimXmlConfig *xmlconfig = (SimXmlConfig *) object;
  
  g_free (xmlconfig->_priv);

  /* chain to parent class */
  parent_class->finalize (object);
}

GType
sim_xml_config_get_type (void)
{
  static GType type = 0;
  
  if (!type) {
    static const GTypeInfo info = {
      sizeof (SimXmlConfigClass),
      (GBaseInitFunc) NULL,
      (GBaseFinalizeFunc) NULL,
      (GClassInitFunc) sim_xml_config_class_init,
      NULL,
      NULL,
      sizeof (SimXmlConfig),
      0,
      (GInstanceInitFunc) sim_xml_config_init
    };
    type = g_type_register_static (G_TYPE_OBJECT, "SimXmlConfig", &info, 0);
  }
  return type;
}

/**
 * sim_xml_config_new
 *
 * Creates a new #SimXmlConfig object, which can be used to describe
 * a config which will then be loaded by a provider to create its
 * defined structure
 */
SimXmlConfig *
sim_xml_config_new (void)
{
  SimXmlConfig *xmlconfig;
  
  xmlconfig = g_object_new (SIM_TYPE_XML_CONFIG, NULL);
  return xmlconfig;
}

/**
 * sim_xml_config_new_from_file
 */
SimXmlConfig*
sim_xml_config_new_from_file (const gchar *file)
{
  SimXmlConfig *xmlconfig;
  gchar *body;
  xmlDocPtr doc;
  xmlNodePtr root;
  xmlNodePtr node;
  
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

  xmlconfig = g_object_new (SIM_TYPE_XML_CONFIG, NULL);

  /* parse the file */
  root = xmlDocGetRootElement (doc);
  if (strcmp (root->name, OBJECT_CONFIG)) {
    g_message ("Invalid XML config file '%s'", file);
    g_object_unref (G_OBJECT (xmlconfig));
    return NULL;
  }

  xmlconfig->_priv->config = sim_xml_config_new_config_from_node (xmlconfig, root);

  return xmlconfig;
}


/**
 * sim_xml_config_changed
 * @xmlconfig: XML config
 *
 * Emit the "changed" signal for the given XML config
 */
void
sim_xml_config_changed (SimXmlConfig * xmlconfig)
{
  g_return_if_fail (SIM_IS_XML_CONFIG (xmlconfig));
  g_signal_emit (G_OBJECT (xmlconfig),
		 xmlconfig_signals[SIM_XML_CONFIG_CHANGED],
		 0);
}

/**
 * sim_xml_config_reload
 * @xmlconfig: XML config.
 *
 * Reload the given XML config from its original place, discarding
 * all changes that may have happened.
 */
void
sim_xml_config_reload (SimXmlConfig *xmlconfig)
{
  /* FIXME: implement */
}

/**
 * sim_xml_config_save
 * @xmlconfig: XML config.
 * @file: FILE to save the XML config to.
 *
 * Save the given XML config to disk.
 */
gboolean
sim_xml_config_save (SimXmlConfig *xmlconfig, const gchar *file)
{
  gchar*xml;
  gboolean result;
  
  g_return_val_if_fail (SIM_IS_XML_CONFIG (xmlconfig), FALSE);
  
  xml = sim_xml_config_to_string (xmlconfig);
  if (xml) {
    result = sim_file_save (file, xml, strlen (xml));
    g_free (xml);
  } else
    result = FALSE;

  return result;
}

/**
 * sim_xml_config_to_string
 * @xmlconfig: a #SimXmlConfig object.
 *
 * Get the given XML config contents as a XML string.
 *
 * Returns: the XML string representing the structure and contents of the
 * given #SimXmlConfig object. The returned value must be freed when no
 * longer needed.
 */
gchar *
sim_xml_config_to_string (SimXmlConfig *xmlconfig)
{
  xmlDocPtr doc;
  xmlNodePtr root;
  xmlNodePtr tables_node = NULL;
  GList *list, *l;
  xmlChar *xml;
  gint size;
  gchar *retval;
  
  g_return_val_if_fail (SIM_IS_XML_CONFIG (xmlconfig), NULL);
  
  /* create the top node */
  doc = xmlNewDoc ("1.0");
  root = xmlNewDocNode (doc, NULL, OBJECT_CONFIG, NULL);
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
void
sim_xml_config_set_config_log (SimXmlConfig  *xmlconfig,
			       SimConfig     *config,
			       xmlNodePtr     node)
{
  gchar  *value;

  g_return_if_fail (xmlconfig);
  g_return_if_fail (SIM_IS_XML_CONFIG (xmlconfig));
  g_return_if_fail (config);
  g_return_if_fail (SIM_IS_CONFIG (config));
  g_return_if_fail (node);

  if (strcmp (node->name, OBJECT_LOG))
    {
      g_message ("Invalid config log node %s", node->name);
      return;
    }

  if ((value = xmlGetProp (node, PROPERTY_FILENAME)))
    {
      config->log.filename = g_strdup (value);
      xmlFree(value);      
    }
}

/*
 *
 *
 *
 *
 */
void
sim_xml_config_set_config_sensor (SimXmlConfig  *xmlconfig,
				  SimConfig     *config,
				  xmlNodePtr     node)
{
  gchar  *value;
  
  g_return_if_fail (xmlconfig);
  g_return_if_fail (SIM_IS_XML_CONFIG (xmlconfig));
  g_return_if_fail (config);
  g_return_if_fail (SIM_IS_CONFIG (config));
  g_return_if_fail (node);

  if (strcmp (node->name, OBJECT_SENSOR))
    {
      g_message ("Invalid sensor log node %s", node->name);
      return;
    }

  if ((value = xmlGetProp (node, PROPERTY_NAME)))
    {
      config->sensor.name = g_strdup (value);
      xmlFree(value);      
    }
  if ((value = xmlGetProp (node, PROPERTY_IP)))
    {
      config->sensor.ip = g_strdup (value);
      xmlFree(value);      
    }
  if ((value = xmlGetProp (node, PROPERTY_INTERFACE)))
    {
      config->sensor.interface = g_strdup (value);
      xmlFree(value);      
    }
}

/*
 *
 *
 *
 *
 */
void
sim_xml_config_set_config_datasource (SimXmlConfig  *xmlconfig,
				      SimConfig     *config,
				      xmlNodePtr     node)
{
  SimConfigDS  *ds;
  gchar        *value;

  g_return_if_fail (xmlconfig);
  g_return_if_fail (SIM_IS_XML_CONFIG (xmlconfig));
  g_return_if_fail (config);
  g_return_if_fail (SIM_IS_CONFIG (config));
  g_return_if_fail (node);

  if (strcmp (node->name, OBJECT_DATASOURCE))
    {
      g_message ("Invalid config datasource node %s", node->name);
      return;
    }

  ds = sim_config_ds_new ();
  if ((value = xmlGetProp (node, PROPERTY_NAME)))
    {
      ds->name = g_strdup (value);
      xmlFree(value);      
    }
  if ((value = xmlGetProp (node, PROPERTY_PROVIDER)))
    {
      ds->provider = g_strdup (value);
      xmlFree(value);      
    }
  if ((value = xmlGetProp (node, PROPERTY_DSN)))
    {
      ds->dsn = g_strdup (value);
      xmlFree(value);      
    }

  config->datasources = g_list_append (config->datasources, ds);
}

/*
 *
 *
 *
 *
 */
void
sim_xml_config_set_config_datasources (SimXmlConfig  *xmlconfig,
				       SimConfig     *config,
				       xmlNodePtr     node)
{
  xmlNodePtr  children;

  g_return_if_fail (xmlconfig);
  g_return_if_fail (SIM_IS_XML_CONFIG (xmlconfig));
  g_return_if_fail (config);
  g_return_if_fail (SIM_IS_CONFIG (config));
  g_return_if_fail (node);

  if (strcmp (node->name, OBJECT_DATASOURCES))
    {
      g_message ("Invalid config datasources node %s", node->name);
      return;
    }

  children = node->xmlChildrenNode;
  while (children) {
    if (!strcmp (children->name, OBJECT_DATASOURCE))
      {
	sim_xml_config_set_config_datasource (xmlconfig, config, children);
      }

    children = children->next;
  }

}

/*
 *
 *
 *
 *
 */
void
sim_xml_config_set_config_directive (SimXmlConfig  *xmlconfig,
				     SimConfig     *config,
				     xmlNodePtr     node)
{
  gchar  *value;

  g_return_if_fail (xmlconfig);
  g_return_if_fail (SIM_IS_XML_CONFIG (xmlconfig));
  g_return_if_fail (config);
  g_return_if_fail (SIM_IS_CONFIG (config));
  g_return_if_fail (node);

  if (strcmp (node->name, OBJECT_DIRECTIVE))
    {
      g_message ("Invalid config directive node %s", node->name);
      return;
    }

  if ((value = xmlGetProp (node, PROPERTY_FILENAME)))
    {
      config->directive.filename = g_strdup (value);
      xmlFree(value);      
    }
  else
    {
      config->directive.filename = g_strdup (OS_SIM_GLOBAL_DIRECTIVE_FILE);
    }
}

/*
 *
 *
 *
 *
 */
void
sim_xml_config_set_config_scheduler (SimXmlConfig  *xmlconfig,
				     SimConfig     *config,
				     xmlNodePtr     node)
{
  gchar  *value;

  g_return_if_fail (xmlconfig);
  g_return_if_fail (SIM_IS_XML_CONFIG (xmlconfig));
  g_return_if_fail (config);
  g_return_if_fail (SIM_IS_CONFIG (config));
  g_return_if_fail (node);

  if (strcmp (node->name, OBJECT_SCHEDULER))
    {
      g_message ("Invalid config scheduler node %s", node->name);
      return;
    }

  if ((value = xmlGetProp (node, PROPERTY_INTERVAL)))
    {
      config->scheduler.interval = strtol (value, (char **) NULL, 10);
      xmlFree(value);      
    }
}

/*
 *
 *
 *
 *
 */
void
sim_xml_config_set_config_server (SimXmlConfig  *xmlconfig,
				  SimConfig     *config,
				  xmlNodePtr     node)
{
  gchar  *value;

  g_return_if_fail (xmlconfig);
  g_return_if_fail (SIM_IS_XML_CONFIG (xmlconfig));
  g_return_if_fail (config);
  g_return_if_fail (SIM_IS_CONFIG (config));
  g_return_if_fail (node);

  if (strcmp (node->name, OBJECT_SERVER))
    {
      g_message ("Invalid config server node %s", node->name);
      return;
    }

  if ((value = xmlGetProp (node, PROPERTY_PORT)))
    {
      config->server.port = strtol (value, (char **) NULL, 10);
      xmlFree(value);      
    }
}

/*
 *
 *
 *
 *
 */
void
sim_xml_config_set_config_notify (SimXmlConfig  *xmlconfig,
				  SimConfig     *config,
				  xmlNodePtr     node)
{
  SimConfigNotify  *notify;
  gchar            *emails;
  gchar            *levels;
  gchar           **values;
  gint              i;

  g_return_if_fail (xmlconfig);
  g_return_if_fail (SIM_IS_XML_CONFIG (xmlconfig));
  g_return_if_fail (config);
  g_return_if_fail (SIM_IS_CONFIG (config));
  g_return_if_fail (node);

  if (strcmp (node->name, OBJECT_NOTIFY))
    {
      g_message ("Invalid config notify node %s", node->name);
      return;
    }

  emails = xmlGetProp (node, PROPERTY_EMAILS);
  levels = xmlGetProp (node, PROPERTY_ALARM_RISKS);

  if (!emails || !levels)
    {
       if (emails) xmlFree(emails);
       if (levels) xmlFree(levels);
      return;
    }

  notify = sim_config_notify_new ();
  notify->emails = g_strdup (emails);

  values = g_strsplit (levels, SIM_DELIMITER_LIST, 0);
  for (i = 0; values[i] != NULL; i++)
    {
      SimAlarmRiskType risk = sim_get_alarm_risk_from_char (values[i]);
      if (risk != SIM_ALARM_RISK_TYPE_NONE)
	notify->alarm_risks =  g_list_append (notify->alarm_risks, GINT_TO_POINTER (risk));
    }
  g_strfreev (values);
  xmlFree (emails);
  xmlFree (levels);

  config->notifies = g_list_append (config->notifies, notify);
}

/*
 *
 *
 *
 *
 */
void
sim_xml_config_set_config_notifies (SimXmlConfig  *xmlconfig,
				    SimConfig     *config,
				    xmlNodePtr     node)
{
  gchar  *value;
  xmlNodePtr  children;
  
  g_return_if_fail (xmlconfig);
  g_return_if_fail (SIM_IS_XML_CONFIG (xmlconfig));
  g_return_if_fail (config);
  g_return_if_fail (SIM_IS_CONFIG (config));
  g_return_if_fail (node);

  if (strcmp (node->name, OBJECT_NOTIFIES))
    {
      g_message ("Invalid config notifies node %s", node->name);
      return;
    }

  if ((value = xmlGetProp (node, PROPERTY_PROGRAM)))
    {
      config->notify_prog = g_strdup (value);
      xmlFree(value);      
    }

  children = node->xmlChildrenNode;
  while (children) {
    if (!strcmp (children->name, OBJECT_NOTIFY))
      {
	sim_xml_config_set_config_notify (xmlconfig, config, children);
      }

    children = children->next;
  }
}


/*
 *
 *
 *
 *
 */
void
sim_xml_config_set_config_smtp (SimXmlConfig  *xmlconfig,
				SimConfig     *config,
				xmlNodePtr     node)
{
  gchar  *value;

  g_return_if_fail (xmlconfig);
  g_return_if_fail (SIM_IS_XML_CONFIG (xmlconfig));
  g_return_if_fail (config);
  g_return_if_fail (SIM_IS_CONFIG (config));
  g_return_if_fail (node);

  if (strcmp (node->name, OBJECT_SMTP))
    {
      g_message ("Invalid config smtp node %s", node->name);
      return;
    }

  if ((value = xmlGetProp (node, PROPERTY_HOST)))
    {
      config->smtp.host = g_strdup (value);
      xmlFree(value);      
    }
  if ((value = xmlGetProp (node, PROPERTY_PORT)))
    {
      config->smtp.port = strtol (value, (char **) NULL, 10);
      xmlFree(value);      
    }
}

/*
 *
 *
 *
 *
 */
void
sim_xml_config_set_config_rserver (SimXmlConfig  *xmlconfig,
				  SimConfig     *config,
				  xmlNodePtr     node)
{
  SimConfigRServer  *rserver;
  gchar             *value;

  g_return_if_fail (xmlconfig);
  g_return_if_fail (SIM_IS_XML_CONFIG (xmlconfig));
  g_return_if_fail (config);
  g_return_if_fail (SIM_IS_CONFIG (config));
  g_return_if_fail (node);

  if (strcmp (node->name, OBJECT_RSERVER))
    {
      g_message ("Invalid config rserver node %s", node->name);
      return;
    }

  rserver = sim_config_rserver_new ();
  rserver->port = 40001;

  if ((value = xmlGetProp (node, PROPERTY_NAME)))
    {
      rserver->name = g_strdup (value);
      xmlFree(value);
    }
  if ((value = xmlGetProp (node, PROPERTY_IP)))
    {
      rserver->ip = g_strdup (value);
      rserver->ia = gnet_inetaddr_new_nonblock (value, 0);
      xmlFree(value);
    }
  if ((value = xmlGetProp (node, PROPERTY_PORT)))
    {
      rserver->port = strtol (value, (char **) NULL, 10);
      xmlFree(value);
    }
  if ((value = xmlGetProp (node, PROPERTY_RESEND)))
    {
      if (!g_ascii_strcasecmp (value, "TRUE"))
	rserver->resend = TRUE;
      else
	rserver->resend = FALSE;

      xmlFree(value);
    }

  config->rservers = g_list_append (config->rservers, rserver);
}

/*
 *
 *
 *
 *
 */
void
sim_xml_config_set_config_rservers (SimXmlConfig  *xmlconfig,
					SimConfig     *config,
					xmlNodePtr     node)
{
  xmlNodePtr  children;
  
  g_return_if_fail (xmlconfig);
  g_return_if_fail (SIM_IS_XML_CONFIG (xmlconfig));
  g_return_if_fail (config);
  g_return_if_fail (SIM_IS_CONFIG (config));
  g_return_if_fail (node);

  if (strcmp (node->name, OBJECT_RSERVERS))
    {
      g_message ("Invalid config rservers node %s", node->name);
      return;
    }

  children = node->xmlChildrenNode;
  while (children) {
    if (!strcmp (children->name, OBJECT_RSERVER))
      {
	sim_xml_config_set_config_rserver (xmlconfig, config, children);
      }

    children = children->next;
  }

}

/*
 *
 *
 *
 *
 */
SimConfig*
sim_xml_config_new_config_from_node (SimXmlConfig  *xmlconfig,
				     xmlNodePtr     node)
{
  SimConfig     *config;
  SimAction     *action;
  GNode         *rule_root;
  xmlNodePtr     children;
  
  g_return_val_if_fail (xmlconfig, NULL);
  g_return_val_if_fail (SIM_IS_XML_CONFIG (xmlconfig), NULL);
  g_return_val_if_fail (node != NULL, NULL);
  
  if (strcmp (node->name, OBJECT_CONFIG))
    {
      g_message ("Invalid config node %s", node->name);
      return NULL;
    }
  
  config = sim_config_new ();

  children = node->xmlChildrenNode;
  while (children) {
    if (!strcmp (children->name, OBJECT_LOG))
      {
	sim_xml_config_set_config_log (xmlconfig, config, children);
      }
    if (!strcmp (children->name, OBJECT_SENSOR))
      {
	sim_xml_config_set_config_sensor (xmlconfig, config, children);
      }
    if (!strcmp (children->name, OBJECT_DATASOURCES))
      {
	sim_xml_config_set_config_datasources (xmlconfig, config, children);
      }
    if (!strcmp (children->name, OBJECT_DIRECTIVE))
      {
	sim_xml_config_set_config_directive (xmlconfig, config, children);
      }
    if (!strcmp (children->name, OBJECT_SCHEDULER))
      {
	sim_xml_config_set_config_scheduler (xmlconfig, config, children);
      }
    if (!strcmp (children->name, OBJECT_SERVER))
      {
	sim_xml_config_set_config_server (xmlconfig, config, children);
      }
    if (!strcmp (children->name, OBJECT_SMTP))
      {
	sim_xml_config_set_config_smtp (xmlconfig, config, children);
      }
    if (!strcmp (children->name, OBJECT_NOTIFIES))
      {
	sim_xml_config_set_config_notifies (xmlconfig, config, children);
      }
    if (!strcmp (children->name, OBJECT_RSERVERS))
      {
	sim_xml_config_set_config_rservers (xmlconfig, config, children);
      }

    children = children->next;
  }

  return config;
}

/*
 *
 *
 *
 *
 */
SimConfig*
sim_xml_config_get_config (SimXmlConfig  *xmlconfig)
{
  g_return_val_if_fail (xmlconfig, NULL);
  g_return_val_if_fail (SIM_IS_XML_CONFIG (xmlconfig), NULL);

  return xmlconfig->_priv->config;
}
