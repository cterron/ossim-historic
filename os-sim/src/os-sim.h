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

#ifndef __OS_SIM_H__
#define __OS_SIM_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <libos-sim.h>

#include <sim-organizer.h>
#include <sim-scheduler.h>
#include <sim-server.h>

typedef struct {
  SimConfig	*config;		//this config is passed to server, scheduler, organizer, etc in sim_*_new,
												//so they can add or remove configuration things individually.

  SimContainer	*container;
  SimOrganizer	*organizer;
  SimScheduler	*scheduler;
  SimServer	*server;
  SimServer	*HA_server;		
  //SimMasterServer	*master_server;

  SimDatabase	*dbossim;
  SimDatabase	*dbsnort;

  GMutex	*mutex_directives;
  GMutex	*mutex_backlogs;

  struct {
    gchar	*filename;
    gint	fd;
    gint	level;
    guint	handler[3]; //we use 3 handlers because we call 3 times to g_log_set_handler().
  } log;

} SimMain;

extern SimMain	ossim;

typedef struct 
{
  gchar			      *config;
  gboolean        daemon;
  gint            debug;
  gchar						*ip;
  gint            port;
} SimCmdArgs;

SimCmdArgs simCmdArgs;

#ifdef __cplusplus
}
#endif /* __cplusplus */
 
#endif /* __OS_SIM_H__ */
