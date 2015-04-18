/* Net
 *
 *
 */

#ifndef __SIM_NET_H__
#define __SIM_NET_H__ 1

#include <glib.h>
#include <glib-object.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "sim-enums.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SIM_TYPE_NET                  (sim_net_get_type ())
#define SIM_NET(obj)                  (G_TYPE_CHECK_INSTANCE_CAST (obj, SIM_TYPE_NET, SimNet))
#define SIM_NET_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST (klass, SIM_TYPE_NET, SimNetClass))
#define SIM_IS_NET(obj)               (G_TYPE_CHECK_INSTANCE_TYPE (obj, SIM_TYPE_NET))
#define SIM_IS_NET_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), SIM_TYPE_NET))
#define SIM_NET_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), SIM_TYPE_NET, SimNetClass))

G_BEGIN_DECLS

typedef struct _SimNet        SimNet;
typedef struct _SimNetClass   SimNetClass;
typedef struct _SimNetPrivate SimNetPrivate;

struct _SimNet {
  GObject parent;

  SimNetPrivate *_priv;
};

struct _SimNetClass {
  GObjectClass parent_class;
};

GType             sim_net_get_type                        (void);
SimNet*           sim_net_new                             (gchar           *name,
							   gint             c,
							   gint             a);

gchar*            sim_net_get_name                        (SimNet          *net);
void              sim_net_set_name                        (SimNet          *net,
							   gchar           *name);

gint              sim_net_get_c                           (SimNet          *net);
void              sim_net_set_c                           (SimNet          *net,
							   gint             c);

gint              sim_net_get_a                           (SimNet          *net);
void              sim_net_set_a                           (SimNet          *net,
							   gint             a);

void              sim_net_add_host_ip                     (SimNet          *net,
							   gchar           *ip);
void              sim_net_add_host                        (SimNet          *net,
							   GObject         *host);
void              sim_net_remove_host                     (SimNet          *net,
							   GObject         *host);
gboolean          sim_net_has_host                        (SimNet          *net,
							   GObject         *host);
GList*            sim_net_get_hosts                       (SimNet          *net);

void              sim_net_set_recovery                    (SimNet          *net,
							   gint             recovery);

G_END_DECLS

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SIM_NET_H__ */
