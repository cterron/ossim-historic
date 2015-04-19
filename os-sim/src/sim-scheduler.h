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

#ifndef __SIM_SCHEDULER_H__
#define __SIM_SCHEDULER_H__ 1

#include <glib.h>
#include <glib-object.h>

#include "sim-container.h"
#include "sim-database.h"
#include "sim-config.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SIM_TYPE_SCHEDULER                  (sim_scheduler_get_type ())
#define SIM_SCHEDULER(obj)                  (G_TYPE_CHECK_INSTANCE_CAST (obj, SIM_TYPE_SCHEDULER, SimScheduler))
#define SIM_SCHEDULER_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST (klass, SIM_TYPE_SCHEDULER, SimSchedulerClass))
#define SIM_IS_SCHEDULER(obj)               (G_TYPE_CHECK_INSTANCE_TYPE (obj, SIM_TYPE_SCHEDULER))
#define SIM_IS_SCHEDULER_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), SIM_TYPE_SCHEDULER))
#define SIM_SCHEDULER_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), SIM_TYPE_SCHEDULER, SimSchedulerClass))

G_BEGIN_DECLS

typedef struct _SimScheduler        SimScheduler;
typedef struct _SimSchedulerClass   SimSchedulerClass;
typedef struct _SimSchedulerPrivate SimSchedulerPrivate;

struct _SimScheduler {
  GObject parent;

  SimSchedulerPrivate *_priv;
};

struct _SimSchedulerClass {
  GObjectClass parent_class;
};

GType             sim_scheduler_get_type                        (void);
SimScheduler*     sim_scheduler_new                             (SimConfig     *config);

void              sim_scheduler_run                             (SimScheduler  *scheduler);

/* Backlogs Time Out */
void              sim_scheduler_backlogs_time_out               (SimScheduler  *scheduler);

void		sim_scheduler_task_execute_at_interval									(SimScheduler  *scheduler,
																																	gpointer       data);				    
void 		sim_scheduler_task_GDAErrorHandling											(void);
void		sim_scheduler_task_store_event_number_at_5min						(SimScheduler  *scheduler);
void		sim_scheduler_task_rservers															(SimSchedulerState state);
	
G_END_DECLS

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SIM_SCHEDULER_H__ */
// vim: set tabstop=2:
