/* Copyright (c) 2007 ossim.net
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
#include <glib-object.h>
#include <config.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include "sim-command.h"
#include "sim-packet.h"
#include "sim-scanner-tokens.h"
#include "sim-config.h"
#include "sim-plugin-sid.h"
#include "os-sim.h"
#include "sim-util.h"
#include <zlib.h>
#include <netinet/in.h>

/* Prototypes */

gint sim_organizer_snort_sensor_get_sid (SimDatabase  *db_snort,gchar        *hostname,  gchar        *interface, gchar        *plugin_name);
/*
 * Scan the snort data. Must be in order
 * date
 * snort_gid
 * snort_sid
 * snort_rev
 * snort_classification
 * snort_priority
 */
gboolean sim_command_snort_event_packet_scan(GScanner *scanner,SimCommand *command){
	gboolean r = FALSE;
	SimPacket *packet;
	if ((packet = sim_packet_new())!=NULL){
		command->packet = packet;
		r = TRUE;
	}
	g_scanner_set_scope(scanner,SIM_COMMAND_SCOPE_SNORT_EVENT_DATA);
	g_scanner_get_next_token(scanner);
	/* type */
	if (r && scanner->token == SIM_COMMAND_SYMBOL_SNORT_EVENT_DATA_TYPE){
		r = FALSE;
		g_scanner_get_next_token(scanner); /* = */
		g_scanner_get_next_token(scanner); /* type */
		if (scanner->token == G_TOKEN_STRING){
			command->data.event.type = g_strdup(scanner->value.v_string);
			r = TRUE;
		}
	}
	if (r){
		g_scanner_get_next_token(scanner);
		r = FALSE;
		if (scanner->token == SIM_COMMAND_SYMBOL_SNORT_EVENT_DATE){
			struct tm t;
			g_scanner_get_next_token(scanner); /* = */
			g_scanner_get_next_token(scanner); /* Date */
			if (scanner->token == G_TOKEN_STRING && 
			(strptime(scanner->value.v_string, "%Y-%m-%d %H:%M:%S", &t)!=NULL)){
					command->data.event.date = g_strdup(scanner->value.v_string);
					r = TRUE;
			}
			

		}		
	}
	/* snort_gid: Snort generator */
	if (r){
		g_scanner_get_next_token(scanner);
		r = FALSE;
		if (scanner->token == SIM_COMMAND_SYMBOL_SNORT_EVENT_GID){
			g_scanner_get_next_token(scanner); /* = */
			g_scanner_get_next_token(scanner); /* snort_gid */
			if (scanner->token == G_TOKEN_STRING && sim_string_is_number (scanner->value.v_string, 0)){
				guint32 snort_gid =  strtoul(scanner->value.v_string,(char**)NULL,10);
				if (errno!=ERANGE && errno!=EINVAL && snort_gid<(G_MAXUINT32-1000)){
					command->snort_event.snort_gid = snort_gid;
					command->data.event.plugin_id = snort_gid+1000;
					r = TRUE;
				}
			}
		}
	}
	/* snort_sid */
	if (r){
		g_scanner_get_next_token(scanner);
		if (scanner->token == SIM_COMMAND_SYMBOL_SNORT_EVENT_SID){
			r = FALSE;
			g_scanner_get_next_token(scanner); /* = */
			g_scanner_get_next_token(scanner); /* snort_sid */
			if (scanner->token == G_TOKEN_STRING && sim_string_is_number(scanner->value.v_string,0)){
				guint32 snort_sid = strtoul(scanner->value.v_string,(char**)NULL,10);
				if (errno!=ERANGE && errno!=EINVAL){
					command->snort_event.snort_sid = snort_sid;
					command->data.event.plugin_sid = snort_sid;
					r = TRUE;
				}
			}
		}
	}
	/* snort_rev */
	if (r){
		g_scanner_get_next_token(scanner);
		if (scanner->token == SIM_COMMAND_SYMBOL_SNORT_EVENT_REV){
			r = FALSE;
			g_scanner_get_next_token(scanner); /* = */
			g_scanner_get_next_token(scanner); /* snort_rev */
			if (scanner->token == G_TOKEN_STRING && sim_string_is_number(scanner->value.v_string,0)){
				guint32 snort_rev = strtoul(scanner->value.v_string,(char**)NULL,10);
				if (errno!=ERANGE && errno!=EINVAL){
					command->snort_event.snort_rev = snort_rev;
					r = TRUE;
				}
			}
		}

	}
	/* snort_classification */
	if (r){
		g_scanner_get_next_token(scanner);
		if (scanner->token == SIM_COMMAND_SYMBOL_SNORT_EVENT_CLASSIFICATION){
			r = FALSE;
			g_scanner_get_next_token(scanner); /* = */
			g_scanner_get_next_token(scanner); /* snort_classification */
			if (scanner->token == G_TOKEN_STRING && sim_string_is_number(scanner->value.v_string,0)){
				guint32 snort_classification  = strtoul(scanner->value.v_string,(char**)NULL,10);
				if (errno!=ERANGE && errno!=EINVAL){
					command->snort_event.snort_classification  = snort_classification;
					r = TRUE;
				}
			}
		}
	}
	/* snort_priority */
	if (r){
		g_scanner_get_next_token(scanner);
		if (scanner->token == SIM_COMMAND_SYMBOL_SNORT_EVENT_PRIORITY){
			r = FALSE;
			g_scanner_get_next_token(scanner); /* = */
			g_scanner_get_next_token(scanner); /* snort_priority */
			if (scanner->token == G_TOKEN_STRING && sim_string_is_number(scanner->value.v_string,0)){
				guint32 snort_priority  = strtoul(scanner->value.v_string,(char**)NULL,10);
				if (errno!=ERANGE && errno!=EINVAL){
					command->snort_event.snort_priority = snort_priority;
					r = TRUE;
				}
			}
		}
	}
	/* Now, scan the packet */
	if (r){
		 g_scanner_get_next_token(scanner);
		r = FALSE;
		if (scanner->token == SIM_COMMAND_SYMBOL_SNORT_EVENT_PACKET_TYPE){
			/* now we haven't raw packects logs. Discart it, but the 
			 * Snort event is created*/
			g_scanner_get_next_token(scanner); /* = */
			g_scanner_get_next_token(scanner); /* value, discar it */
			if (scanner->token == G_TOKEN_STRING){
				if (strcmp(scanner->value.v_string,"raw")==0)
					r = sim_command_snort_event_packet_raw_scan(scanner,command);
				else if (strcmp(scanner->value.v_string,"ip")==0)
					r = sim_command_snort_event_packet_ip_scan(scanner,command);
				else 
					r = FALSE;
			}
		}
	}
	/* remember, after we insert in the database the event, the snort cid,sid of the event data
	 * points to the event*/
	return r;
}
gboolean sim_command_snort_event_packet_icmp_scan(GScanner *scanner,SimCommand *command){
	gboolean r = FALSE;
	g_scanner_set_scope(scanner,SIM_COMMAND_SCOPE_SNORT_EVENT_ICMP);
	/* icmp_type */
	g_scanner_get_next_token(scanner);
	if (scanner->token == SIM_COMMAND_SYMBOL_SNORT_EVENT_ICMP_TYPE){
		g_scanner_get_next_token(scanner);
		g_scanner_get_next_token(scanner);
		if (scanner->token == G_TOKEN_STRING && 
		sim_string_is_number (scanner->value.v_string, 0)){
			guint32 type = strtoul(scanner->value.v_string,(char**)NULL,10);
			if (errno!=ERANGE && errno!=EINVAL && type<256){
				command->packet->hdr.sim_icmphdr.icmp_type = type;
				r = TRUE;
			}
				
		}	
	}
	/* icmp_code */
	if (r){
		g_scanner_get_next_token(scanner);
		if (scanner->token == SIM_COMMAND_SYMBOL_SNORT_EVENT_ICMP_CODE){
		g_scanner_get_next_token(scanner);
			g_scanner_get_next_token(scanner);
			r = FALSE;
			if (scanner->token == G_TOKEN_STRING && 
			sim_string_is_number (scanner->value.v_string, 0)){
				guint32 code = strtoul(scanner->value.v_string,(char**)NULL,10);
				if (errno!=ERANGE && errno!=EINVAL && code <256){
					command->packet->hdr.sim_icmphdr.icmp_code = code;
					r = TRUE;
				}
				
			}	
		}
	}
	/* icmp_csum */
	if (r){
		g_scanner_get_next_token(scanner);
		if (scanner->token == SIM_COMMAND_SYMBOL_SNORT_EVENT_ICMP_CSUM){
			g_scanner_get_next_token(scanner);
			g_scanner_get_next_token(scanner);
			if (scanner->token == G_TOKEN_STRING && 
			sim_string_is_number (scanner->value.v_string, 0))
			{
				guint32 csum  = strtoul(scanner->value.v_string,(char**)NULL,10);
				if (errno!=ERANGE && errno!=EINVAL && csum<65536){
					command->packet->hdr.sim_icmphdr.icmp_cksum = csum;
					r = TRUE;
				}
				
			}	
					

		}
	}
	/* icmp_id */
	if (r){
		g_scanner_get_next_token(scanner);
		r = FALSE;
		if (scanner->token == SIM_COMMAND_SYMBOL_SNORT_EVENT_ICMP_ID){
			g_scanner_get_next_token(scanner);
			g_scanner_get_next_token(scanner);
			if (scanner->token == G_TOKEN_STRING && 
			sim_string_is_number (scanner->value.v_string, 0))
			{
				guint32 id  = strtoul(scanner->value.v_string,(char**)NULL,10);
				if (errno!=EINVAL && errno!=ERANGE && id<65536){
					command->packet->hdr.sim_icmphdr.un.echo.id = id;
					r = TRUE;
				}
				
			}
		}

	}
	/* icmp_seq */
	if (r){
		g_scanner_get_next_token(scanner);
		r = FALSE;
		if (scanner->token == SIM_COMMAND_SYMBOL_SNORT_EVENT_ICMP_SEQ){
			g_scanner_get_next_token(scanner);
			g_scanner_get_next_token(scanner);
			if (scanner->token == G_TOKEN_STRING && 
			sim_string_is_number (scanner->value.v_string, 0))
			{
				guint32 seq  = strtoul(scanner->value.v_string,(char**)NULL,10);
				if (errno!=EINVAL && errno!=ERANGE && seq<65536){
					command->packet->hdr.sim_icmphdr.un.echo.sequence = seq;
					r = TRUE;
				}
				
			}
		}
	}
	/* icmp_payload */
	if (r){
		g_scanner_get_next_token(scanner);
		r = FALSE;
		if (scanner->token == SIM_COMMAND_SYMBOL_SNORT_EVENT_ICMP_PAYLOAD){
			g_scanner_get_next_token(scanner);
			g_scanner_get_next_token(scanner);
			if (scanner->token == G_TOKEN_STRING){
				guint32 payloadlen = strlen(scanner->value.v_string)/2;
				if (payloadlen>0){
					command->packet->payload = (guint8*)sim_hex2bin(scanner->value.v_string);
					if (command->packet->payload!=NULL){
						r = TRUE;
						command->packet->payloadlen = payloadlen;
					}
				}		
				else{
					r = TRUE;
					command->packet->payloadlen = 0;
					command->packet->payload = NULL;
				}
			}
		}
	}
	/* find eof */
	do{
		g_scanner_get_next_token(scanner);
		if (scanner->token!=G_TOKEN_EOF)
			r = FALSE;
	}while (scanner->token!=G_TOKEN_EOF);
	return r;
}

gboolean sim_command_snort_event_packet_udp_scan(GScanner *scanner,SimCommand *command){
	gboolean r = FALSE;
	g_scanner_set_scope(scanner,SIM_COMMAND_SCOPE_SNORT_EVENT_UDP);
	/* udp_sport */
	g_scanner_get_next_token(scanner);
	g_message("udp sport");
	if (scanner->token == SIM_COMMAND_SYMBOL_SNORT_EVENT_UDP_SPORT){
		g_scanner_get_next_token(scanner);
		g_scanner_get_next_token(scanner);
		if (scanner->token == G_TOKEN_STRING &&
		sim_string_is_number(scanner->value.v_string,0))
		{
			guint32 sport = strtoul(scanner->value.v_string,(char**)NULL,10);
			if (errno!=ERANGE && errno!=EINVAL && sport<65536){
				command->packet->hdr.sim_udphdr.uh_sport =  sport;
				command->data.event.src_port = sport;
				r = TRUE;
			}
		}

	}
	/* udp_dport */
	g_message("udp dport");
	if (r){
		g_scanner_get_next_token(scanner);
		r = FALSE;
		if (scanner->token == SIM_COMMAND_SYMBOL_SNORT_EVENT_UDP_DPORT){
			g_scanner_get_next_token(scanner);
			g_scanner_get_next_token(scanner);
			if (scanner->token == G_TOKEN_STRING &&
			sim_string_is_number(scanner->value.v_string,0))
			{
				guint32 dport = strtoul(scanner->value.v_string,(char**)NULL,10);
				if (errno!=ERANGE && errno!=EINVAL && dport<65536){
					command->packet->hdr.sim_udphdr.uh_dport = dport;
					command->data.event.dst_port = dport;
					r = TRUE;
				}
			}
		}
	}
	/* udp_len */
	if (r){
		g_scanner_get_next_token(scanner);
		r = FALSE;
		if (scanner->token == SIM_COMMAND_SYMBOL_SNORT_EVENT_UDP_LEN){
			g_scanner_get_next_token(scanner);
			g_scanner_get_next_token(scanner);
			if (scanner->token == G_TOKEN_STRING &&
			sim_string_is_number(scanner->value.v_string,0))
			{
				guint32 udplen = strtoul(scanner->value.v_string,(char**)NULL,10);
				if (errno!=ERANGE && errno!=EINVAL && udplen<65536){
					command->packet->hdr.sim_udphdr.uh_ulen;
					r = TRUE;
				}
			}

		}

	}
	/* udp_csum */
	if (r){
		g_scanner_get_next_token(scanner);
		r = FALSE;
		if (scanner->token == SIM_COMMAND_SYMBOL_SNORT_EVENT_UDP_CSUM){
			g_scanner_get_next_token(scanner);
			g_scanner_get_next_token(scanner);
			r = FALSE;
			if (scanner->token == G_TOKEN_STRING &&
				sim_string_is_number(scanner->value.v_string,0))
			{
				guint32 udpcsum = strtoul(scanner->value.v_string,(char**)NULL,10);
				if (errno!=ERANGE && errno!=EINVAL && udpcsum<65536){
					command->packet->hdr.sim_udphdr.uh_sum;
					r = TRUE;
				}
			}
		}
	}
	/* udp_payload */
	if (r){
		g_scanner_get_next_token(scanner);
		r = FALSE;
		if (scanner->token == SIM_COMMAND_SYMBOL_SNORT_EVENT_UDP_PAYLOAD){
			g_scanner_get_next_token(scanner);
			g_scanner_get_next_token(scanner);
			if (scanner->token == G_TOKEN_STRING){
				guint32 payloadlen = strlen(scanner->value.v_string)/2;
				if (payloadlen>0){
					command->packet->payload = (guint8*)sim_hex2bin(scanner->value.v_string);
					if (command->packet->payload!=NULL){
						command->packet->payloadlen = payloadlen;
						r = TRUE;
					}
				}		
				else{
					r = TRUE;
					command->packet->payload = NULL;
					command->packet->payloadlen = 0;
				}
			}

		}
	}
	do{
		g_scanner_get_next_token(scanner);
		if (scanner->token!=G_TOKEN_EOF)
			r = FALSE;
	}
	while (scanner->token!=G_TOKEN_EOF);
	g_message("UDP sport=%u dport=%u csum=%u len=%u",command->packet->hdr.sim_udphdr.uh_sport,command->packet->hdr.sim_udphdr.uh_dport,command->packet->hdr.sim_udphdr.uh_sum,command->packet->hdr.sim_udphdr.uh_ulen);
	return r;



}
gboolean sim_command_snort_event_packet_tcp_scan(GScanner *scanner,SimCommand *command){
	gboolean r = FALSE;
	g_scanner_set_scope(scanner,SIM_COMMAND_SCOPE_SNORT_EVENT_TCP);
	/* sport */
	g_scanner_get_next_token(scanner);
	if (scanner->token==SIM_COMMAND_SYMBOL_SNORT_EVENT_TCP_SPORT){
		g_scanner_get_next_token(scanner);
		g_scanner_get_next_token(scanner);
		if (scanner->token == G_TOKEN_STRING &&
			sim_string_is_number(scanner->value.v_string,0)){
			guint32 sport = strtoul(scanner->value.v_string,(char**)NULL,10);
			if (errno!=ERANGE && errno!=EINVAL && sport<65536){
				command->packet->hdr.sim_tcphdr.th_sport = sport;
				command->data.event.src_port = sport;
				r = TRUE;
			}
		}
	}
	/* dport */
	if (r){
		g_scanner_get_next_token(scanner);
		r = FALSE;
		if (scanner->token==SIM_COMMAND_SYMBOL_SNORT_EVENT_TCP_DPORT){
			g_scanner_get_next_token(scanner); /* = */
			g_scanner_get_next_token(scanner); /* value */
			if (scanner->token == G_TOKEN_STRING &&
			sim_string_is_number(scanner->value.v_string,0)){
				guint32 dport = strtoul(scanner->value.v_string,(char**)NULL,10);
				if (errno!=ERANGE && errno!=EINVAL && dport<65536){
					command->packet->hdr.sim_tcphdr.th_dport = dport;
					command->data.event.dst_port = dport;
					r = TRUE;
				}
			}
		}
	}
	/* tcp_seq */
	if (r){
		g_scanner_get_next_token(scanner);
		r = FALSE;
		if (scanner->token == SIM_COMMAND_SYMBOL_SNORT_EVENT_TCP_SEQ){
			g_scanner_get_next_token(scanner);
			g_scanner_get_next_token(scanner);
			if (scanner->token == G_TOKEN_STRING &&
				sim_string_is_number(scanner->value.v_string,0))
				{
					guint32 seq = strtoul(scanner->value.v_string,(char**)NULL,10);
					if (errno!=ERANGE && errno!=EINVAL){
						command->packet->hdr.sim_tcphdr.th_seq = seq;
						r = TRUE;
					}
				}
		}
	}
	/* tcp_ack */
	if (r){
		g_scanner_get_next_token(scanner);
		r = FALSE;
		if (scanner->token == SIM_COMMAND_SYMBOL_SNORT_EVENT_TCP_ACK){
			g_scanner_get_next_token(scanner);
			g_scanner_get_next_token(scanner);
			if (scanner->token == G_TOKEN_STRING &&
			sim_string_is_number(scanner->value.v_string,0))
				{
					guint32 ack  = strtoul(scanner->value.v_string,(char**)NULL,10);
					if (errno!=ERANGE && errno!=EINVAL){
						command->packet->hdr.sim_tcphdr.th_ack = ack;
						r = TRUE;
					}
				} 
		}
	}
	/* tcp_offset*/
	if (r){
		g_scanner_get_next_token(scanner);
		r = FALSE;
		if (scanner->token == SIM_COMMAND_SYMBOL_SNORT_EVENT_TCP_OFFSET){
			g_scanner_get_next_token(scanner);
			g_scanner_get_next_token(scanner);
			r = FALSE;
			if (scanner->token == G_TOKEN_STRING &&
				sim_string_is_number(scanner->value.v_string,0))
			{
				guint32 offset = strtoul(scanner->value.v_string,(char**)NULL,10);
				if (errno!=ERANGE && errno!=EINVAL && offset<256){
					g_message("TCP: offset:%04x",offset);
					command->packet->hdr.sim_tcphdr.th_off = (offset&0xf0)>>4;
					command->packet->hdr.sim_tcphdr.th_x2 = offset&0xf;
					r = TRUE;
				}
			}
		}

	}
	/* tcp_flags */
	if (r){
		g_scanner_get_next_token(scanner);
		r = FALSE;
		if (scanner->token == SIM_COMMAND_SYMBOL_SNORT_EVENT_TCP_FLAGS){
			g_scanner_get_next_token(scanner);
			g_scanner_get_next_token(scanner);
			r = FALSE;
			if (scanner->token == G_TOKEN_STRING &&
				sim_string_is_number(scanner->value.v_string,0))
			{
				guint32 flags = strtoul(scanner->value.v_string,(char**)NULL,10);
				if (errno!=ERANGE && errno!=EINVAL && flags<256){
					g_message("TCP: flags:%04x");
					command->packet->hdr.sim_tcphdr.th_flags = flags;
					r = TRUE;
				}
			}

		}
	}
	/* tcp_window*/
	if (r){
		g_scanner_get_next_token(scanner);
		r = FALSE;
		if (scanner->token == SIM_COMMAND_SYMBOL_SNORT_EVENT_TCP_WINDOW){
			g_scanner_get_next_token(scanner);
			g_scanner_get_next_token(scanner);
			if (scanner->token == G_TOKEN_STRING &&
				sim_string_is_number(scanner->value.v_string,0))
				{
					guint32 window = strtoul(scanner->value.v_string,(char**)NULL,10);
					if (errno!=ERANGE && errno!=EINVAL && window<65536){
						command->packet->hdr.sim_tcphdr.th_win = window;
						r = TRUE;
					}
				}
		}
	}
	/* tcp_csum */
	if (r)
	{
		g_scanner_get_next_token(scanner);
		r = FALSE;
		if (scanner->token == SIM_COMMAND_SYMBOL_SNORT_EVENT_TCP_CSUM){
			g_scanner_get_next_token(scanner);
			g_scanner_get_next_token(scanner);
			if (scanner->token == G_TOKEN_STRING &&
			sim_string_is_number(scanner->value.v_string,0))
			{
				guint32 csum = strtoul(scanner->value.v_string,(char**)NULL,10);
				if (errno!=ERANGE && errno!=EINVAL && csum<65536){
					command->packet->hdr.sim_tcphdr.th_sum = csum;
					r = TRUE;
				}
			}
		}
	}
	/* tcp_urgptr*/
	if (r)
	{
		g_scanner_get_next_token(scanner);
		r = FALSE;
		if (scanner->token == SIM_COMMAND_SYMBOL_SNORT_EVENT_TCP_URGPTR){
			g_scanner_get_next_token(scanner);
			g_scanner_get_next_token(scanner);
			if (scanner->token == G_TOKEN_STRING &&
			sim_string_is_number(scanner->value.v_string,0))
			{
				guint32 urgptr = strtoul(scanner->value.v_string,(char**)NULL,10);
				if (errno!=ERANGE && errno!=EINVAL && urgptr<65536){
					command->packet->hdr.sim_tcphdr.th_urp = urgptr;
					r = TRUE;
				}
			}
		}
	}
	/* There are options? */
	if (r && command->packet->hdr.sim_tcphdr.th_off>5){
		r = sim_command_snort_event_tcp_opt_scan(scanner,command);	
	
	}
	/* tcp_payload */
	if (r){
		g_scanner_get_next_token(scanner);
		r = FALSE;
		if (scanner->token == SIM_COMMAND_SYMBOL_SNORT_EVENT_TCP_PAYLOAD){
			g_scanner_get_next_token(scanner);
			g_scanner_get_next_token(scanner);
			r = FALSE;
			guint32 payloadtcplen = strlen(scanner->value.v_string)/2;
			if (payloadtcplen>0){
				command->packet->payload = (guint8*)sim_hex2bin(scanner->value.v_string);
				if (command->packet->payload!=NULL)
					r = TRUE;
					command->packet->payloadlen = payloadtcplen;
				}
			else{
				command->packet->payload = NULL;	
				r = TRUE;
				command->packet->payloadlen = 0;
			}
			

		}
	}
 	/* check for eof */
	do{
		g_scanner_get_next_token(scanner);
		if (scanner->token!=G_TOKEN_EOF)
			r = FALSE;
	}while(scanner->token!=G_TOKEN_EOF);
	return r;
}
gboolean sim_command_snort_event_tcp_opt_scan(GScanner *scanner,SimCommand *command){
	gboolean f = FALSE;
	int i,j=0;
	/* tcp_optnum*/
	g_scanner_get_next_token(scanner);
	if (scanner->token==SIM_COMMAND_SYMBOL_SNORT_EVENT_TCP_OPTNUM){
		g_scanner_get_next_token(scanner); /* = */
		g_scanner_get_next_token(scanner); /* value */
		if (scanner->token==G_TOKEN_STRING &&
			sim_string_is_number(scanner->value.v_string,0)){
			guint32 optnum = strtol(scanner->value.v_string,(char**)NULL,10);
			if (errno!=ERANGE && errno!=EINVAL){
				/* Now scan options*/
				f = TRUE;
				command->packet->hdr.sim_tcphdr.nOptions = optnum;
				for(i=0;i<optnum && f && j<(10*4);i++){
					guint32 optcode;
					guint32 optlen;
					guint8* optdata;
					g_scanner_get_next_token(scanner); /* token */
					if (scanner->token!=SIM_COMMAND_SYMBOL_SNORT_EVENT_TCP_OPTCODE){
						f = FALSE;
						continue;
					}
					g_scanner_get_next_token(scanner); /* = */
					g_scanner_get_next_token(scanner); /* value */
					if (scanner->token!=G_TOKEN_STRING ||
						sim_string_is_number(scanner->value.v_string,0)!=TRUE){
						f = FALSE;
						continue;
					}
					optcode = strtol(scanner->value.v_string,(char **)NULL,10);
					if (errno == ERANGE || errno == EINVAL || optcode>255){
						f = FALSE;
						continue;
					}
					g_scanner_get_next_token(scanner); /* token */
					if (scanner->token!=SIM_COMMAND_SYMBOL_SNORT_EVENT_TCP_OPTLEN){
						f = FALSE;
						continue;
					}
					g_scanner_get_next_token(scanner); /* = */
					g_scanner_get_next_token(scanner); /* value */
					if (scanner->token!=G_TOKEN_STRING ||
						sim_string_is_number(scanner->value.v_string,0)!=TRUE){
						f = FALSE;
						continue;
					}
					optlen = strtol(scanner->value.v_string,(char **)NULL,10);
					if (errno == ERANGE || errno==EINVAL || optlen>255){
						f = FALSE;
						continue;
					}
					g_scanner_get_next_token(scanner); /* token */
					if (scanner->token!=SIM_COMMAND_SYMBOL_SNORT_EVENT_TCP_OPTPAYLOAD){
						f = FALSE;
						continue;
					}
					g_scanner_get_next_token(scanner); /* = */
					g_scanner_get_next_token(scanner); /* value */
					if (optlen>2){
					if ((optdata = (guint8*)sim_hex2bin(scanner->value.v_string))==NULL){
						f = FALSE;
						continue;
					}
					}
					/* Copy data into de options field, indexed by j*/
					if (optcode!=0 && optcode!=1){
						/* optlen include de c,l value (+2) */
						if ((j+optlen)>40){
							f = FALSE;
							continue;
						}
						command->packet->hdr.sim_tcphdr.th_opt[j++] = (guint8)optcode;
						command->packet->hdr.sim_tcphdr.th_opt[j++] = (guint8)optlen;
						if (optlen>2){
						memcpy(&command->packet->hdr.sim_tcphdr.th_opt[j++],optdata,optlen-2);
						g_free(optdata);
						}
					}else{
						if (j<40)
							command->packet->hdr.sim_tcphdr.th_opt[j++] = (guint8)optcode;
						else{
							f = FALSE;
							continue;
						}
								
					}
						
				} /* end copy data */
			}
		}
	}				
	return f;
}

