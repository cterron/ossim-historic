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

#ifndef __SIM_EVENT_H__
#define __SIM_EVENT_H__ 1

#include <glib.h>
#include <glib-object.h>
#include <gnet.h>

#include "sim-enums.h"
#include "sim-plugin.h"
#include "sim-plugin-sid.h"
#include "sim-packet.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SIM_TYPE_EVENT                  (sim_event_get_type ())
#define SIM_EVENT(obj)                  (G_TYPE_CHECK_INSTANCE_CAST (obj, SIM_TYPE_EVENT, SimEvent))
#define SIM_EVENT_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST (klass, SIM_TYPE_EVENT, SimEventClass))
#define SIM_IS_EVENT(obj)               (G_TYPE_CHECK_INSTANCE_TYPE (obj, SIM_TYPE_EVENT))
#define SIM_IS_EVENT_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), SIM_TYPE_EVENT))
#define SIM_EVENT_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), SIM_TYPE_EVENT, SimEventClass))

typedef struct	_SimRole				SimRole;	//different event role

struct	_SimRole			//this hasn't got any data from sensor associated.
{
  gboolean  correlate;
  gboolean  cross_correlate;
  gboolean  store;
  gboolean  qualify;
  gboolean  resend_event;
  gboolean  resend_alarm;
};

typedef struct _SimHostServices	SimHostServices;	//used only for cross_correlation at this moment.
struct _SimHostServices
{
	gchar	*ip;
	gint port;
	gint protocol;
	gchar	*service;
	gchar *version;
	gchar	*date;
	gchar	*sensor;
};

G_BEGIN_DECLS

typedef struct _SimEvent        SimEvent;
typedef struct _SimEventClass   SimEventClass;

struct _SimEvent {
  GObject parent;

  guint              id;
  guint              id_tmp;	//this applies only to table event_tmp, the column "id". It has nothing to do with the above id.
															//This id is needed to keep control about what events from that table.
  guint              snort_sid;
  guint              snort_cid;

  SimEventType       type;

  /* Event Info */
  time_t              time;
  time_t              diff_time; //as soon as the event arrives, this is setted. Here is stored the difference between the parsed time from agent log
																	//line, and the time when the event arrives to server.
  gchar             *sensor;
  gchar             *interface;

  /* Plugin Info */
  gint               plugin_id;
  gint               plugin_sid;
  gchar*             plugin_sid_name;	//needed for event_tmp table.

  /* Plugin Type Detector */
  SimProtocolType    protocol;
  GInetAddr         *src_ia;
  GInetAddr         *dst_ia;
  gint               src_port;
  gint               dst_port;

  /* Plugin Type Monitor */
  SimConditionType   condition;
  gchar             *value;
  gint               interval;

  /* Extra data */
  gboolean           alarm;
  gint               priority;
  gint               reliability;
  gint               asset_src;
  gint               asset_dst;
  gdouble            risk_c;
  gdouble            risk_a;

  gchar             *data;
  gchar             *log;

  SimPlugin         *plugin;
  SimPluginSid      *pluginsid;

  /* Directives */
  gboolean           sticky;
  gboolean           match;  // TRUE if this has been matched the rule in sim_rule_match_by_event()
  gboolean           matched;
  gint               count;
  gint               level;
  guint32            backlog_id;

  /* replication  server */
  gboolean           rserver;    

	gchar							**data_storage; // This variable must be used ONLY to pass data between the sim-session and 
																		//sim-organizer, where the event is stored in DB.
	gboolean					store_in_DB;		//variable used to know if this specific event should be stored in DB or not. Used in Policy.
  gchar              *buffer;				//used to speed up the resending events so it's not needed to turn it again into a string

	gboolean					is_correlated;	//Just needed for MAC, OS, Service and HIDS events.
																		// Take an example: server1 resend data to server2. We have correlated in server1 a MAC event.
																		// Then we resend the event to server2 in both ways: "host_mac_event...." and "event...". Obviously,
																		// "event..." is the event correlated, with priority, risk information and so on. But we don't want
																		// to re-correlate "host_mac_event...", because the correlation information is in "event...". So in 
																		// sim_organizer_correlation() we check this variable. Also, in this way, we are able to correlate
																		// the event with another event wich arrives to server2. 
	gboolean					is_prioritized;	// Needed to know in the master server if the event sent from children server has the priority changed or not.
	gboolean					is_reliability_setted; //I dont' know how to reduce this variable, it's auto-explained :)
	SimRole						*role;

	/* additional data (not necessary used) */
	gchar							*filename;
	gchar							*username;
	gchar							*password;
	gchar							*userdata1;
	gchar							*userdata2;
	gchar							*userdata3;
	gchar							*userdata4;
	gchar							*userdata5;
	gchar							*userdata6;
	gchar							*userdata7;
	gchar							*userdata8;
	gchar							*userdata9;
	/* packet data */
	SimPacket *packet;
	
};

struct _SimEventClass {
  GObjectClass parent_class;
};

GType			sim_event_get_type								(void);
SimEvent*	sim_event_new											(void);
SimEvent*	sim_event_new_from_type						(SimEventType	 type);

SimEvent*	sim_event_clone										(SimEvent	*event);

gchar*		sim_event_get_insert_clause				(SimEvent	*event);
gchar*		sim_event_get_update_clause				(SimEvent	*event);
gchar*    sim_event_get_replace_clause      (SimEvent   *event);

gchar*		sim_event_get_alarm_insert_clause	(SimEvent	*event);
gchar*		sim_event_get_insert_into_event_tmp_clause (SimEvent   *event);

gchar*		sim_event_to_string								(SimEvent	*event);

void			sim_event_print										(SimEvent	*event);

gchar*		sim_event_get_msg									(SimEvent	*event);
gboolean	sim_event_is_special							(SimEvent *event);
gchar*    sim_event_get_str_from_type       (SimEventType type);

G_END_DECLS

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SIM_EVENT_H__ */

// vim: set tabstop=2:

