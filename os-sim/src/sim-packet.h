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

#ifndef __SIM_PACKET_H__
#define __SIM_PACKET_H__ 1
#include <glib.h>
#include <glib-object.h>
#include <config.h>
#ifdef __cplusplus
extern "C" {
#endif
G_BEGIN_DECLS
#define SIM_TYPE_PACKET			(sim_packet_get_type())
#define SIM_PACKET(obj)			(G_TYPE_CHECK_INSTANCE_CAST (obj,SIM_TYPE_PACKET,SimPacket))
#define SIM_PACKET_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST (klass, SIM_TYPE_PACKET, SimPacketClass))
#define SIM_IS_PACKET(obj)               (G_TYPE_CHECK_INSTANCE_TYPE (obj, SIM_TYPE_PACKET))
#define SIM_IS_PACKET_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), SIM_TYPE_PACKET))
#define SIM_PACKET_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), SIM_TYPE_PACKET, SimPacketClass))



typedef struct _SimPacket	SimPacket;
typedef struct _SimPacketClass	SimPacketClass;
struct _SimPacket{
	GObject parent;
	struct sim_ip{
	#if SIMBIGENDIAN == 0
		guint8 ip_hl:4;
		guint8 ip_v:4;
	#elif SIMBIGENDIAN == 1
		guint8 ip_v:4;
		guint8 ip_hl:4;
	#else
		#error "Please fix SIMBIGENDIAN value (0 litte endian, 1 big endian)"
	#endif
		guint8 ip_tos;
		guint16  ip_len;
		guint16 ip_id;
		guint16 ip_off;
		guint8  ip_ttl;
		guint8 ip_p;
		guint16 ip_sum;
		guint32 ip_src;
		guint32 ip_dst;
		guint8 options[10*4];
		guint8 nOptions;
	} sim_iphdr;
	union{
		struct sim_udp{
			guint16 uh_sport;
			guint16 uh_dport;
			guint16 uh_ulen;
			guint16 uh_sum;
		} sim_udphdr;
		struct sim_icmp{
			guint8 icmp_type;
			guint8 icmp_code;
			guint16 icmp_cksum;
			union{
				struct{
					guint16 id;
		  			guint16  sequence;
			 	} echo; 
			} un;
		} sim_icmphdr;
		struct sim_tcp{
			guint16 th_sport;
			guint16 th_dport;
			guint32 th_seq;
			guint32 th_ack;
		#if SIMBIGENDIAN == 0
			guint8 th_x2:4;
			guint8 th_off:4;
		#elif SIMBIGENDIAN == 1
			guint8 th_off:4;
			guint8 th_x2:4;
		#else
			#error "Please fix SIMBIGENDIAN value (0 litte endian, 1 big endian)"
		#endif
			guint8 th_flags;
			guint16  th_win;
			guint16  th_sum;
			guint16  th_urp;
			guint8 th_opt[10*4]; /* IP options */
			guint8 nOptions;
		}sim_tcphdr;
	} hdr;
	guint8 *payload;
	guint  payloadlen;
};

struct _SimPacketClass {
  GObjectClass parent_class;
  };
static void
sim_packet_impl_dispose (GObject *gobject);
static void
sim_packet_impl_finalize (GObject *gobject);
static void
sim_packet_class_init(SimPacketClass *class);
void
sim_packet_class_init(SimPacketClass *class);
SimPacket *sim_packet_new(void);
GType
sim_packet_get_type (void);

G_END_DECLS

#ifdef __cplusplus
}
#endif 

#endif 
