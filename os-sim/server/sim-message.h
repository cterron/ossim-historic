/* Message
 *
 *
 */

#ifndef __SIM_MESSAGE_H__
#define __SIM_MESSAGE_H__ 1

#include <glib.h>
#include <glib-object.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Definitions */
#define SNORT_DEFAULT_PRIORITY  2
#define SNORT_MAX_PRIORITY      3

#define FW1_DEFAULT_PRIORITY   1

#define FW1_ACCEPT_TYPE        1
#define FW1_DROP_TYPE          2
#define FW1_REJECT_TYPE        3

#define FW1_ACCEPT_PRIORITY    0
#define FW1_DROP_PRIORITY      1
#define FW1_REJECT_PRIORITY    1

#define RRD_DEFAULT_PRIORITY    5

#define GENERATOR_SPP_SPADE         104
#define GENERATOR_SPP_SCAN2         117
#define GENERATOR_SNORT_ENGINE        1


#define SIM_TYPE_MESSAGE                  (sim_message_get_type ())
#define SIM_MESSAGE(obj)                  (G_TYPE_CHECK_INSTANCE_CAST (obj, SIM_TYPE_MESSAGE, SimMessage))
#define SIM_MESSAGE_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST (klass, SIM_TYPE_MESSAGE, SimMessageClass))
#define SIM_IS_MESSAGE(obj)               (G_TYPE_CHECK_INSTANCE_TYPE (obj, SIM_TYPE_MESSAGE))
#define SIM_IS_MESSAGE_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), SIM_TYPE_MESSAGE))
#define SIM_MESSAGE_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), SIM_TYPE_MESSAGE, SimMessageClass))

G_BEGIN_DECLS

typedef enum {
  SIM_MESSAGE_TYPE_INVALID,
  SIM_MESSAGE_TYPE_SNORT,
  SIM_MESSAGE_TYPE_LOGGER,
  SIM_MESSAGE_TYPE_RRD
} SimMessageType;

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

G_END_DECLS

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SIM_MESSAGE_H__ */
