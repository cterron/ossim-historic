#!/usr/bin/perl

use strict;
use warnings;

use DBI;
use ossim_conf;

$| = 1;

my $dsn = "dbi:mysql:".$ossim_conf::ossim_data->{"ossim_base"}.":".$ossim_conf::ossim_data->{"ossim_host"}.":".$ossim_conf::ossim_data->{"ossim_port"};
my $dbh = DBI->connect($dsn, $ossim_conf::ossim_data->{"ossim_user"}, $ossim_conf::ossim_data->{"ossim_pass"})
    or die "Can't connect to DBI\n";

my $UPDATE_INTERVAL = $ossim_conf::ossim_data->{"UPDATE_INTERVAL"};
my $SLEEP = $UPDATE_INTERVAL * 10;

sub update_net_levels {

    my $compromise = 0;
    my $attack = 0;
    my $rows = 0;

    my $query = "SELECT * FROM net;";
    my $sth = $dbh->prepare($query);
    $sth->execute();
    while (my $row = $sth->fetchrow_hashref) 
    {
        my $red = $row->{name};
        
        my $query = "SELECT * FROM net_host_reference WHERE net_name = '$red'";
        my $sth = $dbh->prepare($query);
        $sth->execute();
        while (my $row2 = $sth->fetchrow_hashref) {
            my $host_ip = $row2->{host_ip};
            my $query = "SELECT * FROM host_qualification 
                            WHERE host_ip = '$host_ip'";
            my $sth = $dbh->prepare($query);
            $sth->execute();
            if (my $row3 = $sth->fetchrow_hashref) {
                $compromise += $row3->{compromise};
                $attack += $row3->{attack};
                $rows += 1;
            }
        }
        if ($rows) {
#            $compromise /= $rows;
            my $query = "UPDATE net_qualification SET compromise = $compromise WHERE net_name = '$red'";
            my $sth = $dbh->prepare($query);
            $sth->execute();

#            $attack /= $rows;
            $query = "UPDATE net_qualification SET  attack = $attack WHERE net_name = '$red'";
            $sth = $dbh->prepare($query);
            $sth->execute();
        }

        # next net...
        $compromise = $attack = $rows = 0;
    }
}

while (1) {

    update_net_levels();
    sleep($SLEEP);
}

$dbh->disconnect;
exit 0;

