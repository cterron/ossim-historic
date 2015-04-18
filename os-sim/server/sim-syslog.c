/**
 *
 *
 */

#include "sim-syslog.h"
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

struct _SimSyslogPrivate {
  SimServer   *server;

  gint         fd;
  GIOChannel  *io;
};

static gpointer parent_class = NULL;
static gint sim_server_signals[LAST_SIGNAL] = { 0 };

/* GType Functions */

static void
sim_syslog_class_init (SimSyslogClass * class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  parent_class = g_type_class_peek_parent (class);
}

static void
sim_syslog_instance_init (SimSyslog *syslog)
{
  syslog->_priv = g_new0 (SimSyslogPrivate, 1);

  syslog->_priv->io = NULL;
}

/* Public Methods */

GType
sim_syslog_get_type (void)
{
  static GType object_type = 0;
 
  if (!object_type)
  {
    static const GTypeInfo type_info = {
              sizeof (SimSyslogClass),
              NULL,
              NULL,
              (GClassInitFunc) sim_syslog_class_init,
              NULL,
              NULL,                       /* class data */
              sizeof (SimSyslog),
              0,                          /* number of pre-allocs */
              (GInstanceInitFunc) sim_syslog_instance_init,
              NULL                        /* value table */
    };
    
    g_type_init ();
                                                                                                                             
    object_type = g_type_register_static (G_TYPE_OBJECT, "SimSyslog", &type_info, 0);
  }
                                                                                                                             
  return object_type;
}

/*
 *
 *
 *
 *
 */
SimSyslog *
sim_syslog_new (void)
{
  SimSyslog *syslog = NULL;

  syslog = SIM_SYSLOG (g_object_new (SIM_TYPE_SYSLOG, NULL));

  return syslog;
}

/**
 *
 *
 *
 */
void
sim_syslog_set_server (SimSyslog *syslog,
		       SimServer *server)
{
  g_return_if_fail (syslog != NULL);
  g_return_if_fail (SIM_IS_SYSLOG (syslog));
  g_return_if_fail (server != NULL);
  g_return_if_fail (SIM_IS_SERVER (server));

  syslog->_priv->server = server;
}

/*
 *
 *
 *
 */
void
sim_syslog_run (SimSyslog *syslog)
{
  GIOStatus status;
  gchar *filename = "/tmp/auth.log";
  GString *buffer;
  gsize  lenght;
  gsize pos;
  GError *error = NULL;
  GTimer *timer;
  gdouble sec;
  gulong  micro;
  SimMessage *msg;

  g_return_if_fail (syslog != NULL);
  g_return_if_fail (SIM_IS_SYSLOG (syslog));
  g_return_if_fail (syslog->_priv->server != NULL);
  g_return_if_fail (SIM_IS_SERVER (syslog->_priv->server));

  syslog->_priv->io = g_io_channel_new_file (filename, "r", &error);
  if (error)
    {
      g_warning("Unable to open file %s: %s", filename, error->message);
      g_error_free(error);
      return;
    }

  g_io_channel_set_buffer_size (syslog->_priv->io, BUFFER_SIZE);
                                                                                                                             
  status = g_io_channel_set_flags (syslog->_priv->io, G_IO_FLAG_NONBLOCK, &error);
  if (status == G_IO_STATUS_ERROR)
    {
      g_warning(error->message);
      g_error_free(error);
      error = NULL;
    }

  buffer = g_string_sized_new (BUFFER_SIZE);

  timer = g_timer_new ();
  g_timer_start (timer);
  while (TRUE)
  {
    do
      status = g_io_channel_read_line_string(syslog->_priv->io, buffer, &pos, &error);
    while (status == G_IO_STATUS_AGAIN);

    if (error)
      {
	g_warning("ERROR %s", error->message);
	g_error_free(error);
	error = NULL;
      }

    if (status != G_IO_STATUS_NORMAL)
      break;

    msg = sim_message_new (buffer->str);

    if (msg == NULL)
      continue;

    sim_server_push_tail_messages (syslog->_priv->server, G_OBJECT (msg));
  }

  g_timer_stop (timer);
  sec = g_timer_elapsed (timer, &micro);
  g_message ("SYSLOG: SECUNDS %lf, %d", sec,  sim_server_get_messages_num (syslog->_priv->server));
  g_io_channel_unref(syslog->_priv->io);
}