gboolean sim_command_snort_event_ip_opt_scan(GScanner *scanner,SimCommand *command)
{
	gboolean f = FALSE;
	int i,j=0;
	/* ip_optnum*/
	g_scanner_get_next_token(scanner);
	if (scanner->token==SIM_COMMAND_SYMBOL_SNORT_EVENT_IP_OPTNUM){
		g_scanner_get_next_token(scanner); /* = */
		g_scanner_get_next_token(scanner); /* value */
		if (scanner->token==G_TOKEN_STRING &&
			sim_string_is_number(scanner->value.v_string,0)){
			guint32 optnum = strtol(scanner->value.v_string,(char**)NULL,10);
			if (errno!=ERANGE && errno!=EINVAL){
				/* Now scam options*/
				f = TRUE;
				command->packet->sim_iphdr.nOptions = optnum;
				for(i=0;i<optnum && f && j<(10*4);i++){
					guint32 optcode;
					guint32 optlen;
					guint8* optdata;
					g_scanner_get_next_token(scanner); /* token */
					if (scanner->token!=SIM_COMMAND_SYMBOL_SNORT_EVENT_IP_OPTCODE){
						f = FALSE;
						continue;
					}
					g_scanner_get_next_token(scanner); /* = */
					g_scanner_get_next_token(scanner); /* value */
					if (scanner->token!=G_TOKEN_STRING ||
						sim_string_is_number(scanner->value.v_string,0)!=TRUE){
						f = FALSE;
						continue;
					}
					optcode = strtol(scanner->value.v_string,(char **)NULL,10);
					if (errno == ERANGE || optcode>255){
						f = FALSE;
						continue;
					}
					g_scanner_get_next_token(scanner); /* token */
					if (scanner->token!=SIM_COMMAND_SYMBOL_SNORT_EVENT_IP_OPTLEN){
						f = FALSE;
						continue;
					}
					g_scanner_get_next_token(scanner); /* = */
					g_scanner_get_next_token(scanner); /* value */
					if (scanner->token!=G_TOKEN_STRING ||
						sim_string_is_number(scanner->value.v_string,0)!=TRUE){
						f = FALSE;
						continue;
					}
					optlen = strtol(scanner->value.v_string,(char **)NULL,10);
					if (errno == ERANGE || optlen>255){
						f = FALSE;
						continue;
					}
					g_scanner_get_next_token(scanner); /* token */
					if (scanner->token!=SIM_COMMAND_SYMBOL_SNORT_EVENT_IP_OPTPAYLOAD){
						f = FALSE;
						continue;
					}
					g_scanner_get_next_token(scanner); /* = */
					g_scanner_get_next_token(scanner); /* value */
					if (optlen>0){
					if ((optdata = (guint8*)sim_hex2bin(scanner->value.v_string))==NULL){
						f = FALSE;
						continue;
					}}
					/* Copy data into de options field, indexed by j*/
					if (optcode!=0 && optcode!=1){
						if (optlen>0){
						if ((j+2+optlen)>40){
							f = FALSE;
							continue;
						}
						command->packet->sim_iphdr.options[j++] = (guint8)optcode;
						command->packet->sim_iphdr.options[j++] = (guint8)optlen;
						memcpy(&command->packet->sim_iphdr.options[j++],optdata,optlen);
						g_free(optdata);
						}
					}else{
						if (j<40)
							command->packet->sim_iphdr.options[j++] = (guint8)optcode;
						else{
							f = FALSE;
							continue;
						}
								
					}
						
				} /* end copy data */
			}
		}
	}				
	return f;			
}
gboolean sim_command_snort_event_packet_raw_scan(GScanner *scanner,SimCommand  *command){
	gboolean r = FALSE;
	g_scanner_set_scope(scanner,SIM_COMMAND_SCOPE_SNORT_EVENT_PACKET_RAW);
	g_scanner_get_next_token(scanner);
	/* raw_payload */
	g_scanner_get_next_token(scanner);
	if (scanner->token == SIM_COMMAND_SYMBOL_SNORT_EVENT_PACKET_RAW){
		g_scanner_get_next_token(scanner); /* = */
		g_scanner_get_next_token(scanner); /* value */
	       g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,"sim_command_snort_event_packet_raw_scan: Discarting raw packet");
	}
	do{
		g_scanner_get_next_token(scanner);
	}while(scanner->token!=G_TOKEN_EOF);
	/* always return false
	 * because we don't store raw packets, only ip packets.
	 */
	return FALSE;

}

