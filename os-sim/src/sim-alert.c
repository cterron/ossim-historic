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

#include <config.h>
#include <time.h>

#include "sim-alert.h"

#include <time.h>
#include <math.h>
enum 
{
  DESTROY,
  LAST_SIGNAL
};

static gpointer parent_class = NULL;
static gint sim_server_signals[LAST_SIGNAL] = { 0 };

/* GType Functions */

static void 
sim_alert_impl_dispose (GObject  *gobject)
{
  G_OBJECT_CLASS (parent_class)->dispose (gobject);
}

static void 
sim_alert_impl_finalize (GObject  *gobject)
{
  SimAlert *alert = (SimAlert *) gobject;

  if (alert->sensor)
    g_free (alert->sensor);
  if (alert->src_ia)
    gnet_inetaddr_unref (alert->src_ia);
  if (alert->dst_ia)
    gnet_inetaddr_unref (alert->dst_ia);
  if (alert->value)
    g_free (alert->value);
  if (alert->data)
    g_free (alert->data);

  G_OBJECT_CLASS (parent_class)->finalize (gobject);
}

static void
sim_alert_class_init (SimAlertClass * class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  parent_class = g_type_class_ref (G_TYPE_OBJECT);

  object_class->dispose = sim_alert_impl_dispose;
  object_class->finalize = sim_alert_impl_finalize;
}

static void
sim_alert_instance_init (SimAlert *alert)
{
  alert->type = SIM_ALERT_TYPE_NONE;

  alert->time = time (NULL);
  alert->sensor = NULL;
  alert->interface = NULL;

  alert->plugin_id = 0;
  alert->plugin_sid = 0;

  alert->protocol = SIM_PROTOCOL_TYPE_NONE;
  alert->src_ia = NULL;
  alert->dst_ia = NULL;
  alert->src_port = 0;
  alert->dst_port = 0;

  alert->condition = SIM_CONDITION_TYPE_NONE;
  alert->value = NULL;
  alert->interval = 0;

  alert->alarm = FALSE;
  alert->priority = 1;
  alert->reliability = 1;
  alert->asset_src = 1;
  alert->asset_dst = 1;
  alert->risk_c = 1;
  alert->risk_a = 1;

  alert->data = NULL;
}

/* Public Methods */

GType
sim_alert_get_type (void)
{
  static GType object_type = 0;
 
  if (!object_type)
  {
    static const GTypeInfo type_info = {
              sizeof (SimAlertClass),
              NULL,
              NULL,
              (GClassInitFunc) sim_alert_class_init,
              NULL,
              NULL,                       /* class data */
              sizeof (SimAlert),
              0,                          /* number of pre-allocs */
              (GInstanceInitFunc) sim_alert_instance_init,
              NULL                        /* value table */
    };
    
    g_type_init ();
                                                                                                                             
    object_type = g_type_register_static (G_TYPE_OBJECT, "SimAlert", &type_info, 0);
  }

  return object_type;
}

/*
 *
 *
 *
 *
 */
SimAlert*
sim_alert_new (void)
{
  SimAlert *alert = NULL;

  alert = SIM_ALERT (g_object_new (SIM_TYPE_ALERT, NULL));

  return alert;
}

/*
 *
 *
 *
 *
 */
SimAlert*
sim_alert_new_from_type (SimAlertType   type)
{
  SimAlert *alert = NULL;

  alert = SIM_ALERT (g_object_new (SIM_TYPE_ALERT, NULL));
  alert->type = type;

  return alert;
}

/*
 *
 *
 *
 *
 */
SimAlertType
sim_alert_get_type_from_str (const gchar *str)
{
  g_return_val_if_fail (str, SIM_ALERT_TYPE_NONE);

  if (!g_ascii_strcasecmp (str, SIM_DETECTOR_CONST))
    return SIM_ALERT_TYPE_DETECTOR;
  else if (!g_ascii_strcasecmp (str, SIM_MONITOR_CONST))
    return SIM_ALERT_TYPE_MONITOR;

  return SIM_ALERT_TYPE_NONE;
}

/*
 *
 *
 *
 *
 */
SimAlert*
sim_alert_clone (SimAlert       *alert)
{
  SimAlert *new_alert;

  new_alert = SIM_ALERT (g_object_new (SIM_TYPE_ALERT, NULL));
  new_alert->type = alert->type;

  new_alert->time = alert->time;

  (alert->sensor) ? new_alert->sensor = g_strdup (alert->sensor) : NULL;
  (alert->interface) ? new_alert->interface = g_strdup (alert->interface) : NULL;

  new_alert->plugin_id = alert->plugin_id;
  new_alert->plugin_sid = alert->plugin_sid;

  new_alert->protocol = alert->protocol;
  (alert->src_ia) ? new_alert->src_ia = gnet_inetaddr_clone (alert->src_ia): NULL;
  (alert->dst_ia) ? new_alert->dst_ia = gnet_inetaddr_clone (alert->dst_ia): NULL;
  new_alert->src_port = alert->src_port ;
  new_alert->dst_port = alert->dst_port;

  new_alert->condition = alert->condition;
  (alert->value) ? new_alert->value = g_strdup (alert->value) : NULL;
  new_alert->interval = alert->interval;

  new_alert->alarm = alert->alarm;
  new_alert->priority = alert->priority;
  new_alert->reliability = alert->reliability;
  new_alert->asset_src = alert->asset_src;
  new_alert->asset_dst = alert->asset_dst;
  new_alert->risk_c = alert->risk_c;
  new_alert->risk_a = alert->risk_a;

  return new_alert;
}


/*
 *
 *
 *
 *
 */
void
sim_alert_print (SimAlert   *alert)
{
  gchar    timestamp[TIMEBUF_SIZE];

  g_return_if_fail (alert);
  g_return_if_fail (SIM_IS_ALERT (alert));


  g_print ("alert");
  switch (alert->type)
    {
    case SIM_ALERT_TYPE_DETECTOR:
      g_print (" type=\"detector\"");
      break;
    case SIM_ALERT_TYPE_MONITOR:
      g_print (" type=\"monitor\"");
      break;
    case SIM_ALERT_TYPE_NONE:
      g_print (" type=\"none\"");
      break;
    }

  if (alert->time)
    {
      strftime (timestamp, TIMEBUF_SIZE, "%Y-%m-%d %H:%M:%S", localtime ((time_t *) &alert->time));
      g_print (" timestamp=\"%s\"", timestamp);
    }

  g_print (" alarm=\"%s\"", (alert->alarm) ? "true" : "false");

  if (alert->sensor)
      g_print (" sensor=\"%s\"", alert->sensor);
  if (alert->interface)
      g_print (" interface=\"%s\"", alert->interface);

  if (alert->plugin_id)
      g_print (" plugin_id=\"%d\"", alert->plugin_id);
  if (alert->plugin_sid)
      g_print (" plugin_sid=\"%d\"", alert->plugin_sid);

  if (alert->protocol)
      g_print (" protocol=\"%d\"", alert->protocol);

  if (alert->src_ia)
      g_print (" src_ia=\"%s\"", gnet_inetaddr_get_canonical_name (alert->src_ia));
  if (alert->src_port)
      g_print (" src_port=\"%d\"", alert->src_port);
  if (alert->dst_ia)
      g_print (" dst_ia=\"%s\"", gnet_inetaddr_get_canonical_name (alert->dst_ia));
  if (alert->dst_port)
      g_print (" dst_port=\"%d\"", alert->dst_port);

  if (alert->condition)
      g_print (" condition=\"%d\"", alert->condition);
  if (alert->value)
      g_print (" value=\"%s\"", alert->value);
  if (alert->interval)
      g_print (" ineterval=\"%d\"", alert->interval);

  if (alert->priority)
      g_print (" priority=\"%d\"", alert->priority);
  if (alert->reliability)
      g_print (" reliability=\"%d\"", alert->reliability);
  if (alert->asset_src)
      g_print (" asset_src=\"%d\"", alert->asset_src);
  if (alert->asset_dst)
      g_print (" asset_dst=\"%d\"", alert->asset_dst);
  if (alert->risk_c)
      g_print (" risk_c=\"%d\"", alert->risk_c);
  if (alert->risk_a)
      g_print (" risk_a=\"%d\"", alert->risk_a);


  g_print ("\n");
}

/*
 *
 *
 *
 *
 */
gchar*
sim_alert_get_ossim_insert_clause (SimAlert   *alert)
{
  gchar    timestamp[TIMEBUF_SIZE];
  gchar   *query;
  gint     c;
  gint     a;

  g_return_val_if_fail (alert, NULL);
  g_return_val_if_fail (SIM_IS_ALERT (alert), NULL);

  c = rint (alert->risk_c);
  a = rint (alert->risk_a);

  strftime (timestamp, TIMEBUF_SIZE, "%Y-%m-%d %H:%M:%S", localtime ((time_t *) &alert->time));

  query = g_strdup_printf ("INSERT INTO alert "
			   "(timestamp, sensor, interface, type, plugin_id, plugin_sid, " 
			   "protocol, src_ip, dst_ip, src_port, dst_port, "
			   "condition, value, time_interval, "
			   "priority, reliability, asset_src, asset_dst, risk_c, risk_a, alarm) "
			   " VALUES  ('%s', '%s', '%s', %d, %d, %d,"
			   " %d, %lu, %lu, %d, %d, %d, '%s', %d, %d, %d, %d, %d, %d, %d, %d)",
			   timestamp,
			   (alert->sensor) ? alert->sensor : "",
			   (alert->interface) ? alert->interface : "",
			   alert->type,
			   alert->plugin_id,
			   alert->plugin_sid,
			   alert->protocol,
			   (alert->src_ia) ? sim_inetaddr_ntohl (alert->src_ia) : -1,
			   (alert->dst_ia) ? sim_inetaddr_ntohl (alert->dst_ia) : -1,
			   alert->src_port,
			   alert->dst_port,
			   alert->condition,
			   (alert->value) ? alert->value : "",
			   alert->interval,
			   alert->priority,
			   alert->reliability,
			   alert->asset_src,
			   alert->asset_dst,
			   c, a,
			   alert->alarm);

  return query;
}
