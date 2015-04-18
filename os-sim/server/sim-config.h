/* Config
 *
 *
 */

#ifndef __SIM_CONFIG_H__
#define __SIM_CONFIG_H__ 1

#include <glib.h>
#include <glib-object.h>

#include "sim-enums.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SIM_TYPE_CONFIG                  (sim_config_get_type ())
#define SIM_CONFIG(obj)                  (G_TYPE_CHECK_INSTANCE_CAST (obj, SIM_TYPE_CONFIG, SimConfig))
#define SIM_CONFIG_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST (klass, SIM_TYPE_CONFIG, SimConfigClass))
#define SIM_IS_CONFIG(obj)               (G_TYPE_CHECK_INSTANCE_TYPE (obj, SIM_TYPE_CONFIG))
#define SIM_IS_CONFIG_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), SIM_TYPE_CONFIG))
#define SIM_CONFIG_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), SIM_TYPE_CONFIG, SimConfigClass))

G_BEGIN_DECLS

typedef struct _SimConfig        SimConfig;
typedef struct _SimConfigClass   SimConfigClass;
typedef struct _SimConfigPrivate SimConfigPrivate;

struct _SimConfig {
  GObject parent;

  SimConfigPrivate *_priv;
};

struct _SimConfigClass {
  GObjectClass parent_class;
};

GType           sim_config_get_type                        (void);
SimConfig*      sim_config_new                             (const gchar    *filename);

gchar*          sim_config_get_property_value              (SimConfig              *config,
							    SimConfigPropertyType   type);

G_END_DECLS

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SIM_CONFIG_H__ */
