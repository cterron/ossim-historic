/* Organizer
 *
 *
 */

#ifndef __SIM_ORGANIZER_H__
#define __SIM_ORGANIZER_H__ 1

#include <glib.h>
#include <glib-object.h>
#include "sim-server.h"
#include "sim-message.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SIM_TYPE_ORGANIZER                  (sim_organizer_get_type ())
#define SIM_ORGANIZER(obj)                  (G_TYPE_CHECK_INSTANCE_CAST (obj, SIM_TYPE_ORGANIZER, SimOrganizer))
#define SIM_ORGANIZER_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST (klass, SIM_TYPE_ORGANIZER, SimOrganizerClass))
#define SIM_IS_ORGANIZER(obj)               (G_TYPE_CHECK_INSTANCE_TYPE (obj, SIM_TYPE_ORGANIZER))
#define SIM_IS_ORGANIZER_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), SIM_TYPE_ORGANIZER))
#define SIM_ORGANIZER_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), SIM_TYPE_ORGANIZER, SimOrganizerClass))

G_BEGIN_DECLS

typedef struct _SimOrganizer        SimOrganizer;
typedef struct _SimOrganizerClass   SimOrganizerClass;
typedef struct _SimOrganizerPrivate SimOrganizerPrivate;

struct _SimOrganizer {
  GObject parent;

  SimOrganizerPrivate *_priv;
};

struct _SimOrganizerClass {
  GObjectClass parent_class;
};

GType             sim_organizer_get_type                        (void);
SimOrganizer*     sim_organizer_new                             (void);
void              sim_organizer_set_server                      (SimOrganizer *organizer,
								 SimServer *server);
void              sim_organizer_run                             (SimOrganizer *organizer);
void              sim_organizer_calificate                      (SimOrganizer *organizer,
								 SimMessage *message);


G_END_DECLS

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SIM_ORGANIZER_H__ */
