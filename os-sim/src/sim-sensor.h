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

#ifndef __SIM_SENSOR_H__
#define __SIM_SENSOR_H__ 1

#include <glib.h>
#include <glib-object.h>
#include <gnet.h>
#include <libgda/libgda.h>

#include "sim-enums.h"
#include "sim-plugin.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SIM_TYPE_SENSOR                  (sim_sensor_get_type ())
#define SIM_SENSOR(obj)                  (G_TYPE_CHECK_INSTANCE_CAST (obj, SIM_TYPE_SENSOR, SimSensor))
#define SIM_SENSOR_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST (klass, SIM_TYPE_SENSOR, SimSensorClass))
#define SIM_IS_SENSOR(obj)               (G_TYPE_CHECK_INSTANCE_TYPE (obj, SIM_TYPE_SENSOR))
#define SIM_IS_SENSOR_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), SIM_TYPE_SENSOR))
#define SIM_SENSOR_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), SIM_TYPE_SENSOR, SimSensorClass))

G_BEGIN_DECLS

typedef struct _event_kind	event_kind;
	
struct _event_kind     //used to store temporary (thanks to sim_container_set_sensor_events) how much events has arrived
{
  gint  events;
  gint  host_os_events;
  gint  host_mac_events;
  gint  host_ids_events;
  gint  host_service_events;
};


typedef struct _SimSensor         SimSensor;
typedef struct _SimSensorClass    SimSensorClass;
typedef struct _SimSensorPrivate  SimSensorPrivate;

struct _SimSensor {
  GObject parent;

  SimSensorPrivate  *_priv;
};

struct _SimSensorClass {
  GObjectClass parent_class;
};

GType             sim_sensor_get_type                        (void);
SimSensor*        sim_sensor_new                             (void);
SimSensor*        sim_sensor_new_from_hostname               (gchar *sensor_ip);

SimSensor*        sim_sensor_new_from_dm                     (GdaDataModel     *dm,
							      gint              row);

SimSensor*        sim_sensor_clone                           (SimSensor   *sensor);

gchar*            sim_sensor_get_name                        (SimSensor   *sensor);
void              sim_sensor_set_name                        (SimSensor   *sensor,
							      gchar       *name);
GInetAddr*        sim_sensor_get_ia                          (SimSensor   *sensor);
void              sim_sensor_set_ia                          (SimSensor   *sensor,
							     GInetAddr  *id);
gint              sim_sensor_get_port                        (SimSensor   *sensor);
void              sim_sensor_set_port                        (SimSensor   *sensor,
							     gint        port);
gboolean          sim_sensor_is_connect                      (SimSensor   *sensor);
void              sim_sensor_set_connect                     (SimSensor   *sensor,
							     gboolean    connect);
gboolean          sim_sensor_is_compress                     (SimSensor   *sensor);
void              sim_sensor_set_compress                    (SimSensor   *sensor,
							     gboolean    compress);
gboolean          sim_sensor_is_ssl                          (SimSensor   *sensor);
void              sim_sensor_set_ssl                         (SimSensor   *sensor,
							     gboolean    ssl);

void              sim_sensor_insert_plugin                   (SimSensor    *sensor,
							     SimPlugin   *plugin);
void              sim_sensor_remove_plugin                   (SimSensor    *sensor,
							     SimPlugin   *plugin);
SimPlugin*        sim_sensor_get_plugin_by_id                (SimSensor    *sensor,
							     gint         id);
GList*            sim_sensor_get_plugins                     (SimSensor    *sensor);
gboolean          sim_sensor_has_plugin_by_type              (SimSensor       *sensor,
							     SimPluginType   type);

GList*            sim_sensor_get_plugins                     (SimSensor    *sensor);

inline	void			sim_sensor_add_number_events								(SimSensor	*sensor);
inline	void			sim_sensor_add_number_host_os_events				(SimSensor	*sensor);
inline	void			sim_sensor_add_number_host_mac_events				(SimSensor	*sensor);
inline	void			sim_sensor_add_number_host_service_events		(SimSensor	*sensor);
inline	void			sim_sensor_add_number_host_ids_events				(SimSensor	*sensor);

void							sim_sensor_debug_events_number (SimSensor  *sensor); //debug function
void							sim_sensor_reset_events_number							(SimSensor	*sensor);
event_kind				sim_sensor_get_events_number								(SimSensor *sensor);
	
G_END_DECLS

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SIM_SENSOR_H__ */
// vim: set tabstop=2:
