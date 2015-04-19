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

#ifndef __SIM_SESSION_H__
#define __SIM_SESSION_H__ 1 

#include <glib.h>
#include <glib-object.h>
#include <gnet.h>

#include "sim-enums.h"
#include "sim-command.h"
#include "sim-config.h"
#include "sim-database.h"
#include "sim-sensor.h"


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */



#define SIM_TYPE_SESSION                  (sim_session_get_type ())
#define SIM_SESSION(obj)                  (G_TYPE_CHECK_INSTANCE_CAST (obj, SIM_TYPE_SESSION, SimSession))
#define SIM_SESSION_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST (klass, SIM_TYPE_SESSION, SimSessionClass))
#define SIM_IS_SESSION(obj)               (G_TYPE_CHECK_INSTANCE_TYPE (obj, SIM_TYPE_SESSION))
#define SIM_IS_SESSION_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), SIM_TYPE_SESSION))
#define SIM_SESSION_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), SIM_TYPE_SESSION, SimSessionClass))

//this define's are usefull for sim_container_set_sensor_event_number(), wich is called when
//an event is issued from the agent. We use here the plugin_id to identify the "special" events. Anyway this
//may change in the future, so is better to keep the numbers controlled with a define
#define SIM_EVENT_EVENT								1
#define SIM_EVENT_HOST_OS_EVENT				1511
#define SIM_EVENT_HOST_MAC_EVENT			1512
#define SIM_EVENT_HOST_SERVICE_EVENT	1516
#define SIM_EVENT_HOST_IDS_EVENT			4001

//plugin sids to store MAC changes, OS fingerprinting, etc. This plugin_sids are only used from some plugins, and
//they're needed because when the message arrives, we don't know yet the plugin_sid and we have to deduce it.
#define EVENT_NEW     1
#define EVENT_CHANGE  2
#define EVENT_DELETED 3
#define EVENT_SAME    4
#define EVENT_UNKNOWN 5
	
G_BEGIN_DECLS

typedef struct _SimSession        SimSession;
typedef struct _SimSessionClass   SimSessionClass;
typedef struct _SimSessionPrivate SimSessionPrivate;

struct _SimSession {
  GObject parent;

  SimSessionType      type;

  SimSessionPrivate  *_priv;
};

struct _SimSessionClass {
  GObjectClass parent_class;
};

GType             sim_session_get_type                        (void);
SimSession*       sim_session_new                             (GObject       *server,
																													     SimConfig     *config,
																												       GTcpSocket    *socket);

GInetAddr*        sim_session_get_ia                          (SimSession *session);
gboolean          sim_session_read                            (SimSession  *session);
gint              sim_session_write                           (SimSession  *session,
																												       SimCommand  *command);
guint							sim_session_write_from_buffer								(SimSession  *session,
																																gchar			 *buffer);
gboolean          sim_session_has_plugin_type                 (SimSession     *session,
																												       SimPluginType   type);
gboolean          sim_session_has_plugin_id                   (SimSession     *session, 
																																gint            plugin_id);
//ag, so ugly to do this!
#ifndef __SIM_SERVER_H__
#include "sim-server.h"
SimServer*			  sim_session_get_server                      (SimSession *session);
#endif	

SimSensor*        sim_session_get_sensor                      (SimSession *session);
gboolean          sim_session_is_sensor                       (SimSession *session);
gboolean          sim_session_is_server                       (SimSession *session);
void              sim_session_close                           (SimSession *session);
void							sim_session_resend_command									(SimSession *session,
																		                            SimCommand  *command);
void							sim_session_resend_buffer										(gchar *buffer);
gchar*						sim_session_get_hostname										(SimSession *session);	
void							sim_session_set_hostname										(SimSession *session,
																																gchar			*hostname);

G_END_DECLS

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __SIM_SESSION_H__ */
// vim: set tabstop=2:
