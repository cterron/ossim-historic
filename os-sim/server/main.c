#include <config.h>

#include <libgda/libgda.h>
#include <ossim.h>

int
main (int argc, char *argv[])
{
  SimServer* server = NULL;

  gda_init ("OssimGDA", "0.1", argc, argv);

  server = sim_server_new();
  sim_server_run(server);

  return 0;
}
