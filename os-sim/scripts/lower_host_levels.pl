#!/usr/bin/perl

use ossim_conf;
use DBI;
use strict;
use warnings;

$| = 1;

my $dsn = "dbi:mysql:".$ossim_conf::ossim_data->{"ossim_base"}.":".$ossim_conf::ossim_data->{"ossim_host"}.":".$ossim_conf::ossim_data->{"ossim_port"};
my $dbh = DBI->connect($dsn, $ossim_conf::ossim_data->{"ossim_user"}, $ossim_conf::ossim_data->{"ossim_pass"})
    or die "Can't connect to DBI\n";

my $UPDATE_INTERVAL = $ossim_conf::ossim_data->{"UPDATE_INTERVAL"};
my $SLEEP = $UPDATE_INTERVAL * 5;

while(1) {

    # Get recovery level
    # 
    my $query = "SELECT * FROM conf;";
    my $sth = $dbh->prepare($query);
    $sth->execute();
    my $row = $sth->fetchrow_hashref;
    my $recovery = $row->{recovery};
    

    $query = "SELECT * FROM host_qualification;";
    $sth = $dbh->prepare($query);
    $sth->execute();

    while ($row = $sth->fetchrow_hashref) {

        my $compromise = $row->{compromise};
        my $attack = $row->{attack};
        my $host_ip = $row->{host_ip};
        my $query = "";

        # compromise
        # 
        if ($compromise > $recovery) {
            $query = "UPDATE host_qualification SET compromise = 
                        compromise - $recovery 
                        WHERE host_ip = '$host_ip'";
        } else {
            $query = "UPDATE host_qualification SET compromise = 1 
                        WHERE host_ip = '$host_ip'";
        }
        my $sth = $dbh->prepare($query);
        $sth->execute();

        # attack
        # 
        if ($attack > $recovery) {
            $query = "UPDATE host_qualification SET attack = 
                        attack - $recovery WHERE host_ip = '$host_ip'";
        } else {
            $query = "UPDATE host_qualification SET attack = 1 
                        WHERE host_ip = '$host_ip'";
        }
        $sth = $dbh->prepare($query);
        $sth->execute();
    
        # Remove inactive ips
        # 
        if (($compromise <= $recovery) and ($attack <= $recovery)) {
            my $query = "DELETE FROM host_qualification 
                            WHERE host_ip = '$host_ip'";
            my $sth = $dbh->prepare($query);
            $sth->execute();
        }
    }

    sleep($SLEEP);
}

$dbh->disconnect;
exit 0;

