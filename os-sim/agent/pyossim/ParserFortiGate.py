#!/usr/bin/python

import re
import util


###
### Patterns for all FortiGate log messages
### TODO: May be buggy, i haven't got logs to test it
###

pattern_header = re.compile (
        "(\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}).*?"      +\
        "log_id=\d+.*?"                                 +\
        "type=([^\s]+).*?"                              +\
        "subtype=([^\s]+).*?"                           +\
        "pri=([^\s]+).*?"                               +\
        "(?:vd=[^\s]+ )?"                               +\
        "(?:SN=[^\s]+ )?"
    )


# generic patterns
pattern_template = {

        "null":
            "(?P<src>)(?P<dst>)(?P<sport>)(?P<dport>)(?P<proto>)(?P<data>)",

        "traffic":
            "duration=[^\s]+.*?"              +\
            "rule=[^\s]+.*?"                  +\
            "policyid=[^\s]+.*?"              +\
            "proto=(?P<proto>[^\s]+).*?"      +\
            "service=[^\s]+.*?"               +\
            "status=[^\s]+.*?"                +\
            "src=(?P<src>[^\s]+).*?"          +\
            "srcname=[^\s]+.*?"               +\
            "dst=(?P<dst>[^\s]+).*?"          +\
            "dstname=[^\s]+.*?"               +\
            "src_int=[^\s]+.*?"               +\
            "dst_int=[^\s]+.*?"               +\
            "sent=[^\s]+.*?"                  +\
            "rcvd=[^\s]+.*?"                  +\
            "sent_pkt=[^\s]+.*?"              +\
            "rcvd_pkt=[^\s]+.*?"              +\
            "src_port=(?P<sport>[^\s]+).*?"   +\
            "dst_port=(?P<dport>[^\s]+).*?"   +\
            "vpn=[^\s]+.*?"                   +\
            "tran_ip=[^\s]+.*?"               +\
            "tran_port=[^\s]+.*?"             +\
            "dir_disp=[^\s]+.*?"              +\
            "tran_disp=[^\s]+",

        "virus":
            "src=(?P<src>[^\s]+).*?"            +\
            "dst=(?P<dst>[^\s]+).*?"            +\
            "src_int=[^\s]+.*?"                 +\
            "dst_int=[^\s]+.*?"                 +\
            "service=[^\s]+.*?"                 +\
            "status=[^\s]+.*?"                  +\
            "msg=\"(?P<data>[^\"]+)\""          +\
            "(?P<sport>)(?P<dport>)(?P<proto>)",

        "ipsec":
            "loc_ip=(?P<src>[^\s]+).*?"         +\
            "loc_port=(?P<sport>[^\s]+).*?"     +\
            "rem_ip=(?P<dst>[^\s]+).*?"         +\
            "rem_port=(?P<dport>[^\s]+).*?"     +\
            "out_if=[^\s]+.*?"                  +\
            "vpn_tunnel=[^\s]+.*?",

    }

