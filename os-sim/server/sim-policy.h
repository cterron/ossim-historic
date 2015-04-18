/* Policy
 *
 *
 */

#ifndef __SIM_POLICY_H__
#define __SIM_POLICY_H__ 1

#include <glib.h>
#include <glib-object.h>
#include "sim-server.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define MAX_IPS 4096
#define MAX_PORTS 2048
#define MAX_SIGS 1024
#define MAX_SENSORS 256
#define MAX_DESC 256
#define MAX_NET_NAME 256
#define MAX_HOSTS 1024

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

G_END_DECLS

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SIM_POLICY_H__ */
