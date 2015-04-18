/**
 *
 *
 */


#include "sim-organizer.h"
#include "sim-server.h"
#include "sim-message.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <config.h>
 
#define BUFFER_SIZE 1024

enum 
{
  DESTROY,
  LAST_SIGNAL
};

struct _SimOrganizerPrivate {
  SimServer   *server;

  gint         fd;
  GIOChannel  *io;
};

static gpointer parent_class = NULL;
static gint sim_server_signals[LAST_SIGNAL] = { 0 };

/* GType Functions */

static void
sim_organizer_class_init (SimOrganizerClass * class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  parent_class = g_type_class_peek_parent (class);
}

static void
sim_organizer_instance_init (SimOrganizer *organizer)
{
  organizer->_priv = g_new0 (SimOrganizerPrivate, 1);

  organizer->_priv->io = NULL;
}

/* Public Methods */

GType
sim_organizer_get_type (void)
{
  static GType object_type = 0;
 
  if (!object_type)
  {
    static const GTypeInfo type_info = {
              sizeof (SimOrganizerClass),
              NULL,
              NULL,
              (GClassInitFunc) sim_organizer_class_init,
              NULL,
              NULL,                       /* class data */
              sizeof (SimOrganizer),
              0,                          /* number of pre-allocs */
              (GInstanceInitFunc) sim_organizer_instance_init,
              NULL                        /* value table */
    };
    
    g_type_init ();
                                                                                                                             
    object_type = g_type_register_static (G_TYPE_OBJECT, "SimOrganizer", &type_info, 0);
  }
                                                                                                                             
  return object_type;
}

/*
 *
 *
 *
 *
 */
SimOrganizer *
sim_organizer_new (void)
{
  SimOrganizer *organizer = NULL;

  organizer = SIM_ORGANIZER (g_object_new (SIM_TYPE_ORGANIZER, NULL));

  return organizer;
}

/**
 *
 *
 *
 */
void
sim_organizer_set_server (SimOrganizer *organizer,
		       SimServer *server)
{
  g_return_if_fail (organizer != NULL);
  g_return_if_fail (SIM_IS_ORGANIZER (organizer));
  g_return_if_fail (server != NULL);
  g_return_if_fail (SIM_IS_SERVER (server));

  organizer->_priv->server = server;
}

/*
 *
 *
 *
 */
void
sim_organizer_run (SimOrganizer *organizer)
{
  SimMessage *msg = NULL;

  g_return_if_fail (organizer != NULL);
  g_return_if_fail (SIM_IS_ORGANIZER (organizer));
  g_return_if_fail (organizer->_priv->server != NULL);
  g_return_if_fail (SIM_IS_SERVER (organizer->_priv->server));

  while (TRUE) {
    msg = SIM_MESSAGE (sim_server_pop_head_messages (organizer->_priv->server));

    if (msg == NULL)
      continue;

    
  }
}

/*
 *
 *
 */
void
sim_organizer_calificate (SimOrganizer *organizer, 
			  SimMessage *message)
{
  g_return_if_fail (organizer != NULL);
  g_return_if_fail (SIM_IS_ORGANIZER (organizer));
  g_return_if_fail (message != NULL);
  g_return_if_fail (SIM_IS_MESSAGE (message));
}
