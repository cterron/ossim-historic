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

#ifndef __SIM_COMMAND_H__
#define __SIM_COMMAND_H__ 1

#include <glib.h>
#include <glib-object.h>
#include <gnet.h>

#include "sim-enums.h"
#include "sim-alert.h"
#include "sim-rule.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SIM_TYPE_COMMAND                  (sim_command_get_type ())
#define SIM_COMMAND(obj)                  (G_TYPE_CHECK_INSTANCE_CAST (obj, SIM_TYPE_COMMAND, SimCommand))
#define SIM_COMMAND_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST (klass, SIM_TYPE_COMMAND, SimCommandClass))
#define SIM_IS_COMMAND(obj)               (G_TYPE_CHECK_INSTANCE_TYPE (obj, SIM_TYPE_COMMAND))
#define SIM_IS_COMMAND_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), SIM_TYPE_COMMAND))
#define SIM_COMMAND_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), SIM_TYPE_COMMAND, SimCommandClass))

G_BEGIN_DECLS

typedef struct _SimCommand        SimCommand;
typedef struct _SimCommandClass   SimCommandClass;

struct _SimCommand {
  GObject parent;

  SimCommandType      type;
  gint                id;

  union {
    struct {
      gchar          *username;
      gchar          *password;
      SimSessionType  type;
    } connect;

    struct {
      gint            id;
      SimPluginType   type;
      gchar          *name;
      gboolean        enabled;
      gint            state;
    } session_append_plugin;

    struct {
      gint            id;
      SimPluginType   type;
      gchar          *name;
      gboolean        enabled;
      gint            state;
    } session_remove_plugin;

    struct {
      gint            id;
    } server_get_sensor_plugins;

    struct {
      gint            id;
      gchar          *sensor;
      gint            plugin_id;
      gboolean        enabled;
      gint            state;
    } sensor_plugin;

    struct {
      gint            id;
      gchar          *sensor;
      gint            plugin_id;
    } sensor_plugin_start;

    struct {
      gint            id;
      gchar          *sensor;
      gint            plugin_id;
    } sensor_plugin_stop;

    struct {
      gint            id;
      gchar          *sensor;
      gint            plugin_id;
    } sensor_plugin_enabled;

    struct {
      gint            id;
      gchar          *sensor;
      gint            plugin_id;
    } sensor_plugin_disabled;

    struct {
      gint            id;
      gint            plugin_id;
    } plugin_start;

    struct {
      gint            id;
      gint            plugin_id;
    } plugin_unknown;

    struct {
      gint            id;
      gint            plugin_id;
    } plugin_stop;

    struct {
      gint            id;
      gint            plugin_id;
    } plugin_enabled;

    struct {
      gint            id;
      gint            plugin_id;
    } plugin_disabled;

    struct {
      /* Alert Info */
      gchar             *type;
      gchar             *date;
      gchar             *sensor;
      gchar             *interface;

      /* Plugin Info */
      gint               plugin_id;
      gint               plugin_sid;

      /* Plugin Type Detector */
      gint               priority;
      gchar             *protocol;
      gchar             *src_ip;
      gchar             *dst_ip;
      gint               src_port;
      gint               dst_port;

      /* Plugin Type Monitor */
      gchar             *condition;
      gchar             *value;
      gint               interval;

      gchar             *data;
      gchar             *log;

      guint32            snort_sid;
      guint32            snort_cid;

      gint               reliability;
      gint               asset_src;
      gint               asset_dst;
      gdouble            risk_c;
      gdouble            risk_a;
      gboolean		 alarm;

      SimAlert          *alert;
    } alert;

    struct {
      gchar             *str;
    } watch_rule;


    struct {
      gchar             *date;
      gchar             *host;
      gchar             *os;

      gint               plugin_id;
      gint               plugin_sid;

      gchar             *log;
    } host_os_change;

    struct {
      gchar             *date;
      gchar             *host;
      gchar             *mac;
      gchar             *vendor;

      gint               plugin_id;
      gint               plugin_sid;

      gchar             *log;
    } host_mac_change;

    struct {
      gchar             *date;
      gchar             *host;
      gint               port;
      gint               protocol;
      gchar             *service;
      gchar             *application;

      gint               plugin_id;
      gint               plugin_sid;

      gchar             *log;
    } host_service_new;

    struct {
      gchar             *host;
      gchar             *hostname;
      gchar             *event_type;
      gchar             *target;
      gchar             *what;
      gchar             *extra_data;
      gchar             *sensor;
      gchar             *date;

      gint               plugin_id;
      gint               plugin_sid;

      gchar             *log;
    } host_ids_event;

    struct {
      gint               id;
    } server_get_sensors;

    struct {
      gchar             *host;
      gboolean           state;
    } sensor;

  } data;
};

struct _SimCommandClass {
  GObjectClass parent_class;
};

GType             sim_command_get_type                        (void);
SimCommand*       sim_command_new                             (void);
SimCommand*       sim_command_new_from_buffer                 (const gchar     *buffer);
SimCommand*       sim_command_new_from_type                   (SimCommandType   type);
SimCommand*       sim_command_new_from_rule                   (SimRule         *rule);

gchar*            sim_command_get_string                      (SimCommand      *command);

SimAlert*         sim_command_get_alert                       (SimCommand      *command);

gboolean          sim_command_is_valid                        (SimCommand      *command);

G_END_DECLS

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SIM_COMMAND_H__ */
