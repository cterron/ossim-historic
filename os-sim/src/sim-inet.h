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

#ifndef __SIM_INET_H__
#define __SIM_INET_H__ 1

#include <config.h>
#include <glib.h>
#include <glib-object.h>
#include <gnet.h>

#include "sim-enums.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef HAVE_SOCKADDR_STORAGE
struct sockaddr_storage {
#ifdef HAVE_SOCKADDR_LEN
                unsigned char ss_len;
                unsigned char ss_family;
#else
        unsigned short ss_family;
#endif
        char info[126];
};
#endif

#define SIM_TYPE_INET                  (sim_inet_get_type ())
#define SIM_INET(obj)                  (G_TYPE_CHECK_INSTANCE_CAST (obj, SIM_TYPE_INET, SimInet))
#define SIM_INET_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST (klass, SIM_TYPE_INET, SimInetClass))
#define SIM_IS_INET(obj)               (G_TYPE_CHECK_INSTANCE_TYPE (obj, SIM_TYPE_INET))
#define SIM_IS_INET_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), SIM_TYPE_INET))
#define SIM_INET_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), SIM_TYPE_INET, SimInetClass))

G_BEGIN_DECLS

//A SimInet object defines a single network object. It can be a host or a network.

typedef struct _SimInet        SimInet;
typedef struct _SimInetClass   SimInetClass;
typedef struct _SimInetPrivate SimInetPrivate;

struct _SimInet {
  GObject parent;

  SimInetPrivate *_priv;
};

struct _SimInetClass {
  GObjectClass parent_class;
};

GType             sim_inet_get_type                        (void);
gint              sim_inet_get_mask                        (SimInet          *inet);

SimInet*          sim_inet_new                             (const gchar      *hostname_ip);
SimInet*          sim_inet_new_from_ginetaddr              (const GInetAddr  *ia);

SimInet*          sim_inet_clone                           (SimInet          *inet);

gboolean          sim_inet_equal                           (SimInet          *inet1,
							    SimInet          *inet2);
gboolean          sim_inet_has_inet                        (SimInet          *inet1,
							    SimInet          *inet2);

gboolean          sim_inet_is_reserved                     (SimInet          *inet);

gchar*            sim_inet_ntop                            (SimInet          *inet);
gchar*            sim_inet_cidr_ntop                       (SimInet          *inet);

gboolean          sim_inet_debug_print                     (SimInet          *inet);

G_END_DECLS

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SIM_INET_H__ */
// vim: set tabstop=2:
