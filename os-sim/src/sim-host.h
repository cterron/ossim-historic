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

#ifndef __SIM_HOST_H__
#define __SIM_HOST_H__ 1

#include <glib.h>
#include <glib-object.h>
#include <gnet.h>
#include <libgda/libgda.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SIM_TYPE_HOST                  (sim_host_get_type ())
#define SIM_HOST(obj)                  (G_TYPE_CHECK_INSTANCE_CAST (obj, SIM_TYPE_HOST, SimHost))
#define SIM_HOST_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST (klass, SIM_TYPE_HOST, SimHostClass))
#define SIM_IS_HOST(obj)               (G_TYPE_CHECK_INSTANCE_TYPE (obj, SIM_TYPE_HOST))
#define SIM_IS_HOST_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), SIM_TYPE_HOST))
#define SIM_HOST_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), SIM_TYPE_HOST, SimHostClass))

G_BEGIN_DECLS

typedef struct _SimHost        SimHost;
typedef struct _SimHostClass   SimHostClass;
typedef struct _SimHostPrivate SimHostPrivate;

struct _SimHost {
  GObject parent;

  SimHostPrivate *_priv;
};

struct _SimHostClass {
  GObjectClass parent_class;
};

GType             sim_host_get_type                        (void);
SimHost*          sim_host_new                             (const GInetAddr  *ia,
							    const gchar      *name,
							    gint              asset);
SimHost*          sim_host_new_from_dm                     (GdaDataModel     *dm,
							    gint              row);

GInetAddr*        sim_host_get_ia                          (SimHost          *host);
void              sim_host_set_ia                          (SimHost          *host,
							    const GInetAddr  *ia);

gchar*            sim_host_get_name                        (SimHost          *host);
void              sim_host_set_name                        (SimHost          *host,
							    const gchar      *name);

gint              sim_host_get_asset                       (SimHost          *host);
void              sim_host_set_asset                       (SimHost          *host,
																												    gint              asset);

void							sim_host_debug_print											(SimHost					*host);

G_END_DECLS

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SIM_HOST_H__ */
// vim: set tabstop=2:
