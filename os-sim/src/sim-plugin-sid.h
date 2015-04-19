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

#ifndef __SIM_PLUGIN_SID_H__
#define __SIM_PLUGIN_SID_H__ 1

#include <glib.h>
#include <glib-object.h>
#include <libgda/libgda.h>

#include "sim-enums.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SIM_TYPE_PLUGIN_SID                  (sim_plugin_sid_get_type ())
#define SIM_PLUGIN_SID(obj)                  (G_TYPE_CHECK_INSTANCE_CAST (obj, SIM_TYPE_PLUGIN_SID, SimPluginSid))
#define SIM_PLUGIN_SID_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST (klass, SIM_TYPE_PLUGIN_SID, SimPluginSidClass))
#define SIM_IS_PLUGIN_SID(obj)               (G_TYPE_CHECK_INSTANCE_TYPE (obj, SIM_TYPE_PLUGIN_SID))
#define SIM_IS_PLUGIN_SID_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), SIM_TYPE_PLUGIN_SID))
#define SIM_PLUGIN_SID_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), SIM_TYPE_PLUGIN_SID, SimPluginSidClass))

G_BEGIN_DECLS

typedef struct _SimPluginSid         SimPluginSid;
typedef struct _SimPluginSidClass    SimPluginSidClass;
typedef struct _SimPluginSidPrivate  SimPluginSidPrivate;

struct _SimPluginSid {
  GObject parent;

  SimPluginSidPrivate  *_priv;
};

struct _SimPluginSidClass {
  GObjectClass parent_class;
};

GType             sim_plugin_sid_get_type                        (void);
SimPluginSid*     sim_plugin_sid_new                             (void);
SimPluginSid*     sim_plugin_sid_new_from_data                   (gint           plugin_id,
								  gint           sid,
								  gint           reliability,
								  gint           priority,
								  const gchar   *name);
SimPluginSid*     sim_plugin_sid_new_from_dm                     (GdaDataModel  *dm,
								  gint           row);
gint              sim_plugin_sid_get_plugin_id                   (SimPluginSid  *plugin_sid);
void              sim_plugin_sid_set_plugin_id                   (SimPluginSid  *plugin_sid,
								  gint           plugin_id);
gint              sim_plugin_sid_get_sid                         (SimPluginSid  *plugin_sid);
void              sim_plugin_sid_set_sid                         (SimPluginSid  *plugin_sid,
								  gint           sid);
gint              sim_plugin_sid_get_reliability                 (SimPluginSid  *plugin_sid);
void              sim_plugin_sid_set_reliability                 (SimPluginSid  *plugin_sid,
								  gint           reliability);
gint              sim_plugin_sid_get_priority                    (SimPluginSid  *plugin_sid);
void              sim_plugin_sid_set_priority                    (SimPluginSid  *plugin_sid,
								  gint           priority);
gchar*            sim_plugin_sid_get_name                        (SimPluginSid  *plugin_sid);
void              sim_plugin_sid_set_name                        (SimPluginSid  *plugin_sid,
								  gchar         *name);
void							sim_plugin_sid_debug_print				 						 (SimPluginSid  *plugin_sid); //debug function

gchar*            sim_plugin_sid_get_insert_clause               (SimPluginSid  *plugin_sid);

G_END_DECLS

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SIM_PLUGIN_SID_H__ */
// vim: set tabstop=2:
