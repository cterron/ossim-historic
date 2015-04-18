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
#include "sim-host.h"
#include "sim-net.h"
#include "sim-signature.h"
#include "sim-message.h"
#include "sim-policy.h"
#include "sim-directive.h"
#include "sim-host-level.h"
#include "sim-net-level.h"

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

GType             sim_container_get_type                        (void);
SimContainer*     sim_container_new                             (void);

/* Recovery Function */
gint              sim_container_db_get_recovery                 (SimContainer  *container,
								 SimDatabase   *database);

/* Hosts Functions */
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
void              sim_container_db_load_nets                    (SimContainer  *container,
								 SimDatabase   *database);
void              sim_container_append_net                      (SimContainer  *container,
								 SimNet        *net);
void              sim_container_remove_net                      (SimContainer  *container,
								 SimNet        *net);
GList*            sim_container_get_nets                        (SimContainer  *container);
void              sim_container_set_nets                        (SimContainer  *container,
								 GList         *nets);
void              sim_container_free_hosts                      (SimContainer  *container);

GList*            sim_container_get_nets_has_ia                 (SimContainer  *container,
								 GInetAddr     *ia);
SimNet*           sim_container_get_net_by_name                 (SimContainer  *container,
								 gchar         *name);

/* Signatures Functions */
void              sim_container_db_load_signatures              (SimContainer  *container,
								 SimDatabase   *database);
void              sim_container_append_signature                (SimContainer  *container,
								 SimSignature  *signature);
void              sim_container_remove_signature                (SimContainer  *container,
								 SimSignature  *signature);
GList*            sim_container_get_signatures                  (SimContainer  *container);
SimSignature*     sim_container_get_signature_group_by_id       (SimContainer  *container,
								 gint           id);
SimSignature*     sim_container_get_signature_group_by_name     (SimContainer  *container,
								 gchar         *name);
SimSignature*     sim_container_get_signature_group_by_sid      (SimContainer  *container,
								 gint           sid);


/* Policies Functions */
void              sim_container_db_load_policies                (SimContainer  *container,
								 SimDatabase   *database);
void              sim_container_append_policy                   (SimContainer  *container,
								 SimPolicy     *policy);
void              sim_container_remove_policy                   (SimContainer  *container,
								 SimPolicy     *policy);
GList*            sim_container_get_policies                    (SimContainer  *container);
SimPolicy*        sim_container_get_policy_match                (SimContainer     *container,
								 gint              date,
								 GInetAddr        *src_ip,
								 GInetAddr        *dst_ip,
								 SimPortProtocol  *port,
								 gchar            *signature);

/* Directives Functions */

void              sim_container_load_directives_from_file       (SimContainer  *container,
								 const gchar   *filename);
void              sim_container_append_directive                (SimContainer  *container,
								 SimDirective  *directive);
void              sim_container_remove_directive                (SimContainer  *container,
								 SimDirective  *directive);
GList*            sim_container_get_directives                  (SimContainer  *container);

/* Host Levelss Functions */
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
SimHostLevel*     sim_container_get_host_level_by_ia            (SimContainer  *container,
								 GInetAddr     *ia);
void              sim_container_set_host_levels_recovery        (SimContainer  *container,
								 SimDatabase   *database,
								 gint           recovery);

/* Net Levels s Functions */
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
SimNetLevel*      sim_container_get_net_level_by_name           (SimContainer  *container,
								 gchar         *name);
void              sim_container_set_net_levels_recovery         (SimContainer  *container,
								 SimDatabase   *database,
								 gint           recovery);

/* Backlogs Functions */
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
void              sim_container_time_out_backlogs               (SimContainer  *container,
								 SimDatabase   *database);


/* Messages Functions */
void              sim_container_push_message                    (SimContainer  *container,
								 SimMessage    *message);
SimMessage*       sim_container_pop_message                     (SimContainer  *container);
gboolean          sim_container_is_empty_messages               (SimContainer  *container);
gint              sim_container_length_messages                 (SimContainer  *container);


G_END_DECLS

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SIM_CONTAINER_H__ */
