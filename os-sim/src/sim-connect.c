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
  GInetAddr* addr;
  GTcpSocket* socket;
  GIOChannel* iochannel;
  GIOError error;
  gchar *buffer;
  gchar *hostname;
  guint n;
  GList	*notifies;
  gint	risk,port;
  gchar timestamp[TIMEBUF_SIZE];
  // risk_a gets inserted too, have to check this.

  risk = event->risk_a;

  hostname = g_strdup(config->framework.host);

  if (!hostname)
  {
	  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Hostname error"); 	
    return 1;
  }
   
  port = config->framework.port;
    
 
  addr = gnet_inetaddr_new_nonblock (hostname, port);
  if (!addr)
  {
		g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Error creating the address");   
    return 1;
  }


  socket = gnet_tcp_socket_new (addr);
  gnet_inetaddr_delete (addr);
  if (!socket)
  {
    g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Error creating socket");
    return 1;
   }

	/* String to be sent */
	strftime (timestamp, TIMEBUF_SIZE, "%Y-%m-%d %H:%M:%S", localtime ((time_t *) &event->time));

	buffer=g_strdup_printf("event date=\"%s\" plugin_id=\"%d\" plugin_sid=\"%d\" risk=\"%d\" priority=\"%d\" reliability=\"%d\" event_id=\"%d\" backlog_id=\"%d\" src_ip=\"%s\" src_port=\"%d\" dst_ip=\"%s\" dst_port=\"%d\" protocol=\"%d\" sensor=\"%s\"\n", timestamp, event->plugin_id, event->plugin_sid, risk, event->priority, event->reliability, event->id, event->backlog_id, gnet_inetaddr_get_canonical_name (event->src_ia), event->src_port, gnet_inetaddr_get_canonical_name (event->dst_ia), event->dst_port, event->protocol, event->sensor);

	if (!buffer)
  { 
		g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "message error");
		return 1;
  }
	
	iochannel = gnet_tcp_socket_get_io_channel (socket);

	g_assert (iochannel != NULL);

	n = strlen(buffer);

	error = gnet_io_channel_writen (iochannel, buffer, n, &n);
	//error = gnet_io_channel_readn (iochannel, buffer, n, &n);

	//fwrite(buffer, n, 1, stdout);


	if (error != G_IO_ERROR_NONE)
  { 
		g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "message could not be sent"); 
    free(buffer);
    free (hostname);
    return 1;
  }

	gnet_tcp_socket_delete (socket);
	free(buffer);
	free(hostname);
	g_log(G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "message sent succesfully");
	return 0;
  
}

// vim: set tabstop=2:

