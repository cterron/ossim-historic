#include "generators.h"
#include <mysql.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>


/* paths to external files */
#define ATTACK_RESPONSES_FILE       "/usr/local/share/snort/rules/attack-responses.rules"
#define ATTACK_RESPONSES_SIDS_FILE  "attack_responses_sids.dat"
#define SIDS_FILE                   "sids.dat"
#define SERVICES_FILE               "fw1_objects.dat"
#define OSSIM_CONF                  "/etc/ossim.conf"

/* static memory */
#define SIGNATURE_MAX_SIZE  64
#define QUERY_MAX_SIZE      2048

#define ANY_PORT  0

/* assets */
#define MIN_ASSET 1
#define MAX_ASSET 10
#define MED_ASSET (MAX_ASSET / 2)

/* snort priorities */
#define SNORT_DEFAULT_PRIORITY  2
#define SNORT_MAX_PRIORITY      3

/* fw1 */
//#define FW1
#define FW1_DEFAULT_PRIORITY   1

#define GENERATOR_FW1        200
#define FW1_ACCEPT_TYPE        1
#define FW1_DROP_TYPE          2
#define FW1_REJECT_TYPE        3

#define FW1_ACCEPT_PRIORITY    0
#define FW1_DROP_PRIORITY      1
#define FW1_REJECT_PRIORITY    1

#define FW1_SIG_CLASS_NAME    "FireWall-1 alerts"
#define FW1_SIG_ACCEPT_NAME   "FireWall-1 accept action"
#define FW1_SIG_DROP_NAME     "FireWall-1 drop action"
#define FW1_SIG_REJECT_NAME   "FireWall-1 reject action"

/* rrd anomaly detection */
#define RRD_DEFAULT_PRIORITY    5
#define GENERATOR_RRD_ANOMALY 201

/* policy definitions */
#define MAX_IPS 4096
#define MAX_PORTS 2048
#define MAX_SIGS 1024
#define MAX_SENSORS 256
#define MAX_DESC 256
#define MAX_NET_NAME 256
#define MAX_HOSTS 1024

/* ===================== */
/* ==== structs     ==== */
/* ===================== */

struct policy
{
  int policy_id;
  char source_ips[MAX_IPS];
  char dest_ips[MAX_IPS];
  int priority;
  int begin_hour;
  int end_hour;
  int begin_day;
  int end_day;
  char port_list[MAX_PORTS];
  char sigs[MAX_SIGS];
  char sensors[MAX_SENSORS];
  char desc[256];
  struct policy *next;
};

struct ossim_host
{
  struct in_addr ip;
  long a;
  long c;
  struct ossim_host *next;
};

struct ossim_asset
{
  struct in_addr ip;
  int asset;
  char sensors[MAX_SENSORS];
  struct ossim_asset *next;
};

struct ossim_net
{
  char net_name[MAX_NET_NAME];
  long a;
  long c;
  struct ossim_net *next;
};

struct ossim_net_asset
{
  char net_name[MAX_NET_NAME];
  struct in_addr ips;
  int mask;
  int asset;
  struct ossim_net_asset *next;
};

/* Typedefs */

typedef struct policy POLICY;
typedef POLICY *POL_LINK;

typedef struct ossim_host OSSIM_HOST;
typedef OSSIM_HOST *HOST_LINK;

typedef struct ossim_asset OSSIM_ASSET;
typedef OSSIM_ASSET *ASSET_LINK;

typedef struct ossim_net OSSIM_NET;
typedef OSSIM_NET *NET_LINK;

typedef struct ossim_net_asset OSSIM_NET_ASSET;
typedef OSSIM_NET_ASSET *NET_ASSET_LINK;

/* ===================== */
/* ==== prototypes ===== */
/* ===================== */

/* conf.c */
char *get_conf (char *option);

/* calculate.c */
void calculate (MYSQL * mysql, int plugin, int tplugin,
		unsigned int priority_snort,
		char protocol[5], char source_ip[15], char dest_ip[15],
		int source_port, int dest_port);

/* base.c */
int update_level (MYSQL * mysql, char *ip, char *type, int level);
int update_nets_level (MYSQL * mysql, char *ip, char type, int level);
int insert_level (MYSQL * mysql, char *ip, int levelC, int levelA);

int get_sensor_id (MYSQL * mysql, char *sensor_fw1,
		   unsigned long int *sensor_id);
int get_sig_id (MYSQL * mysql, char *action, unsigned long int *sig_id);
int get_sig_class_id (MYSQL * mysql, unsigned long int *sig_id);

int insert_event (MYSQL * mysql, unsigned long int sid,
		  unsigned long int sig_id, unsigned long int sig_class_id,
		  char *new_sig_name, unsigned long int src_ip,
		  unsigned long int dst_ip, unsigned int src_port,
		  unsigned int dst_port, char *protocol);

/* acid_net.c */
unsigned long int acidIP2long (char *ip);


/* fw1.c */
int getportbyservice (char *service);
int insert_fw1_alert (unsigned long int source_ip,
		      unsigned int source_port,
		      unsigned long int dest_ip,
		      unsigned int dest_port,
		      char *service,
		      char *protocol,
		      char *action, char *sensor_fw, unsigned int rule);

/* graph.c */
void graph (MYSQL * mysql, char *source_ip, char *dest_ip);

/* rrd_anomaly.c */
void log_rrd (MYSQL * mysql, char source_ip[16], char what[128],
	      unsigned int priority, HOST_LINK host, ASSET_LINK assets);

/* policy.c */
POL_LINK load_policy (MYSQL * mysql);
POL_LINK add_policy (MYSQL * mysql, char *row[], POL_LINK pol);
void show_policy (POL_LINK pol);
void free_policy (POL_LINK pol);
int get_priority (char *source_ip, char *dest_ip, char *protocol,
		  char *dest_port, char *signature, int date_expr,
		  POL_LINK pol, int *priority);

/* host_levels.c */
HOST_LINK load_hosts (MYSQL * mysql);
HOST_LINK add_host (char *row[], HOST_LINK host);
int get_host_level (HOST_LINK host, char *ip, char what, int *value);
int update_host_level (HOST_LINK host, char *ip, char what, int value);
void show_hosts (HOST_LINK hosts);
void free_hosts (HOST_LINK hosts);
HOST_LINK lower_hosts (HOST_LINK host, int recovery);

/* host_assets.c */
ASSET_LINK load_assets (MYSQL * mysql);
ASSET_LINK add_asset (MYSQL * mysql, char *row[], ASSET_LINK asset);
int get_asset (ASSET_LINK asset, char *ip);
void show_assets (ASSET_LINK assets);
void free_assets (ASSET_LINK assets);

/* net_levels.c */
NET_LINK load_nets (MYSQL * mysql);
NET_LINK add_net (char *row[], NET_LINK net);
int get_net_level (NET_LINK net, char *net_name, char what, int *value);
int update_net_level (NET_LINK net, char *net_name, char what, int value);
void show_nets (NET_LINK nets);
void save_nets (MYSQL * mysql, NET_LINK nets);
void free_nets (NET_LINK nets);
NET_LINK lower_nets (NET_LINK net, int recovery);

/* net_assets.c */
NET_ASSET_LINK load_net_assets (MYSQL * mysql);
NET_ASSET_LINK add_net_asset (char *row[], NET_ASSET_LINK net_asset);
int get_net_asset (NET_ASSET_LINK net_asset, char *net);
void show_net_assets (NET_ASSET_LINK net_assets);
void free_net_assets (NET_ASSET_LINK net_assets);
