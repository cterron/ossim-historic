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

#ifndef __SIM_COMMAND_H__
#define __SIM_COMMAND_H__ 1

#include <glib.h>
#include <glib-object.h>
#include <gnet.h>

#include "sim-enums.h"
#include "sim-message.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SIM_TYPE_COMMAND                  (sim_command_get_type ())
#define SIM_COMMAND(obj)                  (G_TYPE_CHECK_INSTANCE_CAST (obj, SIM_TYPE_COMMAND, SimCommand))
#define SIM_COMMAND_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST (klass, SIM_TYPE_COMMAND, SimCommandClass))
#define SIM_IS_COMMAND(obj)               (G_TYPE_CHECK_INSTANCE_TYPE (obj, SIM_TYPE_COMMAND))
#define SIM_IS_COMMAND_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), SIM_TYPE_COMMAND))
#define SIM_COMMAND_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), SIM_TYPE_COMMAND, SimCommandClass))

G_BEGIN_DECLS

typedef struct _SimCommand        SimCommand;
typedef struct _SimCommandClass   SimCommandClass;
typedef struct _SimCommandPrivate SimCommandPrivate;

struct _SimCommand {
  GObject parent;

  SimCommandType      type;

  SimCommandPrivate  *_priv;
};

struct _SimCommandClass {
  GObjectClass parent_class;
};

GType             sim_command_get_type                        (void);
SimCommand*       sim_command_new                             (const gchar    *buffer);
SimCommand*       sim_command_new0                            (SimCommandType  type);

gchar*            sim_command_get_str                         (SimCommand     *command);

SimMessage*       sim_command_get_message                     (SimCommand     *command);

G_END_DECLS

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SIM_COMMAND_H__ */
