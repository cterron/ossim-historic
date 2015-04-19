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


#include "sim-inet.h"
#include <config.h>


#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <netinet/in.h>

#if defined(__APPLE__) && defined(__MACH__) && !defined(s6_addr16)
#define s6_addr16   __u6_addr.__u6_addr16
#endif
/*
#ifdef BSD
#define KERNEL
#include <netinet/in.h>
#endif

#ifdef __FreeBSD__
#define _KERNEL
#include <netinet/in.h>
#endif
*/
enum
{
  DESTROY,
  LAST_SIGNAL
};

struct _SimInetPrivate 
{
  guchar    bits;
  struct sockaddr_storage sa;
};

static gpointer parent_class = NULL;
static gint sim_inet_signals[LAST_SIGNAL] = { 0 };

static gint get_bits (gchar   *string_to_count);

/* GType Functions */

static void 
sim_inet_impl_dispose (GObject  *gobject)
{
  G_OBJECT_CLASS (parent_class)->dispose (gobject);
}

static void
sim_inet_impl_finalize (GObject  *gobject)
{
  SimInet *inet = SIM_INET (gobject);

  g_free (inet->_priv);

  G_OBJECT_CLASS (parent_class)->finalize (gobject);
}

static void
sim_inet_class_init (SimInetClass * class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  parent_class = g_type_class_peek_parent (class);

  object_class->dispose = sim_inet_impl_dispose;
  object_class->finalize = sim_inet_impl_finalize;
}

static void
sim_inet_instance_init (SimInet *inet)
{
  inet->_priv = g_new0 (SimInetPrivate, 1);
}

/* Public Methods */

GType
sim_inet_get_type (void)
{
  static GType object_type = 0;
 
  if (!object_type)
  {
    static const GTypeInfo type_info = {
              sizeof (SimInetClass),
              NULL,
              NULL,
              (GClassInitFunc) sim_inet_class_init,
              NULL,
              NULL,                       /* class data */
              sizeof (SimInet),
              0,                          /* number of pre-allocs */
              (GInstanceInitFunc) sim_inet_instance_init,
              NULL                        /* value table */
    };
    
    g_type_init ();
                                                                                                                             
    object_type = g_type_register_static (G_TYPE_OBJECT, "SimInet", &type_info, 0);
  }
                                                                                                                             
  return object_type;
}

/*
 * Returns the number of bits that a string that contains a number has.
 * This is usefull to store the netmask of the networks in the SimInet objects.
 */
static gint
get_bits (gchar   *string_to_count)
{
  gchar        *endptr=NULL;
  gint          bits;

  bits = strtol (string_to_count, &endptr, 10);

  if (*string_to_count == '\0' || *endptr != '\0')
    return -1;

  return bits;
}

/*
 * Transforms something like: "192.168.0.1/24" or "192.168.8.9" into a SimInet object.
 *
 *
 */
SimInet*
sim_inet_new (const gchar      *hostname_ip)
{
  SimInet      *inet;
  gchar        *slash;
  gint          bits = 0;
  struct in_addr inaddr;

#ifdef HAVE_IPV6
  struct in6_addr in6addr;
#endif

  g_return_val_if_fail (hostname_ip, NULL);

  slash = strchr (hostname_ip, '/');
  if (slash)
  {
    *slash = '\0'; //we remove the "/" to do the inet_pton without use another variable.
    bits = get_bits (slash + 1); //count the mask bits.
  }

  if (bits < 0)
    return NULL;

  if ((inet_pton(AF_INET, hostname_ip, &inaddr) > 0) && (bits <= 32))
  {
    struct sockaddr_in* sa_in;

    inet = SIM_INET (g_object_new (SIM_TYPE_INET, NULL));

    sa_in = (struct sockaddr_in*) &inet->_priv->sa;
 
    sa_in->sin_family = AF_INET;
    sa_in->sin_addr = inaddr;

 		//If hostname_ip is "0.0.0.0", instead "0.0.0.0/32", SimInet object must be "0.0.0.0/0"
  	if (!strcmp(hostname_ip, "0.0.0.0"))
	  	inet->_priv->bits = 0;
		else	
		  inet->_priv->bits = (bits) ? bits : 32; //if there are no mask bits, assume it's a host (32 mask bit)

  }
#ifdef HAVE_IPV6
  else
  if ((inet_pton(AF_INET6, hostname_ip, &in6addr) > 0) && (bits <= 128))
  {
    struct sockaddr_in6* sa_in6;

    inet = SIM_INET (g_object_new (SIM_TYPE_INET, NULL));

    sa_in6 = (struct sockaddr_in6*) &inet->_priv->sa;

    sa_in6->sin6_family = AF_INET6;
    sa_in6->sin6_addr = in6addr;

    inet->_priv->bits = (bits) ? bits : 128;
  }
#endif
  else
  {
    if (slash)
      *slash = '/';
    return NULL;
  }

  if (slash)
   *slash = '/';

  return inet;
}

/*
 *
 * Creates a SimInet object, wich can contains a host or a network depending on mask.
 *
 */
SimInet*
sim_inet_new_from_ginetaddr (const GInetAddr  *ia)
{
  SimInet *inet;
  gchar   *hostname;

  g_return_val_if_fail (ia, NULL);

  if (!(hostname = gnet_inetaddr_get_canonical_name (ia)))
    return NULL;

  inet =  sim_inet_new (hostname);

  g_free (hostname);

  return inet;
}

/*
 *
 *
 *
 */
SimInet*
sim_inet_clone (SimInet  *inet)
{
  SimInet *new_inet;

  g_return_val_if_fail (inet, NULL);

  new_inet = SIM_INET (g_object_new (SIM_TYPE_INET, NULL));
  new_inet->_priv->bits = inet->_priv->bits;
  new_inet->_priv->sa = inet->_priv->sa;

  return new_inet;
}

/*
 *
 *
 *
 *
 */
gboolean
sim_inet_equal (SimInet   *inet1,
		SimInet   *inet2)
{
  g_return_val_if_fail (inet1, FALSE);
  g_return_val_if_fail (SIM_IS_INET (inet1),FALSE);
  g_return_val_if_fail (inet2, FALSE);
  g_return_val_if_fail (SIM_IS_INET (inet2),FALSE);

  if (inet1->_priv->sa.ss_family != inet2->_priv->sa.ss_family) 
    return FALSE;

  if (inet1->_priv->sa.ss_family == AF_INET)
    {
      struct sockaddr_in* sa_in1 = (struct sockaddr_in*) &inet1->_priv->sa;
      struct sockaddr_in* sa_in2 = (struct sockaddr_in*) &inet2->_priv->sa;
      
      if (sa_in1->sin_addr.s_addr == sa_in2->sin_addr.s_addr)
	return TRUE;
    }
#ifdef HAVE_IPV6
  else if (inet1->_priv->sa.ss_family == AF_INET6)
    {
      struct sockaddr_in6* sa_in1 = (struct sockaddr_in6*) &inet1->_priv->sa;
      struct sockaddr_in6* sa_in2 = (struct sockaddr_in6*) &inet2->_priv->sa;

      if (IN6_ARE_ADDR_EQUAL(&sa_in1->sin6_addr, &sa_in2->sin6_addr))
        return TRUE;
    }
#endif

  return FALSE;
}

/*
 * Check if inet2 belongs to the inet1 network
 */
gboolean
sim_inet_has_inet (SimInet   *inet1,
		  						 SimInet   *inet2)
{
  g_return_val_if_fail (inet1, FALSE);
  g_return_val_if_fail (SIM_IS_INET (inet1),FALSE);
  g_return_val_if_fail (inet2, FALSE);
  g_return_val_if_fail (SIM_IS_INET (inet2),FALSE);

  if (inet1->_priv->sa.ss_family != inet2->_priv->sa.ss_family)
    return FALSE;

  if (inet1->_priv->sa.ss_family == AF_INET)
    {
      struct sockaddr_in* sa_in1 = (struct sockaddr_in*) &inet1->_priv->sa;
      struct sockaddr_in* sa_in2 = (struct sockaddr_in*) &inet2->_priv->sa;

      guint32 val1 = ntohl (sa_in1->sin_addr.s_addr);
      guint32 val2 = ntohl (sa_in2->sin_addr.s_addr);

/* Debug
      gchar *temp = g_strdup_printf ("%d.%d.%d.%d",
                             (val1 >> 24) & 0xFF,
                             (val1 >> 16) & 0xFF,
                             (val1 >> 8) & 0xFF,
                             (val1) & 0xFF);

      g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_inet_has_inet val1; %d bits: %s",inet1->_priv->bits, temp);
      g_free(temp);
      temp = g_strdup_printf ("%d.%d.%d.%d",
                             (val2 >> 24) & 0xFF,
                             (val2 >> 16) & 0xFF,
                             (val2 >> 8) & 0xFF,
                             (val2) & 0xFF);
      g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_inet_has_inet val2; %d bits: %s",inet2->_priv->bits, temp);
      g_free(temp);
*/

      if ((val1 >> (32 - inet1->_priv->bits)) == (val2 >> (32 - inet1->_priv->bits)))
			{
//	      g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_inet_has_inet MATCH");							
				return TRUE;
			}
//			else
//	      g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_inet_has_inet DOESN'T MATCH");							
			
    }
#ifdef HAVE_IPV6
  else if (inet1->_priv->sa.ss_family == AF_INET6)
    {
      /* TODO */
      return FALSE;
    }
#endif

  return FALSE;
}

/*
 *
 *
 *
 *
 */
gboolean
sim_inet_is_reserved (SimInet          *inet)
{
  g_return_val_if_fail (inet, FALSE);
  g_return_val_if_fail (SIM_IS_INET (inet), FALSE);

  if (inet->_priv->sa.ss_family == AF_INET)
    {
      struct sockaddr_in* sa_in = (struct sockaddr_in*) &inet->_priv->sa;
      guint32 addr = ntohl (sa_in->sin_addr.s_addr);

      if ((addr & 0xFFFF0000) == 0)
        return TRUE;
 
      if ((addr & 0xF8000000) == 0xF0000000)
        return TRUE;
    }
#ifdef HAVE_IPV6
  else if (inet->_priv->sa.ss_family == AF_INET6)
    {
      struct sockaddr_in6* sa_in = (struct sockaddr_in6*) &inet->_priv->sa;
      guint32 addr = ntohl (sa_in->sin6_addr.s6_addr[0]);

      if ((addr & 0xFFFF0000) == 0)
        return TRUE;
    }
#endif

  return FALSE;
}

/*
 *
 *
 *
 *
 */
gchar*
sim_inet_ntop (SimInet  *inet)
{
  gchar    *ret = NULL;
  guint32   val;

  g_return_val_if_fail (inet, NULL);
  g_return_val_if_fail (SIM_IS_INET (inet), NULL);

  if (inet->_priv->sa.ss_family == AF_INET)
    {
      struct sockaddr_in* sa_in = (struct sockaddr_in*) &inet->_priv->sa;
      val = ntohl (sa_in->sin_addr.s_addr);
      ret = g_strdup_printf ("%d.%d.%d.%d", 
			     (val >> 24) & 0xFF,
			     (val >> 16) & 0xFF,
			     (val >> 8) & 0xFF,
			     (val) & 0xFF);
    }
#ifdef HAVE_IPV6
  else if (inet->_priv->sa.ss_family == AF_INET6)
    {
      struct sockaddr_in6* sa_in6 = (struct sockaddr_in6*) &inet->_priv->sa;
      
      ret = g_strdup_printf ("%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x",
			     ntohs(sa_in6->sin6_addr.s6_addr16[0]),
			     ntohs(sa_in6->sin6_addr.s6_addr16[1]),
			     ntohs(sa_in6->sin6_addr.s6_addr16[2]),
			     ntohs(sa_in6->sin6_addr.s6_addr16[3]),
			     ntohs(sa_in6->sin6_addr.s6_addr16[4]),
			     ntohs(sa_in6->sin6_addr.s6_addr16[5]),
			     ntohs(sa_in6->sin6_addr.s6_addr16[6]),
			     ntohs(sa_in6->sin6_addr.s6_addr16[7]));
    }
#endif

  return ret;
}

/*
 *
 *
 *
 *
 */
gchar*
sim_inet_cidr_ntop (SimInet  *inet)
{
  gchar    *ret = NULL;
  guint32   val;

  g_return_val_if_fail (inet, NULL);
  g_return_val_if_fail (SIM_IS_INET (inet), NULL);

  if (inet->_priv->sa.ss_family == AF_INET)
    {
      struct sockaddr_in* sa_in = (struct sockaddr_in*) &inet->_priv->sa;
      val = ntohl (sa_in->sin_addr.s_addr);
      ret = g_strdup_printf ("%d.%d.%d.%d/%d", 
			     (val >> 24) & 0xFF,
			     (val >> 16) & 0xFF,
			     (val >> 8) & 0xFF,
			     (val) & 0xFF,
			     (inet->_priv->bits) ? inet->_priv->bits : 32);
    }
#ifdef HAVE_IPV6
  else if (inet->_priv->sa.ss_family == AF_INET6)
    {
      struct sockaddr_in6* sa_in6 = (struct sockaddr_in6*) &inet->_priv->sa;

      ret = g_strdup_printf ("%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x/%u",
			     ntohs(sa_in6->sin6_addr.s6_addr16[0]),
			     ntohs(sa_in6->sin6_addr.s6_addr16[1]),
			     ntohs(sa_in6->sin6_addr.s6_addr16[2]),
			     ntohs(sa_in6->sin6_addr.s6_addr16[3]),
			     ntohs(sa_in6->sin6_addr.s6_addr16[4]),
			     ntohs(sa_in6->sin6_addr.s6_addr16[5]),
			     ntohs(sa_in6->sin6_addr.s6_addr16[6]),
			     ntohs(sa_in6->sin6_addr.s6_addr16[7]),
			     (inet->_priv->bits) ? inet->_priv->bits : 128);
    }
#endif

  return ret;
}
// vim: set tabstop=2:
