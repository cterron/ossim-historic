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
#include <time.h>
#include <string.h>

#include "sim-command.h"
#include "sim-rule.h"
#include "sim-util.h"
#include <config.h>

typedef enum {
  SIM_COMMAND_SCOPE_COMMAND,
  SIM_COMMAND_SCOPE_CONNECT,
  SIM_COMMAND_SCOPE_SESSION_APPEND_PLUGIN,
  SIM_COMMAND_SCOPE_SESSION_REMOVE_PLUGIN,
  SIM_COMMAND_SCOPE_SERVER_GET_SENSORS,
  SIM_COMMAND_SCOPE_SERVER_GET_SENSOR_PLUGINS,
  SIM_COMMAND_SCOPE_SENSOR_PLUGIN,
  SIM_COMMAND_SCOPE_SENSOR_PLUGIN_START,
  SIM_COMMAND_SCOPE_SENSOR_PLUGIN_STOP,
  SIM_COMMAND_SCOPE_SENSOR_PLUGIN_ENABLED,
  SIM_COMMAND_SCOPE_SENSOR_PLUGIN_DISABLED,
  SIM_COMMAND_SCOPE_PLUGIN_START,
  SIM_COMMAND_SCOPE_PLUGIN_UNKNOWN,
  SIM_COMMAND_SCOPE_PLUGIN_STOP,
  SIM_COMMAND_SCOPE_PLUGIN_ENABLED,
  SIM_COMMAND_SCOPE_PLUGIN_DISABLED,
  SIM_COMMAND_SCOPE_EVENT,
  SIM_COMMAND_SCOPE_RELOAD_PLUGINS,
  SIM_COMMAND_SCOPE_RELOAD_SENSORS,
  SIM_COMMAND_SCOPE_RELOAD_HOSTS,
  SIM_COMMAND_SCOPE_RELOAD_NETS,
  SIM_COMMAND_SCOPE_RELOAD_POLICIES,
  SIM_COMMAND_SCOPE_RELOAD_DIRECTIVES,
  SIM_COMMAND_SCOPE_RELOAD_ALL,
  SIM_COMMAND_SCOPE_HOST_OS_EVENT,
  SIM_COMMAND_SCOPE_HOST_MAC_EVENT,
  SIM_COMMAND_SCOPE_HOST_SERVICE_EVENT,
  SIM_COMMAND_SCOPE_HOST_IDS_EVENT,
  SIM_COMMAND_SCOPE_OK,
  SIM_COMMAND_SCOPE_ERROR
} SimCommandScopeType;

typedef enum {
  SIM_COMMAND_SYMBOL_INVALID = G_TOKEN_LAST,
  SIM_COMMAND_SYMBOL_CONNECT,
  SIM_COMMAND_SYMBOL_SESSION_APPEND_PLUGIN,
  SIM_COMMAND_SYMBOL_SESSION_REMOVE_PLUGIN,
  SIM_COMMAND_SYMBOL_SERVER_GET_SENSORS,
  SIM_COMMAND_SYMBOL_SERVER_GET_SENSOR_PLUGINS,
  SIM_COMMAND_SYMBOL_SENSOR_PLUGIN,
  SIM_COMMAND_SYMBOL_SENSOR_PLUGIN_START,
  SIM_COMMAND_SYMBOL_SENSOR_PLUGIN_STOP,
  SIM_COMMAND_SYMBOL_SENSOR_PLUGIN_ENABLED,
  SIM_COMMAND_SYMBOL_SENSOR_PLUGIN_DISABLED,
  SIM_COMMAND_SYMBOL_PLUGIN_START,
  SIM_COMMAND_SYMBOL_PLUGIN_UNKNOWN,
  SIM_COMMAND_SYMBOL_PLUGIN_STOP,
  SIM_COMMAND_SYMBOL_PLUGIN_ENABLED,
  SIM_COMMAND_SYMBOL_PLUGIN_DISABLED,
  SIM_COMMAND_SYMBOL_EVENT,
  SIM_COMMAND_SYMBOL_RELOAD_PLUGINS,
  SIM_COMMAND_SYMBOL_RELOAD_SENSORS,
  SIM_COMMAND_SYMBOL_RELOAD_HOSTS,
  SIM_COMMAND_SYMBOL_RELOAD_NETS,
  SIM_COMMAND_SYMBOL_RELOAD_POLICIES,
  SIM_COMMAND_SYMBOL_RELOAD_DIRECTIVES,
  SIM_COMMAND_SYMBOL_RELOAD_ALL,
  SIM_COMMAND_SYMBOL_OK,
  SIM_COMMAND_SYMBOL_HOST_OS_EVENT,
  SIM_COMMAND_SYMBOL_HOST_MAC_EVENT,
  SIM_COMMAND_SYMBOL_HOST_SERVICE_EVENT,
  SIM_COMMAND_SYMBOL_HOST_IDS_EVENT,
  SIM_COMMAND_SYMBOL_ERROR,
  SIM_COMMAND_SYMBOL_ID,
  SIM_COMMAND_SYMBOL_SENSOR,
  SIM_COMMAND_SYMBOL_STATE,
  SIM_COMMAND_SYMBOL_ENABLED,
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
  SIM_COMMAND_SYMBOL_HOST,
  SIM_COMMAND_SYMBOL_HOSTNAME,
  SIM_COMMAND_SYMBOL_OS,
  SIM_COMMAND_SYMBOL_MAC,
  SIM_COMMAND_SYMBOL_SERVICE,
  SIM_COMMAND_SYMBOL_VENDOR,
  SIM_COMMAND_SYMBOL_PORT,
  SIM_COMMAND_SYMBOL_APPLICATION,
  SIM_COMMAND_SYMBOL_DATA,
  SIM_COMMAND_SYMBOL_EVENT_TYPE,
  SIM_COMMAND_SYMBOL_TARGET,
  SIM_COMMAND_SYMBOL_WHAT,
  SIM_COMMAND_SYMBOL_EXTRA_DATA,
  SIM_COMMAND_SYMBOL_LOG,
  SIM_COMMAND_SYMBOL_SNORT_SID,
  SIM_COMMAND_SYMBOL_SNORT_CID,
  SIM_COMMAND_SYMBOL_ASSET_SRC,
  SIM_COMMAND_SYMBOL_ASSET_DST,
  SIM_COMMAND_SYMBOL_RISK_A,
  SIM_COMMAND_SYMBOL_RISK_C,
  SIM_COMMAND_SYMBOL_ALARM,
  SIM_COMMAND_SYMBOL_RELIABILITY,
  SIM_COMMAND_SYMBOL_FILENAME,	//this and the following words, are used in events, and in HIDS events (not MAC, OS, or service events)
  SIM_COMMAND_SYMBOL_USERNAME,
  SIM_COMMAND_SYMBOL_PASSWORD,
  SIM_COMMAND_SYMBOL_USERDATA1,
  SIM_COMMAND_SYMBOL_USERDATA2,
  SIM_COMMAND_SYMBOL_USERDATA3,
  SIM_COMMAND_SYMBOL_USERDATA4,
  SIM_COMMAND_SYMBOL_USERDATA5,
  SIM_COMMAND_SYMBOL_USERDATA6,
  SIM_COMMAND_SYMBOL_USERDATA7,
  SIM_COMMAND_SYMBOL_USERDATA8,
  SIM_COMMAND_SYMBOL_USERDATA9
} SimCommandSymbolType;

static const struct
{
  gchar *name;
  guint token;
} command_symbols[] = {
  { "connect", SIM_COMMAND_SYMBOL_CONNECT },
  { "session-append-plugin", SIM_COMMAND_SYMBOL_SESSION_APPEND_PLUGIN },
  { "session-remove-plugin", SIM_COMMAND_SYMBOL_SESSION_REMOVE_PLUGIN },
  { "server-get-sensors", SIM_COMMAND_SYMBOL_SERVER_GET_SENSORS },
  { "server-get-sensor-plugins", SIM_COMMAND_SYMBOL_SERVER_GET_SENSOR_PLUGINS },
  { "sensor-plugin", SIM_COMMAND_SYMBOL_SENSOR_PLUGIN },
  { "sensor-plugin-start", SIM_COMMAND_SYMBOL_SENSOR_PLUGIN_START },
  { "sensor-plugin-stop", SIM_COMMAND_SYMBOL_SENSOR_PLUGIN_STOP },
  { "sensor-plugin-enabled", SIM_COMMAND_SYMBOL_SENSOR_PLUGIN_ENABLED },
  { "sensor-plugin-disabled", SIM_COMMAND_SYMBOL_SENSOR_PLUGIN_DISABLED },
  { "plugin-start", SIM_COMMAND_SYMBOL_PLUGIN_START },
  { "plugin-unknown", SIM_COMMAND_SYMBOL_PLUGIN_UNKNOWN},
  { "plugin-stop", SIM_COMMAND_SYMBOL_PLUGIN_STOP },
  { "plugin-enabled", SIM_COMMAND_SYMBOL_PLUGIN_ENABLED },
  { "plugin-disabled", SIM_COMMAND_SYMBOL_PLUGIN_DISABLED },
  { "event", SIM_COMMAND_SYMBOL_EVENT },
  { "reload-plugins", SIM_COMMAND_SYMBOL_RELOAD_PLUGINS },
  { "reload-sensors", SIM_COMMAND_SYMBOL_RELOAD_SENSORS },
  { "reload-hosts", SIM_COMMAND_SYMBOL_RELOAD_HOSTS },
  { "reload-nets", SIM_COMMAND_SYMBOL_RELOAD_NETS },
  { "reload-policies", SIM_COMMAND_SYMBOL_RELOAD_POLICIES },
  { "reload-directives", SIM_COMMAND_SYMBOL_RELOAD_DIRECTIVES },
  { "reload-all", SIM_COMMAND_SYMBOL_RELOAD_ALL },
  { "host-os-event", SIM_COMMAND_SYMBOL_HOST_OS_EVENT },
  { "host-mac-event", SIM_COMMAND_SYMBOL_HOST_MAC_EVENT },
  { "host-service-event", SIM_COMMAND_SYMBOL_HOST_SERVICE_EVENT },
  { "host-ids-event", SIM_COMMAND_SYMBOL_HOST_IDS_EVENT},
  { "ok", SIM_COMMAND_SYMBOL_OK },
  { "error", SIM_COMMAND_SYMBOL_ERROR }
//  { "distribuye-movidas-a-servers-hijos", SIM_COMMAND_SYMBOL_ERROR }
};

static const struct
{
  gchar *name;
  guint token;
} connect_symbols[] = {
  { "id", SIM_COMMAND_SYMBOL_ID },
  { "type", SIM_COMMAND_SYMBOL_TYPE },
  { "username", SIM_COMMAND_SYMBOL_USERNAME },
  { "password", SIM_COMMAND_SYMBOL_PASSWORD }
};

static const struct
{
  gchar *name;
  guint token;
} session_append_plugin_symbols[] = {
  { "id", SIM_COMMAND_SYMBOL_ID },
  { "plugin_id", SIM_COMMAND_SYMBOL_PLUGIN_ID },
  { "type", SIM_COMMAND_SYMBOL_TYPE },
  { "name", SIM_COMMAND_SYMBOL_NAME },
  { "state", SIM_COMMAND_SYMBOL_STATE },
  { "enabled", SIM_COMMAND_SYMBOL_ENABLED },
};

static const struct
{
  gchar *name;
  guint token;
} session_remove_plugin_symbols[] = {
  { "id", SIM_COMMAND_SYMBOL_ID },
  { "plugin_id", SIM_COMMAND_SYMBOL_PLUGIN_ID },
  { "type", SIM_COMMAND_SYMBOL_TYPE },
  { "name", SIM_COMMAND_SYMBOL_NAME },
  { "state", SIM_COMMAND_SYMBOL_STATE },
  { "enabled", SIM_COMMAND_SYMBOL_ENABLED },
};

static const struct
{
  gchar *name;
  guint token;
} server_get_sensors_symbols[] = {
  { "id", SIM_COMMAND_SYMBOL_ID }
};

static const struct
{
  gchar *name;
  guint token;
} sensor_symbols[] = {
  { "id", SIM_COMMAND_SYMBOL_ID },
  { "host", SIM_COMMAND_SYMBOL_HOST },
  { "state", SIM_COMMAND_SYMBOL_STATE }
};

static const struct
{
  gchar *name;
  guint token;
} server_get_sensor_plugins_symbols[] = {
  { "id", SIM_COMMAND_SYMBOL_ID }
};

static const struct
{
  gchar *name;
  guint token;
} sensor_plugin_symbols[] = {
  { "id", SIM_COMMAND_SYMBOL_ID },
  { "sensor", SIM_COMMAND_SYMBOL_SENSOR },
  { "plugin_id", SIM_COMMAND_SYMBOL_PLUGIN_ID },
  { "state", SIM_COMMAND_SYMBOL_STATE },
  { "enabled", SIM_COMMAND_SYMBOL_ENABLED }
};

static const struct
{
  gchar *name;
  guint token;
} sensor_plugin_start_symbols[] = {
  { "id", SIM_COMMAND_SYMBOL_ID },
  { "sensor", SIM_COMMAND_SYMBOL_SENSOR },
  { "plugin_id", SIM_COMMAND_SYMBOL_PLUGIN_ID }
};


static const struct
{
  gchar *name;
  guint token;
} sensor_plugin_stop_symbols[] = {
  { "id", SIM_COMMAND_SYMBOL_ID },
  { "sensor", SIM_COMMAND_SYMBOL_SENSOR },
  { "plugin_id", SIM_COMMAND_SYMBOL_PLUGIN_ID }
};

static const struct
{
  gchar *name;
  guint token;
} sensor_plugin_enabled_symbols[] = {
  { "id", SIM_COMMAND_SYMBOL_ID },
  { "sensor", SIM_COMMAND_SYMBOL_SENSOR },
  { "plugin_id", SIM_COMMAND_SYMBOL_PLUGIN_ID }
};

static const struct
{
  gchar *name;
  guint token;
} sensor_plugin_disabled_symbols[] = {
  { "id", SIM_COMMAND_SYMBOL_ID },
  { "sensor", SIM_COMMAND_SYMBOL_SENSOR },
  { "plugin_id", SIM_COMMAND_SYMBOL_PLUGIN_ID }
};

static const struct
{
  gchar *name;
  guint token;
} plugin_start_symbols[] = {
  { "id", SIM_COMMAND_SYMBOL_ID },
  { "plugin_id", SIM_COMMAND_SYMBOL_PLUGIN_ID }
};

static const struct
{
  gchar *name;
  guint token;
} plugin_unknown_symbols[] = {
  { "id", SIM_COMMAND_SYMBOL_ID },
  { "plugin_id", SIM_COMMAND_SYMBOL_PLUGIN_ID }
};



static const struct
{
  gchar *name;
  guint token;
} plugin_stop_symbols[] = {
  { "id", SIM_COMMAND_SYMBOL_ID },
  { "plugin_id", SIM_COMMAND_SYMBOL_PLUGIN_ID }
};

