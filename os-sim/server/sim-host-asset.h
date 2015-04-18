/* Host Asset
 *
 *
 */

#ifndef __SIM_HOST_ASSET_H__
#define __SIM_HOST_ASSET_H__ 1

#include <glib.h>
#include <glib-object.h>
#include "sim-server.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define MAX_SENSORS 256

#define SIM_TYPE_HOST_ASSET                  (sim_host_asset_get_type ())
#define SIM_HOST_ASSET(obj)                  (G_TYPE_CHECK_INSTANCE_CAST (obj, SIM_TYPE_HOST_ASSET, SimHostAsset))
#define SIM_HOST_ASSET_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST (klass, SIM_TYPE_HOST_ASSET, SimHostAssetClass))
#define SIM_IS_HOST_ASSET(obj)               (G_TYPE_CHECK_INSTANCE_TYPE (obj, SIM_TYPE_HOST_ASSET))
#define SIM_IS_HOST_ASSET_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), SIM_TYPE_HOST_ASSET))
#define SIM_HOST_ASSET_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), SIM_TYPE_HOST_ASSET, SimHostAssetClass))

G_BEGIN_DECLS

typedef struct _SimHostAsset        SimHostAsset;
typedef struct _SimHostAssetClass   SimHostAssetClass;
typedef struct _SimHostAssetPrivate SimHostAssetPrivate;

struct _SimHostAsset {
  GObject parent;

  SimHostAssetPrivate *_priv;
};

struct _SimHostAssetClass {
  GObjectClass parent_class;
};

GType             sim_host_asset_get_type                        (void);
SimHostAsset*        sim_host_asset_new                             (void);

G_END_DECLS

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SIM_HOST_ASSET_H__ */
