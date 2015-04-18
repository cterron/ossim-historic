/* Server
 *
 *
 */

#ifndef __SIM_SERVER_H__
#define __SIM_SERVER_H__ 1

#include <glib.h>
#include <glib-object.h>
#include <netinet/in.h>

#include "sim-enums.h"
#include "sim-config.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

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
SimServer*      sim_server_new                             (SimConfig       *config);

GObject*        sim_server_get_config                      (SimServer       *server);
void            sim_server_set_config                      (SimServer       *server,
							    GObject         *config);
void            sim_server_db_load_config                  (SimServer       *server);

gint            sim_server_get_recovery                    (SimServer       *server);
void            sim_server_set_recovery                    (SimServer       *server,
							    gint             recovery);

void            sim_server_run                             (SimServer       *server);

/* Messages Functions */
void            sim_server_push_tail_messages              (SimServer       *server,
							    GObject         *message);
GObject*        sim_server_pop_head_messages               (SimServer       *server);
gint            sim_server_get_messages_num                (SimServer       *server);

/* Policies Functions */
void            sim_server_add_policy                      (SimServer       *server,
							    GObject         *policy);
void            sim_server_remove_policy                   (SimServer       *server,
							    GObject         *policy);
GList*          sim_server_get_policies                    (SimServer       *server);
GObject*        sim_server_get_policy_by_match             (SimServer       *server,
							    gint             date,
							    gchar           *src_ip,
							    gchar           *dst_ip,
							    gint             port,
							    SimProtocolType  protocol,
							    gchar           *signature);

/* Hosts Functions */
void            sim_server_add_host                        (SimServer       *server,
							    GObject         *phost);
void            sim_server_remove_host                     (SimServer       *server,
							    GObject         *host);
GList*          sim_server_get_hosts                       (SimServer       *server);
GList*          sim_server_get_hosts_by_net_name           (SimServer       *server,
							    gchar           *net_name);
GObject*        sim_server_get_host_by_ip                  (SimServer       *server,
							    struct in_addr   ip);
void            sim_server_set_hosts_recovery              (SimServer       *server,
							    gint             recovery);
gint            sim_server_db_insert_host                  (SimServer       *server,
							    GObject         *host);
gint            sim_server_db_update_host                  (SimServer       *server,
							    GObject         *host);
gint            sim_server_db_delete_host                  (SimServer       *server,
							    GObject         *host);

void            sim_server_add_host_asset                  (SimServer       *server,
							    GObject         *host_asset);
void            sim_server_remove_host_asset               (SimServer       *server,
							    GObject         *host_asset);
GList*          sim_server_get_host_assets                 (SimServer       *server);
GObject*        sim_server_get_host_asset_by_ip            (SimServer       *server,
							    struct in_addr   ip);

/* Nets Functions */
void            sim_server_add_net                         (SimServer       *server,
							    GObject         *policy);
void            sim_server_remove_net                      (SimServer       *server,
							    GObject         *policy);
GList*          sim_server_get_nets                        (SimServer       *server);
GList*          sim_server_get_nets_by_host                (SimServer       *server,
							    GObject         *host);
GObject*        sim_server_get_net_by_name                 (SimServer       *server,
							    gchar           *name);
void            sim_server_set_nets_recovery                (SimServer      *server,
							    gint             recovery);
GList*          sim_server_db_get_nets                     (SimServer       *server);
gint            sim_server_db_insert_net                   (SimServer       *server,
							    GObject          *net);
gint            sim_server_db_update_net                   (SimServer       *server,
							    GObject         *net);
gint            sim_server_db_delete_net                   (SimServer       *server,
							    GObject         *net);

/* Net Assets Functions */
void            sim_server_add_net_asset                   (SimServer       *server,
							    GObject         *net_asset);
void            sim_server_remove_net_asset                (SimServer       *server,
							    GObject         *net_asset);
GList*          sim_server_get_net_assets                  (SimServer       *server);

/* Signatures Functions */
GObject*        sim_server_get_sig_subgroup_from_sid       (SimServer       *server,
							    gint             sid);
GObject*        sim_server_get_sig_subgroup_from_type      (SimServer       *server,
							    gint             type);

G_END_DECLS

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SIM_SERVER_H__ */