static const struct
{
  gchar *name;
  guint token;
} plugin_enabled_symbols[] = {
  { "id", SIM_COMMAND_SYMBOL_ID },
  { "plugin_id", SIM_COMMAND_SYMBOL_PLUGIN_ID }
};

static const struct
{
  gchar *name;
  guint token;
} plugin_disabled_symbols[] = {
  { "id", SIM_COMMAND_SYMBOL_ID },
  { "plugin_id", SIM_COMMAND_SYMBOL_PLUGIN_ID }
};

static const struct
{
  gchar *name;
  guint token;
} event_symbols[] = {
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
  { "interval", SIM_COMMAND_SYMBOL_INTERVAL },
  { "data", SIM_COMMAND_SYMBOL_DATA },
  { "log", SIM_COMMAND_SYMBOL_LOG },
  { "snort_sid", SIM_COMMAND_SYMBOL_SNORT_SID },
  { "snort_cid", SIM_COMMAND_SYMBOL_SNORT_CID },
  { "asset_src", SIM_COMMAND_SYMBOL_ASSET_SRC },
  { "asset_dst", SIM_COMMAND_SYMBOL_ASSET_DST },
  { "risk_a", SIM_COMMAND_SYMBOL_RISK_A },
  { "risk_c", SIM_COMMAND_SYMBOL_RISK_C },
  { "reliability", SIM_COMMAND_SYMBOL_RELIABILITY },
  { "filename", SIM_COMMAND_SYMBOL_FILENAME },
  { "username", SIM_COMMAND_SYMBOL_USERNAME },
  { "password", SIM_COMMAND_SYMBOL_PASSWORD },
  { "userdata1", SIM_COMMAND_SYMBOL_USERDATA1 },
  { "userdata2", SIM_COMMAND_SYMBOL_USERDATA2 },
  { "userdata3", SIM_COMMAND_SYMBOL_USERDATA3 },
  { "userdata4", SIM_COMMAND_SYMBOL_USERDATA4 },
  { "userdata5", SIM_COMMAND_SYMBOL_USERDATA5 },
  { "userdata6", SIM_COMMAND_SYMBOL_USERDATA6 },
  { "userdata7", SIM_COMMAND_SYMBOL_USERDATA7 },
  { "userdata8", SIM_COMMAND_SYMBOL_USERDATA8 },
  { "userdata9", SIM_COMMAND_SYMBOL_USERDATA9 }
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

static const struct
{
  gchar *name;
  guint token;
} host_os_event_symbols[] = {
  { "date", SIM_COMMAND_SYMBOL_DATE },
  { "host", SIM_COMMAND_SYMBOL_HOST },
  { "os", SIM_COMMAND_SYMBOL_OS },
  { "sensor", SIM_COMMAND_SYMBOL_SENSOR },
  { "interface", SIM_COMMAND_SYMBOL_INTERFACE },
  { "plugin_id", SIM_COMMAND_SYMBOL_PLUGIN_ID },
  { "plugin_sid", SIM_COMMAND_SYMBOL_PLUGIN_SID },
  { "log", SIM_COMMAND_SYMBOL_LOG }
};

static const struct
{
  gchar *name;
  guint token;
} host_mac_event_symbols[] = {
  { "date", SIM_COMMAND_SYMBOL_DATE },
  { "host", SIM_COMMAND_SYMBOL_HOST },
  { "mac", SIM_COMMAND_SYMBOL_MAC },
  { "vendor", SIM_COMMAND_SYMBOL_VENDOR },
  { "sensor", SIM_COMMAND_SYMBOL_SENSOR },
  { "interface", SIM_COMMAND_SYMBOL_INTERFACE },
  { "plugin_id", SIM_COMMAND_SYMBOL_PLUGIN_ID },
  { "plugin_sid", SIM_COMMAND_SYMBOL_PLUGIN_SID },
  { "log", SIM_COMMAND_SYMBOL_LOG }
};

static const struct
{
  gchar *name;
  guint token;
} host_service_event_symbols[] = {
  { "date", SIM_COMMAND_SYMBOL_DATE },
  { "host", SIM_COMMAND_SYMBOL_HOST },
  { "port", SIM_COMMAND_SYMBOL_PORT },
  { "sensor", SIM_COMMAND_SYMBOL_SENSOR },
  { "protocol", SIM_COMMAND_SYMBOL_PROTOCOL },
  { "service", SIM_COMMAND_SYMBOL_SERVICE },
  { "application", SIM_COMMAND_SYMBOL_APPLICATION },
  { "interface", SIM_COMMAND_SYMBOL_INTERFACE },
  { "plugin_id", SIM_COMMAND_SYMBOL_PLUGIN_ID },
  { "plugin_sid", SIM_COMMAND_SYMBOL_PLUGIN_SID },
  { "log", SIM_COMMAND_SYMBOL_LOG }
};

static const struct
{
  gchar *name;
  guint token;
} host_ids_event_symbols[] = {
  { "host", SIM_COMMAND_SYMBOL_HOST },
  { "hostname", SIM_COMMAND_SYMBOL_HOSTNAME },
  { "event_type", SIM_COMMAND_SYMBOL_EVENT_TYPE },
  { "target", SIM_COMMAND_SYMBOL_TARGET },
  { "what", SIM_COMMAND_SYMBOL_WHAT },
  { "extra_data", SIM_COMMAND_SYMBOL_EXTRA_DATA },
  { "sensor", SIM_COMMAND_SYMBOL_SENSOR},
  { "date", SIM_COMMAND_SYMBOL_DATE },
  { "plugin_id", SIM_COMMAND_SYMBOL_PLUGIN_ID },
  { "plugin_sid", SIM_COMMAND_SYMBOL_PLUGIN_SID },
  { "log", SIM_COMMAND_SYMBOL_LOG },
	{ "filename", SIM_COMMAND_SYMBOL_FILENAME },
  { "username", SIM_COMMAND_SYMBOL_USERNAME },
  { "password", SIM_COMMAND_SYMBOL_PASSWORD },
  { "userdata1", SIM_COMMAND_SYMBOL_USERDATA1 },
  { "userdata2", SIM_COMMAND_SYMBOL_USERDATA2 },
  { "userdata3", SIM_COMMAND_SYMBOL_USERDATA3 },
  { "userdata4", SIM_COMMAND_SYMBOL_USERDATA4 },
  { "userdata5", SIM_COMMAND_SYMBOL_USERDATA5 },
  { "userdata6", SIM_COMMAND_SYMBOL_USERDATA6 },
  { "userdata7", SIM_COMMAND_SYMBOL_USERDATA7 },
  { "userdata8", SIM_COMMAND_SYMBOL_USERDATA8 },
  { "userdata9", SIM_COMMAND_SYMBOL_USERDATA9 }
};




enum 
{
  DESTROY,
  LAST_SIGNAL
};

static gboolean sim_command_scan (SimCommand    *command,
			      const gchar   *buffer);
static gboolean sim_command_connect_scan (SimCommand    *command,
				      GScanner      *scanner);
static gboolean sim_command_session_append_plugin_scan (SimCommand    *command,
						    GScanner      *scanner);
static gboolean sim_command_session_remove_plugin_scan (SimCommand    *command,
						    GScanner      *scanner);

static gboolean sim_command_server_get_sensors_scan (SimCommand    *command,
						 GScanner      *scanner);
static gboolean sim_command_server_get_sensor_plugins_scan (SimCommand    *command,
							GScanner      *scanner);

static gboolean sim_command_sensor_plugin_scan (SimCommand    *command,
					    GScanner      *scanner);
static gboolean sim_command_sensor_plugin_start_scan (SimCommand    *command,
						  GScanner      *scanner);
static gboolean sim_command_sensor_plugin_stop_scan (SimCommand    *command,
						 GScanner      *scanner);
static gboolean sim_command_sensor_plugin_enabled_scan (SimCommand    *command,
						    GScanner      *scanner);
static gboolean sim_command_sensor_plugin_disabled_scan (SimCommand    *command,
						     GScanner      *scanner);
static gboolean sim_command_plugin_start_scan (SimCommand    *command,
					   GScanner      *scanner);
static gboolean sim_command_plugin_unknown_scan (SimCommand    *command,
					   GScanner      *scanner);
static gboolean sim_command_plugin_stop_scan (SimCommand    *command,
					  GScanner      *scanner);
static gboolean sim_command_plugin_enabled_scan (SimCommand    *command,
					     GScanner      *scanner);
static gboolean sim_command_plugin_disabled_scan (SimCommand    *command,
					      GScanner      *scanner);
static gboolean sim_command_event_scan (SimCommand    *command,
				    GScanner      *scanner);
static gboolean sim_command_reload_plugins_scan (SimCommand    *command,
					     GScanner      *scanner);
static gboolean sim_command_reload_sensors_scan (SimCommand    *command,
					     GScanner      *scanner);
static gboolean sim_command_reload_hosts_scan (SimCommand    *command,
					   GScanner      *scanner);
static gboolean sim_command_reload_nets_scan (SimCommand    *command,
					  GScanner      *scanner);
static gboolean sim_command_reload_policies_scan (SimCommand    *command,
					      GScanner      *scanner);
static gboolean sim_command_reload_directives_scan (SimCommand    *command,
						GScanner      *scanner);
static gboolean sim_command_reload_all_scan (SimCommand    *command,
					 GScanner      *scanner);
static gboolean sim_command_host_os_event_scan (SimCommand    *command,
					     GScanner      *scanner);
static gboolean sim_command_host_mac_event_scan (SimCommand    *command,
					      GScanner      *scanner);
static gboolean sim_command_host_service_event_scan (SimCommand    *command,
					      GScanner      *scanner);
static gboolean sim_command_host_ids_event_scan (SimCommand    *command,
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
      break;
    case SIM_COMMAND_TYPE_SESSION_APPEND_PLUGIN:
      if (cmd->data.session_append_plugin.name)
	g_free (cmd->data.session_append_plugin.name);
      break;
    case SIM_COMMAND_TYPE_SESSION_REMOVE_PLUGIN:
      if (cmd->data.session_remove_plugin.name)
	g_free (cmd->data.session_remove_plugin.name);
      break;
    case SIM_COMMAND_TYPE_EVENT:
      if (cmd->data.event.type)
	g_free (cmd->data.event.type);
      if (cmd->data.event.date)
	g_free (cmd->data.event.date);
      if (cmd->data.event.sensor)
	g_free (cmd->data.event.sensor);
      if (cmd->data.event.interface)
	g_free (cmd->data.event.interface);
      
      if (cmd->data.event.protocol)
	g_free (cmd->data.event.protocol);
      if (cmd->data.event.src_ip)
	g_free (cmd->data.event.src_ip);
      if (cmd->data.event.dst_ip)
	g_free (cmd->data.event.dst_ip);

      if (cmd->data.event.condition)
	g_free (cmd->data.event.condition);
      if (cmd->data.event.value)
	g_free (cmd->data.event.value);

      if (cmd->data.event.data)
	g_free (cmd->data.event.data);
	
      		if (cmd->data.event.filename)
						g_free (cmd->data.event.filename);
      		if (cmd->data.event.username)
						g_free (cmd->data.event.username);
      		if (cmd->data.event.password)
						g_free (cmd->data.event.password);
      		if (cmd->data.event.userdata1)
						g_free (cmd->data.event.userdata1);
      		if (cmd->data.event.userdata2)
						g_free (cmd->data.event.userdata2);
      		if (cmd->data.event.userdata3)
						g_free (cmd->data.event.userdata3);
      		if (cmd->data.event.userdata4)
						g_free (cmd->data.event.userdata4);
      		if (cmd->data.event.userdata5)
						g_free (cmd->data.event.userdata5);
      		if (cmd->data.event.userdata6)
						g_free (cmd->data.event.userdata6);
      		if (cmd->data.event.userdata7)
						g_free (cmd->data.event.userdata7);
      		if (cmd->data.event.userdata8)
						g_free (cmd->data.event.userdata8);
      		if (cmd->data.event.userdata9)
						g_free (cmd->data.event.userdata9);
	
      break;

    case SIM_COMMAND_TYPE_SENSOR:
      if (cmd->data.sensor.host)
	g_free (cmd->data.sensor.host);
      break;

    case SIM_COMMAND_TYPE_SENSOR_PLUGIN:
      if (cmd->data.sensor_plugin.sensor)
	g_free (cmd->data.sensor_plugin.sensor);
      break;
    case SIM_COMMAND_TYPE_SENSOR_PLUGIN_START:
      if (cmd->data.sensor_plugin_start.sensor)
	g_free (cmd->data.sensor_plugin_start.sensor);
      break;
    case SIM_COMMAND_TYPE_SENSOR_PLUGIN_STOP:
      if (cmd->data.sensor_plugin_stop.sensor)
	g_free (cmd->data.sensor_plugin_stop.sensor);
      break;
    case SIM_COMMAND_TYPE_SENSOR_PLUGIN_ENABLED:
      if (cmd->data.sensor_plugin_enabled.sensor)
	g_free (cmd->data.sensor_plugin_enabled.sensor);
      break;
    case SIM_COMMAND_TYPE_SENSOR_PLUGIN_DISABLED:
      if (cmd->data.sensor_plugin_disabled.sensor)
	g_free (cmd->data.sensor_plugin_disabled.sensor);
      break;

    case SIM_COMMAND_TYPE_WATCH_RULE:
      if (cmd->data.watch_rule.str)
	g_free (cmd->data.watch_rule.str);
      break;

    case SIM_COMMAND_TYPE_HOST_OS_EVENT:
      if (cmd->data.host_os_event.date)
	g_free (cmd->data.host_os_event.date);
      if (cmd->data.host_os_event.host)
	g_free (cmd->data.host_os_event.host);
      if (cmd->data.host_os_event.os)
	g_free (cmd->data.host_os_event.os);
      if (cmd->data.host_os_event.sensor)
	g_free (cmd->data.host_os_event.sensor);
      if (cmd->data.host_os_event.interface)
	g_free (cmd->data.host_os_event.interface);
      break;

    case SIM_COMMAND_TYPE_HOST_MAC_EVENT:
      if (cmd->data.host_mac_event.date)
	g_free (cmd->data.host_mac_event.date);
      if (cmd->data.host_mac_event.host)
	g_free (cmd->data.host_mac_event.host);
      if (cmd->data.host_mac_event.mac)
	g_free (cmd->data.host_mac_event.mac);
      if (cmd->data.host_mac_event.vendor)
	g_free (cmd->data.host_mac_event.vendor);
      if (cmd->data.host_mac_event.sensor)
        g_free (cmd->data.host_mac_event.sensor);	      
      if (cmd->data.host_mac_event.interface)
        g_free (cmd->data.host_mac_event.interface);	      
      break;

    case SIM_COMMAND_TYPE_HOST_SERVICE_EVENT:
      if (cmd->data.host_service_event.date)
	g_free (cmd->data.host_service_event.date);
      if (cmd->data.host_service_event.host)
	g_free (cmd->data.host_service_event.host);
      if (cmd->data.host_service_event.service)
	g_free (cmd->data.host_service_event.service);
      if (cmd->data.host_service_event.application)
	g_free (cmd->data.host_service_event.application);
      if (cmd->data.host_service_event.log)
	g_free (cmd->data.host_service_event.log);
      if (cmd->data.host_service_event.sensor)
	g_free (cmd->data.host_service_event.sensor);
      if (cmd->data.host_service_event.interface)
	g_free (cmd->data.host_service_event.interface);
      break;

    case SIM_COMMAND_TYPE_HOST_IDS_EVENT:
      if (cmd->data.host_ids_event.date)
	g_free (cmd->data.host_ids_event.date);
      if (cmd->data.host_ids_event.host)
	g_free (cmd->data.host_ids_event.host);
      if (cmd->data.host_ids_event.hostname)
	g_free (cmd->data.host_ids_event.hostname);
      if (cmd->data.host_ids_event.event_type)
	g_free (cmd->data.host_ids_event.event_type);
      if (cmd->data.host_ids_event.target)
	g_free (cmd->data.host_ids_event.target);
      if (cmd->data.host_ids_event.what)
	g_free (cmd->data.host_ids_event.what);
      if (cmd->data.host_ids_event.extra_data)
	g_free (cmd->data.host_ids_event.extra_data);
      if (cmd->data.host_ids_event.sensor)
	g_free (cmd->data.host_ids_event.sensor);
      if (cmd->data.host_ids_event.log)
	g_free (cmd->data.host_ids_event.log);
			
      		if (cmd->data.host_ids_event.filename)
						g_free (cmd->data.host_ids_event.filename);
      		if (cmd->data.host_ids_event.username)
						g_free (cmd->data.host_ids_event.username);
      		if (cmd->data.host_ids_event.password)
						g_free (cmd->data.host_ids_event.password);
      		if (cmd->data.host_ids_event.userdata1)
						g_free (cmd->data.host_ids_event.userdata1);
      		if (cmd->data.host_ids_event.userdata2)
						g_free (cmd->data.host_ids_event.userdata2);
      		if (cmd->data.host_ids_event.userdata3)
						g_free (cmd->data.host_ids_event.userdata3);
      		if (cmd->data.host_ids_event.userdata4)
						g_free (cmd->data.host_ids_event.userdata4);
      		if (cmd->data.host_ids_event.userdata5)
						g_free (cmd->data.host_ids_event.userdata5);
      		if (cmd->data.host_ids_event.userdata6)
						g_free (cmd->data.host_ids_event.userdata6);
      		if (cmd->data.host_ids_event.userdata7)
						g_free (cmd->data.host_ids_event.userdata7);
      		if (cmd->data.host_ids_event.userdata8)
						g_free (cmd->data.host_ids_event.userdata8);
      		if (cmd->data.host_ids_event.userdata9)
						g_free (cmd->data.host_ids_event.userdata9);
	
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

  if (!sim_command_scan (command, buffer))
	{
		g_object_unref(command);
		return NULL;
	}

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
  SimCommand        *command;
  GString           *str = NULL;
  GList             *list = NULL;
  gint               plugin_id;
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
  list = sim_rule_get_plugin_sids (rule);
  if (list)
    {
      gint plugin_sid = GPOINTER_TO_INT (list->data);
      g_string_append_printf (str, "plugin_sid=\"%d\" ", plugin_sid);
    }

  /* Condition */
  condition = sim_rule_get_condition (rule);
  if (condition != SIM_CONDITION_TYPE_NONE)
    {
      value = sim_condition_get_str_from_type (condition);
      g_string_append_printf (str, "condition=\"%s\" ", value);
      g_free (value);
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
  list = sim_rule_get_src_inets (rule);
  if (list)
    str = g_string_append (str, "from=\"");
  while (list)
    {
      SimInet *inet = (SimInet *) list->data;
      
      ip = sim_inet_ntop (inet);
      str = g_string_append (str, ip);
      g_free (ip);
      
      if (list->next)
	str = g_string_append (str, ",");
      else 
	str = g_string_append (str, "\" ");

      list = list->next;
    }

  /* DST IAS */
  list = sim_rule_get_dst_inets (rule);
  if (list)
    str = g_string_append (str, "to=\"");
  while (list)
    {
      SimInet *inet = (SimInet *) list->data;

      ip = sim_inet_ntop (inet);
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

  command->data.watch_rule.str = g_string_free (str, FALSE); //free the GString object and returns the string

  return command;
}

/*
 *
 * If the command analyzed has some field incorrect, the command will be rejected.
 * The 'command' parameter is filled inside this function and not returned, outside
 * this function you'll be able to access to it directly.
 */
static gboolean
sim_command_scan (SimCommand    *command,
								  const gchar   *buffer)
{
  GScanner    *scanner;
  gint         i;
	gboolean OK=TRUE; //if a problem appears in the command scanning, we'll return.

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

  /* Added server get sensors symbols */
  for (i = 0; i < G_N_ELEMENTS (server_get_sensors_symbols); i++)
    g_scanner_scope_add_symbol (scanner, SIM_COMMAND_SCOPE_SERVER_GET_SENSORS, server_get_sensors_symbols[i].name, GINT_TO_POINTER (server_get_sensors_symbols[i].token));

  /* Added server get sensor plugins symbols */
  for (i = 0; i < G_N_ELEMENTS (server_get_sensor_plugins_symbols); i++)
    g_scanner_scope_add_symbol (scanner, SIM_COMMAND_SCOPE_SERVER_GET_SENSOR_PLUGINS, server_get_sensor_plugins_symbols[i].name, GINT_TO_POINTER (server_get_sensor_plugins_symbols[i].token));

  /* Added sensor plugin symbols */
  for (i = 0; i < G_N_ELEMENTS (sensor_plugin_symbols); i++)
    g_scanner_scope_add_symbol (scanner, SIM_COMMAND_SCOPE_SENSOR_PLUGIN, sensor_plugin_symbols[i].name, GINT_TO_POINTER (sensor_plugin_symbols[i].token));

  /* Added sensor plugin start symbols */
  for (i = 0; i < G_N_ELEMENTS (sensor_plugin_start_symbols); i++)
    g_scanner_scope_add_symbol (scanner, SIM_COMMAND_SCOPE_SENSOR_PLUGIN_START, sensor_plugin_start_symbols[i].name, GINT_TO_POINTER (sensor_plugin_start_symbols[i].token));

  /* Added sensor plugin stop symbols */
  for (i = 0; i < G_N_ELEMENTS (sensor_plugin_stop_symbols); i++)
    g_scanner_scope_add_symbol (scanner, SIM_COMMAND_SCOPE_SENSOR_PLUGIN_STOP, sensor_plugin_stop_symbols[i].name, GINT_TO_POINTER (sensor_plugin_stop_symbols[i].token));

  /* Added sensor plugin enabled symbols */
  for (i = 0; i < G_N_ELEMENTS (sensor_plugin_enabled_symbols); i++)
    g_scanner_scope_add_symbol (scanner, SIM_COMMAND_SCOPE_SENSOR_PLUGIN_ENABLED, sensor_plugin_enabled_symbols[i].name, GINT_TO_POINTER (sensor_plugin_enabled_symbols[i].token));

  /* Added sensor plugin disabled symbols */
  for (i = 0; i < G_N_ELEMENTS (sensor_plugin_disabled_symbols); i++)
    g_scanner_scope_add_symbol (scanner, SIM_COMMAND_SCOPE_SENSOR_PLUGIN_DISABLED, sensor_plugin_disabled_symbols[i].name, GINT_TO_POINTER (sensor_plugin_disabled_symbols[i].token));

  /* Added plugin start symbols */
  for (i = 0; i < G_N_ELEMENTS (plugin_start_symbols); i++)
    g_scanner_scope_add_symbol (scanner, SIM_COMMAND_SCOPE_PLUGIN_START, plugin_start_symbols[i].name, GINT_TO_POINTER (plugin_start_symbols[i].token));

  /* Added plugin unknown symbols */
  for (i = 0; i < G_N_ELEMENTS (plugin_unknown_symbols); i++)
    g_scanner_scope_add_symbol (scanner, SIM_COMMAND_SCOPE_PLUGIN_UNKNOWN, plugin_unknown_symbols[i].name, GINT_TO_POINTER (plugin_unknown_symbols[i].token));

  /* Added plugin stop symbols */
  for (i = 0; i < G_N_ELEMENTS (plugin_stop_symbols); i++)
    g_scanner_scope_add_symbol (scanner, SIM_COMMAND_SCOPE_PLUGIN_STOP, plugin_stop_symbols[i].name, GINT_TO_POINTER (plugin_stop_symbols[i].token));

  /* Added plugin enabled symbols */
  for (i = 0; i < G_N_ELEMENTS (plugin_enabled_symbols); i++)
    g_scanner_scope_add_symbol (scanner, SIM_COMMAND_SCOPE_PLUGIN_ENABLED, plugin_enabled_symbols[i].name, GINT_TO_POINTER (plugin_enabled_symbols[i].token));

  /* Added plugin disabled symbols */
  for (i = 0; i < G_N_ELEMENTS (plugin_disabled_symbols); i++)
    g_scanner_scope_add_symbol (scanner, SIM_COMMAND_SCOPE_PLUGIN_DISABLED, plugin_disabled_symbols[i].name, GINT_TO_POINTER (plugin_disabled_symbols[i].token));

  /* Added event symbols */
  for (i = 0; i < G_N_ELEMENTS (event_symbols); i++)
    g_scanner_scope_add_symbol (scanner, SIM_COMMAND_SCOPE_EVENT, event_symbols[i].name, GINT_TO_POINTER (event_symbols[i].token));
  
  /* Added reload plugins symbols */
  for (i = 0; i < G_N_ELEMENTS (reload_plugins_symbols); i++)
    g_scanner_scope_add_symbol (scanner, SIM_COMMAND_SCOPE_RELOAD_PLUGINS, reload_plugins_symbols[i].name, GINT_TO_POINTER (reload_plugins_symbols[i].token));

  /* Added reload sensors symbols */
  for (i = 0; i < G_N_ELEMENTS (reload_sensors_symbols); i++)
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

  /* Added host os event symbols */
  for (i = 0; i < G_N_ELEMENTS (host_os_event_symbols); i++)
    g_scanner_scope_add_symbol (scanner, SIM_COMMAND_SCOPE_HOST_OS_EVENT, host_os_event_symbols[i].name, GINT_TO_POINTER (host_os_event_symbols[i].token));

  /* Added host mac event symbols */
  for (i = 0; i < G_N_ELEMENTS (host_mac_event_symbols); i++)
    g_scanner_scope_add_symbol (scanner, SIM_COMMAND_SCOPE_HOST_MAC_EVENT, host_mac_event_symbols[i].name, GINT_TO_POINTER (host_mac_event_symbols[i].token));

  /* Add host service event symbols */
  for (i = 0; i < G_N_ELEMENTS (host_service_event_symbols); i++)
    g_scanner_scope_add_symbol (scanner, SIM_COMMAND_SCOPE_HOST_SERVICE_EVENT, host_service_event_symbols[i].name, GINT_TO_POINTER (host_service_event_symbols[i].token));

  /* Add HIDS symbols */
  for (i = 0; i < G_N_ELEMENTS (host_ids_event_symbols); i++)
    g_scanner_scope_add_symbol (scanner, SIM_COMMAND_SCOPE_HOST_IDS_EVENT, host_ids_event_symbols[i].name, GINT_TO_POINTER (host_ids_event_symbols[i].token));


  /* Sets input text */
  g_scanner_input_text (scanner, buffer, strlen (buffer));

  g_scanner_set_scope (scanner, SIM_COMMAND_SCOPE_COMMAND);
  do
  {
    g_scanner_get_next_token (scanner);
 
    switch (scanner->token)
    {
	    case SIM_COMMAND_SYMBOL_CONNECT:
					  if (!sim_command_connect_scan (command, scanner))
							OK=FALSE;
        	  break;
      case SIM_COMMAND_SYMBOL_SESSION_APPEND_PLUGIN:
					  if (!sim_command_session_append_plugin_scan (command, scanner))
							OK=FALSE;
        	  break;
      case SIM_COMMAND_SYMBOL_SESSION_REMOVE_PLUGIN:
					  if (!sim_command_session_remove_plugin_scan (command, scanner))
							OK=FALSE;
        	  break;
		  case SIM_COMMAND_SYMBOL_SERVER_GET_SENSORS:
					  if (!sim_command_server_get_sensors_scan (command, scanner))
							OK=FALSE;
	          break;
      case SIM_COMMAND_SYMBOL_SERVER_GET_SENSOR_PLUGINS:
					  if (!sim_command_server_get_sensor_plugins_scan (command, scanner))
							OK=FALSE;
	          break;
      case SIM_COMMAND_SYMBOL_SENSOR_PLUGIN:
					  if (!sim_command_sensor_plugin_scan (command, scanner))
							OK=FALSE;
          	break;
	    case SIM_COMMAND_SYMBOL_SENSOR_PLUGIN_START:
					  if (!sim_command_sensor_plugin_start_scan (command, scanner))
							OK=FALSE;
          	break;
      case SIM_COMMAND_SYMBOL_SENSOR_PLUGIN_STOP:
					  if (!sim_command_sensor_plugin_stop_scan (command, scanner))
							OK=FALSE;
	          break;
      case SIM_COMMAND_SYMBOL_SENSOR_PLUGIN_ENABLED:
					  if (!sim_command_sensor_plugin_enabled_scan (command, scanner))
							OK=FALSE;
          	break;
      case SIM_COMMAND_SYMBOL_SENSOR_PLUGIN_DISABLED:
					  if (!sim_command_sensor_plugin_disabled_scan (command, scanner))
							OK=FALSE;
        	  break;
		  case SIM_COMMAND_SYMBOL_PLUGIN_START:
					  if (!sim_command_plugin_start_scan (command, scanner))
							OK=FALSE;
        	  break;
      case SIM_COMMAND_SYMBOL_PLUGIN_UNKNOWN:
					  if (!sim_command_plugin_unknown_scan (command, scanner))
							OK=FALSE;
			      break;
      case SIM_COMMAND_SYMBOL_PLUGIN_STOP:
					  if (!sim_command_plugin_stop_scan (command, scanner))
							OK=FALSE;
		        break;
      case SIM_COMMAND_SYMBOL_PLUGIN_ENABLED:
					  if (!sim_command_plugin_enabled_scan (command, scanner))
							OK=FALSE;
        	  break;
      case SIM_COMMAND_SYMBOL_PLUGIN_DISABLED:
					  if (!sim_command_plugin_disabled_scan (command, scanner))
							OK=FALSE;
        	  break;
      case SIM_COMMAND_SYMBOL_EVENT:
					  if (!sim_command_event_scan (command, scanner))
							OK=FALSE;
        	  break;
      case SIM_COMMAND_SYMBOL_RELOAD_PLUGINS:
					  if (!sim_command_reload_plugins_scan (command, scanner))
							OK=FALSE;
        	  break;
      case SIM_COMMAND_SYMBOL_RELOAD_SENSORS:
					  if (!sim_command_reload_sensors_scan (command, scanner))
							OK=FALSE;
        	  break;
			case SIM_COMMAND_SYMBOL_RELOAD_HOSTS:
					  if (!sim_command_reload_hosts_scan (command, scanner))
							OK=FALSE;
        	  break;
      case SIM_COMMAND_SYMBOL_RELOAD_NETS:
					  if (!sim_command_reload_nets_scan (command, scanner))
							OK=FALSE;
        	  break;
      case SIM_COMMAND_SYMBOL_RELOAD_POLICIES:
					  if (!sim_command_reload_policies_scan (command, scanner))
							OK=FALSE;
        	  break;
      case SIM_COMMAND_SYMBOL_RELOAD_DIRECTIVES:
					  if (!sim_command_reload_directives_scan (command, scanner))
							OK=FALSE;
        	  break;
      case SIM_COMMAND_SYMBOL_RELOAD_ALL:
					  if (!sim_command_reload_all_scan (command, scanner))
							OK=FALSE;
		        break;
      case SIM_COMMAND_SYMBOL_HOST_OS_EVENT:
					  if (!sim_command_host_os_event_scan (command, scanner))
							OK=FALSE;
        	  break;
      case SIM_COMMAND_SYMBOL_HOST_MAC_EVENT:
					  if (!sim_command_host_mac_event_scan (command, scanner))
							OK=FALSE;
        	  break;
      case SIM_COMMAND_SYMBOL_HOST_SERVICE_EVENT:
					  if (!sim_command_host_service_event_scan (command, scanner))
							OK=FALSE;
	          break;
      case SIM_COMMAND_SYMBOL_HOST_IDS_EVENT:
					  if (!sim_command_host_ids_event_scan (command, scanner))
							OK=FALSE;
	          break;
      case SIM_COMMAND_SYMBOL_OK:
					  command->type = SIM_COMMAND_TYPE_OK;
	          break;
      case SIM_COMMAND_SYMBOL_ERROR:
					  command->type = SIM_COMMAND_TYPE_ERROR;
	          break;
      default:
					  if (scanner->token == G_TOKEN_EOF)
					    break;
						g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_command_scan: error command unknown; Buffer from command: %s",buffer);

						g_scanner_destroy (scanner);
						return FALSE;
    }
  }
  while(scanner->token != G_TOKEN_EOF);

  g_scanner_destroy (scanner);
	return OK; //well... ok... or not!
}

/*
 *
 *
 *
 */
static gboolean
sim_command_connect_scan (SimCommand    *command,
												  GScanner      *scanner)
{
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));
  g_return_if_fail (scanner != NULL);

  command->type = SIM_COMMAND_TYPE_CONNECT;
  command->data.connect.username = NULL;
  command->data.connect.password = NULL;
  command->data.connect.type = SIM_SESSION_TYPE_NONE;

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

            if (sim_string_is_number (scanner->value.v_string))
              command->id = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: connect event incorrect. Please check the symbol_id issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }
	
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
						
			case SIM_COMMAND_SYMBOL_TYPE:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
							break;
				
						if (!g_ascii_strcasecmp (scanner->value.v_string, "SERVER")) 
						{
							command->data.connect.type = SIM_SESSION_TYPE_SERVER;
						}
						else 
						if (!g_ascii_strcasecmp (scanner->value.v_string, "SENSOR")) 
						{
							command->data.connect.type = SIM_SESSION_TYPE_SENSOR;
						}
						else
						if (!g_ascii_strcasecmp (scanner->value.v_string, "WEB")) 
						{
							command->data.connect.type = SIM_SESSION_TYPE_WEB;
						}
						else
						{
							command->data.connect.type = SIM_SESSION_TYPE_WEB;
						}
						
						break;
						
			default:
						if (scanner->token == G_TOKEN_EOF)
					    break;
					  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_command_connect_scan: error symbol unknown");
						return FALSE;
    }
  }
  while(scanner->token != G_TOKEN_EOF);

	return TRUE;
}

/*
 *
 *
 *
 */
static gboolean
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
  command->data.session_append_plugin.state = 0;
  command->data.session_append_plugin.enabled = FALSE;

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

            if (sim_string_is_number (scanner->value.v_string))
              command->id = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: append plugin event incorrect. Please check the id issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }
						break;
			case SIM_COMMAND_SYMBOL_PLUGIN_ID:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
							break;
            if (sim_string_is_number (scanner->value.v_string))
							command->data.session_append_plugin.id = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: append plugin event incorrect. Please check the plugin_id issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }

						break;
			case SIM_COMMAND_SYMBOL_TYPE:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
							break;

            if (sim_string_is_number (scanner->value.v_string))
              command->data.session_append_plugin.type = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: append plugin event incorrect. Please check the type issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }
						break;
						
			case SIM_COMMAND_SYMBOL_NAME:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
							break;
						command->data.session_append_plugin.name = g_strdup (scanner->value.v_string);
						break;
			
			case SIM_COMMAND_SYMBOL_STATE:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
							break;

						if (!g_ascii_strcasecmp (scanner->value.v_string, "start"))
							command->data.session_append_plugin.state = 1;
						else 
						if (!g_ascii_strcasecmp (scanner->value.v_string, "stop"))
							command->data.session_remove_plugin.state = 2;
						break;
						
			case SIM_COMMAND_SYMBOL_ENABLED:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
							break;

						if (!g_ascii_strcasecmp (scanner->value.v_string, "true"))
							command->data.session_append_plugin.enabled = TRUE;
						else
						if (!g_ascii_strcasecmp (scanner->value.v_string, "false"))
							command->data.session_remove_plugin.enabled = FALSE;
						break;

			default:
					  if (scanner->token == G_TOKEN_EOF)
					    break;
						g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_command_session_append_plugin_scan: error symbol unknown");
	          return FALSE;
        }
    }
  while(scanner->token != G_TOKEN_EOF);
	return TRUE;
}

/*
 *
 *
 *
 */
static gboolean
sim_command_session_remove_plugin_scan (SimCommand    *command,
																				GScanner      *scanner)
{
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));
  g_return_if_fail (scanner != NULL);

  command->type = SIM_COMMAND_TYPE_SESSION_REMOVE_PLUGIN;
  command->data.session_remove_plugin.id = 0;
  command->data.session_remove_plugin.type = SIM_PLUGIN_TYPE_NONE;
  command->data.session_remove_plugin.name = NULL;
  command->data.session_remove_plugin.state = 0;
  command->data.session_remove_plugin.enabled = FALSE;

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
            if (sim_string_is_number (scanner->value.v_string))
              command->id = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: Remove plugin event incorrect. Please check the id issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }
						break;

			case SIM_COMMAND_SYMBOL_PLUGIN_ID:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
							break;

            if (sim_string_is_number (scanner->value.v_string))
							command->data.session_remove_plugin.id = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: Remove plugin event incorrect. Please check the plugin_id issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }
						break;
						
			case SIM_COMMAND_SYMBOL_TYPE:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
							break;

            if (sim_string_is_number (scanner->value.v_string))
							command->data.session_remove_plugin.type = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: Remove plugin event incorrect. Please check the type issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }
						break;

			case SIM_COMMAND_SYMBOL_NAME:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
							break;

						command->data.session_remove_plugin.name = g_strdup (scanner->value.v_string);
						break;
		
			case SIM_COMMAND_SYMBOL_STATE:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
							break;

						if (!g_ascii_strcasecmp (scanner->value.v_string, "start"))
							command->data.session_remove_plugin.state = 1;
						else
						if (!g_ascii_strcasecmp (scanner->value.v_string, "stop"))
							command->data.session_remove_plugin.state = 2;
						break;

			case SIM_COMMAND_SYMBOL_ENABLED:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
							break;

						if (!g_ascii_strcasecmp (scanner->value.v_string, "true"))
							command->data.session_remove_plugin.enabled = TRUE;
						else 
						if (!g_ascii_strcasecmp (scanner->value.v_string, "false"))
							command->data.session_remove_plugin.enabled = FALSE;
						break;

			default:
					  if (scanner->token == G_TOKEN_EOF)
					    break;
					  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_command_session_remove_plugin_scan: error symbol unknown");
        	  return FALSE;
    }
  }
  while(scanner->token != G_TOKEN_EOF);
	return TRUE;
}


/*
 *
 *
 *
 */
static gboolean
sim_command_server_get_sensors_scan (SimCommand    *command,
																     GScanner      *scanner)
{
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));
  g_return_if_fail (scanner != NULL);

  command->type = SIM_COMMAND_TYPE_SERVER_GET_SENSORS;

  g_scanner_set_scope (scanner, SIM_COMMAND_SCOPE_SERVER_GET_SENSORS);
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

            if (sim_string_is_number (scanner->value.v_string))
					  	command->id = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: get sensors event incorrect. Please check the id issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }
	          break;
						
      default:
					  if (scanner->token == G_TOKEN_EOF)
					    break;
	  				g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_command_server_get_sensors_scan: error symbol unknown");
	          return FALSE;
    }
  }
  while(scanner->token != G_TOKEN_EOF);
  
  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_command_server_get_sensors_scan: id: %d",command->id);
	return TRUE;
}

/*
 *
 *
 *
 */
static gboolean
sim_command_server_get_sensor_plugins_scan (SimCommand    *command,
																				    GScanner      *scanner)
{
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));
  g_return_if_fail (scanner != NULL);

  command->type = SIM_COMMAND_TYPE_SERVER_GET_SENSOR_PLUGINS;

  g_scanner_set_scope (scanner, SIM_COMMAND_SCOPE_SERVER_GET_SENSOR_PLUGINS);
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

            if (sim_string_is_number (scanner->value.v_string))
							command->id = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: get sensor plugin event incorrect. Please check the id issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }
						break;
							
			default:
						if (scanner->token == G_TOKEN_EOF)
							break;
					  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_command_server_get_sensor_plugins_scan: error symbol unknown");
						return FALSE;
          break;
    }
  }
	while(scanner->token != G_TOKEN_EOF);
	return TRUE;
}

/*
 *
 *
 *
 */
static gboolean
sim_command_sensor_plugin_scan (SimCommand    *command,
																GScanner      *scanner)
{
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));
  g_return_if_fail (scanner != NULL);

  command->type = SIM_COMMAND_TYPE_SENSOR_PLUGIN;
  command->data.sensor_plugin.sensor = NULL;
  command->data.sensor_plugin.plugin_id = 0;
  command->data.sensor_plugin.state = 0;
  command->data.sensor_plugin.enabled = FALSE;

  g_scanner_set_scope (scanner, SIM_COMMAND_SCOPE_SENSOR_PLUGIN);
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

            if (sim_string_is_number (scanner->value.v_string))
							command->id = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: sensor plugin event incorrect. Please check the id issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }

						break;

			case SIM_COMMAND_SYMBOL_SENSOR:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
							break;
						command->data.sensor_plugin.sensor = g_strdup (scanner->value.v_string);
						break;

			case SIM_COMMAND_SYMBOL_PLUGIN_ID:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
							break;
	
	          if (sim_string_is_number (scanner->value.v_string))
							command->data.sensor_plugin.plugin_id = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: sensor plugin event incorrect. Please check the plugin_id issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }
						break;

			case SIM_COMMAND_SYMBOL_STATE:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
							break;

						if (g_ascii_strcasecmp (scanner->value.v_string, "start"))
							command->data.sensor_plugin.state = 1;
						else if (g_ascii_strcasecmp (scanner->value.v_string, "stop"))
							command->data.sensor_plugin.state = 2;
						else if (g_ascii_strcasecmp (scanner->value.v_string, "unknown"))
							command->data.sensor_plugin.state = 3;
						break;

			case SIM_COMMAND_SYMBOL_ENABLED:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
							break;

						if (g_ascii_strcasecmp (scanner->value.v_string, "true"))
							command->data.sensor_plugin.enabled = TRUE;
						else if (g_ascii_strcasecmp (scanner->value.v_string, "false"))
							command->data.sensor_plugin.enabled = FALSE;

						break;

			default:
						if (scanner->token == G_TOKEN_EOF)
							break;
					  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_command_sensor_plugin_scan: error symbol unknown");
						return FALSE;
      }
  }
  while(scanner->token != G_TOKEN_EOF);
	return TRUE;
}

/*
 *
 *
 *
 */
static gboolean
sim_command_sensor_plugin_start_scan (SimCommand    *command,
																      GScanner      *scanner)
{
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));
  g_return_if_fail (scanner != NULL);

  command->type = SIM_COMMAND_TYPE_SENSOR_PLUGIN_START;

  g_scanner_set_scope (scanner, SIM_COMMAND_SCOPE_SENSOR_PLUGIN_START);
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

	          if (sim_string_is_number (scanner->value.v_string))
              command->id = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: sensor plugin start event incorrect. Please check the id issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }


						break;

			case SIM_COMMAND_SYMBOL_SENSOR:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
							break;
						command->data.sensor_plugin_start.sensor = g_strdup (scanner->value.v_string);
						break;

			case SIM_COMMAND_SYMBOL_PLUGIN_ID:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
							break;
  	        if (sim_string_is_number (scanner->value.v_string))
							command->data.sensor_plugin_start.plugin_id = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: sensor plugin start event incorrect. Please check the plugin_id issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }
						break;

			default:
						if (scanner->token == G_TOKEN_EOF)
							break;
						g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_command_sensor_plugin_start_scan: error symbol unknown");
						return FALSE;
    }
  }
  while(scanner->token != G_TOKEN_EOF);
	return TRUE;
}

/*
 *
 *
 *
 */
static gboolean
sim_command_sensor_plugin_stop_scan (SimCommand    *command,
																     GScanner      *scanner)
{
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));
  g_return_if_fail (scanner != NULL);

  command->type = SIM_COMMAND_TYPE_SENSOR_PLUGIN_STOP;

  g_scanner_set_scope (scanner, SIM_COMMAND_SCOPE_SENSOR_PLUGIN_STOP);
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
	
	          if (sim_string_is_number (scanner->value.v_string))
							command->id = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: sensor plugin stop event incorrect. Please check the id issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }
						break;

			case SIM_COMMAND_SYMBOL_SENSOR:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
							break;
						command->data.sensor_plugin_stop.sensor = g_strdup (scanner->value.v_string);
						break;

			case SIM_COMMAND_SYMBOL_PLUGIN_ID:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
							break;

	          if (sim_string_is_number (scanner->value.v_string))
							command->data.sensor_plugin_stop.plugin_id = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: sensor plugin event incorrect. Please check the plugin_id issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }
						break;

			default:
						if (scanner->token == G_TOKEN_EOF)
							break;
						g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_command_sensor_plugin_stop_scan: error symbol unknown");
						return FALSE;
        }
    }
  while(scanner->token != G_TOKEN_EOF);
	return TRUE;
}

