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

#define _GNU_SOURCE
#include <getopt.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <libgda/libgda.h>
#include <os-sim.h>

#include "sim-scheduler.h"
#include "sim-organizer.h"
#include "sim-session.h"
#include "sim-server.h"
#include "sim-xml-config.h"

/* Globals Variables */
SimContainer  *sim_ctn = NULL;
SimServer     *sim_svr = NULL;

typedef struct {
  SimConfig      *config;
} SimThreadData;

typedef struct {
  gchar          *config;
  gint            daemon;
} SimCmdArgs;

SimCmdArgs simCmdArgs;

/*
 *
 *
 *
 *
 */
static void
sim_log_handler (const gchar     *log_domain,
		 GLogLevelFlags   log_level,
		 const gchar     *message,
		 gpointer         data)
{
  gint fd = GPOINTER_TO_INT (data);

  write (fd, message, strlen(message));
  write (fd, "\n", 1);
}

/*
 * sim_container_scheduler:
 *
 *   arguments:
 *
 *   results:
 */
static gpointer
sim_thread_scheduler (gpointer data)
{
  SimThreadData   *thr_data = (SimThreadData *) data;
  SimConfig       *config = thr_data->config;
  SimScheduler    *scheduler;
 
  g_message ("sim_thread_scheduler");

  scheduler = sim_scheduler_new (config);
  sim_scheduler_run (scheduler);

  return NULL;
}

/**
 * sim_container_organizer:
 *
 *   arguments:
 *
 *   results:
 */

static gpointer
sim_thread_organizer (gpointer data)
{
  SimThreadData   *thr_data = (SimThreadData *) data;
  SimConfig       *config = thr_data->config;
  SimOrganizer    *organizer;
 
  g_return_if_fail (config);
  g_return_if_fail (SIM_IS_CONFIG (config));

  g_message ("sim_thread_organizer");

  organizer = sim_organizer_new (config);
  sim_organizer_run (organizer);

  return NULL;
}

/**
 * sim_container_organizer:
 *
 *   arguments:
 *
 *   results:
 */

static gpointer
sim_thread_server (gpointer data)
{
  SimThreadData   *thr_data = (SimThreadData *) data;
  SimConfig       *config = thr_data->config;
 
  g_return_if_fail (config);
  g_return_if_fail (SIM_IS_CONFIG (config));

  g_message ("sim_thread_server");

  sim_svr = sim_server_new (config);
  sim_server_run (sim_svr);

  return NULL;
}

/*
 *
 *
 *
 */
static void
options (int argc, char **argv)
{
  int c;
  int digit_optind = 0;

  /* Default Command Line Options */
  simCmdArgs.config = NULL;
  simCmdArgs.daemon = 0;

  while (TRUE)
    {
      int this_option_optind = optind ? optind : 1;
      int option_index = 0;
      static struct option options[] =
	{
	  {"config", 1, 0, 'c'},
	  {"daemon", 0, 0, 'd'},
	  {0, 0, 0, 0}
	};
      
      c = getopt_long (argc, argv, "dc:", options, &option_index);
      if (c == -1)
	break;

      switch (c)
	{
	case 'c':
	  simCmdArgs.config = g_strdup (optarg);
	  break;

	case 'd':
	  simCmdArgs.daemon = 1;
	  break;
	  
	case '?':
	  break;
	  
	default:
	  printf ("?\? getopt() return the caracter %c ?\?\n", c);
	}
    }
  
  if (optind < argc)
    {
      printf ("elements from ARGV are not option: ");
      while (optind < argc)
	printf ("%s ", argv[optind++]);
      printf ("\n");
    }
}

/*
 *
 *
 *
 */
int
main (int argc, char *argv[])
{
  SimThreadData  *data;
  SimXmlConfig   *xmlconfig;
  SimConfig      *config;
  GMainLoop      *loop;
  GThread        *thread;
  gint            fd;

  /* Command Line Options */
  options (argc, argv);

  if(simCmdArgs.daemon){
    if(fork()){
      exit(0);
    } else {
      ;
    }
  }

  /* Thread Init */
  if (!g_thread_supported ()) 
    g_thread_init (NULL);

  gda_init (NULL, "0.9.4", argc, argv);

  /* Config Init */
  if (simCmdArgs.config)
    xmlconfig = sim_xml_config_new_from_file (simCmdArgs.config);
  else
    xmlconfig = sim_xml_config_new_from_file (OS_SIM_GLOBAL_CONFIG_FILE);
  config = sim_xml_config_get_config (xmlconfig);

  /* File Logs */
  fd = creat (OS_SIM_LOG_DIR "/debug.log", S_IWUSR|S_IRUSR|S_IRGRP|S_IROTH);

  g_log_set_handler (NULL,
		     G_LOG_LEVEL_ERROR |
		     G_LOG_LEVEL_CRITICAL |
		     G_LOG_LEVEL_WARNING |
		     G_LOG_LEVEL_MESSAGE |
		     G_LOG_LEVEL_INFO |
		     G_LOG_LEVEL_DEBUG,
		     sim_log_handler, 
		     GINT_TO_POINTER (fd));
  
  g_log_set_handler (G_LOG_DOMAIN,
		     G_LOG_LEVEL_ERROR |
		     G_LOG_LEVEL_CRITICAL |
		     G_LOG_LEVEL_WARNING |
		     G_LOG_LEVEL_MESSAGE |
		     G_LOG_LEVEL_INFO |
		     G_LOG_LEVEL_DEBUG,
		     sim_log_handler, 
		     GINT_TO_POINTER (fd));

  /* Container init */
  sim_ctn = sim_container_new (config);

  /* Data Thread */
  data = g_new0 (SimThreadData, 1);
  data->config = config;

  /* Server Thread */
  thread = g_thread_create (sim_thread_server, data, TRUE, NULL);
  g_return_if_fail (thread);
  g_thread_set_priority (thread, G_THREAD_PRIORITY_NORMAL);

  /* Scheduler Thread */
  thread = g_thread_create (sim_thread_scheduler, data, TRUE, NULL);
  g_return_if_fail (thread);
  g_thread_set_priority (thread, G_THREAD_PRIORITY_NORMAL);

  /* Organizer Thread */
  thread = g_thread_create (sim_thread_organizer, data, TRUE, NULL);
  g_return_if_fail (thread);
  g_thread_set_priority (thread, G_THREAD_PRIORITY_NORMAL);

  /* Main Loop */
  loop = g_main_loop_new (NULL, FALSE);
  g_main_loop_run (loop);

  close (fd);

  return 0;
}
