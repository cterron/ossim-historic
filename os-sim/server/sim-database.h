/* Database
 *
 *
 */

#ifndef __SIM_DATABASE_H__
#define __SIM_DATABASE_H__ 1

#include <glib.h>
#include <glib-object.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SIM_TYPE_DATABASE                  (sim_database_get_type ())
#define SIM_DATABASE(obj)                  (G_TYPE_CHECK_INSTANCE_CAST (obj, SIM_TYPE_DATABASE, SimDatabase))
#define SIM_DATABASE_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST (klass, SIM_TYPE_DATABASE, SimDatabaseClass))
#define SIM_IS_DATABASE(obj)               (G_TYPE_CHECK_INSTANCE_TYPE (obj, SIM_TYPE_DATABASE))
#define SIM_IS_DATABASE_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), SIM_TYPE_DATABASE))
#define SIM_DATABASE_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), SIM_TYPE_DATABASE, SimDatabaseClass))

G_BEGIN_DECLS

typedef struct _SimDatabase        SimDatabase;
typedef struct _SimDatabaseClass   SimDatabaseClass;
typedef struct _SimDatabasePrivate SimDatabasePrivate;

struct _SimDatabase {
  GObject parent;

  SimDatabasePrivate *_priv;
};

struct _SimDatabaseClass {
  GObjectClass parent_class;
};

GType           sim_database_get_type                        (void);
SimDatabase*    sim_database_new                             (void);

G_END_DECLS

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SIM_DATABASE_H__ */
