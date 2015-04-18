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

#ifndef __SIM_SYSLOG_H__
#define __SIM_SYSLOG_H__ 1

#include <glib.h>
#include <glib-object.h>

#include "sim-container.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SIM_TYPE_SYSLOG                  (sim_syslog_get_type ())
#define SIM_SYSLOG(obj)                  (G_TYPE_CHECK_INSTANCE_CAST (obj, SIM_TYPE_SYSLOG, SimSyslog))
#define SIM_SYSLOG_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST (klass, SIM_TYPE_SYSLOG, SimSyslogClass))
#define SIM_IS_SYSLOG(obj)               (G_TYPE_CHECK_INSTANCE_TYPE (obj, SIM_TYPE_SYSLOG))
#define SIM_IS_SYSLOG_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), SIM_TYPE_SYSLOG))
#define SIM_SYSLOG_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), SIM_TYPE_SYSLOG, SimSyslogClass))

G_BEGIN_DECLS

typedef struct _SimSyslog        SimSyslog;
typedef struct _SimSyslogClass   SimSyslogClass;
typedef struct _SimSyslogPrivate SimSyslogPrivate;

struct _SimSyslog {
  GObject parent;

  SimSyslogPrivate *_priv;
};

struct _SimSyslogClass {
  GObjectClass parent_class;
};

GType             sim_syslog_get_type                        (void);
SimSyslog*        sim_syslog_new                             (SimContainer  *container,
							      const gchar   *filename);

void              sim_syslog_run                             (SimSyslog *syslog);

G_END_DECLS

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SIM_SYSLOG_H__ */
