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

#ifndef __SIM_UTIL_H__
#define __SIM_UTIL_H__ 1

#include <glib.h>
#include <gnet.h>

#include "sim-enums.h"
#include "sim-database.h"

G_BEGIN_DECLS

//this struct is valid just for the Policy groups.
typedef struct _Plugin_PluginSid            Plugin_PluginSid;

struct _Plugin_PluginSid
{
  gint  plugin_id;
  GList *plugin_sid; // *gint list
};

typedef struct _SimPortProtocol    SimPortProtocol;
struct _SimPortProtocol {
  gint              port;
  SimProtocolType   protocol;
};

SimPortProtocol* sim_port_protocol_new (gint              port,
					SimProtocolType   protocol);

gboolean sim_port_protocol_equal (SimPortProtocol  *pp1,
				  SimPortProtocol  *pp2);

SimProtocolType sim_protocol_get_type_from_str (const gchar  *str);
gchar*          sim_protocol_get_str_from_type (SimProtocolType type);

SimConditionType sim_condition_get_type_from_str (const gchar  *str);
gchar*           sim_condition_get_str_from_type (SimConditionType  type);

SimRuleVarType sim_get_rule_var_from_char (const gchar *var);

SimAlarmRiskType sim_get_alarm_risk_from_char (const gchar *var);
SimAlarmRiskType sim_get_alarm_risk_from_risk (gint risk);

GList       *sim_get_ias (const gchar *value);
GList       *sim_get_inets (const gchar *value);
GList       *sim_get_SimInet_from_string (const gchar *value);

GList       *sim_string_hash_to_list (GHashTable *hash_table);

/*
 * File management utility functions
 */

gchar    *sim_file_load (const gchar *filename);
gboolean  sim_file_save (const gchar *filename, const gchar *buffer, gint len);

gulong						sim_inetaddr_aton						(GInetAddr		*ia);
inline gulong			sim_inetaddr_ntohl					(GInetAddr		*ia);
inline gulong			sim_ipchar_2_ulong					(gchar				*ip);
inline gboolean		sim_string_is_number				(gchar				*string, 
																							gboolean      may_be_float);
inline gchar *		sim_string_remove_char			(gchar *string,
																								gchar c);
inline gchar *		sim_string_substitute_char  (gchar *string,
										                            gchar c_orig,
										                            gchar c_dest);

guint							sim_g_strv_length						(gchar				**str_array);
gboolean					sim_base64_encode						(gchar *_in, 
																								guint inlen,
																								gchar *_out,
																								guint outmax,
																								guint *outlen);
gboolean					sim_base64_decode						(	gchar *in,
																								guint inlen, 
																								gchar *out, 
																								guint *outlen);

size_t						sim_strnlen									(	const char *str,
																								size_t maxlen);
gchar*						sim_normalize_host_mac			(gchar *old_mac);

	
G_END_DECLS

#endif
// vim: set tabstop=2:
