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

#ifndef __SIM_ENUMS_H__
#define __SIM_ENUMS_H__ 1

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SIM_LOG_FILE                "server.log"

#define SIM_DELIMITER_LIST          ","
#define SIM_DELIMITER_LEVEL         ":"
#define SIM_DELIMITER_RANGE         "-"
#define SIM_DELIMITER_PIPE	        "|"	//used to separate data in remote DB loading
#define SIM_IN_ADDR_ANY_CONST       "ANY"
#define SIM_IN_ADDR_ANY_IP_STR      "0.0.0.0"

#define SIM_HOME_NET_CONST          "HOME_NET"

#define SIM_SRC_IP_CONST            "SRC_IP"
#define SIM_DST_IP_CONST            "DST_IP"
#define SIM_SRC_PORT_CONST          "SRC_PORT"
#define SIM_DST_PORT_CONST          "DST_PORT"
#define SIM_PROTOCOL_CONST          "PROTOCOL"
#define SIM_PLUGIN_SID_CONST        "PLUGIN_SID"
#define SIM_SENSOR_CONST			      "SENSOR"
#define SIM_FILENAME_CONST			    "FILENAME"
#define SIM_USERNAME_CONST			    "USERNAME"
#define SIM_PASSWORD_CONST			    "PASSWORD"
#define SIM_USERDATA1_CONST			    "USERDATA1"
#define SIM_USERDATA2_CONST			    "USERDATA2"
#define SIM_USERDATA3_CONST			    "USERDATA3"
#define SIM_USERDATA4_CONST			    "USERDATA4"
#define SIM_USERDATA5_CONST			    "USERDATA5"
#define SIM_USERDATA6_CONST			    "USERDATA6"
#define SIM_USERDATA7_CONST			    "USERDATA7"
#define SIM_USERDATA8_CONST			    "USERDATA8"
#define SIM_USERDATA9_CONST			    "USERDATA9"

#define SIM_SRC			    0  
#define SIM_DST			    1

#define SIM_DETECTOR_CONST          "DETECTOR"
#define SIM_MONITOR_CONST           "MONITOR"

#define BUFFER_SIZE                 8192
#define TIMEBUF_SIZE                26

#define GENERATOR_SPP_SPADE         104
#define GENERATOR_SPP_SCAN2         117
#define GENERATOR_SNORT_ENGINE      1

#define SNORT_DEFAULT_PRIORITY      2
#define SNORT_MAX_PRIORITY          3

#define FW1_DEFAULT_PRIORITY        1

#define GENERATOR_FW1               200
#define FW1_ACCEPT_TYPE             1
#define FW1_DROP_TYPE               2
#define FW1_REJECT_TYPE             3

#define FW1_ACCEPT_PRIORITY         0
#define FW1_DROP_PRIORITY           1
#define FW1_REJECT_PRIORITY         1

#define RRD_DEFAULT_PRIORITY        5

#define SIM_XML_CONFIG_FILE         "config.xml"
#define SIM_XML_DIRECTIVE_FILE      "directives.xml"

#define SIM_DS_OSSIM                "ossimDS"
#define SIM_DS_SNORT                "snortDS"
#define SIM_DS_OSVDB                "osvdbDS"

#define SIM_PLUGIN_ID_DIRECTIVE     1505

typedef enum
{
  SIM_ALARM_RISK_TYPE_NONE,
  SIM_ALARM_RISK_TYPE_LOW,
  SIM_ALARM_RISK_TYPE_MEDIUM,
  SIM_ALARM_RISK_TYPE_HIGH,
  SIM_ALARM_RISK_TYPE_ALL
} SimAlarmRiskType;

typedef enum
{
  SIM_INET_TYPE_NONE,
  SIM_INET_TYPE_INET,
  SIM_INET_TYPE_CIDR
} SimInetType;

typedef enum
{
  SIM_DATABASE_TYPE_NONE,
  SIM_DATABASE_TYPE_MYSQL,
  SIM_DATABASE_TYPE_PGSQL,
  SIM_DATABASE_TYPE_ORACLE,
  SIM_DATABASE_TYPE_ODBC
} SimDatabaseType;

typedef enum
{
  SIM_CONDITION_TYPE_NONE,
  SIM_CONDITION_TYPE_EQ,
  SIM_CONDITION_TYPE_NE,
  SIM_CONDITION_TYPE_LT,
  SIM_CONDITION_TYPE_LE,
  SIM_CONDITION_TYPE_GT,
  SIM_CONDITION_TYPE_GE  
} SimConditionType;

typedef enum
{
  SIM_PROTOCOL_TYPE_NONE  = -1,
  SIM_PROTOCOL_TYPE_ICMP  = 1,
  SIM_PROTOCOL_TYPE_TCP   = 6,
  SIM_PROTOCOL_TYPE_UDP   = 17,
	// I know, I know, the "protocols" below are not protocols, but we have to assign something to the "protocol"
	// field into event list with arpwatch, snare, etc. The "protocols" below are unnasigned in /etc/protocols
  SIM_PROTOCOL_TYPE_HOST_ARP_EVENT   			= 134,
  SIM_PROTOCOL_TYPE_HOST_OS_EVENT   			= 135,
  SIM_PROTOCOL_TYPE_HOST_SERVICE_EVENT   	= 136,
  SIM_PROTOCOL_TYPE_HOST_IDS_EVENT   			= 137,
  SIM_PROTOCOL_TYPE_INFORMATION_EVENT			= 138,
  SIM_PROTOCOL_TYPE_OTHER   							= 139
} SimProtocolType;

typedef enum {
  SIM_PLUGIN_TYPE_NONE,
  SIM_PLUGIN_TYPE_DETECTOR,
  SIM_PLUGIN_TYPE_MONITOR,
} SimPluginType;

typedef enum {
  SIM_PLUGIN_STATE_TYPE_NONE,
  SIM_PLUGIN_STATE_TYPE_START,
  SIM_PLUGIN_STATE_TYPE_STOP,
} SimPluginStateType;

typedef enum {
  SIM_EVENT_TYPE_NONE,
  SIM_EVENT_TYPE_DETECTOR,
  SIM_EVENT_TYPE_MONITOR,
} SimEventType;

typedef enum {
  SIM_RULE_TYPE_NONE,
  SIM_RULE_TYPE_DETECTOR,
  SIM_RULE_TYPE_MONITOR,
} SimRuleType;

typedef enum {
  SIM_RULE_VAR_NONE,
  SIM_RULE_VAR_SRC_IA,
  SIM_RULE_VAR_DST_IA,
  SIM_RULE_VAR_SRC_PORT,
  SIM_RULE_VAR_DST_PORT,
  SIM_RULE_VAR_PROTOCOL,
  SIM_RULE_VAR_PLUGIN_SID,
  SIM_RULE_VAR_SENSOR,
  SIM_RULE_VAR_FILENAME,
  SIM_RULE_VAR_USERNAME,
  SIM_RULE_VAR_PASSWORD,
  SIM_RULE_VAR_USERDATA1,
  SIM_RULE_VAR_USERDATA2,
  SIM_RULE_VAR_USERDATA3,
  SIM_RULE_VAR_USERDATA4,
  SIM_RULE_VAR_USERDATA5,
  SIM_RULE_VAR_USERDATA6,
  SIM_RULE_VAR_USERDATA7,
  SIM_RULE_VAR_USERDATA8,
  SIM_RULE_VAR_USERDATA9,
} SimRuleVarType;

typedef enum {
  SIM_ACTION_TYPE_NONE,
  SIM_ACTION_TYPE_TIME_OUT,
  SIM_ACTION_TYPE_MATCHED
} SimActionType;

typedef enum {
  SIM_ACTION_DO_TYPE_NONE,
  SIM_ACTION_DO_TYPE_MAILTO,
  SIM_ACTION_DO_TYPE_DATBASE
} SimActionDoType;

typedef enum {
  SIM_COMMAND_TYPE_NONE,
  SIM_COMMAND_TYPE_CONNECT,
  SIM_COMMAND_TYPE_SESSION_APPEND_PLUGIN,
  SIM_COMMAND_TYPE_SESSION_REMOVE_PLUGIN,
  SIM_COMMAND_TYPE_SERVER,										//msg to send to frameworkd or to master servers
  SIM_COMMAND_TYPE_SERVER_GET_SENSORS,
  SIM_COMMAND_TYPE_SERVER_GET_SERVERS,
  SIM_COMMAND_TYPE_SERVER_GET_SENSOR_PLUGINS,
  SIM_COMMAND_TYPE_SERVER_SET_DATA_ROLE,
  SIM_COMMAND_TYPE_SENSOR,										
  SIM_COMMAND_TYPE_SENSOR_PLUGIN,							
  SIM_COMMAND_TYPE_SENSOR_PLUGIN_START,				
  SIM_COMMAND_TYPE_SENSOR_PLUGIN_STOP,				
  SIM_COMMAND_TYPE_SENSOR_PLUGIN_ENABLE,			
  SIM_COMMAND_TYPE_SENSOR_PLUGIN_DISABLE,		
  SIM_COMMAND_TYPE_PLUGIN_STATE_STARTED,
  SIM_COMMAND_TYPE_PLUGIN_STATE_UNKNOWN,
  SIM_COMMAND_TYPE_PLUGIN_STATE_STOPPED,
  SIM_COMMAND_TYPE_PLUGIN_ENABLED,
  SIM_COMMAND_TYPE_PLUGIN_DISABLED,
  SIM_COMMAND_TYPE_EVENT,
  SIM_COMMAND_TYPE_MESSAGE,
  SIM_COMMAND_TYPE_RELOAD_PLUGINS,
  SIM_COMMAND_TYPE_RELOAD_SENSORS,
  SIM_COMMAND_TYPE_RELOAD_HOSTS,
  SIM_COMMAND_TYPE_RELOAD_NETS,
  SIM_COMMAND_TYPE_RELOAD_POLICIES,
  SIM_COMMAND_TYPE_RELOAD_DIRECTIVES,
  SIM_COMMAND_TYPE_RELOAD_ALL,
  SIM_COMMAND_TYPE_WATCH_RULE,
  SIM_COMMAND_TYPE_HOST_OS_EVENT,
  SIM_COMMAND_TYPE_HOST_MAC_EVENT,
  SIM_COMMAND_TYPE_HOST_SERVICE_EVENT,
  SIM_COMMAND_TYPE_HOST_IDS_EVENT,
  SIM_COMMAND_TYPE_OK,
  SIM_COMMAND_TYPE_ERROR,
  SIM_COMMAND_TYPE_DATABASE_QUERY,
  SIM_COMMAND_TYPE_DATABASE_ANSWER,
	SIM_COMMAND_TYPE_SNORT_EVENT
} SimCommandType;

typedef enum {
  SIM_SESSION_TYPE_NONE,
  SIM_SESSION_TYPE_SERVER_UP,	//master servers: servers wich are more "high" in the architecture. Is possible to fetch data from them
  SIM_SESSION_TYPE_SERVER_DOWN, //servers to send data to (children): send the role (correlate, store...), host data, networks, and so on.
//  SIM_SESSION_TYPE_RSERVER,
  SIM_SESSION_TYPE_SENSOR,
  SIM_SESSION_TYPE_WEB,
  SIM_SESSION_TYPE_HA,	//High Availability servers
  SIM_SESSION_TYPE_ALL
} SimSessionType;

typedef enum {
  SIM_SESSION_STATE_NONE,
  SIM_SESSION_STATE_DISCONNECT,
  SIM_SESSION_STATE_CONNECT
} SimSessionState;

typedef enum {
	SIM_DB_ELEMENT_TYPE_PLUGINS			,
	SIM_DB_ELEMENT_TYPE_PLUGIN_SIDS	,
	SIM_DB_ELEMENT_TYPE_PLUGIN_REFERENCES	, //cross correlation
	SIM_DB_ELEMENT_TYPE_HOST_PLUGIN_SIDS	, //cross correlation
	SIM_DB_ELEMENT_TYPE_SENSORS			,
	SIM_DB_ELEMENT_TYPE_HOSTS				,
	SIM_DB_ELEMENT_TYPE_NETS				,
	SIM_DB_ELEMENT_TYPE_POLICIES		,
	SIM_DB_ELEMENT_TYPE_HOST_LEVELS ,
	SIM_DB_ELEMENT_TYPE_NET_LEVELS	,
	SIM_DB_ELEMENT_TYPE_SERVER_ROLE	, //as this is a config parameter it won't be stored in container, it will be stored in server's config.
	SIM_DB_ELEMENT_TYPE_LOAD_COMPLETE, //this is not a type of element to load. But we will use it in sim_container_new() to tell that we have ended the data loading msgs
} SimDBElementType;

typedef enum {
	SIM_SCHEDULER_STATE_NORMAL	= 0,
	SIM_SCHEDULER_STATE_INITIAL	= 1
} SimSchedulerState;

typedef enum
{
	SIM_POLICY_ELEMENT_TYPE_GENERAL,
	SIM_POLICY_ELEMENT_TYPE_ROLE,
	SIM_POLICY_ELEMENT_TYPE_SRC,
	SIM_POLICY_ELEMENT_TYPE_DST,
	SIM_POLICY_ELEMENT_TYPE_PORTS,
	SIM_POLICY_ELEMENT_TYPE_SENSORS,
	SIM_POLICY_ELEMENT_TYPE_PLUGIN_IDS,
	SIM_POLICY_ELEMENT_TYPE_PLUGIN_SIDS,
	SIM_POLICY_ELEMENT_TYPE_PLUGIN_GROUPS,
	SIM_POLICY_ELEMENT_TYPE_TARGETS
} SimPolicyElementType;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SIM_ENUMS_H__ */

// vim: set tabstop=2:

