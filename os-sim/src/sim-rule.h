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

#ifndef __SIM_RULE_H__
#define __SIM_RULE_H__ 1

#include <glib.h>
#include <glib-object.h>
#include <gnet.h>

#include "sim-enums.h"
#include "sim-alert.h"
#include "sim-action.h"
#include "sim-inet.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SIM_TYPE_RULE                  (sim_rule_get_type ())
#define SIM_RULE(obj)                  (G_TYPE_CHECK_INSTANCE_CAST (obj, SIM_TYPE_RULE, SimRule))
#define SIM_RULE_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST (klass, SIM_TYPE_RULE, SimRuleClass))
#define SIM_IS_RULE(obj)               (G_TYPE_CHECK_INSTANCE_TYPE (obj, SIM_TYPE_RULE))
#define SIM_IS_RULE_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), SIM_TYPE_RULE))
#define SIM_RULE_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), SIM_TYPE_RULE, SimRuleClass))

G_BEGIN_DECLS

typedef struct _SimRule         SimRule;
typedef struct _SimRuleClass    SimRuleClass;
typedef struct _SimRulePrivate  SimRulePrivate;
typedef struct _SimRuleVar      SimRuleVar;

struct _SimRule {
  GObject parent;

  SimRuleType      type;

  SimRulePrivate  *_priv;
};

struct _SimRuleClass {
  GObjectClass parent_class;
};

struct _SimRuleVar {
  SimRuleVarType   type;
  SimRuleVarType   attr;
  gint             level;
};

GType             sim_rule_get_type                        (void);
SimRule*          sim_rule_new                             (void);



gint              sim_rule_get_level                       (SimRule     *rule);
void              sim_rule_set_level                       (SimRule     *rule,
							    gint         level);
gboolean          sim_rule_get_sticky                      (SimRule     *rule);
void              sim_rule_set_sticky                      (SimRule     *rule,
							    gboolean     sticky);
SimRuleVarType    sim_rule_get_sticky_different            (SimRule     *rule);
void              sim_rule_set_sticky_different            (SimRule     *rule,
							    SimRuleVarType  sticky_different);

gint              sim_rule_get_protocol                    (SimRule     *rule);
void              sim_rule_set_protocol                    (SimRule     *rule,
							    gint       protocol);
gboolean          sim_rule_get_not                         (SimRule     *rule);
void              sim_rule_set_not                         (SimRule     *rule,
							    gboolean     not);
gchar*            sim_rule_get_name                        (SimRule     *rule);
void              sim_rule_set_name                        (SimRule     *rule,
							    const gchar *name);

gint              sim_rule_get_priority                    (SimRule     *rule);
void              sim_rule_set_priority                    (SimRule     *rule,
							    gint         priority);
gint              sim_rule_get_reliability                 (SimRule     *rule);
void              sim_rule_set_reliability                 (SimRule     *rule,
							    gint         reliability);
gboolean          sim_rule_get_rel_abs                     (SimRule     *rule);
void              sim_rule_set_rel_abs                     (SimRule     *rule,
							    gboolean     rel_abs);
GTime             sim_rule_get_time_out                    (SimRule     *rule);
void              sim_rule_set_time_out                    (SimRule     *rule,
							    GTime        time_out);
gint              sim_rule_get_occurrence                  (SimRule     *rule);
void              sim_rule_set_occurrence                  (SimRule     *rule,
							    gint         occurrence);
gint              sim_rule_get_count                       (SimRule     *rule);
void              sim_rule_set_count                       (SimRule     *rule,
							    gint         count);

SimConditionType  sim_rule_get_condition                   (SimRule     *rule);
void              sim_rule_set_condition                   (SimRule           *rule,
							    SimConditionType   condition);
gchar*            sim_rule_get_value                       (SimRule     *rule);
void              sim_rule_set_value                       (SimRule     *rule,
							    const gchar *value);
gint              sim_rule_get_interval                    (SimRule     *rule);
void              sim_rule_set_interval                    (SimRule     *rule,
							    gint         interval);
gboolean          sim_rule_get_absolute                    (SimRule     *rule);
void              sim_rule_set_absolute                    (SimRule     *rule,
							    gboolean     absolute);

gint              sim_rule_get_plugin_id                   (SimRule     *rule);
void              sim_rule_set_plugin_id                   (SimRule     *rule,
							    gint         plugin_id);
gint              sim_rule_get_plugin_sid                  (SimRule     *rule);
void              sim_rule_set_plugin_sid                  (SimRule     *rule,
							    gint         plugin_sid);

GInetAddr*        sim_rule_get_src_ia                      (SimRule     *rule);
void              sim_rule_set_src_ia                      (SimRule     *rule,
							    GInetAddr   *ia);
GInetAddr*        sim_rule_get_dst_ia                      (SimRule     *rule);
void              sim_rule_set_dst_ia                      (SimRule     *rule,
							    GInetAddr   *ia);

gint              sim_rule_get_src_port                    (SimRule     *rule);
void              sim_rule_set_src_port                    (SimRule     *rule,
							    gint         src_port);
gint              sim_rule_get_dst_port                    (SimRule     *rule);
void              sim_rule_set_dst_port                    (SimRule     *rule,
							    gint         dst_port);

void              sim_rule_append_plugin_sid               (SimRule     *rule,
							    gint         plugin_sid);
void              sim_rule_remove_plugin_sid               (SimRule     *rule,
							    gint         plugin_sid);
GList*            sim_rule_get_plugin_sids                 (SimRule     *rule);


void              sim_rule_append_src_inet                 (SimRule     *rule,
							    SimInet     *inet);
void              sim_rule_remove_src_inet                 (SimRule     *rule,
							    SimInet     *inet);
GList*            sim_rule_get_src_inets                   (SimRule     *rule);

void              sim_rule_append_dst_inet                 (SimRule     *rule,
							    SimInet     *inet);
void              sim_rule_remove_dst_inet                 (SimRule     *rule,
							    SimInet     *inet);
GList*            sim_rule_get_dst_inets                   (SimRule     *rule);


void              sim_rule_append_src_port                 (SimRule     *rule,
							    gint         src_port);
void              sim_rule_remove_src_port                 (SimRule     *rule,
							    gint         src_port);
GList*            sim_rule_get_src_ports                   (SimRule     *rule);

void              sim_rule_append_dst_port                 (SimRule     *rule,
							    gint         dst_port);
void              sim_rule_remove_dst_port                 (SimRule     *rule,
							    gint         dst_port);
GList*            sim_rule_get_dst_ports                   (SimRule     *rule);

void              sim_rule_append_protocol                 (SimRule     *rule,
							    SimProtocolType  protocol);
void              sim_rule_remove_protocol                 (SimRule     *rule,
							    SimProtocolType  protocol);
GList*            sim_rule_get_protocols                   (SimRule     *rule);


void              sim_rule_append_var                      (SimRule     *rule,
							    SimRuleVar  *var);
GList*            sim_rule_get_vars                        (SimRule     *rule);

SimRule*          sim_rule_clone                           (SimRule     *rule);

void              sim_rule_set_alert_data                  (SimRule     *rule,
							    SimAlert    *alert);
gboolean          sim_rule_match_by_alert                  (SimRule     *rule,
							    SimAlert    *alert);

void              sim_rule_print                           (SimRule     *rule);

gchar*            sim_rule_to_string                       (SimRule     *rule);

G_END_DECLS

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SIM_RULE_H__ */
