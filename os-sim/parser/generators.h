/* $Id: generators.h,v 1.3 2003/10/09 15:26:10 dkarg Exp $ */
/*
** Copyright (C) 1998-2002 Martin Roesch <roesch@sourcefire.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef __GENERATORS_H__
#define __GENERATORS_H__

#define GENERATOR_SNORT_ENGINE        1

#define GENERATOR_TAG                 2
#define    TAG_LOG_PKT                1

#define GENERATOR_SPP_PORTSCAN      100
#define     PORTSCAN_SCAN_DETECT        1
#define     PORTSCAN_INTER_INFO         2
#define     PORTSCAN_SCAN_END           3

#define GENERATOR_SPP_MINFRAG       101
#define     MINFRAG_ALERT_ID            1

#define GENERATOR_SPP_HTTP_DECODE   102
#define     HTTP_DECODE_UNICODE_ATTACK  1
#define     HTTP_DECODE_CGINULL_ATTACK  2
#define     HTTP_DECODE_LARGE_METHOD    3
#define     HTTP_DECODE_MISSING_URI     4
#define     HTTP_DECODE_DOUBLE_ENC      5
#define     HTTP_DECODE_ILLEGAL_HEX     6
#define     HTTP_DECODE_OVERLONG_CHAR   7


#define GENERATOR_SPP_DEFRAG        103
#define     DEFRAG_FRAG_OVERFLOW        1
#define     DEFRAG_FRAGS_DISCARDED      2

#define GENERATOR_SPP_SPADE         104
#define     SPADE_ANOM_THRESHOLD_EXCEEDED   1
#define     SPADE_ANOM_THRESHOLD_ADJUSTED   2

#define GENERATOR_SPP_BO            105
#define     BO_TRAFFIC_DETECT               1

#define GENERATOR_SPP_RPC_DECODE    106
#define     RPC_FRAG_TRAFFIC                1

#define GENERATOR_SPP_STREAM2       107
#define GENERATOR_SPP_STREAM3       108
#define GENERATOR_SPP_TELNET_NEG    109

#define GENERATOR_SPP_UNIDECODE     110
#define     UNIDECODE_CGINULL_ATTACK        1
#define     UNIDECODE_DIRECTORY_TRAVERSAL   2
#define     UNIDECODE_UNKNOWN_MAPPING       3
#define     UNIDECODE_INVALID_MAPPING       4

#define GENERATOR_SPP_STREAM4       111
#define     STREAM4_STEALTH_ACTIVITY            1
#define     STREAM4_EVASIVE_RST                 2
#define     STREAM4_EVASIVE_RETRANS             3
#define     STREAM4_WINDOW_VIOLATION            4
#define     STREAM4_DATA_ON_SYN                 5
#define     STREAM4_STEALTH_FULL_XMAS           6
#define     STREAM4_STEALTH_SAPU                7
#define     STREAM4_STEALTH_FIN_SCAN            8
#define     STREAM4_STEALTH_NULL_SCAN           9
#define     STREAM4_STEALTH_NMAP_XMAS_SCAN      10
#define     STREAM4_STEALTH_VECNA_SCAN          11
#define     STREAM4_STEALTH_NMAP_FINGERPRINT    12
#define     STREAM4_STEALTH_SYN_FIN_SCAN        13
#define     STREAM4_FORWARD_OVERLAP             14
#define     STREAM4_TTL_EVASION                 15
#define     STREAM4_EVASIVE_RETRANS_DATA        16
#define     STREAM4_EVASIVE_RETRANS_DATASPLIT   17
#define     STREAM4_MULTIPLE_ACKED              18

#define GENERATOR_SPP_ARPSPOOF      112
#define     ARPSPOOF_UNICAST_ARP_REQUEST         1
#define     ARPSPOOF_ETHERFRAME_ARP_MISMATCH_SRC  2
#define     ARPSPOOF_ETHERFRAME_ARP_MISMATCH_DST  3
#define     ARPSPOOF_ARP_CACHE_OVERWRITE_ATTACK   4

#define GENERATOR_SPP_FRAG2         113
#define     FRAG2_OVERSIZE_FRAG                   1
#define     FRAG2_TEARDROP                        2
#define     FRAG2_TTL_EVASION                     3
#define     FRAG2_OVERLAP                         4
#define     FRAG2_DUPFIRST                        5
#define     FRAG2_MEM_EXCEED                      6
#define     FRAG2_OUTOFORDER                      7
#define     FRAG2_IPOPTIONS                       8

#define GENERATOR_SPP_FNORD         114
#define     FNORD_NOPSLED                         1

#define GENERATOR_SPP_ASN1          115
#define     ASN1_INDEFINITE_LENGTH                1
#define     ASN1_INVALID_LENGTH                   2
#define     ASN1_OVERSIZED_ITEM                   3
#define     ASN1_SPEC_VIOLATION                   4
#define     ASN1_DATUM_BAD_LENGTH                 5


#define GENERATOR_SNORT_DECODE      116
#define     DECODE_NOT_IPV4_DGRAM                 1
#define     DECODE_IPV4_INVALID_HEADER_LEN        2
#define     DECODE_IPV4_DGRAM_LT_IPHDR            3

#define     DECODE_TCP_DGRAM_LT_TCPHDR            45
#define     DECODE_TCP_INVALID_OFFSET             46


#define     DECODE_UDP_DGRAM_LT_UDPHDR            95

#define     DECODE_ICMP_DGRAM_LT_ICMPHDR          105
#define     DECODE_ICMP_DGRAM_LT_TIMESTAMPHDR     106
#define     DECODE_ICMP_DGRAM_LT_ADDRHDR          107
#define     DECODE_IPV4_DGRAM_UNKNOWN             108

#define     DECODE_ARP_TRUNCATED                  109
#define     DECODE_EAPOL_TRUNCATED                110
#define     DECODE_EAPKEY_TRUNCATED               111
#define     DECODE_EAP_TRUNCATED                  112

#define GENERATOR_SPP_SCAN2         117
#define     SCAN_TYPE                             1

#define GENERATOR_SPP_CONV         118
#define     CONV_BAD_IP_PROTOCOL                            1


/*  This is where all the alert messages will be archived for each
    internal alerts */

#define ARPSPOOF_UNICAST_ARP_REQUEST_STR "(spp_arpspoof) Unicast ARP request"
#define ARPSPOOF_ETHERFRAME_ARP_MISMATCH_SRC_STR \
"(spp_arpspoof) Ethernet/ARP Mismatch request for Source"
#define ARPSPOOF_ETHERFRAME_ARP_MISMATCH_DST_STR \
"(spp_arpspoof) Ethernet/ARP Mismatch request for Destination"
#define ARPSPOOF_ARP_CACHE_OVERWRITE_ATTACK_STR \
"(spp_arpspoof) Attempted ARP cache overwrite attack"

#define ASN1_INDEFINITE_LENGTH_STR "(spp_asn1) Indefinite ASN.1 length encoding"
#define ASN1_INVALID_LENGTH_STR "(spp_asn1) Invalid ASN.1 length encoding"
#define ASN1_OVERSIZED_ITEM_STR "(spp_asn1) ASN.1 oversized item, possible overflow"
#define ASN1_SPEC_VIOLATION_STR  "(spp_asn1) ASN.1 spec violation, possible overflow"
#define ASN1_DATUM_BAD_LENGTH_STR "(spp_asn1) ASN.1 Attack: Datum length > packet length"

#define BO_TRAFFIC_DETECT_STR "(spo_bo) Back Orifice Traffic detected"

#define RPC_DECODE_FRAG_STR "(spp_rpc_decode) Fragmented RPC records detected"

#define FNORD_NOPSLED_IA32_STR "(spp_fnord) Possible Mutated IA32 NOP Sled detected"
#define FNORD_NOPSLED_HPPA_STR "(spp_fnord) Possible Mutated HPPA NOP Sled detected"
#define FNORD_NOPSLED_SPARC_STR "(spp_fnord) Possible Mutated SPARC NOP Sled detected"

#define FRAG2_DUPFIRST_STR "(spp_frag2) Duplicate first fragments"
#define FRAG2_IPOPTIONS_STR "(spp_frag2) IP Options on Fragmented Packet"
#define FRAG2_OUTOFORDER_STR "(spp_frag2) Out of order fragments"
#define FRAG2_OVERLAP_STR "(spp_frag2) Overlapping new fragment (probable fragroute)"
#define FRAG2_OVERSIZE_FRAG_STR "(spp_frag2) Oversized fragment, probable DoS"
#define FRAG2_TEARDROP_STR "(spp_frag2) Teardrop attack"
#define FRAG2_TTL_EVASION_STR "(spp_frag2) TTL Limit Exceeded (reassemble) detection"

#define HTTP_DECODE_LARGE_METHOD_STR "(spp_http_decode) A large HTTP method was received"
#define HTTP_DECODE_MISSING_URI_STR "(spp_http_decode) HTTP request without URI"
#define HTTP_DECODE_DOUBLE_ENC_STR  "(spp_http_decode) Double Hex Encoding Received"
#define HTTP_DECODE_ILLEGAL_HEX_STR "(spp_http_decode) Illegal URL hex encoding"
#define HTTP_DECODE_OVERLONG_CHAR_STR "(spp_http_decode) Overlong Unicode character received"

#define STREAM4_MULTIPLE_ACKED_STR "(spp_stream4) Multiple Acked Packets (possible fragroute)"
#define STREAM4_DATA_ON_SYN_STR  "(spp_stream4) DATA ON SYN detection"
#define STREAM4_STEALTH_NMAP_FINGERPRINT_STR "(spp_stream4) NMAP FINGERPRINT (stateful) detection"
#define STREAM4_STEALTH_FULL_XMAS_STR "(spp_stream4) STEALTH ACTIVITY (Full XMAS scan) detection"
#define STREAM4_STEALTH_SAPU_STR "(spp_stream4) STEALTH ACTIVITY (SAPU scan) detection"
#define STREAM4_STEALTH_FIN_SCAN_STR "(spp_stream4) STEALTH ACTIVITY (FIN scan) detection"
#define STREAM4_STEALTH_SYN_FIN_SCAN_STR "(spp_stream4) STEALTH ACTIVITY (SYN FIN scan) detection"
#define STREAM4_STEALTH_NULL_SCAN_STR "(spp_stream4) STEALTH ACTIVITY (NULL scan) detection"
#define STREAM4_STEALTH_NMAP_XMAS_SCAN_STR "(spp_stream4) STEALTH ACTIVITY (XMAS scan) detection"
#define STREAM4_STEALTH_VECNA_SCAN_STR "(spp_stream4) STEALTH ACTIVITY (Vecna scan) detection"
#define STREAM4_STEALTH_ACTIVITY_STR "(spp_stream4) STEALTH ACTIVITY (unknown) detection"
#define STREAM4_EVASIVE_RST_STR "(spp_stream4) possible EVASIVE RST detection"
#define STREAM4_TTL_EVASION_STR "(spp_stream4) TTL LIMIT Exceeded"
#define STREAM4_EVASIVE_RETRANS_STR "(spp_stream4) Possible RETRANSMISSION detection"
#define STREAM4_WINDOW_VIOLATION_STR "(spp_stream4) WINDOW VIOLATION detection"
#define STREAM4_EVASIVE_RETRANS_DATA_STR \
 "(spp_stream4) TCP CHECKSUM CHANGED ON RETRANSMISSION (possible fragroute) detection"
#define STREAM4_FORWARD_OVERLAP_STR "(spp_stream4) FORWARD OVERLAP detection"
#define STREAM4_EVASIVE_RETRANS_DATASPLIT_STR \
"(spp_stream4) TCP TOO FAST RETRANSMISSION WITH DIFFERENT DATA SIZE (possible fragroute) detection"


#define DECODE_NOT_IPV4_DGRAM_STR "(snort_decoder) WARNING: Not IPv4 datagram!"
#define DECODE_IPV4_INVALID_HEADER_LEN_STR "(snort_decoder) WARNING: hlen < IP_HEADER_LEN!"
#define DECODE_IPV4_DGRAM_LT_IPHDR_STR "(snort_decoder) WARNING: IP dgm len < IP Hdr len!"
#define DECODE_TCP_DGRAM_LT_TCPHDR_STR "(snort_decoder) TCP packet len is smaller than 20 bytes!"
#define DECODE_TCP_INVALID_OFFSET_STR "(snort_decoder) WARNING: TCP Data Offset is less than 5!"
#define DECODE_UDP_DGRAM_LT_UDPHDR_STR "(snort_decoder) WARNING: Truncated UDP Header!"
#define DECODE_ICMP_DGRAM_LT_ICMPHDR_STR "(snort_decoder) WARNING: ICMP Header Truncated!"
#define DECODE_ICMP_DGRAM_LT_TIMESTAMPHDR_STR "(snort_decoder) WARNING: ICMP Timestamp Header Truncated!"
#define DECODE_ICMP_DGRAM_LT_ADDRHDR_STR "(snort_decoder) WARNING: ICMP Address Header Truncated!"
#define DECODE_IPV4_DGRAM_UNKNOWN_STR "(snort_decoder) Unknown Datagram decoding problem!"
#define DECODE_ARP_TRUNCATED_STR "(snort_decoder) WARNING: Truncated ARP!"
#define DECODE_EAPOL_TRUNCATED_STR "(snort_decoder) WARNING: Truncated EAP Header!"
#define DECODE_EAPKEY_TRUNCATED_STR "(snort_decoder) WARNING: EAP Key Truncated!"
#define DECODE_EAP_TRUNCATED_STR "(snort_decoder) WARNING: EAP Header Truncated!"

#define SCAN2_PREFIX_STR "(spp_portscan2) Portscan detected from "

#define CONV_BAD_IP_PROTOCOL_STR "(spp_conversation) Bad IP protocol!"
#endif /* __GENERATORS_H__ */
