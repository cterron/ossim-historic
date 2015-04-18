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

#ifndef __SIM_SERVER_H__
#define __SIM_SERVER_H__ 1

#include <glib.h>
#include <glib-object.h>

#include "sim-container.h"
#include "sim-database.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SIM_TYPE_SERVER                  (sim_server_get_type ())
#define SIM_SERVER(obj)                  (G_TYPE_CHECK_INSTANCE_CAST (obj, SIM_TYPE_SERVER, SimServer))
#define SIM_SERVER_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST (klass, SIM_TYPE_SERVER, SimServerClass))
#define SIM_IS_SERVER(obj)               (G_TYPE_CHECK_INSTANCE_TYPE (obj, SIM_TYPE_SERVER))
#define SIM_IS_SERVER_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), SIM_TYPE_SERVER))
#define SIM_SERVER_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), SIM_TYPE_SERVER, SimServerClass))

G_BEGIN_DECLS

typedef struct _SimServer        SimServer;
typedef struct _SimServerClass   SimServerClass;
typedef struct _SimServerPrivate SimServerPrivate;

struct _SimServer {
  GObject parent;

  SimServerPrivate *_priv;
};

struct _SimServerClass {
  GObjectClass parent_class;
};

GType             sim_server_get_type                      (void);
SimServer*        sim_server_new                           (SimContainer  *container,
							    SimDatabase   *database);

void              sim_server_run                           (SimServer     *server);

SimContainer*     sim_server_get_container                 (SimServer     *server);


G_END_DECLS

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SIM_SERVER_H__ */
