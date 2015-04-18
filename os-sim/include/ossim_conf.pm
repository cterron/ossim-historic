package ossim_conf;
use strict;

BEGIN {

local %ossim_conf::ossim_data;
    #
    # Read config from /etc/ossim.conf
    #
    open FILE, "/etc/ossim.conf" or die "Can't open logfile:  $!";
        while ($_ = <FILE>) {
            if(!(/^#/)){
                if(/^(.*)=(.*)$/){
                $ossim_conf::ossim_data->{$1} = $2;
                }
            }
        }
    close(FILE);
}
1;
