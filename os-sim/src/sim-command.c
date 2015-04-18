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

#include <gnet.h>
#include <config.h>
#include <time.h>

#include "sim-command.h"
#include "sim-rule.h"

typedef enum {
  SIM_COMMAND_SCOPE_COMMAND,
  SIM_COMMAND_SCOPE_CONNECT,
  SIM_COMMAND_SCOPE_SESSION_APPEND_PLUGIN,
  SIM_COMMAND_SCOPE_SESSION_REMOVE_PLUGIN,
  SIM_COMMAND_SCOPE_ALERT,
  SIM_COMMAND_SCOPE_RELOAD_PLUGINS,
  SIM_COMMAND_SCOPE_RELOAD_SENSORS,
  SIM_COMMAND_SCOPE_RELOAD_HOSTS,
  SIM_COMMAND_SCOPE_RELOAD_NETS,
  SIM_COMMAND_SCOPE_RELOAD_POLICIES,
  SIM_COMMAND_SCOPE_RELOAD_DIRECTIVES,
  SIM_COMMAND_SCOPE_RELOAD_ALL,
  SIM_COMMAND_SCOPE_OK,
  SIM_COMMAND_SCOPE_ERROR
} SimCommandScopeType;

typedef enum {
  SIM_COMMAND_SYMBOL_INVALID = G_TOKEN_LAST,
  SIM_COMMAND_SYMBOL_CONNECT,
  SIM_COMMAND_SYMBOL_SESSION_APPEND_PLUGIN,
  SIM_COMMAND_SYMBOL_SESSION_REMOVE_PLUGIN,
  SIM_COMMAND_SYMBOL_ALERT,
  SIM_COMMAND_SYMBOL_RELOAD_PLUGINS,
  SIM_COMMAND_SYMBOL_RELOAD_SENSORS,
  SIM_COMMAND_SYMBOL_RELOAD_HOSTS,
  SIM_COMMAND_SYMBOL_RELOAD_NETS,
  SIM_COMMAND_SYMBOL_RELOAD_POLICIES,
  SIM_COMMAND_SYMBOL_RELOAD_DIRECTIVES,
  SIM_COMMAND_SYMBOL_RELOAD_ALL,
  SIM_COMMAND_SYMBOL_OK,
  SIM_COMMAND_SYMBOL_ERROR,
  SIM_COMMAND_SYMBOL_ID,
  SIM_COMMAND_SYMBOL_USERNAME,
  SIM_COMMAND_SYMBOL_PASSWORD,
  SIM_COMMAND_SYMBOL_SENSOR,
  SIM_COMMAND_SYMBOL_INTERFACE,
  SIM_COMMAND_SYMBOL_TYPE,
  SIM_COMMAND_SYMBOL_NAME,
  SIM_COMMAND_SYMBOL_DATE,
  SIM_COMMAND_SYMBOL_PLUGIN_TYPE,
  SIM_COMMAND_SYMBOL_PLUGIN_ID,
  SIM_COMMAND_SYMBOL_PLUGIN_SID,
  SIM_COMMAND_SYMBOL_PRIORITY,
  SIM_COMMAND_SYMBOL_PROTOCOL,
  SIM_COMMAND_SYMBOL_SRC_IP,
  SIM_COMMAND_SYMBOL_SRC_PORT,
  SIM_COMMAND_SYMBOL_DST_IP,
  SIM_COMMAND_SYMBOL_DST_PORT,
  SIM_COMMAND_SYMBOL_CONDITION,
  SIM_COMMAND_SYMBOL_VALUE,
  SIM_COMMAND_SYMBOL_INTERVAL,
} SimCommandSymbolType;

static const struct
{
  gchar *name;
  guint token;
} command_symbols[] = {
  { "connect", SIM_COMMAND_SYMBOL_CONNECT },
  { "session-append-plugin", SIM_COMMAND_SYMBOL_SESSION_APPEND_PLUGIN },
  { "session-remove-plugin", SIM_COMMAND_SYMBOL_SESSION_REMOVE_PLUGIN },
  { "alert", SIM_COMMAND_SYMBOL_ALERT },
  { "reload-plugins", SIM_COMMAND_SYMBOL_RELOAD_PLUGINS },
  { "reload-sensors", SIM_COMMAND_SYMBOL_RELOAD_SENSORS },
  { "reload-hosts", SIM_COMMAND_SYMBOL_RELOAD_HOSTS },
  { "reload-nets", SIM_COMMAND_SYMBOL_RELOAD_NETS },
  { "reload-policies", SIM_COMMAND_SYMBOL_RELOAD_POLICIES },
  { "reload-directives", SIM_COMMAND_SYMBOL_RELOAD_DIRECTIVES },
  { "reload-all", SIM_COMMAND_SYMBOL_RELOAD_ALL },
  { "ok", SIM_COMMAND_SYMBOL_OK },
  { "error", SIM_COMMAND_SYMBOL_ERROR }
};

static const struct
{
  gchar *name;
  guint token;
} connect_symbols[] = {
  { "id", SIM_COMMAND_SYMBOL_ID },
  { "username", SIM_COMMAND_SYMBOL_USERNAME },
  { "password", SIM_COMMAND_SYMBOL_PASSWORD }
};

static const struct
{
  gchar *name;
  guint token;
} session_append_plugin_symbols[] = {
  { "id", SIM_COMMAND_SYMBOL_ID },
  { "plugin-id", SIM_COMMAND_SYMBOL_PLUGIN_ID },
  { "type", SIM_COMMAND_SYMBOL_TYPE },
  { "name", SIM_COMMAND_SYMBOL_NAME },
};

static const struct
{
  gchar *name;
  guint token;
} session_remove_plugin_symbols[] = {
  { "id", SIM_COMMAND_SYMBOL_ID },
  { "plugin-id", SIM_COMMAND_SYMBOL_PLUGIN_ID },
  { "type", SIM_COMMAND_SYMBOL_TYPE },
  { "name", SIM_COMMAND_SYMBOL_NAME },
};

static const struct
{
  gchar *name;
  guint token;
} alert_symbols[] = {
  { "type", SIM_COMMAND_SYMBOL_TYPE },
  { "plugin_id", SIM_COMMAND_SYMBOL_PLUGIN_ID },
  { "plugin_sid", SIM_COMMAND_SYMBOL_PLUGIN_SID },
  { "date", SIM_COMMAND_SYMBOL_DATE },
  { "sensor", SIM_COMMAND_SYMBOL_SENSOR },
  { "interface", SIM_COMMAND_SYMBOL_INTERFACE },
  { "priority", SIM_COMMAND_SYMBOL_PRIORITY },
  { "protocol", SIM_COMMAND_SYMBOL_PROTOCOL },
  { "src_ip", SIM_COMMAND_SYMBOL_SRC_IP },
  { "src_port", SIM_COMMAND_SYMBOL_SRC_PORT },
  { "dst_ip", SIM_COMMAND_SYMBOL_DST_IP },
  { "dst_port", SIM_COMMAND_SYMBOL_DST_PORT },
  { "condition", SIM_COMMAND_SYMBOL_CONDITION },
  { "value", SIM_COMMAND_SYMBOL_VALUE },
  { "interval", SIM_COMMAND_SYMBOL_INTERVAL }
};

static const struct
{
  gchar *name;
  guint token;
} reload_plugins_symbols[] = {
  { "id", SIM_COMMAND_SYMBOL_ID },
};

static const struct
{
  gchar *name;
  guint token;
} reload_sensors_symbols[] = {
  { "id", SIM_COMMAND_SYMBOL_ID },
};

static const struct
{
  gchar *name;
  guint token;
} reload_hosts_symbols[] = {
  { "id", SIM_COMMAND_SYMBOL_ID },
};

static const struct
{
  gchar *name;
  guint token;
} reload_nets_symbols[] = {
  { "id", SIM_COMMAND_SYMBOL_ID },
};

static const struct
{
  gchar *name;
  guint token;
} reload_policies_symbols[] = {
  { "id", SIM_COMMAND_SYMBOL_ID },
};

static const struct
{
  gchar *name;
  guint token;
} reload_directives_symbols[] = {
  { "id", SIM_COMMAND_SYMBOL_ID },
};

static const struct
{
  gchar *name;
  guint token;
} reload_all_symbols[] = {
  { "id", SIM_COMMAND_SYMBOL_ID },
};

enum 
{
  DESTROY,
  LAST_SIGNAL
};

static void sim_command_scan (SimCommand    *command,
			      const gchar   *buffer);
static void sim_command_connect_scan (SimCommand    *command,
				      GScanner      *scanner);
static void sim_command_session_append_plugin_scan (SimCommand    *command,
						    GScanner      *scanner);
static void sim_command_session_remove_plugin_scan (SimCommand    *command,
						    GScanner      *scanner);
static void sim_command_alert_scan (SimCommand    *command,
				    GScanner      *scanner);
static void sim_command_reload_plugins_scan (SimCommand    *command,
					     GScanner      *scanner);
static void sim_command_reload_sensors_scan (SimCommand    *command,
					     GScanner      *scanner);
static void sim_command_reload_hosts_scan (SimCommand    *command,
					   GScanner      *scanner);
static void sim_command_reload_nets_scan (SimCommand    *command,
					  GScanner      *scanner);
static void sim_command_reload_policies_scan (SimCommand    *command,
					      GScanner      *scanner);
static void sim_command_reload_directives_scan (SimCommand    *command,
						GScanner      *scanner);
static void sim_command_reload_all_scan (SimCommand    *command,
					 GScanner      *scanner);


static gpointer parent_class = NULL;
static gint sim_server_signals[LAST_SIGNAL] = { 0 };

/* GType Functions */

static void 
sim_command_impl_dispose (GObject  *gobject)
{
  G_OBJECT_CLASS (parent_class)->dispose (gobject);
}

static void 
sim_command_impl_finalize (GObject  *gobject)
{
  SimCommand *cmd = SIM_COMMAND (gobject);

  switch (cmd->type)
    {
    case SIM_COMMAND_TYPE_CONNECT:
      if (cmd->data.connect.username)
	g_free (cmd->data.connect.username);
      if (cmd->data.connect.password)
	g_free (cmd->data.connect.password);
      if (cmd->data.connect.sensor)
	g_free (cmd->data.connect.sensor);
      break;
    case SIM_COMMAND_TYPE_ALERT:
      if (cmd->data.alert.type)
	g_free (cmd->data.alert.type);
      if (cmd->data.alert.date)
	g_free (cmd->data.alert.date);
      if (cmd->data.alert.sensor)
	g_free (cmd->data.alert.sensor);
      
      if (cmd->data.alert.protocol)
	g_free (cmd->data.alert.protocol);
      if (cmd->data.alert.src_ip)
	g_free (cmd->data.alert.src_ip);
      if (cmd->data.alert.dst_ip)
	g_free (cmd->data.alert.dst_ip);
      
      if (cmd->data.alert.condition)
	g_free (cmd->data.alert.condition);
      if (cmd->data.alert.value)
	g_free (cmd->data.alert.value);
      break;
    case SIM_COMMAND_TYPE_SESSION_APPEND_PLUGIN:
      if (cmd->data.session_append_plugin.name)
	g_free (cmd->data.connect.username);
      break;
    case SIM_COMMAND_TYPE_SESSION_REMOVE_PLUGIN:
      if (cmd->data.session_remove_plugin.name)
	g_free (cmd->data.connect.username);
      break;

    case SIM_COMMAND_TYPE_WATCH_RULE:
      if (cmd->data.watch_rule.str)
	g_free (cmd->data.watch_rule.str);
      break;

    default:
      break;
    }

  G_OBJECT_CLASS (parent_class)->finalize (gobject);
}

static void
sim_command_class_init (SimCommandClass * class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  parent_class = g_type_class_ref (G_TYPE_OBJECT);

  object_class->dispose = sim_command_impl_dispose;
  object_class->finalize = sim_command_impl_finalize;
}

static void
sim_command_instance_init (SimCommand *command)
{
  command->type = SIM_COMMAND_TYPE_NONE;
  command->id = 0;
}

/* Public Methods */

GType
sim_command_get_type (void)
{
  static GType object_type = 0;
 
  if (!object_type)
  {
    static const GTypeInfo type_info = {
              sizeof (SimCommandClass),
              NULL,
              NULL,
              (GClassInitFunc) sim_command_class_init,
              NULL,
              NULL,                       /* class data */
              sizeof (SimCommand),
              0,                          /* number of pre-allocs */
              (GInstanceInitFunc) sim_command_instance_init,
              NULL                        /* value table */
    };
    
    g_type_init ();
                                                                                                                             
    object_type = g_type_register_static (G_TYPE_OBJECT, "SimCommand", &type_info, 0);
  }

  return object_type;
}

/*
 *
 *
 *
 *
 */
SimCommand*
sim_command_new (void)
{
  SimCommand *command = NULL;

  command = SIM_COMMAND (g_object_new (SIM_TYPE_COMMAND, NULL));

  return command;
}

/*
 *
 *
 *
 *
 */
SimCommand*
sim_command_new_from_buffer (const gchar    *buffer)
{
  SimCommand *command = NULL;

  g_return_val_if_fail (buffer, NULL);

  command = SIM_COMMAND (g_object_new (SIM_TYPE_COMMAND, NULL));

  sim_command_scan (command, buffer);

  return command;
}

/*
 *
 *
 *
 *
 */
SimCommand*
sim_command_new_from_type (SimCommandType  type)
{
  SimCommand *command = NULL;

  command = SIM_COMMAND (g_object_new (SIM_TYPE_COMMAND, NULL));
  command->type = type;

  return command;
}

/*
 *
 *
 *
 *
 */
SimCommand*
sim_command_new_from_rule (SimRule  *rule)
{
  SimCommand        *command = NULL;
  GString           *str = NULL;
  GList             *list = NULL;
  gint               plugin_id;
  gint               plugin_sid;
  gint               interval;
  gboolean           absolute;
  SimConditionType   condition;
  gchar             *value;
  gchar             *ip;

  g_return_val_if_fail (rule, NULL);
  g_return_val_if_fail (SIM_IS_RULE (rule), NULL);

  command = SIM_COMMAND (g_object_new (SIM_TYPE_COMMAND, NULL));
  command->type = SIM_COMMAND_TYPE_WATCH_RULE;

  str = g_string_new ("watch-rule ");

  /* Plugin ID */
  plugin_id = sim_rule_get_plugin_id (rule);
  if (plugin_id > 0)
    {
      g_string_append_printf (str, "plugin_id=\"%d\" ", plugin_id);
    }

  /* Plugin SID */
  plugin_sid = sim_rule_get_plugin_sid (rule);
  if (plugin_sid > 0)
    {
      g_string_append_printf (str, "plugin_sid=\"%d\" ", plugin_sid);
    }

  /* Condition */
  condition = sim_rule_get_condition (rule);
  if (condition != SIM_CONDITION_TYPE_NONE)
    {
      g_string_append_printf (str, "condition=\"%s\" ", sim_condition_get_str_from_type (condition));
    }

  /* Value */
  value = sim_rule_get_value (rule);
  if (value)
    {
      g_string_append_printf (str, "value=\"%s\" ", value);
    }

  /* PORT FROM */
  list = sim_rule_get_src_ports (rule);
  if (list)
    g_string_append (str, "port_from=\"");
  while (list)
    {
      gint port = GPOINTER_TO_INT (list->data);

      g_string_append_printf (str, "%d", port);

      if (list->next)
	str = g_string_append (str, ",");
      else 
	str = g_string_append (str, "\" ");

      list = list->next;
    }

  /* PORT TO  */
  list = sim_rule_get_dst_ports (rule);
  if (list)
    str = g_string_append (str, "port_to=\"");
  while (list)
    {
      gint port = GPOINTER_TO_INT (list->data);

      g_string_append_printf (str, "%d", port);

      if (list->next)
	str = g_string_append (str, ",");
      else 
	str = g_string_append (str, "\" ");

      list = list->next;
    }

  /* Interval */
  interval = sim_rule_get_interval (rule);
  if (interval > 0)
    {
      g_string_append_printf (str, "interval=\"%d\" ", interval);
    }

  /* SRC IAS */
  list = sim_rule_get_src_ias (rule);
  if (list)
    str = g_string_append (str, "from=\"");
  while (list)
    {
      GInetAddr *ia = (GInetAddr *) list->data;
      
      ip = gnet_inetaddr_get_canonical_name (ia);
      str = g_string_append (str, ip);
      g_free (ip);
      
      if (list->next)
	str = g_string_append (str, ",");
      else 
	str = g_string_append (str, "\" ");

      list = list->next;
    }

  /* DST IAS */
  list = sim_rule_get_dst_ias (rule);
  if (list)
    str = g_string_append (str, "to=\"");
  while (list)
    {
      GInetAddr *ia = (GInetAddr *) list->data;

      ip = gnet_inetaddr_get_canonical_name (ia);
      str = g_string_append (str, ip);
      g_free (ip);

      if (list->next)
	str = g_string_append (str, ",");
      else 
	str = g_string_append (str, "\" ");

      list = list->next;
    }

  /* Absolute */
  absolute = sim_rule_get_absolute (rule);
  if (absolute)
    {
      str = g_string_append (str, "absolute=\"true\"");
    }

  str = g_string_append (str, "\n");

  command->data.watch_rule.str = g_string_free (str, FALSE);

  return command;
}

/*
 *
 *
 *
 */
static void
sim_command_scan (SimCommand    *command,
		  const gchar   *buffer)
{
  GScanner    *scanner;
  gint         i;

  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));
  g_return_if_fail (buffer != NULL);
  
  /* Create scanner */
  scanner = g_scanner_new (NULL);

  /* Config scanner */
  scanner->config->cset_identifier_nth = (G_CSET_a_2_z ":._-0123456789" G_CSET_A_2_Z);
  scanner->config->case_sensitive = TRUE;
  scanner->config->symbol_2_token = TRUE;

  /* Added command symbols */
  for (i = 0; i < G_N_ELEMENTS (command_symbols); i++)
    g_scanner_scope_add_symbol (scanner, SIM_COMMAND_SCOPE_COMMAND, command_symbols[i].name, GINT_TO_POINTER (command_symbols[i].token));
  
  /* Added connect symbols */
  for (i = 0; i < G_N_ELEMENTS (connect_symbols); i++)
    g_scanner_scope_add_symbol (scanner, SIM_COMMAND_SCOPE_CONNECT, connect_symbols[i].name, GINT_TO_POINTER (connect_symbols[i].token));

  /* Added append plugin symbols */
  for (i = 0; i < G_N_ELEMENTS (session_append_plugin_symbols); i++)
    g_scanner_scope_add_symbol (scanner, SIM_COMMAND_SCOPE_SESSION_APPEND_PLUGIN, session_append_plugin_symbols[i].name, GINT_TO_POINTER (session_append_plugin_symbols[i].token));

  /* Added remove plugin symbols */
  for (i = 0; i < G_N_ELEMENTS (session_remove_plugin_symbols); i++)
    g_scanner_scope_add_symbol (scanner, SIM_COMMAND_SCOPE_SESSION_REMOVE_PLUGIN, session_remove_plugin_symbols[i].name, GINT_TO_POINTER (session_remove_plugin_symbols[i].token));

  /* Added alert symbols */
  for (i = 0; i < G_N_ELEMENTS (alert_symbols); i++)
    g_scanner_scope_add_symbol (scanner, SIM_COMMAND_SCOPE_ALERT, alert_symbols[i].name, GINT_TO_POINTER (alert_symbols[i].token));
  
  /* Added reload plugins symbols */
  for (i = 0; i < G_N_ELEMENTS (reload_plugins_symbols); i++)
    g_scanner_scope_add_symbol (scanner, SIM_COMMAND_SCOPE_RELOAD_PLUGINS, reload_plugins_symbols[i].name, GINT_TO_POINTER (reload_plugins_symbols[i].token));

  /* Added reload sensors symbols */
  for (i = 0; i < G_N_ELEMENTS (reload_plugins_symbols); i++)
    g_scanner_scope_add_symbol (scanner, SIM_COMMAND_SCOPE_RELOAD_SENSORS, reload_sensors_symbols[i].name, GINT_TO_POINTER (reload_sensors_symbols[i].token));

  /* Added reload hosts symbols */
  for (i = 0; i < G_N_ELEMENTS (reload_hosts_symbols); i++)
    g_scanner_scope_add_symbol (scanner, SIM_COMMAND_SCOPE_RELOAD_HOSTS, reload_hosts_symbols[i].name, GINT_TO_POINTER (reload_hosts_symbols[i].token));

  /* Added reload nets symbols */
  for (i = 0; i < G_N_ELEMENTS (reload_nets_symbols); i++)
    g_scanner_scope_add_symbol (scanner, SIM_COMMAND_SCOPE_RELOAD_NETS, reload_nets_symbols[i].name, GINT_TO_POINTER (reload_nets_symbols[i].token));

  /* Added reload policies symbols */
  for (i = 0; i < G_N_ELEMENTS (reload_policies_symbols); i++)
    g_scanner_scope_add_symbol (scanner, SIM_COMMAND_SCOPE_RELOAD_POLICIES, reload_policies_symbols[i].name, GINT_TO_POINTER (reload_policies_symbols[i].token));

  /* Added reload directives symbols */
  for (i = 0; i < G_N_ELEMENTS (reload_directives_symbols); i++)
    g_scanner_scope_add_symbol (scanner, SIM_COMMAND_SCOPE_RELOAD_DIRECTIVES, reload_directives_symbols[i].name, GINT_TO_POINTER (reload_directives_symbols[i].token));

  /* Added reload all symbols */
  for (i = 0; i < G_N_ELEMENTS (reload_all_symbols); i++)
    g_scanner_scope_add_symbol (scanner, SIM_COMMAND_SCOPE_RELOAD_ALL, reload_all_symbols[i].name, GINT_TO_POINTER (reload_all_symbols[i].token));

  /* Sets input text */
  g_scanner_input_text (scanner, buffer, strlen (buffer));

  g_scanner_set_scope (scanner, SIM_COMMAND_SCOPE_COMMAND);
  do
    {
      g_scanner_get_next_token (scanner);
 
      switch (scanner->token)
        {
        case SIM_COMMAND_SYMBOL_CONNECT:
	  sim_command_connect_scan (command, scanner);
          break;
        case SIM_COMMAND_SYMBOL_SESSION_APPEND_PLUGIN:
	  sim_command_session_append_plugin_scan (command, scanner);
          break;
        case SIM_COMMAND_SYMBOL_SESSION_REMOVE_PLUGIN:
	  sim_command_session_remove_plugin_scan (command, scanner);
          break;
        case SIM_COMMAND_SYMBOL_ALERT:
	  sim_command_alert_scan (command, scanner);
          break;
        case SIM_COMMAND_SYMBOL_RELOAD_PLUGINS:
	  sim_command_reload_plugins_scan (command, scanner);
          break;
        case SIM_COMMAND_SYMBOL_RELOAD_SENSORS:
	  sim_command_reload_sensors_scan (command, scanner);
          break;
        case SIM_COMMAND_SYMBOL_RELOAD_HOSTS:
	  sim_command_reload_hosts_scan (command, scanner);
          break;
        case SIM_COMMAND_SYMBOL_RELOAD_NETS:
	  sim_command_reload_nets_scan (command, scanner);
          break;
        case SIM_COMMAND_SYMBOL_RELOAD_POLICIES:
	  sim_command_reload_policies_scan (command, scanner);
          break;
        case SIM_COMMAND_SYMBOL_RELOAD_DIRECTIVES:
	  sim_command_reload_directives_scan (command, scanner);
          break;
        case SIM_COMMAND_SYMBOL_RELOAD_ALL:
	  sim_command_reload_all_scan (command, scanner);
          break;
        case SIM_COMMAND_SYMBOL_OK:
	  command->type = SIM_COMMAND_TYPE_OK;
          break;
        case SIM_COMMAND_SYMBOL_ERROR:
	  command->type = SIM_COMMAND_TYPE_ERROR;
          break;
        default:
          break;
        }
    }
  while(scanner->token != G_TOKEN_EOF);
                                                                                                                                                                            
  g_scanner_destroy (scanner);
}

/*
 *
 *
 *
 */
