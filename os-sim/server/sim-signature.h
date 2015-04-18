/* Signature
 *
 *
 */

#ifndef __SIM_SIGNATURE_H__
#define __SIM_SIGNATURE_H__ 1

#include <glib.h>
#include <glib-object.h>

#include "sim-enums.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SIM_TYPE_SIGNATURE                  (sim_signature_get_type ())
#define SIM_SIGNATURE(obj)                  (G_TYPE_CHECK_INSTANCE_CAST (obj, SIM_TYPE_SIGNATURE, SimSignature))
#define SIM_SIGNATURE_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST (klass, SIM_TYPE_SIGNATURE, SimSignatureClass))
#define SIM_IS_SIGNATURE(obj)               (G_TYPE_CHECK_INSTANCE_TYPE (obj, SIM_TYPE_SIGNATURE))
#define SIM_IS_SIGNATURE_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), SIM_TYPE_SIGNATURE))
#define SIM_SIGNATURE_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), SIM_TYPE_SIGNATURE, SimSignatureClass))

G_BEGIN_DECLS

typedef struct _SimSignature        SimSignature;
typedef struct _SimSignatureClass   SimSignatureClass;
typedef struct _SimSignaturePrivate SimSignaturePrivate;

struct _SimSignature {
  GObject                   parent;

  SimSignatureType          type;
  SimSignatureSubgroupType  subgroup;

  SimSignaturePrivate      *_priv;
};

struct _SimSignatureClass {
  GObjectClass           parent_class;
};

GType             sim_signature_get_type               (void);
SimSignature*     sim_signature_new                    (void);

SimSignatureSubgroupType 
sim_signature_get_subgroup_type_enum                   (gchar *subgroup);

GList*            sim_signature_load_from_db           (GObject      *database,
							GNode        *root);

gint              sim_signature_get_id                 (SimSignature *signature);
void              sim_signature_set_id                 (SimSignature *signature,
							gint          id);

gchar*            sim_signature_get_name               (SimSignature *signature);
void              sim_signature_set_name               (SimSignature *signature,
							gchar        *name);


G_END_DECLS

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SIM_SIGNATURE_H__ */
