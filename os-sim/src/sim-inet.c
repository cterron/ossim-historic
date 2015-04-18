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

#include "sim-inet.h"


#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#ifdef BSD
#define KERNEL
#include <netinet/in.h>
#endif

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

static gint get_bits (gchar   *numbits);

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
 *
 *
 *
 *
 */
static gint
get_bits (gchar   *numbits)
{
  gchar        *endptr;
  gint          bits;

  bits = strtol (numbits, &endptr, 10);

  if (*numbits == '\0' || *endptr != '\0')
    return -1;

  return bits;
}

/*
 *
 *
 *
 *
 */
SimInet*
sim_inet_new (const gchar      *hostname)
{
  SimInet      *inet;
  gchar        *slash;
  gint          bits = 0;
  struct in_addr inaddr;

#ifdef HAVE_IPV6
  struct in6_addr in6addr;
#endif

  g_return_val_if_fail (hostname, NULL);

  slash = strchr (hostname, '/');
  if (slash)
    *slash = '\0';

  if (slash)
    bits = get_bits (slash + 1);

  if (bits < 0)
    return NULL;

  if ((inet_pton(AF_INET, hostname, &inaddr) > 0) && (bits <= 32))
    {
      struct sockaddr_in* sa_in;

      inet = SIM_INET (g_object_new (SIM_TYPE_INET, NULL));

      sa_in = (struct sockaddr_in*) &inet->_priv->sa;
 
      sa_in->sin_family = AF_INET;
      sa_in->sin_addr = inaddr;

      inet->_priv->bits = (bits) ? bits : 32;
    }
#ifdef HAVE_IPV6
  else if ((inet_pton(AF_INET6, hostname, &in6addr) > 0) && (bits <= 128))
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
 *
 *
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
 *
 *
 *
 *
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

      if ((val1 >> (32 - inet1->_priv->bits)) == (val2 >> (32 - inet1->_priv->bits)))
	return TRUE;
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
