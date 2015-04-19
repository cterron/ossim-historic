#!/usr/bin/perl

use strict;

use DBI;
use ossim_conf;


my $user = "";
if (!$ARGV[0]) {
    print("Usage: ./read-data.pl <user>\n");
    exit 1;
} else {
    $user = $ARGV[0];
}

#
# database connect
#
my $dsn = "dbi:mysql:" . 
    $ossim_conf::ossim_data->{"ossim_base"} . ":" . 
    $ossim_conf::ossim_data->{"ossim_host"} . ":" . 
    $ossim_conf::ossim_data->{"ossim_port"};

my $dbh = DBI->connect($dsn, 
                       $ossim_conf::ossim_data->{"ossim_user"}, 
                       $ossim_conf::ossim_data->{"ossim_pass"})
    or die "Can't connect to DBI\n";


#
# rrd paths
#
my $rrdtool = "$ossim_conf::ossim_data->{\"rrdtool_path\"}/rrdtool";
my $rrdpath = $ossim_conf::ossim_data->{rrdpath_global};


#
# get default threshold
# 
my $THRESHOLD = $ossim_conf::ossim_data->{"threshold"};

my $C_level = my $A_level = my $count = 0;
open(INPUT, "$rrdtool fetch $rrdpath/global_$user.rrd AVERAGE -s N-1D -e N|");
while (<INPUT>)
{
     if(/(\d+):\s+(\S+)\s+(\S+)/)
     {
        my $date = $1;
        my $C = $2;
        my $A = $3;
        
        if (($C ne "nan") and ($A ne "nan"))
        {
            if ($C <= $THRESHOLD) {
                $C_level += 1;
            }
            if ($A <= $THRESHOLD) {
                $A_level += 1;
            }
            $count += 1;
        }
     }
}

my $compromise = my $attack = 0;

if ($count != 0) {
    if ($C_level != 0) { $compromise = ($C_level * 100) / $count; }
    if ($A_level != 0) { $attack     = ($A_level * 100) / $count; }
}

print "$compromise\n$attack\n0\n";
print "Stats from level\n\n";

exit 0;

