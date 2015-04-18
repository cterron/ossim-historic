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

#ifndef __SIM_SMTP_H__
#define __SIM_SMTP_H__ 1

#include <config.h>

#include <glib.h>
#include <glib-object.h>
#include <gnet.h>

#include "sim-enums.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SIM_TYPE_SMTP                  (sim_smtp_get_type ())
#define SIM_SMTP(obj)                  (G_TYPE_CHECK_INSTANCE_CAST (obj, SIM_TYPE_SMTP, SimSmtp))
#define SIM_SMTP_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST (klass, SIM_TYPE_SMTP, SimSmtpClass))
#define SIM_IS_SMTP(obj)               (G_TYPE_CHECK_INSTANCE_TYPE (obj, SIM_TYPE_SMTP))
#define SIM_IS_SMTP_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), SIM_TYPE_SMTP))
#define SIM_SMTP_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), SIM_TYPE_SMTP, SimSmtpClass))

G_BEGIN_DECLS

typedef struct _SimSmtp        SimSmtp;
typedef struct _SimSmtpClass   SimSmtpClass;
typedef struct _SimSmtpPrivate SimSmtpPrivate;

struct _SimSmtp {
  GObject parent;

  SimSmtpPrivate *_priv;
};

struct _SimSmtpClass {
  GObjectClass parent_class;
};

GType             sim_smtp_get_type                        (void);

SimSmtp*          sim_smtp_new                             (const gchar   *hostname,
							    gint           port);

G_END_DECLS

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SIM_SMTP_H__ */
