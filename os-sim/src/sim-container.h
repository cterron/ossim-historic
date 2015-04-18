/* Copyright (c) 2003 ossim.net
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission
 *    from the author.
 *
 * 4. Products derived from this software may not be called "Os-sim" nor
 *    may "Os-sim" appear in their names without specific prior written
 *    permission from the author.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
 * THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __SIM_CONTAINER_H__
#define __SIM_CONTAINER_H__ 1

#include <glib.h>
#include <glib-object.h>
#include <gnet.h>

#include "sim-enums.h"
#include "sim-database.h"
#include "sim-category.h"
#include "sim-classification.h"
#include "sim-plugin.h"
#include "sim-plugin-sid.h"
#include "sim-sensor.h"
#include "sim-host.h"
#include "sim-net.h"
#include "sim-alert.h"
#include "sim-policy.h"
#include "sim-directive.h"
#include "sim-host-level.h"
#include "sim-net-level.h"
#include "sim-config.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SIM_TYPE_CONTAINER                  (sim_container_get_type ())
#define SIM_CONTAINER(obj)                  (G_TYPE_CHECK_INSTANCE_CAST (obj, SIM_TYPE_CONTAINER, SimContainer))
#define SIM_CONTAINER_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST (klass, SIM_TYPE_CONTAINER, SimContainerClass))
#define SIM_IS_CONTAINER(obj)               (G_TYPE_CHECK_INSTANCE_TYPE (obj, SIM_TYPE_CONTAINER))
#define SIM_IS_CONTAINER_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), SIM_TYPE_CONTAINER))
#define SIM_CONTAINER_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), SIM_TYPE_CONTAINER, SimContainerClass))

G_BEGIN_DECLS

typedef struct _SimContainer         SimContainer;
typedef struct _SimContainerClass    SimContainerClass;
typedef struct _SimContainerPrivate  SimContainerPrivate;

struct _SimContainer {
  GObject parent;

  SimContainerPrivate  *_priv;
};

struct _SimContainerClass {
  GObjectClass parent_class;
};

G_LOCK_DEFINE_STATIC (s_mutex_config);
G_LOCK_DEFINE_STATIC (s_mutex_categories);
G_LOCK_DEFINE_STATIC (s_mutex_classifications);
G_LOCK_DEFINE_STATIC (s_mutex_plugins);
G_LOCK_DEFINE_STATIC (s_mutex_plugin_sids);
G_LOCK_DEFINE_STATIC (s_mutex_sensors);
G_LOCK_DEFINE_STATIC (s_mutex_hosts);
G_LOCK_DEFINE_STATIC (s_mutex_nets);
G_LOCK_DEFINE_STATIC (s_mutex_policies);
G_LOCK_DEFINE_STATIC (s_mutex_directives);
G_LOCK_DEFINE_STATIC (s_mutex_host_levels);
G_LOCK_DEFINE_STATIC (s_mutex_net_levels);
G_LOCK_DEFINE_STATIC (s_mutex_backlogs);
G_LOCK_DEFINE_STATIC (s_mutex_alerts);

GType             sim_container_get_type                        (void);
SimContainer*     sim_container_new                             (SimConfig     *config);

/* Recovery Function */

gint              sim_container_db_get_recovery_ul              (SimContainer  *container,
								 SimDatabase   *database);
gint              sim_container_db_get_recovery                 (SimContainer  *container,
								 SimDatabase   *database);
/* Categories Functions */

void              sim_container_db_load_categories_ul           (SimContainer  *container,
								 SimDatabase   *database);
void              sim_container_append_category_ul              (SimContainer  *container,
								 SimCategory     *category);
void              sim_container_remove_category_ul              (SimContainer  *container,
								 SimCategory     *category);
GList*            sim_container_get_categories_ul               (SimContainer  *container);
void              sim_container_set_categories_ul               (SimContainer  *container,
								 GList         *categories);
void              sim_container_free_categories_ul              (SimContainer  *container);

SimCategory*      sim_container_get_category_by_id_ul           (SimContainer  *container,
								 gint           id);
SimCategory*      sim_container_get_category_by_name_ul         (SimContainer  *container,
								 const gchar   *name);

void              sim_container_db_load_categories              (SimContainer  *container,
								 SimDatabase   *database);
void              sim_container_append_category                 (SimContainer  *container,
								 SimCategory     *category);
void              sim_container_remove_category                 (SimContainer  *container,
								 SimCategory     *category);
GList*            sim_container_get_categories                  (SimContainer  *container);
void              sim_container_set_categories                  (SimContainer  *container,
								 GList         *categories);
void              sim_container_free_categories                 (SimContainer  *container);

SimCategory*      sim_container_get_category_by_id              (SimContainer  *container,
								 gint           id);
SimCategory*      sim_container_get_category_by_name            (SimContainer  *container,
								 const gchar   *name);

/* Classifications Functions */

void              sim_container_db_load_classifications_ul      (SimContainer  *container,
								 SimDatabase   *database);
void              sim_container_append_classification_ul        (SimContainer  *container,
								 SimClassification     *classification);
void              sim_container_remove_classification_ul        (SimContainer  *container,
								 SimClassification     *classification);
GList*            sim_container_get_classifications_ul          (SimContainer  *container);
void              sim_container_set_classifications_ul          (SimContainer  *container,
								 GList         *classifications);
void              sim_container_free_classifications_ul         (SimContainer  *container);

SimClassification* sim_container_get_classification_by_id_ul    (SimContainer  *container,
								 gint           id);
SimClassification* sim_container_get_classification_by_name_ul  (SimContainer  *container,
								 const gchar   *name);

void              sim_container_db_load_classifications         (SimContainer  *container,
								 SimDatabase   *database);
void              sim_container_append_classification           (SimContainer  *container,
								 SimClassification     *classification);
void              sim_container_remove_classification           (SimContainer  *container,
								 SimClassification     *classification);
GList*            sim_container_get_classifications             (SimContainer  *container);
void              sim_container_set_classifications             (SimContainer  *container,
								 GList         *classifications);
void              sim_container_free_classifications            (SimContainer  *container);

SimClassification* sim_container_get_classification_by_id       (SimContainer  *container,
								 gint           id);
SimClassification* sim_container_get_classification_by_name     (SimContainer  *container,
								 const gchar   *name);

/* Plugins Functions */

void              sim_container_db_load_plugins_ul              (SimContainer  *container,
								 SimDatabase   *database);
void              sim_container_append_plugin_ul                (SimContainer  *container,
								 SimPlugin     *plugin);
void              sim_container_remove_plugin_ul                (SimContainer  *container,
								 SimPlugin     *plugin);
GList*            sim_container_get_plugins_ul                  (SimContainer  *container);
void              sim_container_set_plugins_ul                  (SimContainer  *container,
								 GList         *plugins);
void              sim_container_free_plugins_ul                 (SimContainer  *container);

SimPlugin*        sim_container_get_plugin_by_id_ul             (SimContainer  *container,
								 gint           id);

void              sim_container_db_load_plugins                 (SimContainer  *container,
								 SimDatabase   *database);
void              sim_container_append_plugin                   (SimContainer  *container,
								 SimPlugin     *plugin);
void              sim_container_remove_plugin                   (SimContainer  *container,
								 SimPlugin     *plugin);
GList*            sim_container_get_plugins                     (SimContainer  *container);
void              sim_container_set_plugins                     (SimContainer  *container,
								 GList         *plugins);
void              sim_container_free_plugins                    (SimContainer  *container);

SimPlugin*        sim_container_get_plugin_by_id                (SimContainer  *container,
								 gint           id);

/* Plugin Sids Functions */

void              sim_container_db_load_plugin_sids_ul          (SimContainer  *container,
								 SimDatabase   *database);
void              sim_container_append_plugin_sid_ul            (SimContainer  *container,
								 SimPluginSid  *plugin_sid);
void              sim_container_remove_plugin_sid_ul            (SimContainer  *container,
								 SimPluginSid  *plugin_sid);
GList*            sim_container_get_plugin_sids_ul              (SimContainer  *container);
void              sim_container_set_plugin_sids_ul              (SimContainer  *container,
								 GList         *plugin_sids);
void              sim_container_free_plugin_sids_ul             (SimContainer  *container);

SimPluginSid*     sim_container_get_plugin_sid_by_pky_ul        (SimContainer  *container,
								 gint           plugin_id,
								 gint           sid);

void              sim_container_db_load_plugin_sids             (SimContainer  *container,
								 SimDatabase   *database);
void              sim_container_append_plugin_sid               (SimContainer  *container,
								 SimPluginSid  *plugin_sid);
void              sim_container_remove_plugin_sid               (SimContainer  *container,
								 SimPluginSid  *plugin_sid);
GList*            sim_container_get_plugin_sids                 (SimContainer  *container);
void              sim_container_set_plugin_sids                 (SimContainer  *container,
								 GList         *plugin_sids);
void              sim_container_free_plugin_sids                (SimContainer  *container);

SimPluginSid*     sim_container_get_plugin_sid_by_pky           (SimContainer  *container,
								 gint           plugin_id,
								 gint           sid);

/* Sensors Functions */

void              sim_container_db_load_sensors_ul              (SimContainer  *container,
								 SimDatabase   *database);
void              sim_container_append_sensor_ul                (SimContainer  *container,
								 SimSensor     *sensor);
void              sim_container_remove_sensor_ul                (SimContainer  *container,
								 SimSensor     *sensor);
GList*            sim_container_get_sensors_ul                  (SimContainer  *container);
void              sim_container_set_sensors_ul                  (SimContainer  *container,
								 GList         *sensors);
void              sim_container_free_sensors_ul                 (SimContainer  *container);

SimSensor*        sim_container_get_sensor_by_name_ul           (SimContainer  *container,
								 gchar         *name);

void              sim_container_db_load_sensors                 (SimContainer  *container,
								 SimDatabase   *database);
void              sim_container_append_sensor                   (SimContainer  *container,
								 SimSensor     *sensor);
void              sim_container_remove_sensor                   (SimContainer  *container,
								 SimSensor     *sensor);
GList*            sim_container_get_sensors                     (SimContainer  *container);
void              sim_container_set_sensors                     (SimContainer  *container,
								 GList         *sensors);
void              sim_container_free_sensors                    (SimContainer  *container);

SimSensor*        sim_container_get_sensor_by_name              (SimContainer  *container,
								 gchar         *name);

/* Hosts Functions */

void              sim_container_db_load_hosts_ul                (SimContainer  *container,
								 SimDatabase   *database);
void              sim_container_append_host_ul                  (SimContainer  *container,
								 SimHost       *host);
void              sim_container_remove_host_ul                  (SimContainer  *container,
								 SimHost       *host);
GList*            sim_container_get_hosts_ul                    (SimContainer  *container);
void              sim_container_set_hosts_ul                    (SimContainer  *container,
								 GList         *hosts);
void              sim_container_free_hosts_ul                   (SimContainer  *container);

SimHost*          sim_container_get_host_by_ia_ul               (SimContainer  *container,
								 GInetAddr     *ia);

void              sim_container_db_load_hosts                   (SimContainer  *container,
								 SimDatabase   *database);
void              sim_container_append_host                     (SimContainer  *container,
								 SimHost       *host);
void              sim_container_remove_host                     (SimContainer  *container,
								 SimHost       *host);
GList*            sim_container_get_hosts                       (SimContainer  *container);
void              sim_container_set_hosts                       (SimContainer  *container,
								 GList         *hosts);
void              sim_container_free_hosts                      (SimContainer  *container);

SimHost*          sim_container_get_host_by_ia                  (SimContainer  *container,
								 GInetAddr     *ia);

/* Nets Functions */
void              sim_container_db_load_nets_ul                 (SimContainer  *container,
								 SimDatabase   *database);
void              sim_container_append_net_ul                   (SimContainer  *container,
								 SimNet        *net);
void              sim_container_remove_net_ul                   (SimContainer  *container,
								 SimNet        *net);
GList*            sim_container_get_nets_ul                     (SimContainer  *container);
void              sim_container_set_nets_ul                     (SimContainer  *container,
								 GList         *nets);
void              sim_container_free_nets_ul                    (SimContainer  *container);

GList*            sim_container_get_nets_has_ia_ul              (SimContainer  *container,
								 GInetAddr     *ia);
SimNet*           sim_container_get_net_by_name_ul              (SimContainer  *container,
								 const gchar   *name);

void              sim_container_db_load_nets                    (SimContainer  *container,
								 SimDatabase   *database);
void              sim_container_append_net                      (SimContainer  *container,
								 SimNet        *net);
void              sim_container_remove_net                      (SimContainer  *container,
								 SimNet        *net);
GList*            sim_container_get_nets                        (SimContainer  *container);
void              sim_container_set_nets                        (SimContainer  *container,
								 GList         *nets);
void              sim_container_free_nets                       (SimContainer  *container);

GList*            sim_container_get_nets_has_ia                 (SimContainer  *container,
								 GInetAddr     *ia);
SimNet*           sim_container_get_net_by_name                 (SimContainer  *container,
								 const gchar   *name);


/* Policies Functions */
void              sim_container_db_load_policies_ul             (SimContainer  *container,
								 SimDatabase   *database);
void              sim_container_append_policy_ul                (SimContainer  *container,
								 SimPolicy     *policy);
void              sim_container_remove_policy_ul                (SimContainer  *container,
								 SimPolicy     *policy);
GList*            sim_container_get_policies_ul                 (SimContainer  *container);
void              sim_container_set_policies_ul                 (SimContainer  *container,
								 GList         *policies);
void              sim_container_free_policies_ul                (SimContainer  *container);

SimPolicy*        sim_container_get_policy_match_ul             (SimContainer     *container,
								 gint              date,
								 GInetAddr        *src_ip,
								 GInetAddr        *dst_ip,
								 SimPortProtocol  *port,
								 const gchar      *category);

void              sim_container_db_load_policies                (SimContainer  *container,
								 SimDatabase   *database);
void              sim_container_append_policy                   (SimContainer  *container,
								 SimPolicy     *policy);
void              sim_container_remove_policy                   (SimContainer  *container,
								 SimPolicy     *policy);
GList*            sim_container_get_policies                    (SimContainer  *container);
void              sim_container_set_policies                    (SimContainer  *container,
								 GList         *policies);
void              sim_container_free_policies                   (SimContainer  *container);

SimPolicy*        sim_container_get_policy_match                (SimContainer     *container,
								 gint              date,
								 GInetAddr        *src_ip,
								 GInetAddr        *dst_ip,
								 SimPortProtocol  *port,
								 const gchar      *category);

/* Directives Functions */

void              sim_container_load_directives_from_file_ul    (SimContainer  *container,
								 const gchar   *filename);
void              sim_container_append_directive_ul             (SimContainer  *container,
								 SimDirective  *directive);
void              sim_container_remove_directive_ul             (SimContainer  *container,
								 SimDirective  *directive);
GList*            sim_container_get_directives_ul               (SimContainer  *container);
void              sim_container_set_directives_ul               (SimContainer  *container,
								 GList         *directives);
void              sim_container_free_directives_ul              (SimContainer  *container);


void              sim_container_load_directives_from_file       (SimContainer  *container,
								 const gchar   *filename);
void              sim_container_append_directive                (SimContainer  *container,
								 SimDirective  *directive);
void              sim_container_remove_directive                (SimContainer  *container,
								 SimDirective  *directive);
GList*            sim_container_get_directives                  (SimContainer  *container);
void              sim_container_set_directives                  (SimContainer  *container,
								 GList         *directives);
void              sim_container_free_directives                 (SimContainer  *container);


/* Host Levelss Functions */
void              sim_container_db_load_host_levels_ul          (SimContainer  *container,
								 SimDatabase   *database);
void              sim_container_db_insert_host_level_ul         (SimContainer  *container,
								 SimDatabase   *database,
								 SimHostLevel  *host_level);
void              sim_container_db_update_host_level_ul         (SimContainer  *container,
								 SimDatabase   *database,
								 SimHostLevel  *host_level);
void              sim_container_db_delete_host_level_ul         (SimContainer  *container,
								 SimDatabase   *database,
								 SimHostLevel  *host_level);
void              sim_container_append_host_level_ul            (SimContainer  *container,
								 SimHostLevel  *host_level);
void              sim_container_remove_host_level_ul            (SimContainer  *container,
								 SimHostLevel  *host_level);
GList*            sim_container_get_host_levels_ul              (SimContainer  *container);
void              sim_container_set_host_levels_ul              (SimContainer  *container,
								 GList         *host_levels);
void              sim_container_free_host_levels_ul             (SimContainer  *container);

SimHostLevel*     sim_container_get_host_level_by_ia_ul         (SimContainer  *container,
								 GInetAddr     *ia);
void              sim_container_set_host_levels_recovery_ul     (SimContainer  *container,
								 SimDatabase   *database,
								 gint           recovery);

void              sim_container_db_load_host_levels             (SimContainer  *container,
								 SimDatabase   *database);
void              sim_container_db_insert_host_level            (SimContainer  *container,
								 SimDatabase   *database,
								 SimHostLevel  *host_level);
void              sim_container_db_update_host_level            (SimContainer  *container,
								 SimDatabase   *database,
								 SimHostLevel  *host_level);
void              sim_container_db_delete_host_level            (SimContainer  *container,
								 SimDatabase   *database,
								 SimHostLevel  *host_level);
void              sim_container_append_host_level               (SimContainer  *container,
								 SimHostLevel  *host_level);
void              sim_container_remove_host_level               (SimContainer  *container,
								 SimHostLevel  *host_level);
GList*            sim_container_get_host_levels                 (SimContainer  *container);
void              sim_container_set_host_levels                 (SimContainer  *container,
								 GList         *host_levels);
void              sim_container_free_host_levels                (SimContainer  *container);

SimHostLevel*     sim_container_get_host_level_by_ia            (SimContainer  *container,
								 GInetAddr     *ia);
void              sim_container_set_host_levels_recovery        (SimContainer  *container,
								 SimDatabase   *database,
								 gint           recovery);

/* Net Levels s Functions */
void              sim_container_db_load_net_levels_ul           (SimContainer  *container,
								 SimDatabase   *database);
void              sim_container_db_insert_net_level_ul          (SimContainer  *container,
								 SimDatabase   *database,
								 SimNetLevel   *net_level);
void              sim_container_db_update_net_level_ul          (SimContainer  *container,
								 SimDatabase   *database,
								 SimNetLevel   *net_level);
void              sim_container_db_delete_net_level_ul          (SimContainer  *container,
								 SimDatabase   *database,
								 SimNetLevel   *net_level);
void              sim_container_append_net_level_ul             (SimContainer  *container,
								 SimNetLevel   *net_level);
void              sim_container_remove_net_level_ul             (SimContainer  *container,
								 SimNetLevel   *net_level);
