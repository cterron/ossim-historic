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

#ifndef __SIM_MESSAGE_H__
#define __SIM_MESSAGE_H__ 1

#include <glib.h>
#include <glib-object.h>
#include <gnet.h>

#include "sim-enums.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#define SIM_TYPE_MESSAGE                  (sim_message_get_type ())
#define SIM_MESSAGE(obj)                  (G_TYPE_CHECK_INSTANCE_CAST (obj, SIM_TYPE_MESSAGE, SimMessage))
#define SIM_MESSAGE_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST (klass, SIM_TYPE_MESSAGE, SimMessageClass))
#define SIM_IS_MESSAGE(obj)               (G_TYPE_CHECK_INSTANCE_TYPE (obj, SIM_TYPE_MESSAGE))
#define SIM_IS_MESSAGE_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), SIM_TYPE_MESSAGE))
#define SIM_MESSAGE_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), SIM_TYPE_MESSAGE, SimMessageClass))

G_BEGIN_DECLS

typedef struct _SimMessage        SimMessage;
typedef struct _SimMessageClass   SimMessageClass;
typedef struct _SimMessagePrivate SimMessagePrivate;

struct _SimMessage {
  GObject parent;

  SimMessageType type;

  SimMessagePrivate *_priv;
};

struct _SimMessageClass {
  GObjectClass parent_class;
};

GType           sim_message_get_type                    (void);
SimMessage*     sim_message_new                         (gchar *buffer);
SimMessage*     sim_message_new0                        (SimMessageType    type);

void            sim_message_print                       (SimMessage       *msg);

gint            sim_message_get_plugin                  (SimMessage       *msg);
void            sim_message_set_plugin                  (SimMessage       *msg,
							 gint              plugin);

gint            sim_message_get_tplugin                 (SimMessage       *msg);
void            sim_message_set_tplugin                 (SimMessage       *msg,
							 gint              tplugin);

gint            sim_message_get_priority                (SimMessage       *msg);
void            sim_message_set_priority                (SimMessage       *msg,
							 gint              priority);

SimProtocolType sim_message_get_protocol                (SimMessage       *msg);
void            sim_message_set_protocol                (SimMessage       *msg,
							 SimProtocolType   type);

GInetAddr*      sim_message_get_src_ia                  (SimMessage       *msg);
void            sim_message_set_src_ia                  (SimMessage       *msg,
							 GInetAddr        *src_ia);

GInetAddr*      sim_message_get_dst_ia                  (SimMessage       *msg);
void            sim_message_set_dst_ia                  (SimMessage       *msg,
							 GInetAddr        *dst_ia);

gint            sim_message_get_src_port                (SimMessage       *msg);
void            sim_message_set_src_port                (SimMessage       *msg,
							 gint              port);

gint            sim_message_get_dst_port                (SimMessage       *msg);
void            sim_message_set_dst_port                (SimMessage       *msg,
							 gint              port);

G_END_DECLS

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SIM_MESSAGE_H__ */
