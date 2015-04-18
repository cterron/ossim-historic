package ossim_conf;
use strict;

BEGIN {

    #
    # Read config from /etc/ossim.conf
    #
    open FILE, "/etc/ossim.conf" or die "Can't open logfile:  $!";
    while ($_ = <FILE>) {
        if (/^rrdpath_host=(.*)/)     { $ossim_conf::rrdpath_host = $1; }
        if (/^rrdpath_net=(.*)/)      { $ossim_conf::rrdpath_net = $1; }
        if (/^rrdpath_global=(.*)/)   { $ossim_conf::rrdpath_global = $1; }
        if (/^rrdtool_lib_path=(.*)/) { $ossim_conf::rrdtool_lib_path = $1; }
        if (/^rrdtool_path=(.*)/)     { $ossim_conf::rrdtool_path = $1; }
        if (/^mrtg_link=(.*)/)        { $ossim_conf::mrtg_link = $1; }
        if (/^graph_link=(.*)/)       { $ossim_conf::graph_link = $1; }
        if (/^out_path=(.*)/)         { $ossim_conf::out_path= $1; }
        if (/^out_web_path=(.*)/)     { $ossim_conf::out_web_path= $1; }
        if (/^arial_path=(.*)/)       { $ossim_conf::arial_path = $1; }
        if (/^ossim_base=(.*)/)       { $ossim_conf::base = $1; }
        if (/^ossim_user=(.*)/)       { $ossim_conf::user = $1; }
        if (/^ossim_pass=(.*)/)       { $ossim_conf::pass = $1; }
        if (/^ossim_host=(.*)/)       { $ossim_conf::host = $1; }
        if (/^base_dir=(.*)/)         { $ossim_conf::base_dir = $1; }
        if (/^mrtg_rrd_files_path=(.*)/) 
                                      { $ossim_conf::mrtg_rrd_files_path = $1;}
    }
    close(FILE);
    $ossim_conf::port = "3306";             # mysql port

}

1;

