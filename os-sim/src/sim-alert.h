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

#ifndef __SIM_ALERT_H__
#define __SIM_ALERT_H__ 1

#include <glib.h>
#include <glib-object.h>
#include <gnet.h>

#include "sim-enums.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SIM_TYPE_ALERT                  (sim_alert_get_type ())
#define SIM_ALERT(obj)                  (G_TYPE_CHECK_INSTANCE_CAST (obj, SIM_TYPE_ALERT, SimAlert))
#define SIM_ALERT_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST (klass, SIM_TYPE_ALERT, SimAlertClass))
#define SIM_IS_ALERT(obj)               (G_TYPE_CHECK_INSTANCE_TYPE (obj, SIM_TYPE_ALERT))
#define SIM_IS_ALERT_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), SIM_TYPE_ALERT))
#define SIM_ALERT_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), SIM_TYPE_ALERT, SimAlertClass))

G_BEGIN_DECLS

typedef struct _SimAlert        SimAlert;
typedef struct _SimAlertClass   SimAlertClass;

struct _SimAlert {
  GObject parent;

  SimAlertType       type;

  /* Alert Info */
  GTime              time;
  gchar             *sensor;
  gchar             *interface;

  /* Plugin Info */
  gint               plugin_id;
  gint               plugin_sid;

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

  guint              sid;
  guint              cid;

  gboolean           match;
};

struct _SimAlertClass {
  GObjectClass parent_class;
};

GType             sim_alert_get_type                        (void);
SimAlert*         sim_alert_new                             (void);
SimAlert*         sim_alert_new_from_type                   (SimAlertType    type);

SimAlert*         sim_alert_clone                           (SimAlert       *alert);


gchar*            sim_alert_get_ossim_insert_clause         (SimAlert       *alert);

void              sim_alert_print                           (SimAlert       *alert);

G_END_DECLS

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SIM_ALERT_H__ */
