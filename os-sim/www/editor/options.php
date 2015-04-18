<?php

/*
- sameip
- stateless
- rawbytes
- nocase
*/

    $ruleOptions = array (
                      /* "msg", */
                      "logto",
                      "ttl",
                      "tos",
                      /* "id", */
                      "ipoption",
                      "fragbits",
                      "dsize",
                      "flags",
                      "seq",
                      "ack",
                      "itype",
                      "icode",
                      "icmp_id",
                      "icmp_seq",
                      /* "content", */
                      "content-list",
                      "offset",
                      "depth",
                      "session",
                      "rpc",
                      "resp",
                      "react",
                      /* "reference", */
                      /* "sid", */
                      "rev",
                      "classtype",
                      "priority",
                      "uricontent",
                      "tag",
                      "ip_proto",
                      "regex",
                      "flow",
                      "fragoffset"
                      );

        $ruleSingleOptions = array ("nocase",
                                    "sameip",
                                    "stateless",
                                    "rawbytes"
                                   );
?>
