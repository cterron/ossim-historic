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

#ifndef __SIM_HOST_LEVEL_H__
#define __SIM_HOST_LEVEL_H__ 1

#include <glib.h>
#include <glib-object.h>
#include <gnet.h>
#include <libgda/libgda.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SIM_TYPE_HOST_LEVEL             (sim_host_level_get_type ())
#define SIM_HOST_LEVEL(obj)             (G_TYPE_CHECK_INSTANCE_CAST (obj, SIM_TYPE_HOST_LEVEL, SimHostLevel))
#define SIM_HOST_LEVEL_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST (klass, SIM_TYPE_HOST_LEVEL, SimHostLevelClass))
#define SIM_IS_HOST_LEVEL(obj)          (G_TYPE_CHECK_INSTANCE_TYPE (obj, SIM_TYPE_HOST_LEVEL))
#define SIM_IS_HOST_LEVEL_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), SIM_TYPE_HOST_LEVEL))
#define SIM_HOST_LEVEL_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), SIM_TYPE_HOST_LEVEL, SimHostLevelClass))

G_BEGIN_DECLS

typedef struct _SimHostLevel            SimHostLevel;
typedef struct _SimHostLevelClass       SimHostLevelClass;
typedef struct _SimHostLevelPrivate     SimHostLevelPrivate;

struct _SimHostLevel {
  GObject parent;

  SimHostLevelPrivate *_priv;
};

struct _SimHostLevelClass {
  GObjectClass parent_class;
};

GType             sim_host_level_get_type                        (void);
SimHostLevel*     sim_host_level_new                             (const GInetAddr  *ia,
								  gint              c,
								  gint              a);
SimHostLevel*     sim_host_level_new_from_dm                     (GdaDataModel     *dm,
								  gint              row);

GInetAddr*        sim_host_level_get_ia                          (SimHostLevel     *host_level);
void              sim_host_level_set_ia                          (SimHostLevel     *host_level,
								  const GInetAddr  *ia);

gint              sim_host_level_get_c                           (SimHostLevel     *host_level);
void              sim_host_level_set_c                           (SimHostLevel     *host_level,
								  gint              c);
void              sim_host_level_plus_c                          (SimHostLevel     *host_level,
								  gint              c);

gint              sim_host_level_get_a                           (SimHostLevel     *host_level);
void              sim_host_level_set_a                           (SimHostLevel     *host_level,
								  gint              a);
void              sim_host_level_plus_a                          (SimHostLevel     *host_level,
								  gint              a);

void              sim_host_level_set_recovery                    (SimHostLevel     *host_level,
								  gint              recovery);

gchar*            sim_host_level_get_insert_clause               (SimHostLevel     *host_level);
gchar*            sim_host_level_get_update_clause               (SimHostLevel     *host_level);
gchar*            sim_host_level_get_delete_clause               (SimHostLevel     *host_level);

G_END_DECLS

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SIM_HOST_LEVEL_H__ */