static void
sim_command_connect_scan (SimCommand    *command,
			  GScanner      *scanner)
{
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));
  g_return_if_fail (scanner != NULL);

  command->type = SIM_COMMAND_TYPE_CONNECT;
  command->data.connect.username = NULL;
  command->data.connect.password = NULL;
  command->data.connect.sensor = NULL;

  g_scanner_set_scope (scanner, SIM_COMMAND_SCOPE_CONNECT);
  do
    {
      g_scanner_get_next_token (scanner);
 
      switch (scanner->token)
        {
        case SIM_COMMAND_SYMBOL_ID:
	  g_scanner_get_next_token (scanner); /* = */
	  g_scanner_get_next_token (scanner); /* value */

	  if (scanner->token != G_TOKEN_STRING)
	    break;

	  command->id = strtol (scanner->value.v_string, (char **) NULL, 10);
          break;
        case SIM_COMMAND_SYMBOL_USERNAME:
	  g_scanner_get_next_token (scanner); /* = */
	  g_scanner_get_next_token (scanner); /* value */

	  if (scanner->token != G_TOKEN_STRING)
	    break;

	  command->data.connect.username = g_strdup (scanner->value.v_string);
          break;
        case SIM_COMMAND_SYMBOL_PASSWORD:
	  g_scanner_get_next_token (scanner); /* = */
	  g_scanner_get_next_token (scanner); /* value */

	  if (scanner->token != G_TOKEN_STRING)
	    break;

	  command->data.connect.password = g_strdup (scanner->value.v_string);
          break;
        case SIM_COMMAND_SYMBOL_SENSOR:
	  g_scanner_get_next_token (scanner); /* = */
	  g_scanner_get_next_token (scanner); /* value */

	  if (scanner->token != G_TOKEN_STRING)
	    break;

	  command->data.connect.sensor = g_strdup (scanner->value.v_string);
          break;
        default:
          break;
        }
    }
  while(scanner->token != G_TOKEN_EOF);
}

/*
 *
 *
 *
 */
static void
sim_command_session_append_plugin_scan (SimCommand    *command,
				GScanner      *scanner)
{
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));
  g_return_if_fail (scanner != NULL);

  command->type = SIM_COMMAND_TYPE_SESSION_APPEND_PLUGIN;
  command->data.session_append_plugin.id = 0;
  command->data.session_append_plugin.type = SIM_PLUGIN_TYPE_NONE;
  command->data.session_append_plugin.name = NULL;

  g_scanner_set_scope (scanner, SIM_COMMAND_SCOPE_SESSION_APPEND_PLUGIN);
  do
    {
      g_scanner_get_next_token (scanner);
 
      switch (scanner->token)
        {
        case SIM_COMMAND_SYMBOL_ID:
	  g_scanner_get_next_token (scanner); /* = */
	  g_scanner_get_next_token (scanner); /* value */

	  if (scanner->token != G_TOKEN_STRING)
	    break;

	  command->id = strtol (scanner->value.v_string, (char **) NULL, 10);
          break;
        case SIM_COMMAND_SYMBOL_PLUGIN_ID:
	  g_scanner_get_next_token (scanner); /* = */
	  g_scanner_get_next_token (scanner); /* value */

	  if (scanner->token != G_TOKEN_STRING)
	    break;

	  command->data.session_append_plugin.id = strtol (scanner->value.v_string, (char **) NULL, 10);
          break;
        case SIM_COMMAND_SYMBOL_TYPE:
	  g_scanner_get_next_token (scanner); /* = */
	  g_scanner_get_next_token (scanner); /* value */

	  if (scanner->token != G_TOKEN_STRING)
	    break;

	  command->data.session_append_plugin.type = strtol (scanner->value.v_string, (char **) NULL, 10);
          break;
        case SIM_COMMAND_SYMBOL_NAME:
	  g_scanner_get_next_token (scanner); /* = */
	  g_scanner_get_next_token (scanner); /* value */

	  if (scanner->token != G_TOKEN_STRING)
	    break;

	  command->data.session_append_plugin.name = g_strdup (scanner->value.v_string);
          break;
        default:
          break;
        }
    }
  while(scanner->token != G_TOKEN_EOF);
}

/*
 *
 *
 *
 */
static void
sim_command_session_remove_plugin_scan (SimCommand    *command,
				GScanner      *scanner)
{
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));
  g_return_if_fail (scanner != NULL);

  command->type = SIM_COMMAND_TYPE_SESSION_REMOVE_PLUGIN;
  command->data.session_append_plugin.id = 0;
  command->data.session_append_plugin.type = SIM_PLUGIN_TYPE_NONE;
  command->data.session_append_plugin.name = NULL;

  g_scanner_set_scope (scanner, SIM_COMMAND_SCOPE_SESSION_REMOVE_PLUGIN);
  do
    {
      g_scanner_get_next_token (scanner);
 
      switch (scanner->token)
        {
        case SIM_COMMAND_SYMBOL_ID:
	  g_scanner_get_next_token (scanner); /* = */
	  g_scanner_get_next_token (scanner); /* value */

	  if (scanner->token != G_TOKEN_STRING)
	    break;

	  command->id = strtol (scanner->value.v_string, (char **) NULL, 10);
          break;
        case SIM_COMMAND_SYMBOL_PLUGIN_ID:
	  g_scanner_get_next_token (scanner); /* = */
	  g_scanner_get_next_token (scanner); /* value */

	  if (scanner->token != G_TOKEN_STRING)
	    break;

	  command->data.session_append_plugin.id = strtol (scanner->value.v_string, (char **) NULL, 10);
          break;
        case SIM_COMMAND_SYMBOL_TYPE:
	  g_scanner_get_next_token (scanner); /* = */
	  g_scanner_get_next_token (scanner); /* value */

	  if (scanner->token != G_TOKEN_STRING)
	    break;

	  command->data.session_append_plugin.type = strtol (scanner->value.v_string, (char **) NULL, 10);
          break;
        case SIM_COMMAND_SYMBOL_NAME:
	  g_scanner_get_next_token (scanner); /* = */
	  g_scanner_get_next_token (scanner); /* value */

	  if (scanner->token != G_TOKEN_STRING)
	    break;

	  command->data.session_append_plugin.name = g_strdup (scanner->value.v_string);
          break;
        default:
          break;
        }
    }
  while(scanner->token != G_TOKEN_EOF);
}

/*
 *
 *
 *
 */
static void
sim_command_alert_scan (SimCommand    *command,
			GScanner      *scanner)
{
  GInetAddr    *ia;
  gchar        *ip;

  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));
  g_return_if_fail (scanner != NULL);

  command->type = SIM_COMMAND_TYPE_ALERT;
  command->data.alert.type = NULL;
  command->data.alert.date = NULL;
  command->data.alert.sensor = NULL;
  command->data.alert.interface = NULL;

  command->data.alert.plugin_id = 0;
  command->data.alert.plugin_sid = 0;

  command->data.alert.priority = 0;
  command->data.alert.protocol = NULL;
  command->data.alert.src_ip = NULL;
  command->data.alert.src_port = 0;
  command->data.alert.dst_ip = NULL;
  command->data.alert.dst_port = 0;

  command->data.alert.condition = NULL;
  command->data.alert.value = NULL;
  command->data.alert.interval = 0;

  g_scanner_set_scope (scanner, SIM_COMMAND_SCOPE_ALERT);
  do
    {
      g_scanner_get_next_token (scanner);
 
      switch (scanner->token)
        {
        case SIM_COMMAND_SYMBOL_TYPE:
	  g_scanner_get_next_token (scanner); /* = */
	  g_scanner_get_next_token (scanner); /* value */
	  
	  if (scanner->token != G_TOKEN_STRING)
	    {
	      command->type = SIM_COMMAND_TYPE_NONE;
	      break;
	    }

	  command->data.alert.type = g_strdup (scanner->value.v_string);
          break;
        case SIM_COMMAND_SYMBOL_PLUGIN_ID:
	  g_scanner_get_next_token (scanner); /* = */
	  g_scanner_get_next_token (scanner); /* value */
	  
	  if (scanner->token != G_TOKEN_STRING)
	    {
	      command->type = SIM_COMMAND_TYPE_NONE;
	      break;
	    }

	  command->data.alert.plugin_id = strtol (scanner->value.v_string, (char **) NULL, 10);
          break;
        case SIM_COMMAND_SYMBOL_PLUGIN_SID:
	  g_scanner_get_next_token (scanner); /* = */
	  g_scanner_get_next_token (scanner); /* value */
	  
	  if (scanner->token != G_TOKEN_STRING)
	    {
	      command->type = SIM_COMMAND_TYPE_NONE;
	      break;
	    }

	  command->data.alert.plugin_sid = strtol (scanner->value.v_string, (char **) NULL, 10);
          break;
        case SIM_COMMAND_SYMBOL_DATE:
	  g_scanner_get_next_token (scanner); /* = */
	  g_scanner_get_next_token (scanner); /* value */

	  if (scanner->token != G_TOKEN_STRING)
	    {
	      command->type = SIM_COMMAND_TYPE_NONE;
	      break;
	    }

	  command->data.alert.date = g_strdup (scanner->value.v_string);
          break;
        case SIM_COMMAND_SYMBOL_SENSOR:
	  g_scanner_get_next_token (scanner); /* = */
	  g_scanner_get_next_token (scanner); /* value */

	  if (scanner->token != G_TOKEN_STRING)
	    {
	      command->type = SIM_COMMAND_TYPE_NONE;
	      break;
	    }

	  command->data.alert.sensor = g_strdup (scanner->value.v_string);
          break;
        case SIM_COMMAND_SYMBOL_INTERFACE:
	  g_scanner_get_next_token (scanner); /* = */
	  g_scanner_get_next_token (scanner); /* value */

	  if (scanner->token != G_TOKEN_STRING)
	    {
	      command->type = SIM_COMMAND_TYPE_NONE;
	      break;
	    }

	  command->data.alert.interface = g_strdup (scanner->value.v_string);
          break;
        case SIM_COMMAND_SYMBOL_PRIORITY:
	  g_scanner_get_next_token (scanner); /* = */
	  g_scanner_get_next_token (scanner); /* value */

	  if (scanner->token != G_TOKEN_STRING)
	    {
	      command->type = SIM_COMMAND_TYPE_NONE;
	      break;
	    }

	  command->data.alert.priority = strtol (scanner->value.v_string, (char **) NULL, 10);
          break;
        case SIM_COMMAND_SYMBOL_PROTOCOL:
	  g_scanner_get_next_token (scanner); /* = */
	  g_scanner_get_next_token (scanner); /* value */

	  if (scanner->token != G_TOKEN_STRING)
	    {
	      command->type = SIM_COMMAND_TYPE_NONE;
	      break;
	    }

	  command->data.alert.protocol = g_strdup (scanner->value.v_string);
          break;
        case SIM_COMMAND_SYMBOL_SRC_IP:
	  g_scanner_get_next_token (scanner); /* = */
	  g_scanner_get_next_token (scanner); /* value */

	  if (scanner->token != G_TOKEN_STRING)
	    {
	      command->type = SIM_COMMAND_TYPE_NONE;
	      break;
	    }

	  command->data.alert.src_ip = g_strdup (scanner->value.v_string);
          break;
        case SIM_COMMAND_SYMBOL_SRC_PORT:
	  g_scanner_get_next_token (scanner); /* = */
	  g_scanner_get_next_token (scanner); /* value */

	  if (scanner->token != G_TOKEN_STRING)
	    {
	      command->type = SIM_COMMAND_TYPE_NONE;
	      break;
	    }

	  command->data.alert.src_port = strtol (scanner->value.v_string, (char **) NULL, 10);
          break;
        case SIM_COMMAND_SYMBOL_DST_IP:
	  g_scanner_get_next_token (scanner); /* = */
	  g_scanner_get_next_token (scanner); /* value */

	  if (scanner->token != G_TOKEN_STRING)
	    {
	      command->type = SIM_COMMAND_TYPE_NONE;
	      break;
	    }

	  command->data.alert.dst_ip = g_strdup (scanner->value.v_string);
          break;
        case SIM_COMMAND_SYMBOL_DST_PORT:
	  g_scanner_get_next_token (scanner); /* = */
	  g_scanner_get_next_token (scanner); /* value */

	  if (scanner->token != G_TOKEN_STRING)
	    {
	      command->type = SIM_COMMAND_TYPE_NONE;
	      break;
	    }

	  command->data.alert.dst_port = strtol (scanner->value.v_string, (char **) NULL, 10);
          break;
        case SIM_COMMAND_SYMBOL_CONDITION:
	  g_scanner_get_next_token (scanner); /* = */
	  g_scanner_get_next_token (scanner); /* value */

	  if (scanner->token != G_TOKEN_STRING)
	    {
	      command->type = SIM_COMMAND_TYPE_NONE;
	      break;
	    }

	  command->data.alert.condition = g_strdup (scanner->value.v_string);
	  break;
        case SIM_COMMAND_SYMBOL_VALUE:
	  g_scanner_get_next_token (scanner); /* = */
	  g_scanner_get_next_token (scanner); /* value */

	  if (scanner->token != G_TOKEN_STRING)
	    {
	      command->type = SIM_COMMAND_TYPE_NONE;
	      break;
	    }

	  command->data.alert.value = g_strdup (scanner->value.v_string);
	  break;
        case SIM_COMMAND_SYMBOL_INTERVAL:
	  g_scanner_get_next_token (scanner); /* = */
	  g_scanner_get_next_token (scanner); /* value */

	  if (scanner->token != G_TOKEN_STRING)
	    {
	      command->type = SIM_COMMAND_TYPE_NONE;
	      break;
	    }

	  command->data.alert.interval = strtol (scanner->value.v_string, (char **) NULL, 10);
          break;
        default:
          break;
        }
    }
  while(scanner->token != G_TOKEN_EOF);
}

/*
 *
 *
 *
 */
static void
sim_command_reload_plugins_scan (SimCommand    *command,
				 GScanner      *scanner)
{
  g_return_if_fail (command);
  g_return_if_fail (SIM_IS_COMMAND (command));
  g_return_if_fail (scanner);

  command->type = SIM_COMMAND_TYPE_RELOAD_PLUGINS;

  g_scanner_set_scope (scanner, SIM_COMMAND_SCOPE_RELOAD_PLUGINS);
  do
    {
      g_scanner_get_next_token (scanner);
 
      switch (scanner->token)
        {
        case SIM_COMMAND_SYMBOL_ID:
	  g_scanner_get_next_token (scanner); /* = */
	  g_scanner_get_next_token (scanner); /* value */

	  if (scanner->token != G_TOKEN_STRING)
	    break;

	  command->id = strtol (scanner->value.v_string, (char **) NULL, 10);
          break;
        default:
          break;
        }
    }
  while(scanner->token != G_TOKEN_EOF);
}

/*
 *
 *
 *
 */
static void
sim_command_reload_sensors_scan (SimCommand    *command,
				 GScanner      *scanner)
{
  g_return_if_fail (command);
  g_return_if_fail (SIM_IS_COMMAND (command));
  g_return_if_fail (scanner);

  command->type = SIM_COMMAND_TYPE_RELOAD_SENSORS;

  g_scanner_set_scope (scanner, SIM_COMMAND_SCOPE_RELOAD_SENSORS);
  do
    {
      g_scanner_get_next_token (scanner);
 
      switch (scanner->token)
        {
        case SIM_COMMAND_SYMBOL_ID:
	  g_scanner_get_next_token (scanner); /* = */
	  g_scanner_get_next_token (scanner); /* value */

	  if (scanner->token != G_TOKEN_STRING)
	    break;

	  command->id = strtol (scanner->value.v_string, (char **) NULL, 10);
          break;
        default:
          break;
        }
    }
  while(scanner->token != G_TOKEN_EOF);
}

/*
 *
 *
 *
 */
static void
sim_command_reload_hosts_scan (SimCommand    *command,
			       GScanner      *scanner)
{
  g_return_if_fail (command);
  g_return_if_fail (SIM_IS_COMMAND (command));
  g_return_if_fail (scanner);

  command->type = SIM_COMMAND_TYPE_RELOAD_HOSTS;

  g_scanner_set_scope (scanner, SIM_COMMAND_SCOPE_RELOAD_HOSTS);
  do
    {
      g_scanner_get_next_token (scanner);
 
      switch (scanner->token)
        {
        case SIM_COMMAND_SYMBOL_ID:
	  g_scanner_get_next_token (scanner); /* = */
	  g_scanner_get_next_token (scanner); /* value */

	  if (scanner->token != G_TOKEN_STRING)
	    break;

	  command->id = strtol (scanner->value.v_string, (char **) NULL, 10);
          break;
        default:
          break;
        }
    }
  while(scanner->token != G_TOKEN_EOF);
}

/*
 *
 *
 *
 */
static void
sim_command_reload_nets_scan (SimCommand    *command,
			       GScanner      *scanner)
{
  g_return_if_fail (command);
  g_return_if_fail (SIM_IS_COMMAND (command));
  g_return_if_fail (scanner);

  command->type = SIM_COMMAND_TYPE_RELOAD_NETS;

  g_scanner_set_scope (scanner, SIM_COMMAND_SCOPE_RELOAD_NETS);
  do
    {
      g_scanner_get_next_token (scanner);
 
      switch (scanner->token)
        {
        case SIM_COMMAND_SYMBOL_ID:
	  g_scanner_get_next_token (scanner); /* = */
	  g_scanner_get_next_token (scanner); /* value */

	  if (scanner->token != G_TOKEN_STRING)
	    break;

	  command->id = strtol (scanner->value.v_string, (char **) NULL, 10);
          break;
        default:
          break;
        }
    }
  while(scanner->token != G_TOKEN_EOF);
}

/*
 *
 *
 *
 */
static void
sim_command_reload_policies_scan (SimCommand    *command,
				    GScanner      *scanner)
{
  g_return_if_fail (command);
  g_return_if_fail (SIM_IS_COMMAND (command));
  g_return_if_fail (scanner);

  command->type = SIM_COMMAND_TYPE_RELOAD_POLICIES;

  g_scanner_set_scope (scanner, SIM_COMMAND_SCOPE_RELOAD_POLICIES);
  do
    {
      g_scanner_get_next_token (scanner);
 
      switch (scanner->token)
        {
        case SIM_COMMAND_SYMBOL_ID:
	  g_scanner_get_next_token (scanner); /* = */
	  g_scanner_get_next_token (scanner); /* value */

	  if (scanner->token != G_TOKEN_STRING)
	    break;

	  command->id = strtol (scanner->value.v_string, (char **) NULL, 10);
          break;
        default:
          break;
        }
    }
  while(scanner->token != G_TOKEN_EOF);
}

/*
 *
 *
 *
 */
static void
sim_command_reload_directives_scan (SimCommand    *command,
				    GScanner      *scanner)
{
  g_return_if_fail (command);
  g_return_if_fail (SIM_IS_COMMAND (command));
  g_return_if_fail (scanner);

  command->type = SIM_COMMAND_TYPE_RELOAD_DIRECTIVES;

  g_scanner_set_scope (scanner, SIM_COMMAND_SCOPE_RELOAD_DIRECTIVES);
  do
    {
      g_scanner_get_next_token (scanner);
 
      switch (scanner->token)
        {
        case SIM_COMMAND_SYMBOL_ID:
	  g_scanner_get_next_token (scanner); /* = */
	  g_scanner_get_next_token (scanner); /* value */

	  if (scanner->token != G_TOKEN_STRING)
	    break;

	  command->id = strtol (scanner->value.v_string, (char **) NULL, 10);
          break;
        default:
          break;
        }
    }
  while(scanner->token != G_TOKEN_EOF);
}

/*
 *
 *
 *
 */
static void
sim_command_reload_all_scan (SimCommand    *command,
			     GScanner      *scanner)
{
  g_return_if_fail (command);
  g_return_if_fail (SIM_IS_COMMAND (command));
  g_return_if_fail (scanner);

  command->type = SIM_COMMAND_TYPE_RELOAD_ALL;

  g_scanner_set_scope (scanner, SIM_COMMAND_SCOPE_RELOAD_ALL);
  do
    {
      g_scanner_get_next_token (scanner);
 
      switch (scanner->token)
        {
        case SIM_COMMAND_SYMBOL_ID:
	  g_scanner_get_next_token (scanner); /* = */
	  g_scanner_get_next_token (scanner); /* value */

	  if (scanner->token != G_TOKEN_STRING)
	    break;

	  command->id = strtol (scanner->value.v_string, (char **) NULL, 10);
          break;
        default:
          break;
        }
    }
  while(scanner->token != G_TOKEN_EOF);
}

/*
 *
 *
 *
 */
gchar*
sim_command_get_string (SimCommand    *command)
{
  SimRule  *rule;
  gchar    *str = NULL;

  g_return_val_if_fail (command != NULL, NULL);
  g_return_val_if_fail (SIM_IS_COMMAND (command), NULL);

  switch (command->type)
    {
    case SIM_COMMAND_TYPE_OK:
      str = g_strdup_printf ("ok id=\"%d\"\n", command->id);
      break;
    case SIM_COMMAND_TYPE_ERROR:
      str = g_strdup_printf ("error id=\"%d\"\n", command->id);
      break;
    case SIM_COMMAND_TYPE_WATCH_RULE:
      if (!command->data.watch_rule.str)
	break;

      str = g_strdup (command->data.watch_rule.str);
      break;
    default:
      break;
    }

  return str;
}

/*
 *
 *
 *
 */
SimAlert*
sim_command_get_alert (SimCommand     *command)
{
  SimAlertType   type;
  SimAlert      *alert;
  struct tm      tm;

  g_return_val_if_fail (command, NULL);
  g_return_val_if_fail (SIM_IS_COMMAND (command), NULL);
  g_return_val_if_fail (command->type == SIM_COMMAND_TYPE_ALERT, NULL);
  g_return_val_if_fail (command->data.alert.type, NULL);

  type = sim_alert_get_type_from_str (command->data.alert.type);

  if (type == SIM_ALERT_TYPE_NONE)
    return NULL;

  alert = sim_alert_new_from_type (type);

  if (command->data.alert.date)
    {
      if (strptime (command->data.alert.date, "%Y-%m-%d %H:%M:%S", &tm))
	alert->time =  mktime (&tm);
    }
  if (command->data.alert.sensor) 
    alert->sensor = g_strdup (command->data.alert.sensor);
  if (command->data.alert.interface) 
    alert->interface = g_strdup (command->data.alert.interface);

  if (command->data.alert.plugin_id)
    alert->plugin_id = command->data.alert.plugin_id;
  if (command->data.alert.plugin_sid)
    alert->plugin_sid = command->data.alert.plugin_sid;
  
  if (command->data.alert.priority)
    alert->priority = command->data.alert.priority;
  if (command->data.alert.protocol)
    alert->protocol = sim_protocol_get_type_from_str (command->data.alert.protocol);
  
  if (command->data.alert.src_ip)
    alert->src_ia = gnet_inetaddr_new_nonblock (command->data.alert.src_ip, 0);
  if (command->data.alert.src_port)
    alert->src_port = command->data.alert.src_port;
  if (command->data.alert.dst_ip)
    alert->dst_ia = gnet_inetaddr_new_nonblock (command->data.alert.dst_ip, 0);
  if (command->data.alert.dst_port)
    alert->dst_port = command->data.alert.dst_port;

  if (command->data.alert.condition)
    alert->condition = sim_condition_get_type_from_str (command->data.alert.condition);
  if (command->data.alert.value)
    alert->value = g_strdup (command->data.alert.value);
  if (command->data.alert.interval)
    alert->interval = command->data.alert.interval;
  
  return alert;
}

/*
 *
 *
 *
 */
gboolean
sim_command_is_valid (SimCommand      *cmd)
{
  g_return_val_if_fail (cmd, FALSE);
  g_return_val_if_fail (SIM_IS_COMMAND (cmd), FALSE);

  switch (cmd->type)
    {
    case SIM_COMMAND_TYPE_CONNECT:
      break;
    case SIM_COMMAND_TYPE_ALERT:
      break;
    case SIM_COMMAND_TYPE_SESSION_APPEND_PLUGIN:
      break;
    case SIM_COMMAND_TYPE_SESSION_REMOVE_PLUGIN:
      break;
    case SIM_COMMAND_TYPE_WATCH_RULE:
      break;
    default:
      break;
    }

  return TRUE;
}
