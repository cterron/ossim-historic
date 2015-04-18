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

#ifndef __SIM_POLICY_H__
#define __SIM_POLICY_H__ 1

#include <glib.h>
#include <glib-object.h>
#include <gnet.h>
#include <libgda/libgda.h>

#include "sim-enums.h"
#include "sim-util.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SIM_TYPE_POLICY                  (sim_policy_get_type ())
#define SIM_POLICY(obj)                  (G_TYPE_CHECK_INSTANCE_CAST (obj, SIM_TYPE_POLICY, SimPolicy))
#define SIM_POLICY_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST (klass, SIM_TYPE_POLICY, SimPolicyClass))
#define SIM_IS_POLICY(obj)               (G_TYPE_CHECK_INSTANCE_TYPE (obj, SIM_TYPE_POLICY))
#define SIM_IS_POLICY_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), SIM_TYPE_POLICY))
#define SIM_POLICY_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), SIM_TYPE_POLICY, SimPolicyClass))

G_BEGIN_DECLS

typedef struct _SimPolicy        SimPolicy;
typedef struct _SimPolicyClass   SimPolicyClass;
typedef struct _SimPolicyPrivate SimPolicyPrivate;

struct _SimPolicy {
  GObject parent;

  SimPolicyPrivate *_priv;
};

struct _SimPolicyClass {
  GObjectClass parent_class;
};


GType             sim_policy_get_type                        (void);

SimPolicy*        sim_policy_new                             (void);
SimPolicy*        sim_policy_new_from_dm                     (GdaDataModel     *dm,
							      gint              row);

gint              sim_policy_get_id                          (SimPolicy        *policy);
void              sim_policy_set_id                          (SimPolicy        *policy,
							      gint              id);

gint              sim_policy_get_priority                    (SimPolicy        *policy);
void              sim_policy_set_priority                    (SimPolicy        *policy,
							      gint              priority);

gint              sim_policy_get_begin_day                   (SimPolicy        *policy);
void              sim_policy_set_begin_day                   (SimPolicy        *policy,
							      gint              begin_day);

gint              sim_policy_get_end_day                     (SimPolicy        *policy);
void              sim_policy_set_end_day                     (SimPolicy        *policy,
							      gint              end_day);

gint              sim_policy_get_begin_hour                  (SimPolicy        *policy);
void              sim_policy_set_begin_hour                  (SimPolicy        *policy,
							      gint              begin_hour);

gint              sim_policy_get_end_hour                    (SimPolicy        *policy);
void              sim_policy_set_end_hour                    (SimPolicy        *policy,
							      gint              end_hour);

/* Sources Inet Address */
void              sim_policy_append_src_ia                   (SimPolicy        *policy,
							      GInetAddr        *ia);
void              sim_policy_remove_src_ia                   (SimPolicy        *policy,
							      GInetAddr        *ia);
GList*            sim_policy_get_src_ias                     (SimPolicy        *policy);

/* Destination Inet Address */
void              sim_policy_append_dst_ia                   (SimPolicy        *policy,
							      GInetAddr        *ia);
void              sim_policy_remove_dst_ia                   (SimPolicy        *policy,
							      GInetAddr        *ia);
GList*            sim_policy_get_dst_ias                     (SimPolicy        *policy);

/* Ports */
void              sim_policy_append_port                     (SimPolicy        *policy,
							      SimPortProtocol  *pp);
void              sim_policy_remove_port                     (SimPolicy        *policy,
							      SimPortProtocol  *pp);
GList*            sim_policy_get_ports                       (SimPolicy        *policy);

/* Signatures */
void              sim_policy_append_signature                (SimPolicy        *policy,
							      gchar            *signature);
void              sim_policy_remove_signature                (SimPolicy        *policy,
							      gchar            *signature);
GList*            sim_policy_get_signatures                  (SimPolicy        *policy);

/* Sensors */
GList*            sim_policy_get_sensors                     (SimPolicy        *policy);

gboolean          sim_policy_match                           (SimPolicy        *policy,
							      gint              date,
							      GInetAddr        *src_ia,
							      GInetAddr        *dst_ia,
							      SimPortProtocol  *pp,
							      gchar            *signature);

G_END_DECLS

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SIM_POLICY_H__ */