/*
 *
 *
 *
 */
static gboolean
sim_command_sensor_plugin_enabled_scan (SimCommand    *command,
																				GScanner      *scanner)
{
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));
  g_return_if_fail (scanner != NULL);

  command->type = SIM_COMMAND_TYPE_SENSOR_PLUGIN_ENABLED;

  g_scanner_set_scope (scanner, SIM_COMMAND_SCOPE_SENSOR_PLUGIN_ENABLED);
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

	          if (sim_string_is_number (scanner->value.v_string))
              command->id = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: sensor plugin enabled event incorrect. Please check the id issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }
						break;

			case SIM_COMMAND_SYMBOL_SENSOR:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
							break;
						command->data.sensor_plugin_enabled.sensor = g_strdup (scanner->value.v_string);
						break;

			case SIM_COMMAND_SYMBOL_PLUGIN_ID:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
							break;

	          if (sim_string_is_number (scanner->value.v_string))
							command->data.sensor_plugin_enabled.plugin_id = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: sensor plugin enabled event incorrect. Please check the plugin_id issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }


						break;

			default:
						if (scanner->token == G_TOKEN_EOF)
							break;
						g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_command_sensor_plugin_enabled_scan: error symbol unknown");
						return FALSE;
    }
  }
  while(scanner->token != G_TOKEN_EOF);
	return TRUE;
}

/*
 *
 *
 *
 */
static gboolean
sim_command_sensor_plugin_disabled_scan (SimCommand    *command,
																				 GScanner      *scanner)
{
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));
  g_return_if_fail (scanner != NULL);

  command->type = SIM_COMMAND_TYPE_SENSOR_PLUGIN_DISABLED;

  g_scanner_set_scope (scanner, SIM_COMMAND_SCOPE_SENSOR_PLUGIN_DISABLED);
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

  	        if (sim_string_is_number (scanner->value.v_string))
	            command->id = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: sensor plugin disabled event incorrect. Please check the id issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }
						break;
	
			case SIM_COMMAND_SYMBOL_SENSOR:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
							break;
						command->data.sensor_plugin_disabled.sensor = g_strdup (scanner->value.v_string);
						break;

			case SIM_COMMAND_SYMBOL_PLUGIN_ID:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
							break;

            if (sim_string_is_number (scanner->value.v_string))
							command->data.sensor_plugin_disabled.plugin_id = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: sensor plugin disabled event incorrect. Please check the plugin_id issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }

						break;

			default:
						if (scanner->token == G_TOKEN_EOF)
							break;
						g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_command_sensor_plugin_disabled_scan: error symbol unknown");
						return FALSE;
    }
  }
  while(scanner->token != G_TOKEN_EOF);
	return TRUE;
}

/*
 *
 *
 *
 */
static gboolean
sim_command_plugin_start_scan (SimCommand    *command,
												       GScanner      *scanner)
{
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));
  g_return_if_fail (scanner != NULL);

  command->type = SIM_COMMAND_TYPE_PLUGIN_START;

  g_scanner_set_scope (scanner, SIM_COMMAND_SCOPE_PLUGIN_START);
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

            if (sim_string_is_number (scanner->value.v_string))
              command->id = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: sensor plugin start event incorrect. Please check the id issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }
						break;

			case SIM_COMMAND_SYMBOL_PLUGIN_ID:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
							break;

            if (sim_string_is_number (scanner->value.v_string))
							command->data.plugin_start.plugin_id = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: sensor plugin start event incorrect. Please check the plugin_id issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }

						break;

			default:
						if (scanner->token == G_TOKEN_EOF)
							break;
						g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_command_plugin_start_scan: error symbol unknown");
						return FALSE;
    }
  }
  while(scanner->token != G_TOKEN_EOF);
	return TRUE;
}

/*
 *
 *
 *
 */
static gboolean
sim_command_plugin_unknown_scan (SimCommand    *command,
													       GScanner      *scanner)
{
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));
  g_return_if_fail (scanner != NULL);

  command->type = SIM_COMMAND_TYPE_PLUGIN_UNKNOWN;

  g_scanner_set_scope (scanner, SIM_COMMAND_SCOPE_PLUGIN_UNKNOWN);
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

            if (sim_string_is_number (scanner->value.v_string))
              command->id = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: sensor plugin unknown event incorrect. Please check the id issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }
						break;

			case SIM_COMMAND_SYMBOL_PLUGIN_ID:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
							break;
            if (sim_string_is_number (scanner->value.v_string))
							command->data.plugin_unknown.plugin_id = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: sensor plugin unknown event incorrect. Please check the plugin_id issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }
						break;

			default:
						if (scanner->token == G_TOKEN_EOF)
							break;
						g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_command_plugin_unknown_scan: error symbol unknown");
						return FALSE;
    }
  }
  while(scanner->token != G_TOKEN_EOF);
	return TRUE;
}



/*
 *
 *
 *
 */
static gboolean
sim_command_plugin_stop_scan (SimCommand    *command,
												      GScanner      *scanner)
{
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));
  g_return_if_fail (scanner != NULL);

  command->type = SIM_COMMAND_TYPE_PLUGIN_STOP;

  g_scanner_set_scope (scanner, SIM_COMMAND_SCOPE_PLUGIN_STOP);
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
            if (sim_string_is_number (scanner->value.v_string))
              command->id = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: sensor plugin stop event incorrect. Please check the id issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }
						break;

			case SIM_COMMAND_SYMBOL_PLUGIN_ID:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
							break;
            if (sim_string_is_number (scanner->value.v_string))
							command->data.plugin_stop.plugin_id = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: sensor plugin stop event incorrect. Please check the plugin_id issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }
						break;

			default:
						if (scanner->token == G_TOKEN_EOF)
							break;
						g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_command_plugin_stop_scan: error symbol unknown");
						return FALSE;
    }
  }
  while(scanner->token != G_TOKEN_EOF);
	return TRUE;
}

/*
 *
 *
 *
 */
static gboolean
sim_command_plugin_enabled_scan (SimCommand    *command,
																 GScanner      *scanner)
{
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));
  g_return_if_fail (scanner != NULL);

  command->type = SIM_COMMAND_TYPE_PLUGIN_ENABLED;

  g_scanner_set_scope (scanner, SIM_COMMAND_SCOPE_PLUGIN_ENABLED);
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
            if (sim_string_is_number (scanner->value.v_string))
              command->id = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: sensor plugin enabled event incorrect. Please check the id issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }
						break;

			case SIM_COMMAND_SYMBOL_PLUGIN_ID:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
							break;

            if (sim_string_is_number (scanner->value.v_string))
							command->data.plugin_enabled.plugin_id = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: sensor plugin enabled event incorrect. Please check the plugin_id issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }
						break;

			default:
						if (scanner->token == G_TOKEN_EOF)
							break;
						g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_command_plugin_enabled_scan: error symbol unknown");
						return FALSE;
    }
  }
  while(scanner->token != G_TOKEN_EOF);
	return TRUE;
}

/*
 *
 *
 *
 */
static gboolean
sim_command_plugin_disabled_scan (SimCommand    *command,
																  GScanner      *scanner)
{
  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));
  g_return_if_fail (scanner != NULL);

  command->type = SIM_COMMAND_TYPE_PLUGIN_DISABLED;

  g_scanner_set_scope (scanner, SIM_COMMAND_SCOPE_PLUGIN_DISABLED);
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

            if (sim_string_is_number (scanner->value.v_string))
              command->id = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: sensor plugin disabled event incorrect. Please check the id issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }
						break;

			case SIM_COMMAND_SYMBOL_PLUGIN_ID:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
							break;
            if (sim_string_is_number (scanner->value.v_string))
							command->data.plugin_disabled.plugin_id = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: sensor plugin disabled event incorrect. Please check the plugin_id issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }
						break;

			default:
						if (scanner->token == G_TOKEN_EOF)
							break;
						g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_command_plugin_disabled_scan: error symbol unknown");
						return FALSE;
    }
  }
  while(scanner->token != G_TOKEN_EOF);
	return TRUE;
}

/*
 *
 *
 *
 */
