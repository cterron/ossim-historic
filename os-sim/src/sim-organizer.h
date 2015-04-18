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

#ifndef __SIM_ORGANIZER_H__
#define __SIM_ORGANIZER_H__ 1

#include <glib.h>
#include <glib-object.h>
#include <gnet.h>

#include "sim-container.h"
#include "sim-config.h"
#include "sim-alert.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SIM_TYPE_ORGANIZER                  (sim_organizer_get_type ())
#define SIM_ORGANIZER(obj)                  (G_TYPE_CHECK_INSTANCE_CAST (obj, SIM_TYPE_ORGANIZER, SimOrganizer))
#define SIM_ORGANIZER_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST (klass, SIM_TYPE_ORGANIZER, SimOrganizerClass))
#define SIM_IS_ORGANIZER(obj)               (G_TYPE_CHECK_INSTANCE_TYPE (obj, SIM_TYPE_ORGANIZER))
#define SIM_IS_ORGANIZER_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), SIM_TYPE_ORGANIZER))
#define SIM_ORGANIZER_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), SIM_TYPE_ORGANIZER, SimOrganizerClass))

G_BEGIN_DECLS

typedef struct _SimOrganizer        SimOrganizer;
typedef struct _SimOrganizerClass   SimOrganizerClass;
typedef struct _SimOrganizerPrivate SimOrganizerPrivate;

struct _SimOrganizer {
  GObject parent;

  SimOrganizerPrivate *_priv;
};

struct _SimOrganizerClass {
  GObjectClass parent_class;
};

GType             sim_organizer_get_type                        (void);
SimOrganizer*     sim_organizer_new                             (SimConfig     *config);

void              sim_organizer_run                             (SimOrganizer  *organizer);

void              sim_organizer_correlation_plugin              (SimOrganizer *organizer, 
								 SimAlert     *alert);

/* Correlate Function */
void              sim_organizer_calificate                      (SimOrganizer  *organizer,
								 SimAlert      *alert);

/* Correlate Function */
void              sim_organizer_correlation                     (SimOrganizer  *organizer,
								 SimAlert      *alert);
/* Correlate Function */
void              sim_organizer_snort                           (SimOrganizer  *organizer,
								 SimAlert      *alert);
/* Util Function */
void              sim_organizer_backlog_match                   (SimDatabase   *db_ossim,
								 SimDirective  *backlog,
								 SimAlert      *alert);

G_END_DECLS

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SIM_ORGANIZER_H__ */
