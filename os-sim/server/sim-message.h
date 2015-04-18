/* Message
 *
 *
 */

#ifndef __SIM_MESSAGE_H__
#define __SIM_MESSAGE_H__ 1

#include <glib.h>
#include <glib-object.h>

#include "sim-enums.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#define SIM_TYPE_MESSAGE                  (sim_message_get_type ())
#define SIM_MESSAGE(obj)                  (G_TYPE_CHECK_INSTANCE_CAST (obj, SIM_TYPE_MESSAGE, SimMessage))
#define SIM_MESSAGE_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST (klass, SIM_TYPE_MESSAGE, SimMessageClass))
#define SIM_IS_MESSAGE(obj)               (G_TYPE_CHECK_INSTANCE_TYPE (obj, SIM_TYPE_MESSAGE))
#define SIM_IS_MESSAGE_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), SIM_TYPE_MESSAGE))
#define SIM_MESSAGE_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), SIM_TYPE_MESSAGE, SimMessageClass))

G_BEGIN_DECLS

typedef struct _SimMessage        SimMessage;
typedef struct _SimMessageClass   SimMessageClass;
typedef struct _SimMessagePrivate SimMessagePrivate;

struct _SimMessage {
  GObject parent;

  SimMessageType type;

  SimMessagePrivate *_priv;
};

struct _SimMessageClass {
  GObjectClass parent_class;
};

GType           sim_message_get_type                    (void);
SimMessage*     sim_message_new                         (gchar *buffer);
void            sim_parser_buffer                       (gchar *buffer);

void            sim_message_print                       (SimMessage *msg);

gint            sim_message_get_plugin                  (SimMessage *msg);
gint            sim_message_get_tplugin                 (SimMessage *msg);
gint            sim_message_get_priority                (SimMessage *msg);

gchar*          sim_message_get_source_ip               (SimMessage *msg);
gchar*          sim_message_get_destination_ip          (SimMessage *msg);
gint            sim_message_get_source_port             (SimMessage *msg);
gint            sim_message_get_destination_port        (SimMessage *msg);
SimProtocolType sim_message_get_protocol                (SimMessage *msg);

G_END_DECLS

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SIM_MESSAGE_H__ */
