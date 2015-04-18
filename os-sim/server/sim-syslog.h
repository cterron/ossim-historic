/* Syslog
 *
 *
 */

#ifndef __SIM_SYSLOG_H__
#define __SIM_SYSLOG_H__ 1

#include <glib.h>
#include <glib-object.h>
#include "sim-server.h"

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
SimSyslog*        sim_syslog_new                             (void);
void              sim_syslog_set_server                      (SimSyslog *syslog,
							      SimServer *server);
void              sim_syslog_run                             (SimSyslog *syslog);


G_END_DECLS

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SIM_SYSLOG_H__ */