pattern_body = {

    # Traffic Log
    "traffic": {

        # Policy allowed traffic
        "allowed": [
            {
                "sid": 10001, 
                "pattern": re.compile(pattern_template["traffic"])
            }
        ],

        # Policy violation traffic
        "violation": [
            {
                "sid": 13001,
                "pattern": re.compile(pattern_template["traffic"])
            }
        ]
    },

    # Event Log
    "event": {

        # System activity event
        "system": [
            {
                "sid": 20001,
                "pattern": re.compile (
                    "gateway=[^\s]+.*?"     +\
                    "interface=[^\s]+.*?"   +\
                    "status=[^\s]+"         +\
                    pattern_template["null"]
                )
            },
            {
                "sid": 20002,
                "pattern": re.compile (
                    "user=[^\s]+.*?"              +\
                    "ui=[^\s]+.*?"                +\
                    "action=[^\s]+.*?"            +\
                    "status=[^\s]+.*?"            +\
                    "msg=\"(?P<data>[^\"]+)\""    +\
                    "(?P<src>)(?P<dst>)(?P<sport>)(?P<dport>)(?P<proto>)"
                )
            },
            {
                "sid": 20031,
                "pattern": re.compile (
                    "Out of memory in [^\s+]" +\
                    pattern_template["null"]
                )
            },
            {
                "sid": 20032,
                "pattern": re.compile (
                    "Interface [^\s]+ not found in [^\s]+" +\
                    pattern_template["null"]
                )
            },
            {
                "sid": 20033,
                "pattern": re.compile (
                    "using Mobile IPv6 extensions" +\
                    pattern_template["null"]
                )
            },
            {
                "sid": 20034,
                "pattern": re.compile (
                    "MinRtrAdvInterval for [^\s]+ " +\
                    "must be between [^\s]+ and [^\s]+" +\
                    pattern_template["null"]
                )
            },
            {
                "sid": 20035,
                "pattern": re.compile (
                    "MinRtrAdvInterval must be between [^\s]+ " +\
                    "and [^\s]+ for [^\s]+" +\
                    pattern_template["null"]
                )
            },
            {
                "sid": 20036,
                "pattern": re.compile (
                    "MaxRtrAdvInterval for [^\s]+ " +\
                    "must be between [^\s]+ and [^\s]+" +\
                    pattern_template["null"]
                )
            },
            {
                "sid": 20037,
                "pattern": re.compile (
                    "MaxRtrAdvInterval must be between [^\s]+ " +\
                    "and [^\s]+ for [^\s]+" +\
                    pattern_template["null"]
                )
            },
            {
                "sid": 20038,
                "pattern": re.compile (
                    "AdvLinkMTU must be zero or between [^\s]+ " +\
                    "and [^\s]+ for [^\s]+" +\
                    pattern_template["null"]
                )
            },
            {
                "sid": 20039,
                "pattern": re.compile (
                    "AdvLinkMTU must be zero or greater than [^\s]+ " +\
                    "for [^\s]+" +\
                    pattern_template["null"]
                )
            },
            {
                "sid": 20040,
                "pattern": re.compile (
                    "AdvReachableTime must be less than [^\s]+ " +\
                    "for [^\s]+" +\
                    pattern_template["null"]
                )
            },
            {
                "sid": 20041,
                "pattern": re.compile (
                    "AdvCurHopLimit must not be greater than [^\s]+ " +\
                    "for [^\s]+" +\
                    pattern_template["null"]
                )
            },
            {
                "sid": 20042,
                "pattern": re.compile (
                    "AdvDefault Lifetime for [^\s]+ " +\
                    "must be zero or between [^\s]+ and [^\s]+" +\
                    pattern_template["null"]
                )
            },
            {
                "sid": 20043,
                "pattern": re.compile (
                    "HomeAgentLifetime must be between [^\s]+ " +\
                    "and [^\s]+ for [^\s]+" +\
                    pattern_template["null"]
                )
            },
            {
                "sid": 20044,
                "pattern": re.compile (
                    "AdvHomeAgentFlag must be set with HomeAgentInfo" +\
                    pattern_template["null"]
                )
            },
            {
                "sid": 20045,
                "pattern": re.compile (
                    "invalid prefix length for [^\s]+" +\
                    pattern_template["null"]
                )
            },
            ### TODO: pfff, a lot of event->system patterns...
        ],

        # IPSec negotiation event
        "ipsec": [
            {
                "sid": 23001,
                "pattern": re.compile (
                    "FortiGate report: replay packet is detected, "     +\
                    "(?P<src>\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3})"       +\
                    ".*?->.*?"                                          +\
                    "(?P<dst>\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3})"       +\
                    ", seq=[^\s]+"                                      +\
                    "(?P<sport>)(?P<dport>)(?P<proto>)(?P<data>)"
                )
            },
            {
                "sid": 23002,
                "pattern": re.compile (
                    pattern_template["ipsec"]           +\
                    "action=negotiate.*?"               +\
                    "status=[^\s]+.*?"                  +\
                    "msg=\"(?P<data>[^\"]+)\""          +\
                    "(?P<proto>)"
                )
            },
            {
                "sid": 23003,
                "pattern": re.compile (
                    pattern_template["ipsec"]           +\
                    "status=[^\s]+.*?"                  +\
                    "msg=\"(?P<data>[^\"]+)\""          +\
                    "(?P<proto>)"
                )
            },
            {
                "sid": 23004,
                "pattern": re.compile (
                    pattern_template["ipsec"]           +\
                    "action=negotiate.*?"               +\
                    "init=[^\s]+.*?"                    +\
                    "mode=[^\s]+.*?"                    +\
                    "stage=[^\s]+.*?"                   +\
                    "dir=[^\s]+.*?"                     +\
                    "status=[^\s]+.*?"                  +\
                    "msg=\"(?P<data>[^\"]+)\""          +\
                    "(?P<proto>)"
                )
            },
            {
                "sid": 23005,
                "pattern": re.compile (
                    pattern_template["ipsec"]           +\
                    "status=[^\s]+.*?"                  +\
                    "spi=[^\s]+.*?"                     +\
                    "seqno=[^\s]+.*?"                   +\
                    "msg=\"(?P<data>[^\"]+)\""          +\
                    "(?P<proto>)"
                )
            },
            {
                "sid": 23006,
                "pattern": re.compile (
                    pattern_template["ipsec"]           +\
                    "action=install_sa.*?"              +\
                    "in_spi=[^\s]+.*?"                  +\
                    "out_spi=[^\s]+.*?"                 +\
                    "msg=\"(?P<data>[^\"]+)\""          +\
                    "(?P<proto>)"
                )
            },
            {
                "sid": 23007,
                "pattern": re.compile (
                    pattern_template["ipsec"]           +\
                    "action=delete_phase1_sa.*?"        +\
                    "spi=[^\s]+.*?"                     +\
                    "msg=\"(?P<data>[^\"]+)\""          +\
                    "(?P<proto>)"
                )
            },
            {
                "sid": 23008,
                "pattern": re.compile (
                    pattern_template["ipsec"]           +\
                    "action=delete_ipsec_sa.*?"         +\
                    "spi=[^\s]+.*?"                     +\
                    "msg=\"(?P<data>[^\"]+)\""          +\
                    "(?P<proto>)"
                )
            },
        ],

        # DHCP service event
        "dhcp": [
            {
                # DHCP request and response log
                "sid": 26001,
                "pattern": re.compile (
                    "dhcp_msg=[^\s]+.*?"    +\
                    "dir=[^\s]+.*?"         +\
                    "mac=[^\s]+.*?"         +\
                    "ip=(?P<src>[^\s]+).*?" +\
                    "lease=[^\s]+.*?"       +\
                    "msg=(?P<data>.*)"      +\
                    "(?P<dst>)(?P<sport>)(?P<dport>)(?P<proto>)"
                )
            }
        ],

        # L2TP/PPTP/PPPoE service event
        "ppp": [],

        # admin event
        "admin": [],

        # HA activity event
        "ha": [],

        # Firewall authentication event
        "auth": [
            {
                # The specified user has failed to get authenticated 
                # before timeout occurred
                "sid": 38001,
                "pattern": re.compile (
                    "user=[^\s]+.*?"            +\
                    "service=[^\s]+.*?"         +\
                    "action=no.*?"              +\
                    "status=failure.*?"         +\
                    "reason=timeout.*?"         +\
                    "src=(?P<src>[^\s]+).*?"    +\
                    "dst=(?P<dst>[^\s]+).*?"    +\
                    "(?P<sport>)(?P<dport>)(?P<proto>)(?P<data>)"
                )
            },
            {
                # The specified user was authenticated successfully
                "sid": 38002,
                "pattern": re.compile (
                    "user=[^\s]+.*?"            +\
                    "service=[^\s]+.*?"         +\
                    "action=[^\s]+.*?"          +\
                    "status=success.*?"         +\
                    "reason=none.*?"            +\
                    "src=(?P<src>[^\s]+).*?"    +\
                    "dst=(?P<dst>[^\s]+).*?"    +\
                    "(?P<sport>)(?P<dport>)(?P<proto>)(?P<data>)"
                )
            },
            {
                # The specified user failed to get authenticated
                "sid": 38003,
                "pattern": re.compile (
                    "user=[^\s]+.*?"            +\
                    "service=[^\s]+.*?"         +\
                    "action=[^\s]+.*?"          +\
                    "status=failure.*?"         +\
                    "reason=[^\s]+.*?"          +\
                    "src=(?P<src>[^\s]+).*?"    +\
                    "dst=(?P<dst>[^\s]+).*?"    +\
                    "(?P<sport>)(?P<dport>)(?P<proto>)(?P<data>)"
                )
            },
        ],

        # Pattern update event
        "pattern": [],

        # FortiGate-4000 and FortiGate-5000 series chassis event
        "chassis": [],
    },

    # Content Log
    "contentlog": {

        # Virus infected
        "HTTP": [],
        
        # FTP content meta-data
        "FTP": [],

        # IMAP conent meta-data
        "IMAP": [],

        # POP3 conent meta-data
        "POP3": [],

        # SMTP conent meta-data
        "SMTP": [],
    },

    # Antivirus Log
    "virus": {
        
        # Virus infected
        "infected": [
            {
                "sid": 60001,
                "pattern": re.compile (
                    "Virus/Worm detected: (?P<data>.*)" +\
                    "(?P<src>)(?P<dst>)(?P<sport>)(?P<dport>)(?P<proto>)"
                )
            },
            {
                "sid": 60002,
                "pattern": re.compile (pattern_template["virus"])
            },
        ],

        # Filename blocked
        "filename": [
            {
                "sid": 60003, # No info about this event!
                "pattern": re.compile (pattern_template["virus"])
            },
        ],

        # File oversized
        "oversize": [
            {
                "sid": 60004, # No info about this event!
                "pattern": re.compile (pattern_template["virus"])
            },
        ],
    },

    # Web Filter Log
    "webfilter": {
        
        # Content block
        "content": [],

        # URL block
        "urlblock": [],

        # URL exempt
        "urlexempt": [],

        # Blocked category ratings
        # Monitored category ratings
        # Category rating errors
        "catblock": [],
    },

    # Attack log
    "ids": {
        
        # Attack signature
        "signature": [
            {
                "sid": 70000,
                "pattern": re.compile (
                        "attack_id=[^\s]+.*?"               +\
                        "src=(?P<src>[^\s]+).*?"            +\
                        "dst=(?P<dst>[^\s]+).*?"            +\
                        "src_port=(?P<sport>[^\s]+).*?"     +\
                        "dst_port=(?P<dport>[^\s]+).*?"     +\
                        "interface=[^\s]+.*?"               +\
                        "src_int=[^\s]+.*?"                 +\
                        "dst_int=[^\s]+.*?"                 +\
                        "status=[^\s]+.*?"                  +\
                        "proto=(?P<proto>[^\s]+).*?"        +\
                        "service=[^\s]+.*?"                 +\
                        "msg=\"(?P<data>[^\"]+)\""
                    )
            }
        ],

        # Attack anomaly
        "anomaly": [
            {
                "sid": 73001,
                "pattern": re.compile (
                        "attack_id=[^\s]+.*?"               +\
                        "src=(?P<src>[^\s]+).*?"            +\
                        "dst=(?P<dst>[^\s]+).*?"            +\
                        "src_port=(?P<sport>[^\s]+).*?"     +\
                        "dst_port=(?P<dport>[^\s]+).*?"     +\
                        "interface=[^\s]+.*?"               +\
                        "src_int=[^\s]+.*?"                 +\
                        "dst_int=[^\s]+.*?"                 +\
                        "status=[^\s]+.*?"                  +\
                        "proto=(?P<proto>[^\s]+).*?"        +\
                        "service=[^\s]+.*?"                 +\
                        "msg=\"(?P<data>[^\"]+)\""
                    )
            }
        ],
    },

    # Spam Filter Log
    "emailfilter": {

        # Block email detected
        "blocklist": [],

        # Banned word detected
        "bannedword": [],
    }
}

fd = open("logs/fortinet.log")
for line in fd.readlines():
    result_header = pattern_header.search(line)
    if result_header is not None:
        (date, type, subtype, pri) = result_header.groups()

        # set priority
        priority = 1
        if pri in ("emergency", "alert", "critical"):
            priority = 3
        elif pri in ("error", "warning"):
            priority = 2
        elif pri in ("notification", "information"):
            priority = 1

        print "=="
        print "date:", date
        print "type:", type
        print "subtype:", subtype
        print "priority:", pri

        try:
            events = pattern_body[type][subtype]
        except KeyError:
            util.debug (__name__, "Unknown type - subtype (%s - %s)" % \
                (type, subtype), "**", "YELLOW")

        for event in events:
            result_body = event["pattern"].search(line)
            if result_body is not None:
                result_hash = result_body.groupdict()
                print "sid:",   event["sid"]
                print "src:",   result_hash['src']
                print "dst:",   result_hash['dst']
                print "sport:", result_hash['sport']
                print "dport:", result_hash['dport']
                print "proto:", result_hash['proto']
                print "msg:",   result_hash['data']
                break


