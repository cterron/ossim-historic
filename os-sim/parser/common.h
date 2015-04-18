#include "generators.h"
#include <mysql.h>

/* paths to external files */
#define ATTACK_RESPONSES_FILE       "/etc/snort/rules/attack-responses.rules"
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



/* ===================== */
/* ==== prototypes ===== */
/* ===================== */

/* conf.c */
char* get_conf(char *option);

/* calculate.c */
void calculate(MYSQL *mysql, int plugin, int tplugin, 
               unsigned int priority_snort, 
               char protocol[5], char source_ip[15], char dest_ip[15],
               int source_port, int dest_port);

/* base.c */
int get_priority(MYSQL *mysql, char *query, int *priority);
int get_asset(MYSQL *mysql, char *ip, int *asset);
int get_level(MYSQL *mysql, char *ip, char *type, int *level);
int update_level(MYSQL *mysql, char *ip, char *type, int level);
int update_nets_level(MYSQL *mysql, char *ip, char *type, int level);
int insert_level(MYSQL *mysql, char *ip, int levelC, int levelA);

int get_sensor_id(MYSQL *mysql, char *sensor_fw1, unsigned long int *sensor_id);
int get_sig_id(MYSQL *mysql, char *action, unsigned long int *sig_id);
int get_sig_class_id(MYSQL *mysql, unsigned long int *sig_id);

int insert_event(MYSQL *mysql, unsigned long int sid, unsigned long int sig_id,
                 unsigned long int sig_class_id, char *new_sig_name,
                 unsigned long int src_ip, unsigned long int dst_ip,
                 unsigned int src_port, unsigned int dst_port,
                 char *protocol);

/* acid_net.c */
unsigned long int acidIP2long(char *ip);


/* fw1.c */
int getportbyservice(char *service);
int insert_fw1_alert(unsigned long int source_ip,
                     unsigned int source_port,
                     unsigned long int dest_ip,
                     unsigned int dest_port,
                     char *service, 
                     char *protocol, 
                     char *action, 
                     char *sensor_fw, 
                     unsigned int rule);

/* graph.c */
void graph (MYSQL *mysql, char *source_ip, char *dest_ip);

/* rrd_anomaly.c */
void log_rrd(MYSQL *mysql, char source_ip[16], char what[128]);

