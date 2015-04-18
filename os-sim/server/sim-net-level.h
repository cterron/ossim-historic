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

#ifndef __SIM_NET_LEVEL_H__
#define __SIM_NET_LEVEL_H__ 1

#include <libgda/libgda.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SIM_TYPE_NET_LEVEL                  (sim_net_level_get_type ())
#define SIM_NET_LEVEL(obj)                  (G_TYPE_CHECK_INSTANCE_CAST (obj, SIM_TYPE_NET_LEVEL, SimNetLevel))
#define SIM_NET_LEVEL_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST (klass, SIM_TYPE_NET_LEVEL, SimNetLevelClass))
#define SIM_IS_NET_LEVEL(obj)               (G_TYPE_CHECK_INSTANCE_TYPE (obj, SIM_TYPE_NET_LEVEL))
#define SIM_IS_NET_LEVEL_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), SIM_TYPE_NET_LEVEL))
#define SIM_NET_LEVEL_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), SIM_TYPE_NET_LEVEL, SimNetLevelClass))

G_BEGIN_DECLS

typedef struct _SimNetLevel        SimNetLevel;
typedef struct _SimNetLevelClass   SimNetLevelClass;
typedef struct _SimNetLevelPrivate SimNetLevelPrivate;

struct _SimNetLevel {
  GObject parent;

  SimNetLevelPrivate *_priv;
};

struct _SimNetLevelClass {
  GObjectClass parent_class;
};

GType             sim_net_level_get_type                        (void);
SimNetLevel*      sim_net_level_new                             (const gchar         *name,
								 gint           c,
								 gint           a);
SimNetLevel*      sim_net_level_new_from_dm                     (GdaDataModel  *dm,
								 gint           row);

gchar*            sim_net_level_get_name                        (SimNetLevel   *net_level);
void              sim_net_level_set_name                        (SimNetLevel   *net_level,
								 gchar         *name);

gint              sim_net_level_get_c                           (SimNetLevel   *net_level);
void              sim_net_level_set_c                           (SimNetLevel   *net_level,
								 gint           c);
void              sim_net_level_plus_c                          (SimNetLevel   *net_level,
								 gint           c);

gint              sim_net_level_get_a                           (SimNetLevel   *net_level);
void              sim_net_level_set_a                           (SimNetLevel   *net_level,
								 gint           a);
void              sim_net_level_plus_a                          (SimNetLevel   *net_level,
								 gint           a);

void              sim_net_level_set_recovery                    (SimNetLevel   *net_level,
								 gint           recovery);
gchar*            sim_net_level_get_insert_clause               (SimNetLevel   *net_level);
gchar*            sim_net_level_get_update_clause               (SimNetLevel   *net_level);
gchar*            sim_net_level_get_delete_clause               (SimNetLevel   *net_level);

G_END_DECLS

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SIM_NET_LEVEL_H__ */
