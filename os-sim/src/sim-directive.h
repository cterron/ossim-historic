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

#ifndef __SIM_DIRECTIVE_H__
#define __SIM_DIRECTIVE_H__ 1

#include <glib.h>
#include <glib-object.h>

#include "sim-enums.h"
#include "sim-alert.h"
#include "sim-action.h"
#include "sim-rule.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SIM_TYPE_DIRECTIVE                  (sim_directive_get_type ())
#define SIM_DIRECTIVE(obj)                  (G_TYPE_CHECK_INSTANCE_CAST (obj, SIM_TYPE_DIRECTIVE, SimDirective))
#define SIM_DIRECTIVE_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST (klass, SIM_TYPE_DIRECTIVE, SimDirectiveClass))
#define SIM_IS_DIRECTIVE(obj)               (G_TYPE_CHECK_INSTANCE_TYPE (obj, SIM_TYPE_DIRECTIVE))
#define SIM_IS_DIRECTIVE_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), SIM_TYPE_DIRECTIVE))
#define SIM_DIRECTIVE_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), SIM_TYPE_DIRECTIVE, SimDirectiveClass))

G_BEGIN_DECLS

typedef struct _SimDirective        SimDirective;
typedef struct _SimDirectiveClass   SimDirectiveClass;
typedef struct _SimDirectivePrivate SimDirectivePrivate;

struct _SimDirective {
  GObject parent;

  SimDirectivePrivate  *_priv;
};

struct _SimDirectiveClass {
  GObjectClass parent_class;
};

GType             sim_directive_get_type                        (void);
SimDirective*     sim_directive_new                             (void);

void              sim_directive_lock                            (SimDirective     *directive);
void              sim_directive_unlock                          (SimDirective     *directive);
gboolean          sim_directive_trylock                         (SimDirective     *directive);

gint              sim_directive_get_id                          (SimDirective     *directive);
void              sim_directive_set_id                          (SimDirective     *directive,
								 gint              id);
gchar*            sim_directive_get_name                        (SimDirective     *directive);
void              sim_directive_set_name                        (SimDirective     *directive,
								 const gchar      *name);
GTime             sim_directive_get_time_out                    (SimDirective     *directive);
void              sim_directive_set_time_out                    (SimDirective     *directive,
								 GTime             time_out);
GTime             sim_directive_get_time_last                   (SimDirective     *directive);
void              sim_directive_set_time_last                   (SimDirective     *directive,
								 GTime             time_out);

GNode*            sim_directive_get_root_node                   (SimDirective     *directive);
void              sim_directive_set_root_node                   (SimDirective     *directive,
								 GNode            *rule_root);
GNode*            sim_directive_get_curr_node                   (SimDirective     *directive);
void              sim_directive_set_curr_node                   (SimDirective     *directive,
								 GNode            *rule_root);

SimRule*          sim_directive_get_root_rule                   (SimDirective     *directive);
SimRule*          sim_directive_get_curr_rule                   (SimDirective     *directive);

gint              sim_directive_get_rule_level                  (SimDirective     *directive);

GTime             sim_directive_get_rule_curr_time_out_max      (SimDirective     *directive);

void              sim_directive_append_action                   (SimDirective     *directive,
								 SimAction        *action);
void              sim_directive_remove_action                   (SimDirective     *directive,
								 SimAction        *action);
GList*            sim_directive_get_actions                     (SimDirective     *directive);
void              sim_directive_free_actions                    (SimDirective     *directive);


gint              sim_directive_get_level                       (SimDirective     *directive);

gboolean          sim_directive_match_by_alert                  (SimDirective     *directive,
								 SimAlert         *alert);
gboolean          sim_directive_backlog_match_by_alert          (SimDirective     *directive,
								 SimAlert         *alert);
void              sim_directive_set_rule_vars                   (SimDirective     *directive,
								 GNode            *node);

GNode*            sim_directive_get_node_branch_by_level        (SimDirective     *directive,
								 GNode            *node,
								 gint              level);

gboolean          sim_directive_get_matched                     (SimDirective     *directive);
gboolean          sim_directive_is_time_out                     (SimDirective     *directive);

GNode*            sim_directive_node_data_clone                 (GNode            *node);
void              sim_directive_node_data_destroy               (GNode            *node);
SimDirective*     sim_directive_clone                           (SimDirective     *directive);

gchar*            sim_directive_backlog_get_insert_clause       (SimDirective     *directive);
gchar*            sim_directive_backlog_get_update_clause       (SimDirective     *directive);
gchar*            sim_directive_backlog_get_delete_clause       (SimDirective     *directive);
gchar*            sim_directive_backlog_to_string               (SimDirective     *directive);

G_END_DECLS

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SIM_DIRECTIVE_H__ */
