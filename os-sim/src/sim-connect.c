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

#include <glib.h>
#include "sim-util.h"
#include "sim-connect.h"
#include <config.h>

int
sim_connect_send_alarm(SimConfig *config, SimEvent *event) 
{
  GInetAddr* addr = NULL;
  GTcpSocket* socket = NULL;
  GIOChannel* iochannel = NULL;
  GIOError error;
  gchar *buffer = NULL;
  gchar *aux = NULL;
  gchar *ip_src = NULL;
  gchar *ip_dst = NULL;
  gchar *hostname = NULL;
  gsize n;
  GList	*notifies = NULL;
  gint	risk,port;
  gchar timestamp[TIMEBUF_SIZE];

  risk = event->risk_a;

	// Send max risk 
  // i.e., to avoid risk=0 when destination is 0.0.0.0
  if (event->risk_a > event->risk_c)
    risk = event->risk_a;
  else
    risk = event->risk_c;

  hostname = g_strdup(config->framework.host);

  if (!hostname)
  {
		//may be that this host hasn't got any frameworkd. If the event is forwarded to other server, it will be sended to the
		//other server framework (supposed it has a defined one).
	  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_connect_send_alarm: Hostname error");
    return 1;
  }
   
  port = config->framework.port;
    
 
  addr = gnet_inetaddr_new_nonblock (hostname, port);
  if (!addr)
  {
		g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_connect_send_alarm: Error creating the address");   
    return 1;
  }


  socket = gnet_tcp_socket_new (addr);
  gnet_inetaddr_delete (addr);
  if (!socket)
  {
    g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_connect_send_alarm: Error creating socket");
    g_free (hostname);
    return 1;
   }

	/* String to be sent */
	strftime (timestamp, TIMEBUF_SIZE, "%Y-%m-%d %H:%M:%S", localtime ((time_t *) &event->time));


  ip_src = gnet_inetaddr_get_canonical_name (event->src_ia);
  ip_dst = gnet_inetaddr_get_canonical_name (event->dst_ia);

  //FIXME? In a future, Policy will substitute this and this won't be neccesary. Also is needed to check
  //if this funcionality is really interesting
  aux = g_strdup_printf("event date=\"%s\" plugin_id=\"%d\" plugin_sid=\"%d\" risk=\"%d\" priority=\"%d\" reliability=\"%d\" event_id=\"%d\" backlog_id=\"%d\" src_ip=\"%s\" src_port=\"%d\" dst_ip=\"%s\" dst_port=\"%d\" protocol=\"%d\" sensor=\"%s\"", timestamp, event->plugin_id, event->plugin_sid, risk, event->priority, event->reliability, event->id, event->backlog_id, ip_src, event->src_port, ip_dst, event->dst_port, event->protocol, event->sensor);

  g_free (ip_src);
  g_free (ip_dst);

  buffer = g_strconcat (aux,
    event->filename ?  " filename=\"" : "", event->filename ? event->filename : "", event->filename ? "\"" : "",
    event->username ?  " username=\"" : "", event->username ? event->username : "", event->username ? "\"" : "",
    event->password ?  " password=\"" : "", event->password ? event->password : "",   event->password ? "\"" : "",
    event->userdata1 ? " userdata1=\"" : "",event->userdata1 ? event->userdata1 : "",event->userdata1 ? "\"" : "",
    event->userdata2 ? " userdata2=\"" : "",event->userdata2 ? event->userdata2 : "",event->userdata2 ? "\"" : "",
    event->userdata3 ? " userdata3=\"" : "",event->userdata3 ? event->userdata3 : "",event->userdata3 ? "\"" : "",
    event->userdata4 ? " userdata4=\"" : "",event->userdata4 ? event->userdata4 : "",event->userdata4 ? "\"" : "",
    event->userdata5 ? " userdata5=\"" : "",event->userdata5 ? event->userdata5 : "",event->userdata5 ? "\"" : "",
    event->userdata6 ? " userdata6=\"" : "",event->userdata6 ? event->userdata6 : "",event->userdata6 ? "\"" : "",
    event->userdata7 ? " userdata7=\"" : "",event->userdata7 ? event->userdata7 : "",event->userdata7 ? "\"" : "",
    event->userdata8 ? " userdata8=\"" : "",event->userdata8 ? event->userdata8 : "",event->userdata8 ? "\"" : "",
    event->userdata9 ? " userdata9=\"" : "",event->userdata9 ? event->userdata9 : "",event->userdata9 ? "\"" : "",
    "\n", NULL);

	if (!buffer)
  { 
		g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_connect_send_alarm: message error");
    g_free (aux);
    g_free (hostname);
	  gnet_tcp_socket_delete (socket);
		return 1;
  }
  else
    g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_connect_send_alarm: Message sended: %s", buffer); 
	
	iochannel = gnet_tcp_socket_get_io_channel (socket);

	g_assert (iochannel != NULL);

	n = strlen(buffer);

	error = gnet_io_channel_writen (iochannel, buffer, n, &n);
	//error = gnet_io_channel_readn (iochannel, buffer, n, &n);

	//fwrite(buffer, n, 1, stdout);


	gnet_tcp_socket_delete (socket);

	if (error != G_IO_ERROR_NONE)
  { 
		g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_connect_send_alarm: message could not be sent"); 
    g_free (buffer);
    g_free (aux);
    g_free (hostname);
    return 1;
  }

	g_free (buffer);
	g_free (aux);
	g_free (hostname);
	g_log(G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_connect_send_alarm: message sent succesfully");
	return 0;
  
}

// vim: set tabstop=2:

