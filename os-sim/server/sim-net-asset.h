/* Net Asset
 *
 *
 */

#ifndef __SIM_NET_ASSET_H__
#define __SIM_NET_ASSET_H__ 1

#include <glib.h>
#include <glib-object.h>
#include "sim-server.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define MAX_NET_NAME 256

#define SIM_TYPE_NET_ASSET                  (sim_net_asset_get_type ())
#define SIM_NET_ASSET(obj)                  (G_TYPE_CHECK_INSTANCE_CAST (obj, SIM_TYPE_NET_ASSET, SimNetAsset))
#define SIM_NET_ASSET_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST (klass, SIM_TYPE_NET_ASSET, SimNetAssetClass))
#define SIM_IS_NET_ASSET(obj)               (G_TYPE_CHECK_INSTANCE_TYPE (obj, SIM_TYPE_NET_ASSET))
#define SIM_IS_NET_ASSET_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), SIM_TYPE_NET_ASSET))
#define SIM_NET_ASSET_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), SIM_TYPE_NET_ASSET, SimNetAssetClass))

G_BEGIN_DECLS

typedef struct _SimNetAsset        SimNetAsset;
typedef struct _SimNetAssetClass   SimNetAssetClass;
typedef struct _SimNetAssetPrivate SimNetAssetPrivate;

struct _SimNetAsset {
  GObject parent;

  SimNetAssetPrivate *_priv;
};

struct _SimNetAssetClass {
  GObjectClass parent_class;
};

GType             sim_net_asset_get_type                        (void);
SimNetAsset*        sim_net_asset_new                             (void);

G_END_DECLS

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SIM_NET_ASSET_H__ */
