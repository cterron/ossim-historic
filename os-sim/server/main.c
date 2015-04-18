#include <config.h>

#include <libgda/libgda.h>
#include <ossim.h>

int
main (int argc, char *argv[])
{
  SimConfig   *config; 
  SimServer   *server;

  if (!g_thread_supported ()) 
    {
      g_thread_init (NULL);
    }

  gda_init ("Ossim", "0.1", argc, argv);

  config = sim_config_new (SIM_CONFIG_FILE);

  server = sim_server_new(config);
  sim_server_run(server);

  return 0;
}
