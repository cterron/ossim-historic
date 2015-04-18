/* Server
 *
 *
 */

#ifndef __SIM_SERVER_H__
#define __SIM_SERVER_H__ 1

#include <glib.h>
#include <glib-object.h>
#include <netinet/in.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define STACK_SIZE 16384

#define SIM_TYPE_SERVER                  (sim_server_get_type ())
#define SIM_SERVER(obj)                  (G_TYPE_CHECK_INSTANCE_CAST (obj, SIM_TYPE_SERVER, SimServer))
#define SIM_SERVER_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST (klass, SIM_TYPE_SERVER, SimServerClass))
#define SIM_IS_SERVER(obj)               (G_TYPE_CHECK_INSTANCE_TYPE (obj, SIM_TYPE_SERVER))
#define SIM_IS_SERVER_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), SIM_TYPE_SERVER))
#define SIM_SERVER_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), SIM_TYPE_SERVER, SimServerClass))

G_BEGIN_DECLS

typedef struct _SimServer        SimServer;
typedef struct _SimServerClass   SimServerClass;
typedef struct _SimServerPrivate SimServerPrivate;

struct _SimServer {
  GObject parent;

  SimServerPrivate *_priv;
};

struct _SimServerClass {
  GObjectClass parent_class;
};

GType           sim_server_get_type                        (void);
SimServer*      sim_server_new                             (void);
void            sim_server_run                             (SimServer *server);
void            sim_server_push_tail_messages              (SimServer *server,
							    GObject *message);
GObject*        sim_server_pop_head_messages               (SimServer *server);
gint            sim_server_get_messages_num                (SimServer *server);

G_END_DECLS

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SIM_SERVER_H__ */