static gboolean
sim_command_event_scan (SimCommand    *command,
												GScanner      *scanner)
{
  GInetAddr    *ia;
  gchar        *ip;

  g_return_if_fail (command != NULL);
  g_return_if_fail (SIM_IS_COMMAND (command));
  g_return_if_fail (scanner != NULL);

  command->type = SIM_COMMAND_TYPE_EVENT;
  command->data.event.type = NULL;
  command->data.event.date = NULL;
  command->data.event.sensor = NULL;
  command->data.event.interface = NULL;

  command->data.event.plugin_id = 0;
  command->data.event.plugin_sid = 0;

  command->data.event.priority = 0;
  command->data.event.protocol = NULL;
  command->data.event.src_ip = NULL;
  command->data.event.src_port = 0;
  command->data.event.dst_ip = NULL;
  command->data.event.dst_port = 0;

  command->data.event.condition = NULL;
  command->data.event.value = NULL;
  command->data.event.interval = 0;

  command->data.event.data = NULL;
  command->data.event.snort_sid = 0;
  command->data.event.snort_cid = 0;

  command->data.event.reliability = 0;
  command->data.event.asset_src = 0;
  command->data.event.asset_dst = 0;
  command->data.event.risk_a = 0;
  command->data.event.risk_c = 0;
  command->data.event.alarm = FALSE;
  command->data.event.event = NULL;

	command->data.event.filename = NULL;
	command->data.event.username = NULL;
	command->data.event.password = NULL;
	command->data.event.userdata1 = NULL;
	command->data.event.userdata2 = NULL;
	command->data.event.userdata3 = NULL;
	command->data.event.userdata4 = NULL;
	command->data.event.userdata5 = NULL;
	command->data.event.userdata6 = NULL;
	command->data.event.userdata7 = NULL;
	command->data.event.userdata8 = NULL;
	command->data.event.userdata9 = NULL;

  g_scanner_set_scope (scanner, SIM_COMMAND_SCOPE_EVENT);
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
					  command->data.event.type = g_strdup (scanner->value.v_string);
    	      break;
						
      case SIM_COMMAND_SYMBOL_PLUGIN_ID:
					  g_scanner_get_next_token (scanner); /* = */
					  g_scanner_get_next_token (scanner); /* value */
	  
					  if (scanner->token != G_TOKEN_STRING)
				    {
				      command->type = SIM_COMMAND_TYPE_NONE;
	    			  break;
				    }

            if (sim_string_is_number (scanner->value.v_string))
					  	command->data.event.plugin_id = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: event incorrect. Please check the plugin_id issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }

        	  break;
						
      case SIM_COMMAND_SYMBOL_PLUGIN_SID:
					  g_scanner_get_next_token (scanner); /* = */
					  g_scanner_get_next_token (scanner); /* value */
	  
					  if (scanner->token != G_TOKEN_STRING)
				    {
	    			  command->type = SIM_COMMAND_TYPE_NONE;
					    break;
				    }
            if (sim_string_is_number (scanner->value.v_string))
					  	command->data.event.plugin_sid = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: event incorrect. Please check the plugin_sid issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }

        	  break;
						
      case SIM_COMMAND_SYMBOL_DATE:
					  g_scanner_get_next_token (scanner); /* = */
					  g_scanner_get_next_token (scanner); /* value */

					  if (scanner->token != G_TOKEN_STRING)
				    {
				      command->type = SIM_COMMAND_TYPE_NONE;
	    			  break;
				    }
	  				command->data.event.date = g_strdup (scanner->value.v_string);
          	break;
			
			case SIM_COMMAND_SYMBOL_SENSOR:
					  g_scanner_get_next_token (scanner); /* = */
	  				g_scanner_get_next_token (scanner); /* value */

					  if (scanner->token != G_TOKEN_STRING)
						{
							command->type = SIM_COMMAND_TYPE_NONE;
							break;
						}
						command->data.event.sensor = g_strdup (scanner->value.v_string);
						break;
						
			case SIM_COMMAND_SYMBOL_INTERFACE:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
						{
							command->type = SIM_COMMAND_TYPE_NONE;
							break;
						}	
						command->data.event.interface = g_strdup (scanner->value.v_string);
						break;
						
			case SIM_COMMAND_SYMBOL_PRIORITY:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
						{
							command->type = SIM_COMMAND_TYPE_NONE;
							break;
						}
            if (sim_string_is_number (scanner->value.v_string))
							command->data.event.priority = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: event incorrect. Please check the priority issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }

						break;
						
			case SIM_COMMAND_SYMBOL_PROTOCOL:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
						{
							command->type = SIM_COMMAND_TYPE_NONE;
							break;
						}
						command->data.event.protocol = g_strdup (scanner->value.v_string);
						break;
						
			case SIM_COMMAND_SYMBOL_SRC_IP:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
						{
							command->type = SIM_COMMAND_TYPE_NONE;
							break;
						}
						command->data.event.src_ip = g_strdup (scanner->value.v_string);
						break;
						
			case SIM_COMMAND_SYMBOL_SRC_PORT:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
						{
							command->type = SIM_COMMAND_TYPE_NONE;
							break;
						}
            if (sim_string_is_number (scanner->value.v_string))
							command->data.event.src_port = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: event incorrect. Please check the src_port issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }

						break;
						
			case SIM_COMMAND_SYMBOL_DST_IP:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
						{
							command->type = SIM_COMMAND_TYPE_NONE;
							break;
						}
						command->data.event.dst_ip = g_strdup (scanner->value.v_string);
						break;
						
			case SIM_COMMAND_SYMBOL_DST_PORT:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
						{
							command->type = SIM_COMMAND_TYPE_NONE;
							break;
						}
            if (sim_string_is_number (scanner->value.v_string))
							command->data.event.dst_port = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: event incorrect. Please check the dst_port issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }
						break;
						
			case SIM_COMMAND_SYMBOL_CONDITION:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
						{
							command->type = SIM_COMMAND_TYPE_NONE;
							break;
						}
						command->data.event.condition = g_strdup (scanner->value.v_string);
						break;
						
			case SIM_COMMAND_SYMBOL_VALUE:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
						{
							command->type = SIM_COMMAND_TYPE_NONE;
							break;
						}
						command->data.event.value = g_strdup (scanner->value.v_string);
						break;
						
			case SIM_COMMAND_SYMBOL_INTERVAL:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
						{
							command->type = SIM_COMMAND_TYPE_NONE;
							break;
						}
            if (sim_string_is_number (scanner->value.v_string))
							command->data.event.interval = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: event incorrect. Please check the interval issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }

						break;
						
			case SIM_COMMAND_SYMBOL_DATA:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
						{
							command->type = SIM_COMMAND_TYPE_NONE;
							break;
						}
						command->data.event.data = g_strdup (scanner->value.v_string);
						break;

			case SIM_COMMAND_SYMBOL_LOG:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
						{
							command->type = SIM_COMMAND_TYPE_NONE;
							break;
						}
						command->data.event.log = g_strdup (scanner->value.v_string);
						break;

			case SIM_COMMAND_SYMBOL_SNORT_SID:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
						{
							command->type = SIM_COMMAND_TYPE_NONE;
							break;
						}
            if (sim_string_is_number (scanner->value.v_string))
							command->data.event.snort_sid = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: event incorrect. Please check the snort_sid issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }
						break;

			case SIM_COMMAND_SYMBOL_SNORT_CID:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
						{
							command->type = SIM_COMMAND_TYPE_NONE;
							break;
						}
            if (sim_string_is_number (scanner->value.v_string))
							command->data.event.snort_cid = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: event incorrect. Please check the snort_cid issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }
						break;

			case SIM_COMMAND_SYMBOL_ASSET_SRC:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
						{
							command->type = SIM_COMMAND_TYPE_NONE;
							break;
						}
            if (sim_string_is_number (scanner->value.v_string))
							command->data.event.asset_src = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: event incorrect. Please check the asset src issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }
						break;
		
			case SIM_COMMAND_SYMBOL_ASSET_DST:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
						{
							command->type = SIM_COMMAND_TYPE_NONE;
							break;
						}
            if (sim_string_is_number (scanner->value.v_string))
							command->data.event.asset_dst = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: event incorrect. Please check the asset dst issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }
						break;
						
			case SIM_COMMAND_SYMBOL_RISK_A:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
						{
							command->type = SIM_COMMAND_TYPE_NONE;
							break;
						}
            if (sim_string_is_number (scanner->value.v_string))
							command->data.event.risk_a = strtod (scanner->value.v_string, (char **) NULL);
            else
            {
              g_message("Error: event incorrect. Please check the Risk_A issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }
						break;

			case SIM_COMMAND_SYMBOL_RISK_C:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
						{
							command->type = SIM_COMMAND_TYPE_NONE;
							break;
						}
            if (sim_string_is_number (scanner->value.v_string))
							command->data.event.risk_c = strtod (scanner->value.v_string, (char **) NULL);
            else
            {
              g_message("Error: event incorrect. Please check the Risk_C issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }

						break;
						
			case SIM_COMMAND_SYMBOL_RELIABILITY:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
						{
							command->type = SIM_COMMAND_TYPE_NONE;
							break;
						}
            if (sim_string_is_number (scanner->value.v_string))
							command->data.event.reliability = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: event incorrect. Please check the reliability issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }

						break;

			case SIM_COMMAND_SYMBOL_ALARM:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
						{
							command->type = SIM_COMMAND_TYPE_NONE;
							break;
						}

						if (!g_ascii_strcasecmp (scanner->value.v_string, "TRUE"))
							command->data.event.alarm = TRUE;
						break;

      case SIM_COMMAND_SYMBOL_FILENAME:
            g_scanner_get_next_token (scanner); /* = */
            g_scanner_get_next_token (scanner); /* value */

            if (scanner->token != G_TOKEN_STRING)
            {
              command->type = SIM_COMMAND_TYPE_NONE;
              break;
            }
            command->data.event.filename = g_strdup (scanner->value.v_string);
            break;

      case SIM_COMMAND_SYMBOL_USERNAME:
            g_scanner_get_next_token (scanner); /* = */
            g_scanner_get_next_token (scanner); /* value */

            if (scanner->token != G_TOKEN_STRING)
            {
              command->type = SIM_COMMAND_TYPE_NONE;
              break;
            }
            command->data.event.username = g_strdup (scanner->value.v_string);
            break;

      case SIM_COMMAND_SYMBOL_PASSWORD:
            g_scanner_get_next_token (scanner); /* = */
            g_scanner_get_next_token (scanner); /* value */

            if (scanner->token != G_TOKEN_STRING)
            {
              command->type = SIM_COMMAND_TYPE_NONE;
              break;
            }
            command->data.event.password = g_strdup (scanner->value.v_string);
            break;

      case SIM_COMMAND_SYMBOL_USERDATA1:
            g_scanner_get_next_token (scanner); /* = */
            g_scanner_get_next_token (scanner); /* value */

            if (scanner->token != G_TOKEN_STRING)
            {
              command->type = SIM_COMMAND_TYPE_NONE;
              break;
            }
            command->data.event.userdata1 = g_strdup (scanner->value.v_string);
            break;

      case SIM_COMMAND_SYMBOL_USERDATA2:
            g_scanner_get_next_token (scanner); /* = */
            g_scanner_get_next_token (scanner); /* value */

            if (scanner->token != G_TOKEN_STRING)
            {
              command->type = SIM_COMMAND_TYPE_NONE;
              break;
            }
            command->data.event.userdata2 = g_strdup (scanner->value.v_string);
            break;

      case SIM_COMMAND_SYMBOL_USERDATA3:
            g_scanner_get_next_token (scanner); /* = */
            g_scanner_get_next_token (scanner); /* value */

            if (scanner->token != G_TOKEN_STRING)
            {
              command->type = SIM_COMMAND_TYPE_NONE;
              break;
            }
            command->data.event.userdata3 = g_strdup (scanner->value.v_string);
            break;

      case SIM_COMMAND_SYMBOL_USERDATA4:
            g_scanner_get_next_token (scanner); /* = */
            g_scanner_get_next_token (scanner); /* value */

            if (scanner->token != G_TOKEN_STRING)
            {
              command->type = SIM_COMMAND_TYPE_NONE;
              break;
            }
            command->data.event.userdata4 = g_strdup (scanner->value.v_string);
            break;

      case SIM_COMMAND_SYMBOL_USERDATA5:
            g_scanner_get_next_token (scanner); /* = */
            g_scanner_get_next_token (scanner); /* value */

            if (scanner->token != G_TOKEN_STRING)
            {
              command->type = SIM_COMMAND_TYPE_NONE;
              break;
            }
            command->data.event.userdata5 = g_strdup (scanner->value.v_string);
            break;

      case SIM_COMMAND_SYMBOL_USERDATA6:
            g_scanner_get_next_token (scanner); /* = */
            g_scanner_get_next_token (scanner); /* value */

            if (scanner->token != G_TOKEN_STRING)
            {
              command->type = SIM_COMMAND_TYPE_NONE;
              break;
            }
            command->data.event.userdata6 = g_strdup (scanner->value.v_string);
            break;

      case SIM_COMMAND_SYMBOL_USERDATA7:
            g_scanner_get_next_token (scanner); /* = */
            g_scanner_get_next_token (scanner); /* value */

            if (scanner->token != G_TOKEN_STRING)
            {
              command->type = SIM_COMMAND_TYPE_NONE;
              break;
            }
            command->data.event.userdata7 = g_strdup (scanner->value.v_string);
            break;

      case SIM_COMMAND_SYMBOL_USERDATA8:
            g_scanner_get_next_token (scanner); /* = */
            g_scanner_get_next_token (scanner); /* value */

            if (scanner->token != G_TOKEN_STRING)
            {
              command->type = SIM_COMMAND_TYPE_NONE;
              break;
            }
            command->data.event.userdata8 = g_strdup (scanner->value.v_string);
            break;

      case SIM_COMMAND_SYMBOL_USERDATA9:
            g_scanner_get_next_token (scanner); /* = */
            g_scanner_get_next_token (scanner); /* value */

            if (scanner->token != G_TOKEN_STRING)
            {
              command->type = SIM_COMMAND_TYPE_NONE;
              break;
            }
            command->data.event.userdata9 = g_strdup (scanner->value.v_string);
            break;

			default:
					  if (scanner->token == G_TOKEN_EOF)
					    break;
					  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_command_event_scan: error symbol unknown; Symbol number:%d. Event Rejected.",scanner->token);								
						return FALSE; //we will return with the first rare token
    }
  }
  while(scanner->token != G_TOKEN_EOF);

	return TRUE;
}

/*
 *
 *
 *
 */
static gboolean
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
            if (sim_string_is_number (scanner->value.v_string))
							command->id = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: Reload plugins event incorrect. Please check the id issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }
						break;

			default:
           	if (scanner->token == G_TOKEN_EOF)
							break;
					 
						g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_command_reload_plugins_scan: error symbol unknown");
          return FALSE;
    }
  }
  while(scanner->token != G_TOKEN_EOF);
	return TRUE;
}

/*
 *
 *
 *
 */
static gboolean
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
            if (sim_string_is_number (scanner->value.v_string))
              command->id = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: Reload sensors event incorrect. Please check the id issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }
						break;

			default:
						if (scanner->token == G_TOKEN_EOF)
					    break;
					  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_command_reload_sensors_scan: error symbol unknown");
						return FALSE;
    }
  }
  while(scanner->token != G_TOKEN_EOF);
	return TRUE;
}

/*
 *
 *
 *
 */
static gboolean
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
            if (sim_string_is_number (scanner->value.v_string))
              command->id = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: Reload hosts event incorrect. Please check the id issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }
						break;

			default:
						if (scanner->token == G_TOKEN_EOF)
							break;
						g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_command_reload_host_scan: error symbol unknown");
						return FALSE;
    }
  }
  while(scanner->token != G_TOKEN_EOF);
	return TRUE;
}

/*
 *
 *
 *
 */
static gboolean
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
            if (sim_string_is_number (scanner->value.v_string))
              command->id = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: Reload inets event incorrect. Please check the id issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }
						break;

			default:
						if (scanner->token == G_TOKEN_EOF)
							break;
						g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_command_reload_nets_scan: error symbol unknown");
						return FALSE;
    }
  }
  while(scanner->token != G_TOKEN_EOF);
	return TRUE;
}

/*
 *
 *
 *
 */
static gboolean
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
            if (sim_string_is_number (scanner->value.v_string))
              command->id = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: Reload policies event incorrect. Please check the id issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }
						break;

			default:
						if (scanner->token == G_TOKEN_EOF)
							break;
						g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_command_reload_policies_scan: error symbol unknown");
						return FALSE;
    }
  }
  while(scanner->token != G_TOKEN_EOF);
	return TRUE;
}

/*
 *
 *
 *
 */
static gboolean
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
            if (sim_string_is_number (scanner->value.v_string))
              command->id = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: Reload directives event incorrect. Please check the id issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }
						break;

			default:
						if (scanner->token == G_TOKEN_EOF)
							break;
						g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_command_reload_directives_scan: error symbol unknown");
						return FALSE;
    }
  }
  while(scanner->token != G_TOKEN_EOF);
	return TRUE;
}

/*
 *
 *
 *
 */
static gboolean
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
            if (sim_string_is_number (scanner->value.v_string))
              command->id = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: Reload all event incorrect. Please check the id issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }
						break;

			default:
						if (scanner->token == G_TOKEN_EOF)
							break;
						g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_command_reload_all_scan: error symbol unknown");
						return FALSE;
    }
  }
  while(scanner->token != G_TOKEN_EOF);
	return TRUE;
}

/*
 *
 *
 *
 */
static gboolean
sim_command_host_os_event_scan (SimCommand    *command,
																 GScanner      *scanner)
{
  g_return_if_fail (command);
  g_return_if_fail (SIM_IS_COMMAND (command));
  g_return_if_fail (scanner);

  command->type = SIM_COMMAND_TYPE_HOST_OS_EVENT;
  command->data.host_os_event.date = NULL;
  command->data.host_os_event.host = NULL;
  command->data.host_os_event.os = NULL;
  command->data.host_os_event.sensor = NULL;
  command->data.host_os_event.interface = NULL;
  command->data.host_os_event.plugin_id = 0;
  command->data.host_os_event.plugin_sid = 0;
  command->data.host_os_event.log = NULL;

  g_scanner_set_scope (scanner, SIM_COMMAND_SCOPE_HOST_OS_EVENT);
  do
  {
    g_scanner_get_next_token (scanner);
 
    switch (scanner->token)
    {
      case SIM_COMMAND_SYMBOL_DATE:
					  g_scanner_get_next_token (scanner); /* = */
					  g_scanner_get_next_token (scanner); /* value */

	  				if (scanner->token != G_TOKEN_STRING)
		    		{
					    command->type = SIM_COMMAND_TYPE_NONE;
	  	  		  break;
		  		  }

					  command->data.host_os_event.date = g_strdup (scanner->value.v_string);
		  			break;

      case SIM_COMMAND_SYMBOL_HOST:
	 					g_scanner_get_next_token (scanner); /* = */
			  		g_scanner_get_next_token (scanner); /* value */

			  		if (scanner->token != G_TOKEN_STRING)
				    {
			  		  command->type = SIM_COMMAND_TYPE_NONE;
			    	  break;
				    }
	
					  command->data.host_os_event.host = g_strdup (scanner->value.v_string);
		  			break;

      case SIM_COMMAND_SYMBOL_OS:
					  g_scanner_get_next_token (scanner); /* = */
					  g_scanner_get_next_token (scanner); /* value */

	  				if (scanner->token != G_TOKEN_STRING)
				    {
				      command->type = SIM_COMMAND_TYPE_NONE;
	  	  		  break;
				    }

					  command->data.host_os_event.os = g_strdup (scanner->value.v_string);
						break;

      case SIM_COMMAND_SYMBOL_PLUGIN_ID:
					  g_scanner_get_next_token (scanner); /* = */
					  g_scanner_get_next_token (scanner); /* value */	
	  
	  				if (scanner->token != G_TOKEN_STRING)
				    {
					    command->type = SIM_COMMAND_TYPE_NONE;
	    			  break;
				    }
            if (sim_string_is_number (scanner->value.v_string))
						  command->data.host_os_event.plugin_id = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: Host_OS event incorrect. Please check the plugin_id issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }
			      break;

      case SIM_COMMAND_SYMBOL_SENSOR:
      		  g_scanner_get_next_token (scanner); /* = */
		        g_scanner_get_next_token (scanner); /* value */

    		    if (scanner->token != G_TOKEN_STRING)
        		{
		          command->type = SIM_COMMAND_TYPE_NONE;
    		      break;
        		}

		       	command->data.host_os_event.sensor = g_strdup (scanner->value.v_string);
    			  break;

      case SIM_COMMAND_SYMBOL_INTERFACE:
            g_scanner_get_next_token (scanner); /* = */
            g_scanner_get_next_token (scanner); /* value */

            if (scanner->token != G_TOKEN_STRING)
            {
              command->type = SIM_COMMAND_TYPE_NONE;
              break;
            }

            command->data.host_os_event.interface = g_strdup (scanner->value.v_string);
            break;

      case SIM_COMMAND_SYMBOL_PLUGIN_SID:
					  g_scanner_get_next_token (scanner); /* = */
					  g_scanner_get_next_token (scanner); /* value */
	  
					  if (scanner->token != G_TOKEN_STRING)
				    {
      				command->type = SIM_COMMAND_TYPE_NONE;
				      break;
    				}
	           if (sim_string_is_number (scanner->value.v_string))
						  command->data.host_os_event.plugin_sid = strtol (scanner->value.v_string, (char **) NULL, 10);
    	       else
      	     {
        	     g_message("Error: Host_OS event incorrect. Please check the plugin_sid issued from the agent: %s", scanner->value.v_string);
          	   return FALSE;
	           }
  	     		break;

      case SIM_COMMAND_SYMBOL_LOG:
					  g_scanner_get_next_token (scanner); /* = */
					  g_scanner_get_next_token (scanner); /* value */

					  if (scanner->token != G_TOKEN_STRING)
				    {
				      command->type = SIM_COMMAND_TYPE_NONE;
      				break;
				    }

					  command->data.host_os_event.log = g_strdup (scanner->value.v_string);
			      break;

       default:
					  if (scanner->token == G_TOKEN_EOF)
					    break;
					  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_command_host_os_event_scan: error symbol unknown");
						return FALSE;
		}
 	}
  while(scanner->token != G_TOKEN_EOF);
	return TRUE;
}

/*
 *
 *
 *
 */
static gboolean
sim_command_host_mac_event_scan (SimCommand    *command,
																	GScanner      *scanner)
{
  g_return_if_fail (command);
  g_return_if_fail (SIM_IS_COMMAND (command));
  g_return_if_fail (scanner);

  command->type = SIM_COMMAND_TYPE_HOST_MAC_EVENT;
  command->data.host_mac_event.date = NULL;
  command->data.host_mac_event.host = NULL;
  command->data.host_mac_event.mac = NULL;
  command->data.host_mac_event.vendor = NULL;
  command->data.host_mac_event.sensor = NULL;
  command->data.host_mac_event.interface = NULL;
  command->data.host_mac_event.plugin_id = 0;
  command->data.host_mac_event.plugin_sid = 0;
  command->data.host_mac_event.log = NULL;

  g_scanner_set_scope (scanner, SIM_COMMAND_SCOPE_HOST_MAC_EVENT);
  do
  {
    g_scanner_get_next_token (scanner);
 
    switch (scanner->token)
    {
    	case SIM_COMMAND_SYMBOL_DATE:
					  g_scanner_get_next_token (scanner); /* = */
					  g_scanner_get_next_token (scanner); /* value */

			  		if (scanner->token != G_TOKEN_STRING)
				    {
	 			    	command->type = SIM_COMMAND_TYPE_NONE;
	   				  break;
				    }	
	
			 			command->data.host_mac_event.date = g_strdup (scanner->value.v_string);
						break;

      case SIM_COMMAND_SYMBOL_HOST:
					  g_scanner_get_next_token (scanner); /* = */
					  g_scanner_get_next_token (scanner); /* value */

					  if (scanner->token != G_TOKEN_STRING)
				    {
	    			  command->type = SIM_COMMAND_TYPE_NONE;
				      break;
				    }

	 					command->data.host_mac_event.host = g_strdup (scanner->value.v_string);
						break;

      case SIM_COMMAND_SYMBOL_MAC:
				  	g_scanner_get_next_token (scanner); /* = */
					  g_scanner_get_next_token (scanner); /* value */

					  if (scanner->token != G_TOKEN_STRING)
				    {
				      command->type = SIM_COMMAND_TYPE_NONE;
	    			  break;
				    }

					  command->data.host_mac_event.mac = g_strdup (scanner->value.v_string);
					  break;

      case SIM_COMMAND_SYMBOL_VENDOR:
					  g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
						{
							command->type = SIM_COMMAND_TYPE_NONE;
							break;
						}

						command->data.host_mac_event.vendor = g_strdup (scanner->value.v_string);
						break;

			case SIM_COMMAND_SYMBOL_SENSOR:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
						{
							command->type = SIM_COMMAND_TYPE_NONE;
							break;
						}

						command->data.host_mac_event.sensor = g_strdup (scanner->value.v_string);
						break;

			case SIM_COMMAND_SYMBOL_PLUGIN_ID:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */
				
						if (scanner->token != G_TOKEN_STRING)
						{
							command->type = SIM_COMMAND_TYPE_NONE;
							break;
						}
            if (sim_string_is_number (scanner->value.v_string))
							command->data.host_mac_event.plugin_id = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: Host_MAC event incorrect. Please check the plugin_id issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }

						break;
			case SIM_COMMAND_SYMBOL_PLUGIN_SID:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */
				
						if (scanner->token != G_TOKEN_STRING)
						{
							command->type = SIM_COMMAND_TYPE_NONE;
							break;
						}
            if (sim_string_is_number (scanner->value.v_string))
							command->data.host_mac_event.plugin_sid = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: Host_MAC event incorrect. Please check the plugin_sid issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }

						break;

			case SIM_COMMAND_SYMBOL_INTERFACE:
					  g_scanner_get_next_token (scanner); /* = */
					  g_scanner_get_next_token (scanner); /* value */

			  		if (scanner->token != G_TOKEN_STRING)
				    {
	 			    	command->type = SIM_COMMAND_TYPE_NONE;
	   				  break;
				    }	
	
			 			command->data.host_mac_event.interface = g_strdup (scanner->value.v_string);
						break;


			case SIM_COMMAND_SYMBOL_LOG:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
						{
							command->type = SIM_COMMAND_TYPE_NONE;
							break;
						}
	
						command->data.host_mac_event.log = g_strdup (scanner->value.v_string);
						break;

			default:
						if (scanner->token == G_TOKEN_EOF)
							break;

						g_message ("sim_command_host_mac_event_scan: error symbol unknown");
						return FALSE;
    }
  }
  while(scanner->token != G_TOKEN_EOF);
	return TRUE;
}

/*
 *
 * Host service new
 *
 */
static gboolean
sim_command_host_service_event_scan (SimCommand    *command,
																		  GScanner      *scanner)
{
  g_return_if_fail (command);
  g_return_if_fail (SIM_IS_COMMAND (command));
  g_return_if_fail (scanner);

  command->type = SIM_COMMAND_TYPE_HOST_SERVICE_EVENT;
  command->data.host_service_event.date = NULL;
  command->data.host_service_event.host = NULL;
  command->data.host_service_event.port = 0;
  command->data.host_service_event.protocol = 0;
  command->data.host_service_event.service = NULL;
  command->data.host_service_event.application = NULL;
  command->data.host_service_event.sensor = NULL;
  command->data.host_service_event.interface = NULL;
  command->data.host_service_event.plugin_id = 0;
  command->data.host_service_event.plugin_sid = 0;
  command->data.host_service_event.log = NULL;

  g_scanner_set_scope (scanner, SIM_COMMAND_SCOPE_HOST_SERVICE_EVENT);
  do
  {
    g_scanner_get_next_token (scanner);
 
    switch (scanner->token)
    {
      case SIM_COMMAND_SYMBOL_DATE:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
						{
							command->type = SIM_COMMAND_TYPE_NONE;
							break;
						}
						command->data.host_service_event.date = g_strdup (scanner->value.v_string);
						break;


			case SIM_COMMAND_SYMBOL_HOST:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
						{
							command->type = SIM_COMMAND_TYPE_NONE;
							break;
						}

						command->data.host_service_event.host = g_strdup (scanner->value.v_string);
						break;

			case SIM_COMMAND_SYMBOL_PORT:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */
						
						if (scanner->token != G_TOKEN_STRING)
						{
							command->type = SIM_COMMAND_TYPE_NONE;
							break;
						}
						if (sim_string_is_number (scanner->value.v_string))
							command->data.host_service_event.port = strtol (scanner->value.v_string, (char **) NULL, 10);
						else
						{
							g_message("Error: Host service event incorrect. Please check the port issued from the agent: %s", scanner->value.v_string);
							return FALSE;
						}
						break;

			case SIM_COMMAND_SYMBOL_PROTOCOL:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */
						
						if (scanner->token != G_TOKEN_STRING)
						{
							command->type = SIM_COMMAND_TYPE_NONE;
							break;
						}

	          if (sim_string_is_number (scanner->value.v_string))
              command->data.host_service_event.protocol = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: host service event incorrect. Please check the protocol issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }
						break;

			case SIM_COMMAND_SYMBOL_SERVICE:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
						{
							command->type = SIM_COMMAND_TYPE_NONE;
							break;
						}
						command->data.host_service_event.service = g_strdup (scanner->value.v_string);
						break;

			case SIM_COMMAND_SYMBOL_APPLICATION:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
						{
							command->type = SIM_COMMAND_TYPE_NONE;
							break;
						}
						command->data.host_service_event.application = g_strdup (scanner->value.v_string);
						break;

      case SIM_COMMAND_SYMBOL_SENSOR:
 						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
						{
							command->type = SIM_COMMAND_TYPE_NONE;
							break;
						}
		        command->data.host_service_event.sensor = g_strdup (scanner->value.v_string);
    			  break;

      case SIM_COMMAND_SYMBOL_INTERFACE:
            g_scanner_get_next_token (scanner); /* = */
            g_scanner_get_next_token (scanner); /* value */

            if (scanner->token != G_TOKEN_STRING)
            {
              command->type = SIM_COMMAND_TYPE_NONE;
              break;
            }

            command->data.host_service_event.interface = g_strdup (scanner->value.v_string);
            break;


			case SIM_COMMAND_SYMBOL_LOG:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
						{
							command->type = SIM_COMMAND_TYPE_NONE;
							break;
						}
						command->data.host_service_event.log = g_strdup (scanner->value.v_string);
						break;


			case SIM_COMMAND_SYMBOL_PLUGIN_ID:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */
						
						if (scanner->token != G_TOKEN_STRING)
						{
							command->type = SIM_COMMAND_TYPE_NONE;
							break;
						}
            if (sim_string_is_number (scanner->value.v_string))
							command->data.host_service_event.plugin_id = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: Host_service event incorrect. Please check the plugin_id issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }

						break;
				
			case SIM_COMMAND_SYMBOL_PLUGIN_SID:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */
						
						if (scanner->token != G_TOKEN_STRING)
						{
							command->type = SIM_COMMAND_TYPE_NONE;
							break;
						}
            if (sim_string_is_number (scanner->value.v_string))
							command->data.host_service_event.plugin_sid = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: host service event incorrect. Please check the plugin_sid issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }
						break;

			default:
						if (scanner->token == G_TOKEN_EOF)
							break;
						g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_command_host_service_event_scan: error symbol unknown");
						return FALSE;
    }
  }
  while(scanner->token != G_TOKEN_EOF);
	return TRUE;
}

/*
 *
 * HIDS
 *
 */
static gboolean
sim_command_host_ids_event_scan (SimCommand    *command,
																 GScanner      *scanner)
{
  char *temporal;
  g_return_if_fail (command);
  g_return_if_fail (SIM_IS_COMMAND (command));
  g_return_if_fail (scanner);

  command->type = SIM_COMMAND_TYPE_HOST_IDS_EVENT;
  command->data.host_ids_event.date = NULL;
  command->data.host_ids_event.host = NULL;
  command->data.host_ids_event.hostname = NULL;
  command->data.host_ids_event.event_type = NULL;
  command->data.host_ids_event.target = NULL;
  command->data.host_ids_event.what = NULL;
  command->data.host_ids_event.extra_data = NULL;
  command->data.host_ids_event.sensor = NULL;
  command->data.host_ids_event.plugin_id = 0;
  command->data.host_ids_event.plugin_sid = 0;
  command->data.host_ids_event.log = NULL;

	command->data.host_ids_event.filename = NULL;
	command->data.host_ids_event.username = NULL;
	command->data.host_ids_event.password = NULL;
	command->data.host_ids_event.userdata1 = NULL;
	command->data.host_ids_event.userdata2 = NULL;
	command->data.host_ids_event.userdata3 = NULL;
	command->data.host_ids_event.userdata4 = NULL;
	command->data.host_ids_event.userdata5 = NULL;
	command->data.host_ids_event.userdata6 = NULL;
	command->data.host_ids_event.userdata7 = NULL;
	command->data.host_ids_event.userdata8 = NULL;
	command->data.host_ids_event.userdata9 = NULL;


  g_scanner_set_scope (scanner, SIM_COMMAND_SCOPE_HOST_IDS_EVENT);
  do
  {
    g_scanner_get_next_token (scanner);
 
    switch (scanner->token)
    {
      case SIM_COMMAND_SYMBOL_DATE:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
						{
							command->type = SIM_COMMAND_TYPE_NONE;
							break;
						}
						command->data.host_ids_event.date = g_strdup (scanner->value.v_string);
						break;

			case SIM_COMMAND_SYMBOL_HOST:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
						{
							command->type = SIM_COMMAND_TYPE_NONE;
							break;
						}
						command->data.host_ids_event.host = g_strdup (scanner->value.v_string);
						break;

			case SIM_COMMAND_SYMBOL_HOSTNAME:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
						{
							command->type = SIM_COMMAND_TYPE_NONE;
							break;
						}

						command->data.host_ids_event.hostname = g_strdup (scanner->value.v_string);
						break;

			case SIM_COMMAND_SYMBOL_EVENT_TYPE:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
						{
							command->type = SIM_COMMAND_TYPE_NONE;
							break;
						}
						command->data.host_ids_event.event_type = g_strdup (scanner->value.v_string);
						break;

			case SIM_COMMAND_SYMBOL_TARGET:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */
						
						if (scanner->token != G_TOKEN_STRING)
						{
							command->type = SIM_COMMAND_TYPE_NONE;
							break;
						}
						command->data.host_ids_event.target = g_strdup (scanner->value.v_string);
						break;

			case SIM_COMMAND_SYMBOL_WHAT:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */
						
						if (scanner->token != G_TOKEN_STRING)
						{
							command->type = SIM_COMMAND_TYPE_NONE;
							break;
						}
						command->data.host_ids_event.what = g_strdup (scanner->value.v_string);
						break;

			case SIM_COMMAND_SYMBOL_EXTRA_DATA:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */
						
						if (scanner->token != G_TOKEN_STRING)
						{
							command->type = SIM_COMMAND_TYPE_NONE;
							break;
						}
						command->data.host_ids_event.extra_data = g_strdup (scanner->value.v_string);
						break;

			case SIM_COMMAND_SYMBOL_SENSOR:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */
						
						if (scanner->token != G_TOKEN_STRING)
						{
							command->type = SIM_COMMAND_TYPE_NONE;
							break;
						}
						command->data.host_ids_event.sensor = g_strdup (scanner->value.v_string);
						break;
							
			case SIM_COMMAND_SYMBOL_LOG:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */

						if (scanner->token != G_TOKEN_STRING)
						{
							command->type = SIM_COMMAND_TYPE_NONE;
							break;
						}
						command->data.host_ids_event.log = g_strdup (scanner->value.v_string);
						break;
						
			case SIM_COMMAND_SYMBOL_PLUGIN_ID:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */
						
						if (scanner->token != G_TOKEN_STRING)
						{
							command->type = SIM_COMMAND_TYPE_NONE;
							break;
						}
            if (sim_string_is_number (scanner->value.v_string))
							command->data.host_ids_event.plugin_id = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: HIDS event incorrect. Please check the plugin_id issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }
						break;

			case SIM_COMMAND_SYMBOL_PLUGIN_SID:
						g_scanner_get_next_token (scanner); /* = */
						g_scanner_get_next_token (scanner); /* value */
						
						if (scanner->token != G_TOKEN_STRING)
						{
							command->type = SIM_COMMAND_TYPE_NONE;
							break;
						}
            if (sim_string_is_number (scanner->value.v_string))
							command->data.host_ids_event.plugin_sid = strtol (scanner->value.v_string, (char **) NULL, 10);
            else
            {
              g_message("Error: HIDS event incorrect. Please check the plugin_sid issued from the agent: %s", scanner->value.v_string);
              return FALSE;
            }

						break;

      case SIM_COMMAND_SYMBOL_FILENAME:
            g_scanner_get_next_token (scanner); /* = */
            g_scanner_get_next_token (scanner); /* value */

            if (scanner->token != G_TOKEN_STRING)
            {
              command->type = SIM_COMMAND_TYPE_NONE;
              break;
            }
            command->data.host_ids_event.filename = g_strdup (scanner->value.v_string);
	           g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_command_host_ids_event_scan filename: %s", command->data.host_ids_event.filename);
													
            break;

      case SIM_COMMAND_SYMBOL_USERNAME:
            g_scanner_get_next_token (scanner); /* = */
            g_scanner_get_next_token (scanner); /* value */

            if (scanner->token != G_TOKEN_STRING)
            {
              command->type = SIM_COMMAND_TYPE_NONE;
              break;
            }
            command->data.host_ids_event.username = g_strdup (scanner->value.v_string);
            break;

      case SIM_COMMAND_SYMBOL_PASSWORD:
            g_scanner_get_next_token (scanner); /* = */
            g_scanner_get_next_token (scanner); /* value */

            if (scanner->token != G_TOKEN_STRING)
            {
              command->type = SIM_COMMAND_TYPE_NONE;
              break;
            }
            command->data.host_ids_event.password = g_strdup (scanner->value.v_string);
            break;

      case SIM_COMMAND_SYMBOL_USERDATA1:
            g_scanner_get_next_token (scanner); /* = */
            g_scanner_get_next_token (scanner); /* value */

            if (scanner->token != G_TOKEN_STRING)
            {
              command->type = SIM_COMMAND_TYPE_NONE;
              break;
            }
            command->data.host_ids_event.userdata1 = g_strdup (scanner->value.v_string);
            break;

      case SIM_COMMAND_SYMBOL_USERDATA2:
            g_scanner_get_next_token (scanner); /* = */
            g_scanner_get_next_token (scanner); /* value */

            if (scanner->token != G_TOKEN_STRING)
            {
              command->type = SIM_COMMAND_TYPE_NONE;
              break;
            }
            command->data.host_ids_event.userdata2 = g_strdup (scanner->value.v_string);
            break;

      case SIM_COMMAND_SYMBOL_USERDATA3:
            g_scanner_get_next_token (scanner); /* = */
            g_scanner_get_next_token (scanner); /* value */

            if (scanner->token != G_TOKEN_STRING)
            {
              command->type = SIM_COMMAND_TYPE_NONE;
              break;
            }
            command->data.host_ids_event.userdata3 = g_strdup (scanner->value.v_string);
            break;

      case SIM_COMMAND_SYMBOL_USERDATA4:
            g_scanner_get_next_token (scanner); /* = */
            g_scanner_get_next_token (scanner); /* value */

            if (scanner->token != G_TOKEN_STRING)
            {
              command->type = SIM_COMMAND_TYPE_NONE;
              break;
            }
            command->data.host_ids_event.userdata4 = g_strdup (scanner->value.v_string);
            break;

      case SIM_COMMAND_SYMBOL_USERDATA5:
            g_scanner_get_next_token (scanner); /* = */
            g_scanner_get_next_token (scanner); /* value */

            if (scanner->token != G_TOKEN_STRING)
            {
              command->type = SIM_COMMAND_TYPE_NONE;
              break;
            }
            command->data.host_ids_event.userdata5 = g_strdup (scanner->value.v_string);
            break;

      case SIM_COMMAND_SYMBOL_USERDATA6:
            g_scanner_get_next_token (scanner); /* = */
            g_scanner_get_next_token (scanner); /* value */

            if (scanner->token != G_TOKEN_STRING)
            {
              command->type = SIM_COMMAND_TYPE_NONE;
              break;
            }
            command->data.host_ids_event.userdata6 = g_strdup (scanner->value.v_string);
            break;

      case SIM_COMMAND_SYMBOL_USERDATA7:
            g_scanner_get_next_token (scanner); /* = */
            g_scanner_get_next_token (scanner); /* value */

            if (scanner->token != G_TOKEN_STRING)
            {
              command->type = SIM_COMMAND_TYPE_NONE;
              break;
            }
            command->data.host_ids_event.userdata7 = g_strdup (scanner->value.v_string);
            break;

      case SIM_COMMAND_SYMBOL_USERDATA8:
            g_scanner_get_next_token (scanner); /* = */
            g_scanner_get_next_token (scanner); /* value */

            if (scanner->token != G_TOKEN_STRING)
            {
              command->type = SIM_COMMAND_TYPE_NONE;
              break;
            }
            command->data.host_ids_event.userdata8 = g_strdup (scanner->value.v_string);
            break;

      case SIM_COMMAND_SYMBOL_USERDATA9:
            g_scanner_get_next_token (scanner); /* = */
            g_scanner_get_next_token (scanner); /* value */

            if (scanner->token != G_TOKEN_STRING)
            {
              command->type = SIM_COMMAND_TYPE_NONE;
              break;
            }
            command->data.host_ids_event.userdata9 = g_strdup (scanner->value.v_string);
            break;


			default:
						if (scanner->token == G_TOKEN_EOF)
							break;
						g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_command_host_ids_event_scan: error symbol unknown");
						return FALSE;
    }
  }
  while(scanner->token != G_TOKEN_EOF);
	return TRUE;
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
  gchar    *value = NULL;
  gchar    *state;

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

    case SIM_COMMAND_TYPE_CONNECT:
      switch (command->data.connect.type)
	{
	case SIM_SESSION_TYPE_SERVER:
	  value = g_strdup ("server");
	  break;
	case SIM_SESSION_TYPE_RSERVER:
	  value = g_strdup ("rserver");
	  break;
	case SIM_SESSION_TYPE_SENSOR:
	  value = g_strdup ("sensor");
	  break;
	case SIM_SESSION_TYPE_WEB:
	  value = g_strdup ("web");
	  break;
	default:
	  value = g_strdup ("web");
	}

      str = g_strdup_printf ("connect id=\"%d\" type=\"%s\"\n", command->id, value);
      g_free (value);
      break;

    case SIM_COMMAND_TYPE_EVENT:
      str = sim_event_to_string (command->data.event.event);
      break;

    case SIM_COMMAND_TYPE_WATCH_RULE:
      if (!command->data.watch_rule.str)
	break;

      str = g_strdup (command->data.watch_rule.str);
      break;

    case SIM_COMMAND_TYPE_SENSOR:
      str = g_strdup_printf ("sensor host=\"%s\" state=\"%s\"\n", 
			     command->data.sensor.host,
			     (command->data.sensor.state) ? "on" : "off");
      break;

    case SIM_COMMAND_TYPE_SENSOR_PLUGIN:
      switch (command->data.sensor_plugin.state)
	{
	case 1:
	  state = g_strdup ("start");
	  break;
	case 2:
	  state = g_strdup ("stop");
	  break;
	case 3:
	  state = g_strdup ("unknown");
	  break;
	default:
	  state = g_strdup ("unknown");
	}

      str = g_strdup_printf ("sensor-plugin sensor=\"%s\" plugin_id=\"%d\" state=\"%s\" enabled=\"%s\"\n",
			     command->data.sensor_plugin.sensor,
			     command->data.sensor_plugin.plugin_id,
			     state,
			     (command->data.sensor_plugin.enabled) ? "true" : "false");

      g_free (state);
      break;
    case SIM_COMMAND_TYPE_PLUGIN_START:
      str = g_strdup_printf ("plugin-start plugin_id=\"%d\"\n", command->data.plugin_start.plugin_id);
      break;
    case SIM_COMMAND_TYPE_PLUGIN_UNKNOWN:
      str = g_strdup_printf ("plugin-unknown plugin_id=\"%d\"\n", command->data.plugin_unknown.plugin_id);
      break;
    case SIM_COMMAND_TYPE_PLUGIN_STOP:
      str = g_strdup_printf ("plugin-stop plugin_id=\"%d\"\n", command->data.plugin_stop.plugin_id);
      break;
    case SIM_COMMAND_TYPE_PLUGIN_ENABLED:
      str = g_strdup_printf ("plugin-enabled plugin_id=\"%d\"\n", command->data.plugin_enabled.plugin_id);
      break;
    case SIM_COMMAND_TYPE_PLUGIN_DISABLED:
      str = g_strdup_printf ("plugin-disabled plugin_id=\"%d\"\n", command->data.plugin_disabled.plugin_id);
      break;

    default:
      g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_command_get_string: error command unknown");
      break;
    }

  return str;
}

/*
 * Transforms the data received in a new event object. Returns it.
 *
 */
SimEvent*
sim_command_get_event (SimCommand     *command)
{
  SimEventType   type;
  SimEvent      *event;
  struct tm      tm;

  g_return_val_if_fail (command, NULL);
  g_return_val_if_fail (SIM_IS_COMMAND (command), NULL);
  g_return_val_if_fail (command->type == SIM_COMMAND_TYPE_EVENT, NULL);
  g_return_val_if_fail (command->data.event.type, NULL);

  type = sim_event_get_type_from_str (command->data.event.type); //monitor or detector?

  if (type == SIM_EVENT_TYPE_NONE)
    return NULL;

  event = sim_event_new_from_type (type); //creates a new event just filled with type.

  if (command->data.event.date)
  {
    if (strptime (command->data.event.date, "%Y-%m-%d %H:%M:%S", &tm))
			event->time =  mktime (&tm);
		else
			return NULL;
  }
  if (command->data.event.sensor) 
    event->sensor = g_strdup (command->data.event.sensor);
	if (!gnet_inetaddr_new_nonblock (event->sensor, 0)) //sanitize
		return NULL;
					
  if (command->data.event.interface) 
    event->interface = g_strdup (command->data.event.interface);

  if (command->data.event.plugin_id)
    event->plugin_id = command->data.event.plugin_id;
	else
		return NULL;
	
  if (command->data.event.plugin_sid)
    event->plugin_sid = command->data.event.plugin_sid;

  if (command->data.event.protocol)
	{
    event->protocol = sim_protocol_get_type_from_str (command->data.event.protocol);

		if (event->protocol == SIM_PROTOCOL_TYPE_NONE)
		{
			if (sim_string_is_number (command->data.event.protocol))
				event->protocol = (SimProtocolType) atoi(command->data.event.protocol);
			else
				return NULL;
		}
	}
	else
		event->protocol = SIM_PROTOCOL_TYPE_OTHER; 
  
	//sanitize the event. An event ALWAYS must have a src_ip. And should have a dst_ip (not mandatory). 
	//If it's not defined, it will be 0.0.0.0 to avoid problems inside DB and other places.
  if (command->data.event.src_ip)
    event->src_ia = gnet_inetaddr_new_nonblock (command->data.event.src_ip, 0);
	if (!event->src_ia)
		return NULL;
	
  if (command->data.event.dst_ip)
    event->dst_ia = gnet_inetaddr_new_nonblock (command->data.event.dst_ip, 0);
	else
		event->dst_ia = gnet_inetaddr_new_nonblock ("0.0.0.0", 0);
		
  if (command->data.event.src_port)
    event->src_port = command->data.event.src_port;
  if (command->data.event.dst_port)
    event->dst_port = command->data.event.dst_port;

  if (command->data.event.condition)
    event->condition = sim_condition_get_type_from_str (command->data.event.condition);
  if (command->data.event.value)
    event->value = g_strdup (command->data.event.value);
  if (command->data.event.interval)
    event->interval = command->data.event.interval;

  if (command->data.event.data)
    event->data = g_strdup (command->data.event.data);
  if (command->data.event.log)
    event->log = g_strdup (command->data.event.log);

  if (command->data.event.snort_sid)
    event->snort_sid = command->data.event.snort_sid;

  if (command->data.event.snort_cid)
    event->snort_cid = command->data.event.snort_cid;

  event->reliability = command->data.event.reliability;
  event->asset_src = command->data.event.asset_src;
  event->asset_dst = command->data.event.asset_dst;
  event->risk_a = command->data.event.risk_a;
  event->risk_c = command->data.event.risk_c;
  event->alarm = command->data.event.alarm;

  if (command->data.event.priority)
  {
    if (command->data.event.priority < 0)
			event->priority = 0;
    else if (command->data.event.priority > 5)
			event->priority = 5;
    else
			event->priority = command->data.event.priority;
  }

	if (command->data.event.filename)
		event->filename = g_strdup (command->data.event.filename);
	if (command->data.event.username)
		event->username = g_strdup (command->data.event.username);
	if (command->data.event.password)
		event->password = g_strdup (command->data.event.password);
	if (command->data.event.userdata1)
		event->userdata1 = g_strdup (command->data.event.userdata1);
	if (command->data.event.userdata2)
		event->userdata2 = g_strdup (command->data.event.userdata2);
	if (command->data.event.userdata3)
		event->userdata3 = g_strdup (command->data.event.userdata3);
	if (command->data.event.userdata4)
		event->userdata4 = g_strdup (command->data.event.userdata4);
	if (command->data.event.userdata5)
		event->userdata5 = g_strdup (command->data.event.userdata5);
	if (command->data.event.userdata6)
		event->userdata6 = g_strdup (command->data.event.userdata6);
	if (command->data.event.userdata7)
		event->userdata7 = g_strdup (command->data.event.userdata7);
	if (command->data.event.userdata8)
		event->userdata8 = g_strdup (command->data.event.userdata8);
	if (command->data.event.userdata9)
		event->userdata9 = g_strdup (command->data.event.userdata9);

  return event;
}

/*
 *
 * FIXME: This function is not called from anywhere
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
    case SIM_COMMAND_TYPE_EVENT:
					break;
    case SIM_COMMAND_TYPE_SESSION_APPEND_PLUGIN:
					break;
    case SIM_COMMAND_TYPE_SESSION_REMOVE_PLUGIN:
					break;
    case SIM_COMMAND_TYPE_WATCH_RULE:
					break;
    default:
					return FALSE;			
		      break;
  }
  return TRUE;
}

// vim: set tabstop=2:

