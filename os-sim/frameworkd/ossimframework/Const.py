#
# !!
#

# Ossim framework daemon version
VERSION = "0.9.8"

# default delay between iterations
# overriden with -s option
SLEEP = 300

# default configuration file
# overriden with -c option
CONFIG_FILE = "/etc/ossim/framework/ossim.conf"

# default log directory location
LOG_DIR = "/var/log/ossim/"

# default rrdtool bin path
# overriden if there is an entry at ossim.conf
RRD_BIN = "/usr/bin/rrdtool"

# don't show debug by default
# overriden with -v option
VERBOSE = 0

# default listener port
# overriden with -p option
LISTENER_PORT = 40003

# vim:ts=4 sts=4 tw=79 expandtab:
