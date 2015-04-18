#!/usr/bin/perl

use ossim_conf;
use DBI;
use strict;
use warnings;

$| = 1;


my $dsn = "dbi:mysql:".$ossim_conf::base.":".$ossim_conf::host.":".$ossim_conf::port;
my $dbh = DBI->connect($dsn, $ossim_conf::user, $ossim_conf::pass) 
    or die "Can't connect to DBI\n";

my $SLEEP = 30;

while(1) {

    # Get recovery level
    # 
    my $query = "SELECT * FROM conf;";
    my $sth = $dbh->prepare($query);
    $sth->execute();
    my $row = $sth->fetchrow_hashref;
    my $recovery = $row->{recovery};
    

    $query = "SELECT * FROM net_qualification;";
    $sth = $dbh->prepare($query);
    $sth->execute();

    while ($row = $sth->fetchrow_hashref) {

        my $compromise = $row->{compromise};
        my $attack = $row->{attack};
        my $net_name = $row->{net_name};
        my $query = "";

        # compromise
        # 
        if ($compromise > $recovery) {
            $query = "UPDATE net_qualification SET compromise = 
                        compromise - $recovery 
                        WHERE net_name = '$net_name'";
        } else {
            $query = "UPDATE net_qualification SET compromise = 1 
                        WHERE net_name = '$net_name'";
        }
        my $sth = $dbh->prepare($query);
        $sth->execute();

        # attack
        # 
        if ($attack > $recovery) {
            $query = "UPDATE net_qualification SET attack = 
                        attack - $recovery WHERE net_name = '$net_name'";
        } else {
            $query = "UPDATE net_qualification SET attack = 1 
                        WHERE net_name = '$net_name'";
        }
        $sth = $dbh->prepare($query);
        $sth->execute();
    
    }

    sleep($SLEEP);
}

$dbh->disconnect;
exit 0;