GList*            sim_container_get_net_levels_ul               (SimContainer  *container);
void              sim_container_set_net_levels_ul               (SimContainer  *container,
								 GList         *net_levels);
void              sim_container_free_net_levels_ul              (SimContainer  *container);

SimNetLevel*      sim_container_get_net_level_by_name_ul        (SimContainer  *container,
								 const gchar   *name);
void              sim_container_set_net_levels_recovery_ul      (SimContainer  *container,
								 SimDatabase   *database,
								 gint           recovery);

void              sim_container_db_load_net_levels              (SimContainer  *container,
								 SimDatabase   *database);
void              sim_container_db_insert_net_level             (SimContainer  *container,
								 SimDatabase   *database,
								 SimNetLevel   *net_level);
void              sim_container_db_update_net_level             (SimContainer  *container,
								 SimDatabase   *database,
								 SimNetLevel   *net_level);
void              sim_container_db_delete_net_level             (SimContainer  *container,
								 SimDatabase   *database,
								 SimNetLevel   *net_level);
void              sim_container_append_net_level                (SimContainer  *container,
								 SimNetLevel   *net_level);
void              sim_container_remove_net_level                (SimContainer  *container,
								 SimNetLevel   *net_level);
GList*            sim_container_get_net_levels                  (SimContainer  *container);
void              sim_container_set_net_levels                  (SimContainer  *container,
								 GList         *net_levels);
void              sim_container_free_net_levels                 (SimContainer  *container);

SimNetLevel*      sim_container_get_net_level_by_name           (SimContainer  *container,
								 const gchar   *name);
void              sim_container_set_net_levels_recovery         (SimContainer  *container,
								 SimDatabase   *database,
								 gint           recovery);

/* Backlogs Functions */
void              sim_container_db_insert_backlog_ul            (SimContainer  *container,
								 SimDatabase   *database,
								 SimDirective  *backlog);
void              sim_container_db_update_backlog_ul            (SimContainer  *container,
								 SimDatabase   *database,
								 SimDirective  *backlog);
void              sim_container_db_delete_backlog_ul            (SimContainer  *container,
								 SimDatabase   *database,
								 SimDirective  *backlog);
void              sim_container_append_backlog_ul               (SimContainer  *container,
								 SimDirective  *backlog);
void              sim_container_remove_backlog_ul               (SimContainer  *container,
								 SimDirective  *backlog);
GList*            sim_container_get_backlogs_ul                 (SimContainer  *container);
void              sim_container_set_backlogs_ul                 (SimContainer  *container,
								 GList         *backlogs);
void              sim_container_free_backlogs_ul                (SimContainer  *container);

void              sim_container_db_insert_backlog               (SimContainer  *container,
								 SimDatabase   *database,
								 SimDirective  *backlog);
void              sim_container_db_update_backlog               (SimContainer  *container,
								 SimDatabase   *database,
								 SimDirective  *backlog);
void              sim_container_db_delete_backlog               (SimContainer  *container,
								 SimDatabase   *database,
								 SimDirective  *backlog);
void              sim_container_append_backlog                  (SimContainer  *container,
								 SimDirective  *backlog);
void              sim_container_remove_backlog                  (SimContainer  *container,
								 SimDirective  *backlog);
GList*            sim_container_get_backlogs                    (SimContainer  *container);
void              sim_container_set_backlogs                    (SimContainer  *container,
								 GList         *backlogs);
void              sim_container_free_backlogs                   (SimContainer  *container);

/* Alerts Functions */
void              sim_container_push_alert                      (SimContainer  *container,
								 SimAlert      *alert);
SimAlert*         sim_container_pop_alert                       (SimContainer  *container);
void              sim_container_free_alerts                     (SimContainer  *container);

gboolean          sim_container_is_empty_alerts                 (SimContainer  *container);
gint              sim_container_length_alerts                   (SimContainer  *container);


G_END_DECLS

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SIM_CONTAINER_H__ */
