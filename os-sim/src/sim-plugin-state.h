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

#ifndef __SIM_PLUGIN_STATE_H__
#define __SIM_PLUGIN_STATE_H__ 1

#include <glib.h>
#include <glib-object.h>

#include "sim-plugin.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SIM_TYPE_PLUGIN_STATE                  (sim_plugin_state_get_type ())
#define SIM_PLUGIN_STATE(obj)                  (G_TYPE_CHECK_INSTANCE_CAST (obj, SIM_TYPE_PLUGIN_STATE, SimPluginState))
#define SIM_PLUGIN_STATE_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST (klass, SIM_TYPE_PLUGIN_STATE, SimPluginStateClass))
#define SIM_IS_PLUGIN_STATE(obj)               (G_TYPE_CHECK_INSTANCE_TYPE (obj, SIM_TYPE_PLUGIN_STATE))
#define SIM_IS_PLUGIN_STATE_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), SIM_TYPE_PLUGIN_STATE))
#define SIM_PLUGIN_STATE_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), SIM_TYPE_PLUGIN_STATE, SimPluginStateClass))

G_BEGIN_DECLS

typedef struct _SimPluginState         SimPluginState;
typedef struct _SimPluginStateClass    SimPluginStateClass;
typedef struct _SimPluginStatePrivate  SimPluginStatePrivate;

struct _SimPluginState {
  GObject parent;

  SimPluginStatePrivate  *_priv;
};

struct _SimPluginStateClass {
  GObjectClass parent_class;
};

GType             sim_plugin_state_get_type                        (void);
SimPluginState*   sim_plugin_state_new                             (void);
SimPluginState*   sim_plugin_state_new_from_data                   (SimPlugin        *plugin,
								    gint              plugin_id,
								    gint              state,
								    gboolean          enable);

SimPlugin*        sim_plugin_state_get_plugin                      (SimPluginState   *plugin_state);
void              sim_plugin_state_set_plugin                      (SimPluginState   *plugin_state,
								    SimPlugin        *plugin);
gint              sim_plugin_state_get_state                       (SimPluginState   *plugin_state);
void              sim_plugin_state_set_state                       (SimPluginState   *plugin_state,
								    gint              state);
gboolean          sim_plugin_state_get_enabled                     (SimPluginState   *plugin_state);
void              sim_plugin_state_set_enabled                     (SimPluginState   *plugin_state,
								    gboolean          enabled);

G_END_DECLS

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SIM_PLUGIN_STATE_H__ */
// vim: set tabstop=2:
