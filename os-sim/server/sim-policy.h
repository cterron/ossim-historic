/* Policy
 *
 *
 */

#ifndef __SIM_POLICY_H__
#define __SIM_POLICY_H__ 1

#include <glib.h>
#include <glib-object.h>

#include "sim-enums.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SIM_TYPE_POLICY                  (sim_policy_get_type ())
#define SIM_POLICY(obj)                  (G_TYPE_CHECK_INSTANCE_CAST (obj, SIM_TYPE_POLICY, SimPolicy))
#define SIM_POLICY_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST (klass, SIM_TYPE_POLICY, SimPolicyClass))
#define SIM_IS_POLICY(obj)               (G_TYPE_CHECK_INSTANCE_TYPE (obj, SIM_TYPE_POLICY))
#define SIM_IS_POLICY_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), SIM_TYPE_POLICY))
#define SIM_POLICY_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), SIM_TYPE_POLICY, SimPolicyClass))

G_BEGIN_DECLS

typedef struct _SimPolicy        SimPolicy;
typedef struct _SimPolicyClass   SimPolicyClass;
typedef struct _SimPolicyPrivate SimPolicyPrivate;

struct _SimPolicy {
  GObject parent;

  SimPolicyPrivate *_priv;
};

struct _SimPolicyClass {
  GObjectClass parent_class;
};

GType             sim_policy_get_type                        (void);
SimPolicy*        sim_policy_new                             (void);
GList*            sim_policy_load_from_db                    (GObject    *db);

gint              sim_policy_get_priority                    (SimPolicy  *policy);
void              sim_policy_set_priority                    (SimPolicy  *policy,
							      gint        priority);

gint              sim_policy_get_begin_day                   (SimPolicy  *policy);
void              sim_policy_set_begin_day                   (SimPolicy  *policy,
							      gint        begin_day);
gint              sim_policy_get_end_day                     (SimPolicy  *policy);
void              sim_policy_set_end_day                     (SimPolicy  *policy,
							      gint        end_day);
gint              sim_policy_get_begin_hour                  (SimPolicy  *policy);
void              sim_policy_set_begin_hour                  (SimPolicy  *policy,
							      gint        begin_hour);
gint              sim_policy_get_end_hour                    (SimPolicy  *policy);
void              sim_policy_set_end_hour                    (SimPolicy  *policy,
							      gint        end_hour);

GList*            sim_policy_get_sources                     (SimPolicy  *policy);
GList*            sim_policy_get_destinations                (SimPolicy  *policy);
GList*            sim_policy_get_ports                       (SimPolicy  *policy);
GList*            sim_policy_get_signatures                  (SimPolicy  *policy);
GList*            sim_policy_get_sensors                     (SimPolicy  *policy);

gboolean          sim_policy_match                           (SimPolicy        *policy,
							      gint              date,
							      gchar            *src_ip,
							      gchar            *dst_ip,
							      gint              port,
							      SimProtocolType   protocol,
							      gchar            *signature);

G_END_DECLS

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SIM_POLICY_H__ */
