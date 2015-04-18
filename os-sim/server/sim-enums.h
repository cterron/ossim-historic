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

#define SIM_CONFIG_FILE             "/etc/ossim.conf"

#define SIM_DELIMITER_LIST          ","
#define SIM_DELIMITER_LEVEL         ":"
#define SIM_IN_ADDR_ANY_CONST       "ANY"
#define SIM_IN_ADDR_ANY_IP_STR      "0.0.0.0"

#define SIM_SRC_IP_CONST            "SRC_IP"
#define SIM_DST_IP_CONST            "DST_IP"
#define SIM_SRC_PORT_CONST          "SRC_PORT"
#define SIM_DST_PORT_CONST          "DST_PORT"

#define BUFFER_SIZE                 1024

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

typedef enum
{
  SIM_PROTOCOL_TYPE_NONE  = -1,
  SIM_PROTOCOL_TYPE_ICMP  = 1,
  SIM_PROTOCOL_TYPE_TCP   = 6,
  SIM_PROTOCOL_TYPE_UDP   = 17
} SimProtocolType;

typedef enum
{
  SIM_CONFIG_PROPERTY_TYPE_NOME,
  SIM_CONFIG_PROPERTY_TYPE_LOG_FILE,
  SIM_CONFIG_PROPERTY_TYPE_DATABASE,
  SIM_CONFIG_PROPERTY_TYPE_USERNAME,
  SIM_CONFIG_PROPERTY_TYPE_PASSWORD,
  SIM_CONFIG_PROPERTY_TYPE_UPDATE_INTERVAL
}  SimConfigPropertyType;

typedef enum 
{
  SIM_SIGNATURE_TYPE_NONE,
  SIM_SIGNATURE_TYPE_GROUP,
  SIM_SIGNATURE_TYPE_SIGNATURE
} SimSignatureType;

typedef enum 
{
  SIM_SIGNATURE_GROUP_TYPE_NONE,
  SIM_SIGNATURE_GROUP_TYPE_ATTACK_RESPONSES,
  SIM_SIGNATURE_GROUP_TYPE_BACKDOOR,
  SIM_SIGNATURE_GROUP_TYPE_BAD_TRAFFIC,
  SIM_SIGNATURE_GROUP_TYPE_CHAT,
  SIM_SIGNATURE_GROUP_TYPE_DDOS,
  SIM_SIGNATURE_GROUP_TYPE_DELETED,
  SIM_SIGNATURE_GROUP_TYPE_DNS,
  SIM_SIGNATURE_GROUP_TYPE_DOS,
  SIM_SIGNATURE_GROUP_TYPE_EXPERIMENTAL,
  SIM_SIGNATURE_GROUP_TYPE_EXPLOIT,
  SIM_SIGNATURE_GROUP_TYPE_FINGER,
  SIM_SIGNATURE_GROUP_TYPE_FTP,
  SIM_SIGNATURE_GROUP_TYPE_FW1_ACCEPT,
  SIM_SIGNATURE_GROUP_TYPE_FW1_DROP,
  SIM_SIGNATURE_GROUP_TYPE_FW1_REJECT,
  SIM_SIGNATURE_GROUP_TYPE_ICMP,
  SIM_SIGNATURE_GROUP_TYPE_ICMP_INFO,
  SIM_SIGNATURE_GROUP_TYPE_IMAP,
  SIM_SIGNATURE_GROUP_TYPE_INFO,
  SIM_SIGNATURE_GROUP_TYPE_LOCAL,
  SIM_SIGNATURE_GROUP_TYPE_MISC,
  SIM_SIGNATURE_GROUP_TYPE_MULTIMEDIA,
  SIM_SIGNATURE_GROUP_TYPE_MYSQL,
  SIM_SIGNATURE_GROUP_TYPE_NETBIOS,
  SIM_SIGNATURE_GROUP_TYPE_NNTP,
  SIM_SIGNATURE_GROUP_TYPE_ORACLE,
  SIM_SIGNATURE_GROUP_TYPE_OTHER_IDS,
  SIM_SIGNATURE_GROUP_TYPE_P2P,
  SIM_SIGNATURE_GROUP_TYPE_POLICY,
  SIM_SIGNATURE_GROUP_TYPE_POP2,
  SIM_SIGNATURE_GROUP_TYPE_POP3,
  SIM_SIGNATURE_GROUP_TYPE_PORN,
  SIM_SIGNATURE_GROUP_TYPE_RPC,
  SIM_SIGNATURE_GROUP_TYPE_RSERVICES,
  SIM_SIGNATURE_GROUP_TYPE_SCAN,
  SIM_SIGNATURE_GROUP_TYPE_SHELLCODE,
  SIM_SIGNATURE_GROUP_TYPE_SMTP,
  SIM_SIGNATURE_GROUP_TYPE_SNMP,
  SIM_SIGNATURE_GROUP_TYPE_SPADE,
  SIM_SIGNATURE_GROUP_TYPE_SQL,
  SIM_SIGNATURE_GROUP_TYPE_TELNET,
  SIM_SIGNATURE_GROUP_TYPE_TFTP,
  SIM_SIGNATURE_GROUP_TYPE_VIRUS,
  SIM_SIGNATURE_GROUP_TYPE_WEB_ATTACKS,
  SIM_SIGNATURE_GROUP_TYPE_WEB_CGI,
  SIM_SIGNATURE_GROUP_TYPE_WEB_CLIENT,
  SIM_SIGNATURE_GROUP_TYPE_WEB_COLDFUSION,
  SIM_SIGNATURE_GROUP_TYPE_WEB_FRONTPAGE,
  SIM_SIGNATURE_GROUP_TYPE_WEB_IIS,
  SIM_SIGNATURE_GROUP_TYPE_WEB_MISC,
  SIM_SIGNATURE_GROUP_TYPE_WEB_PHP,
  SIM_SIGNATURE_GROUP_TYPE_X11
} SimSignatureGroupType;

typedef enum {
  SIM_MESSAGE_TYPE_INVALID,
  SIM_MESSAGE_TYPE_SNORT,
  SIM_MESSAGE_TYPE_LOGGER,
  SIM_MESSAGE_TYPE_RRD
} SimMessageType;

typedef enum {
  SIM_PLUGIN_TYPE_NONE = 0x00,
  SIM_PLUGIN_TYPE_SNORT = 0x01,
  SIM_PLUGIN_TYPE_NTOP = 0x02,
  SIM_PLUGIN_TYPE_FW1 = 0x03
} SimPluginType;

typedef enum {
  SIM_COMMAND_TYPE_NONE,
  SIM_COMMAND_TYPE_CONNECT,
  SIM_COMMAND_TYPE_CONNECT_OK,
  SIM_COMMAND_TYPE_PLUGINS_FOLLOW,
  SIM_COMMAND_TYPE_MESSAGE,
  SIM_COMMAND_TYPE_ERROR
} SimCommandType;

typedef enum {
  SIM_SESSION_TYPE_NONE,
  SIM_SESSION_TYPE_AGENT,
  SIM_SESSION_TYPE_WEB
} SimSessionType;

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
  SIM_RULE_VAR_DST_PORT
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

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SIM_ENUMS_H__ */