gboolean sim_command_snort_event_packet_ip_scan(GScanner *scanner,SimCommand *command){
	gboolean r = FALSE;
	g_scanner_set_scope(scanner,SIM_COMMAND_SCOPE_SNORT_EVENT_IP);
	g_scanner_get_next_token(scanner);
	/* ip_ver */
	if (scanner->token == SIM_COMMAND_SYMBOL_SNORT_EVENT_IP_VER){
		g_scanner_get_next_token(scanner); /* = */
		g_scanner_get_next_token(scanner); /* value */
		if (scanner->token == G_TOKEN_STRING && 
			sim_string_is_number (scanner->value.v_string, 0))
			{
				guint32 version = strtol(scanner->value.v_string,(char**)NULL,10);
				if (version>=0 && version<=15){
					command->packet->sim_iphdr.ip_v = version;
					r = TRUE;
				}
				
			}

	}
	/* ip_hdrlen */
	if (r){
		g_scanner_get_next_token(scanner);
		r = FALSE;
		if (scanner->token == SIM_COMMAND_SYMBOL_SNORT_EVENT_IP_HDRLEN){
			g_scanner_get_next_token(scanner); /* = */
			g_scanner_get_next_token(scanner); /* value */
			if (scanner->token == G_TOKEN_STRING &&
				sim_string_is_number(scanner->value.v_string,0)){
				guint32 hdrlen = strtol(scanner->value.v_string,(char**)NULL,10);
				if (errno!=ERANGE && hdrlen<15){
					command->packet->sim_iphdr.ip_hl = hdrlen;
					r = TRUE;
				}
			
			}
		}
	}
	/* ip_tos */
	if (r){
		g_scanner_get_next_token(scanner);
		r = FALSE;
		if (scanner->token == SIM_COMMAND_SYMBOL_SNORT_EVENT_IP_TOS){
			g_scanner_get_next_token(scanner);
			g_scanner_get_next_token(scanner);
			if (scanner->token == G_TOKEN_STRING &&
				sim_string_is_number(scanner->value.v_string,0)){
				guint32 tos = strtol(scanner->value.v_string,(char**)NULL,10);
				if (errno!=ERANGE && tos<=255){
					command->packet->sim_iphdr.ip_tos = tos;
					r = TRUE;
				}
			}
		}
	}
	/* ip_len */
	if (r){
		g_scanner_get_next_token(scanner);
		r = FALSE;
		if (scanner->token == SIM_COMMAND_SYMBOL_SNORT_EVENT_IP_LEN){
			g_scanner_get_next_token(scanner); /* = */
			g_scanner_get_next_token(scanner); /* value */
			if (scanner->token == G_TOKEN_STRING &&
					sim_string_is_number(scanner->value.v_string,0))
				{
					guint32 iplen = strtol(scanner->value.v_string,(char**)NULL,10);
					if (errno!=ERANGE && iplen<65536){
						command->packet->sim_iphdr.ip_len=iplen;
						r = TRUE;
					}
				}

		}

	}
	/* ip_id */
	if (r){
		g_scanner_get_next_token(scanner);
		r = FALSE;
		if (scanner->token == SIM_COMMAND_SYMBOL_SNORT_EVENT_IP_ID){
				g_scanner_get_next_token(scanner); /* = */
				g_scanner_get_next_token(scanner); /* value */
				r = FALSE;
				if (scanner->token == G_TOKEN_STRING &&
					sim_string_is_number(scanner->value.v_string,0))
				{
					guint32 id = strtol(scanner->value.v_string,(char**)NULL,10);
					if (errno!=ERANGE && id<=65535){
						command->packet->sim_iphdr.ip_id;
						r = TRUE;
					}
				}

		}	
	}
	/* ip_offset */
	if (r){
		g_scanner_get_next_token(scanner);
		r = FALSE;
		if (scanner->token == SIM_COMMAND_SYMBOL_SNORT_EVENT_IP_OFFSET){
			g_scanner_get_next_token(scanner); /* = */
			g_scanner_get_next_token(scanner); /* value */
			if (scanner->token == G_TOKEN_STRING &&
				sim_string_is_number(scanner->value.v_string,0))
			{
				guint32 offset = strtol(scanner->value.v_string,(char**)NULL,10);
				if (errno!=ERANGE && offset<65536){
					command->packet->sim_iphdr.ip_off=offset;
					r = TRUE;
				}
			}
		}

	}
	/* ip_ttl */ 
	if (r){
		g_scanner_get_next_token(scanner);
		r = FALSE;
		if (scanner->token == SIM_COMMAND_SYMBOL_SNORT_EVENT_IP_TTL){
			g_scanner_get_next_token(scanner); /* = */
			g_scanner_get_next_token(scanner); /* value */
			r = FALSE;
			if (scanner->token == G_TOKEN_STRING &&
				sim_string_is_number(scanner->value.v_string,0))
			{
				guint32 ttl = strtol(scanner->value.v_string,(char**)NULL,10);
				if (errno!=ERANGE && ttl<256){
					command->packet->sim_iphdr.ip_ttl = ttl;
					r = TRUE;
				}
			}
		}
	}
	/* ip_proto */
	if (r){
		g_scanner_get_next_token(scanner);
		r = FALSE;
		if (scanner->token == SIM_COMMAND_SYMBOL_SNORT_EVENT_IP_PROTO){
			g_scanner_get_next_token(scanner); /* = */
			g_scanner_get_next_token(scanner); /* value */
			if (scanner->token == G_TOKEN_STRING &&
				sim_string_is_number(scanner->value.v_string,0))
			{
				guint32 protocol = strtol(scanner->value.v_string,(char**)NULL,10);
				if (errno!=ERANGE && protocol<256){
					command->packet->sim_iphdr.ip_p = protocol;
					command->data.event.protocol = g_strdup(scanner->value.v_string);
					r = TRUE;
				}
			}

		}
	}
	/* ip_csum */
	if (r){
		g_scanner_get_next_token(scanner);
		r = FALSE;
		if (scanner->token == SIM_COMMAND_SYMBOL_SNORT_EVENT_IP_CSUM){
			g_scanner_get_next_token(scanner); /* = */
			g_scanner_get_next_token(scanner); /* values */
			if (scanner->token == G_TOKEN_STRING &&
				sim_string_is_number(scanner->value.v_string,0))
			{
				guint32 csum = strtol(scanner->value.v_string,(char**)NULL,10);
				if (errno!=ERANGE && csum<65536){
					command->packet->sim_iphdr.ip_sum = csum;
					r = TRUE;
				}
			}
		}
	
	}
	/* ip_src */
	if (r){
		g_scanner_get_next_token(scanner);
		r = FALSE;
		struct in_addr in;
		if (scanner->token == SIM_COMMAND_SYMBOL_SNORT_EVENT_IP_SRC){
			g_scanner_get_next_token(scanner); /* = */
			g_scanner_get_next_token(scanner); /* value */
			if (scanner->token == G_TOKEN_STRING &&
				inet_aton(scanner->value.v_string,&in)!=0){
				command->packet->sim_iphdr.ip_src = in.s_addr;
				command->data.event.src_ip = g_strdup(scanner->value.v_string);
				r = TRUE;
			}
		}
	}
	/* ip_dst */
	if (r){
		g_scanner_get_next_token(scanner);
		r = FALSE;
		struct in_addr in;
		if (scanner->token == SIM_COMMAND_SYMBOL_SNORT_EVENT_IP_DST){
			g_scanner_get_next_token(scanner); /* = */
			g_scanner_get_next_token(scanner); /* value */
			if (scanner->token == G_TOKEN_STRING &&
				inet_aton(scanner->value.v_string,&in)!=0){
				command->packet->sim_iphdr.ip_dst = in.s_addr;
				command->data.event.dst_ip = g_strdup(scanner->value.v_string);
				r = TRUE;
			}
		}
	}
	/* Oka, now search the ip_optnum*/
	if (r && command->packet->sim_iphdr.ip_hl>5){
		r = sim_command_snort_event_ip_opt_scan(scanner,command);
	}
	g_message("Scanning packet contecnt");
	if (r){
		switch(command->packet->sim_iphdr.ip_p){
			case IPPROTO_UDP:
				g_message("Scanning UDP packet");
				r = sim_command_snort_event_packet_udp_scan(scanner,command);
				break;
			case IPPROTO_TCP:
				g_message("Scanning TCP packet");
				r = sim_command_snort_event_packet_tcp_scan(scanner,command);
				break;
			case IPPROTO_ICMP:
				g_message("Scanning ICMP packet");
				r = sim_command_snort_event_packet_icmp_scan(scanner,command);
				break;
			default:
				/* ip packet doesn't correspond */
				g_scanner_get_next_token(scanner);
				r = FALSE;
				if (scanner->token==SIM_COMMAND_SYMBOL_SNORT_EVENT_IP_PAYLOAD){
					g_scanner_get_next_token(scanner); /* = */
					g_scanner_get_next_token(scanner); /* value */
					guint32 payloadlen = strlen(scanner->value.v_string)/2;
					if (payloadlen>0){
						command->packet->payload = (guint8*)sim_hex2bin(scanner->value.v_string);
						if (command->packet->payload!=NULL){
							command->packet->payloadlen = payloadlen;
							r = TRUE;
						}
						
					}
					else{
						r = TRUE;
						command->packet->payloadlen = 0;
						command->packet->payload = NULL;
					}
				}	
		}
	}
	/* oka */
	/* Update the tables at snoradatabase, and return if OK*/
	/*if (r)
		r  = update_snort_database(command);*/
	return r;
}

/* 
 * Scan and decompress the Snort Event  
 *
 */


gboolean
sim_command_snort_event_scan	(SimCommand	*command,GScanner *scanner)
{
	g_return_if_fail (command != NULL);
	g_return_if_fail (SIM_IS_COMMAND (command));
	g_return_if_fail (scanner != NULL);
	gboolean r = FALSE;
	z_stream zstr;
	guchar *unzipdata;
	/* from zlib.h */
	uLongf size;
	guint lenzip;
	command->type = SIM_COMMAND_TYPE_SNORT_EVENT;
	memset(&command->data,sizeof(command->data),0);
	memset(&zstr,sizeof(zstr),0);
	g_scanner_set_scope (scanner, SIM_COMMAND_SCOPE_SNORT_EVENT);
	g_scanner_get_next_token(scanner);
	if (scanner->token == SIM_COMMAND_SYMBOL_SNORT_EVENT_SENSOR){
		g_scanner_get_next_token(scanner); /* = */
		g_scanner_get_next_token(scanner); /* value */
		if (scanner->token == G_TOKEN_STRING){
			command->data.event.sensor = g_strdup(scanner->value.v_string);
			r = TRUE;
		}

	}
	if (r){
		g_scanner_get_next_token(scanner); 
		if (scanner->token == SIM_COMMAND_SYMBOL_SNORT_EVENT_IF){
			g_scanner_get_next_token(scanner);
			g_scanner_get_next_token(scanner);
			if (scanner->token == G_TOKEN_STRING){
				command->data.event.interface = g_strdup(scanner->value.v_string);
				r = TRUE;
			}
		}
	}
	if (r){
		g_scanner_get_next_token(scanner);
		/* Scan the gziped data */
		if (scanner->token == SIM_COMMAND_SYMBOL_GZIPDATA){
			g_scanner_get_next_token(scanner); /* = */
			g_scanner_get_next_token(scanner); /* gzip data */
			if (scanner->token==G_TOKEN_STRING &&
				((command->snort_event.gzipdata = (guint8*)sim_hex2bin(scanner->value.v_string)))!=NULL){
				lenzip = strlen(scanner->value.v_string)/2;
				r = TRUE;

			}
		}	
	}
	/* Unziplen data */
	if (r){
		g_scanner_get_next_token(scanner);
		if (scanner->token == SIM_COMMAND_SYMBOL_UNZIPLEN){
			g_scanner_get_next_token(scanner); /* = */
			g_scanner_get_next_token(scanner); /* unzip data len */
			r = FALSE;
			if (scanner->token== G_TOKEN_STRING &&
				sim_string_is_number(scanner->value.v_string,0)){
				guint32 unziplen = strtol(scanner->value.v_string,(char**)NULL,10);
				if (errno!=ERANGE){
					command->snort_event.unziplen = unziplen;
					r = TRUE;
				}
			
			}


		}

	}
	/* event_type */
	if (r){
		g_scanner_get_next_token(scanner);
		if (scanner->token == SIM_COMMAND_SYMBOL_SNORT_EVENT_TYPE){
			g_scanner_get_next_token(scanner);
			g_scanner_get_next_token(scanner);
			if (scanner->token == G_TOKEN_STRING)
				r = TRUE;
		}

	}

	if (!r){
			g_log(G_LOG_DOMAIN,G_LOG_LEVEL_DEBUG,"sim_command_snort_event_scan: Bad symbol %s",scanner->value.v_string);
			return r;
	}
	/* OK, know unzip the data and decode it */
	size = 1024*(1+(command->snort_event.unziplen/1024));
	int errorzip = 0;
	gchar *puta;
	if ((unzipdata =g_new(guchar,size))!=NULL){
			if ((errorzip = uncompress(unzipdata,( uLongf * )&size,command->snort_event.gzipdata,lenzip)!=Z_OK)){
					r = FALSE;
					g_log(G_LOG_DOMAIN,G_LOG_LEVEL_DEBUG,"sim_command_snort_event_scan: Error inflated data %u",errorzip);
			}else{
			unzipdata[size]='\0';
			puta = g_strdup(unzipdata);
			g_log(G_LOG_DOMAIN,G_LOG_LEVEL_DEBUG,"sim_command_snort_event_scan: gzipdata %s",unzipdata);
			/* Now, we must parse the event data 
			 * I think that is better to parse data from binary here and not in the agent, but ....
			 */
			g_scanner_input_text(scanner,unzipdata,size);
			g_scanner_set_scope (scanner,SIM_COMMAND_SCOPE_SNORT_EVENT_DATA);
			r = sim_command_snort_event_packet_scan(scanner,command);
			if (!r)
				printf("PUTO COMANDO: %s\n",puta);
			}
			g_free(puta);
			g_free(unzipdata);
			
	}
	else{
			r = FALSE;
			g_log(G_LOG_DOMAIN,G_LOG_LEVEL_DEBUG,"sim_command_snort_event_scan: Cannot alloc memory");
	}

	return r;
}
/* Update the snort database */
G_LOCK_DEFINE_STATIC(s_snort_update);
gboolean update_snort_database(SimEvent *event){
	gboolean r = FALSE;
	SimConfig *config;
	SimDatabase *snort;
	guint cid;
	guint sid;
	guint sig_id = 0;
	gchar *query;
	GdaDataModel *dm;
	g_return_if_fail( event !=NULL);
	g_return_if_fail ( SIM_IS_EVENT(event));
	SimPacket * packet = event->packet;
	struct tm tm;
	char datetime[27]; //at least 26 chars length
	SimPluginSid *plugin_sid;
	G_LOCK(s_snort_update);
	/* now config */
	config = sim_server_get_config (ossim.server);
	/* Get the sid for the sensor*/
	sid = sim_organizer_snort_sensor_get_sid(ossim.dbsnort,event->sensor,
		event->interface,
		NULL);
	cid = sim_organizer_snort_event_get_max_cid(ossim.dbsnort,sid);
	cid++;
	event->snort_sid = sid; //sensor ID
	event->snort_cid = cid; // event id
	/* search for signature id in table signature 
	 * if not present, discard the event */
        plugin_sid = sim_container_get_plugin_sid_by_pky(ossim.container,event->plugin_id,event->plugin_sid);
	r = TRUE;
	if (!plugin_sid)
	{
		g_message("update_snort_database: Error Plugin:%u, PluginSid:%u",event->plugin_id,event->plugin_sid);
		r = FALSE;

	}
	if (r &&
		(sig_id = sim_organizer_snort_signature_get_id (ossim.dbsnort, sim_plugin_sid_get_name (plugin_sid)))==0){
		g_message("Unknown signature %u:%u",event->plugin_id-1000,event->plugin_sid);
		r = FALSE;
	}
	g_message("Attempt to insert event with sensor:%u and cid:%u with %u",sid,cid,sig_id);
	if (sid && r ){
		r = FALSE;
		query = g_strdup_printf("INSERT INTO iphdr (sid,cid,ip_src,ip_dst,ip_ver,ip_hlen,ip_tos,ip_len,ip_id,ip_flags,ip_off,ip_ttl,ip_proto,ip_csum) VALUES (%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u)",
		sid,cid,
		ntohl(packet->sim_iphdr.ip_src), /* byte host order */
		ntohl(packet->sim_iphdr.ip_dst),
		packet->sim_iphdr.ip_v,
		packet->sim_iphdr.ip_hl,
		packet->sim_iphdr.ip_tos,
		packet->sim_iphdr.ip_len,
		packet->sim_iphdr.ip_id,
		packet->sim_iphdr.ip_off& 0x7,
		(packet->sim_iphdr.ip_off & 0xf8)>>3,
		packet->sim_iphdr.ip_ttl,
		packet->sim_iphdr.ip_p,
		packet->sim_iphdr.ip_sum);
		sim_database_execute_no_query (ossim.dbsnort, query);
		g_free (query);
		/* Options */
		if (packet->sim_iphdr.ip_hl>5){
			int n = packet->sim_iphdr.ip_hl*4;
			int i=0,j=0,k=0;
			for (k=0;k<packet->sim_iphdr.nOptions;k++){
				if (packet->sim_iphdr.options[i]==0 || packet->sim_iphdr.options[i]==1){
					query = g_strdup_printf("INSERT INTO opt (sid,cid,optid,opt_proto,opt_code,opt_len,opt_data) VALUES(%u,%u,%u,%u,%u,%u,'%s')",
					sid,cid,j,0,packet->sim_iphdr.options[i],0,"");
					j++;
					i++;
					sim_database_execute_no_query(ossim.dbsnort,query);
					g_free(query);
				}
				else
				{
					guint optcode = packet->sim_iphdr.options[i];
					guint optlen = packet->sim_iphdr.options[i+1];
					guint8 *hexdata = sim_bin2hex(&packet->sim_iphdr.options[i+2],optlen);
					query = g_strdup_printf("INSERT INTO opt (sid,cid,optid,opt_proto,opt_code,opt_len,opt_data) VALUES	(%u,%u,%u,%u,%u,%u,'%s')",
					sid,cid,j,0,optcode,optlen,hexdata);
					sim_database_execute_no_query(ossim.dbsnort,query);
					g_free(query);
					i=i+2+optlen;
					g_free(hexdata);
					j++;
				}			
			}
		}
		/* Now, we insert in function of protocols*/
	switch(packet->sim_iphdr.ip_p){
			case IPPROTO_TCP:
				/*Insert protocol header*/
				query = g_strdup_printf("INSERT INTO tcphdr (sid,cid,tcp_sport,tcp_dport,tcp_seq,tcp_ack,tcp_off,tcp_res,tcp_flags,tcp_win,tcp_csum,tcp_urp) VALUES (%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u)",
				sid,cid,
				packet->hdr.sim_tcphdr.th_sport,
				packet->hdr.sim_tcphdr.th_dport,
				packet->hdr.sim_tcphdr.th_seq,
				packet->hdr.sim_tcphdr.th_ack,
				packet->hdr.sim_tcphdr.th_off,
				packet->hdr.sim_tcphdr.th_x2,
				packet->hdr.sim_tcphdr.th_flags,
				packet->hdr.sim_tcphdr.th_win,
				packet->hdr.sim_tcphdr.th_sum,
				packet->hdr.sim_tcphdr.th_urp);
				g_message("INSERT TCP: %s",query);
				sim_database_execute_no_query(ossim.dbsnort,query);
				g_free(query);
				/* tcp options */
				if (packet->hdr.sim_tcphdr.th_off>5){
					guint n = packet->hdr.sim_tcphdr.th_off;
					guint i=0,j=0,k=0;
					for (k=0;k<packet->hdr.sim_tcphdr.nOptions;k++){
						if (packet->hdr.sim_tcphdr.th_opt[i]==0 || 
						packet->hdr.sim_tcphdr.th_opt[i]==1){
						query = g_strdup_printf("INSERT INTO opt (sid,cid,optid,opt_proto,opt_code,opt_len,opt_data) VALUES(%u,%u,%u,%u,%u,%u,'%s')",
						sid,cid,j,6,packet->sim_iphdr.options[i],0,"");
						j++;
						i++;
						sim_database_execute_no_query(ossim.dbsnort,query);
						g_free(query);
						}else{
							guint optcode = packet->hdr.sim_tcphdr.th_opt[i];
							guint optlen = packet->hdr.sim_tcphdr.th_opt[i+1];
							gchar *hexdata = sim_bin2hex(&packet->hdr.sim_tcphdr.th_opt[i+2],optlen);
							query = g_strdup_printf("INSERT INTO opt (sid,cid,optid,opt_proto,opt_code,opt_len,opt_data) VALUES	(%u,%u,%u,%u,%u,%u,'%s')",
							sid,cid,j,6,optcode,optlen,hexdata);
							sim_database_execute_no_query(ossim.dbsnort,query);
							g_free(query);
							i=i+2+optlen;
							g_free(hexdata);
							j++;
						}	
								
					} /* end for */
				}
				break;
			case IPPROTO_UDP:
				query = g_strdup_printf("INSERT INTO udphdr (sid,cid,udp_sport,udp_dport,udp_len,udp_csum) VALUES (%u,%u,%u,%u,%u,%u) ",
				sid,cid,
				packet->hdr.sim_udphdr.uh_sport,
				packet->hdr.sim_udphdr.uh_dport,
				packet->hdr.sim_udphdr.uh_ulen,
				packet->hdr.sim_udphdr.uh_sum);
				g_message("INSERT UDP: %s",query);
				sim_database_execute_no_query(ossim.dbsnort,query);
				g_free(query);
				break;
			case IPPROTO_ICMP:
				query = g_strdup_printf("INSERT INTO icmphdr (sid,cid,icmp_type,icmp_code,icmp_csum,icmp_id,icmp_seq) VALUES (%u,%u,%u,%u,%u,%u,%u) ",
				sid,cid,
				packet->hdr.sim_icmphdr.icmp_type,
				packet->hdr.sim_icmphdr.icmp_code,
				packet->hdr.sim_icmphdr.icmp_cksum,
				packet->hdr.sim_icmphdr.un.echo.id,
				packet->hdr.sim_icmphdr.un.echo.sequence);
				sim_database_execute_no_query(ossim.dbsnort,query);
				g_message("INSERT ICMP: %s",query);
				g_free(query);
				break;
			default:
				g_message("Unknown protocol send from Snort %d",packet->sim_iphdr.ip_p);
				break;
				/* Now insert the payload */
		}
		if (packet->payloadlen>0){
				gchar *payload;
				payload = sim_bin2hex(packet->payload,packet->payloadlen);
				query = g_strdup_printf("INSERT INTO data (sid,cid,data_payload) VALUES (%u,%u,'%s')",
				sid,cid,payload);
				sim_database_execute_no_query(ossim.dbsnort,query);
				g_free(query);
				g_free(payload);
		}
		/* now the event */
		/* generate timestamp*/
		localtime_r(&event->time,&tm); // FIXME: Check for NULL
		strftime(datetime,26,"%Y-%m-%d %T",&tm);
		datetime[26]='\0';
		query = g_strdup_printf("INSERT INTO event (sid,cid,signature,timestamp) VALUES (%u,%u,%u,'%s')",
			sid,cid,
			sig_id,
			datetime);
		sim_database_execute_no_query(ossim.dbsnort,query);
		g_free(query);
		r = TRUE;
	}else
		g_message("Can't get last cid for snort event. Discarting Snort Event");
	//g_object_unref(command->packet);
	G_UNLOCK(s_snort_update);
	return r;
}
