/* Scheduler
 *
 *
 */

#ifndef __SIM_SCHEDULER_H__
#define __SIM_SCHEDULER_H__ 1

#include <glib.h>
#include <glib-object.h>

#include "sim-enums.h"
#include "sim-server.h"

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
SimScheduler*     sim_scheduler_new                             (void);

void              sim_scheduler_run                             (SimScheduler *scheduler);

void              sim_scheduler_set_server                      (SimScheduler *scheduler,
								 SimServer    *server);


G_END_DECLS

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SIM_SCHEDULER_H__ */
