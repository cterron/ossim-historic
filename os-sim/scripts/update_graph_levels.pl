#!/usr/bin/perl

use strict;
use warnings;

use DBI;
use ossim_conf;

$| = 1;

my $dsn = "dbi:mysql:".$ossim_conf::ossim_data->{"ossim_base"}.":".$ossim_conf::ossim_data->{"ossim_host"}.":".$ossim_conf::ossim_data->{"ossim_port"};
my $dbh = DBI->connect($dsn, $ossim_conf::ossim_data->{"ossim_user"}, $ossim_conf::ossim_data->{"ossim_pass"})
    or die "Can't connect to DBI\n";

my $SLEEP = 15;

while(1) {

    # delete graph statics table
    # 
    my $query = "DELETE FROM graph_qualification;";
    my $sth = $dbh->prepare($query);
    $sth->execute();

    # get a list of graph's ids
    # 
    $query = "SELECT DISTINCT id FROM graph;";
    $sth = $dbh->prepare($query);
    $sth->execute();
    while (my $row = $sth->fetchrow_hashref) {

        my $id = $row->{id};
        my $compromise = 0;
        my $attack = 0;
 
        my $query = "SELECT sum(e.compromise), sum(e.attack) 
            FROM graph g, host_qualification e 
            WHERE g.id = $id and g.ip = e.host_ip;";
        my $sth = $dbh->prepare($query);
        $sth->execute();
        my $row2 = $sth->fetchrow_hashref;
        if ($sth->rows > 0) {

            my $query = "select count(*) from graph g, host_qualification e 
                where g.id = $id and g.ip = e.host_ip;";
            my $sth = $dbh->prepare($query);
            $sth->execute();
            my $row3 = $sth->fetchrow_hashref;

            my $hosts = $row3->{"count(*)"};
            if ($hosts == 0) {
                $compromise = $attack = 0
            } else {
                $compromise = int($row2->{"sum(e.compromise)"} / $hosts);
                $attack = int($row2->{"sum(e.attack)"} / $hosts);
            }
        
            if (($compromise == 0) and ($attack == 0)) {
                $query = "DELETE FROM graph_qualification 
                          WHERE graph_id = $id;";
                my $sth = $dbh->prepare($query);
                $sth->execute();
            } else {
                $query = "INSERT INTO graph_qualification 
                    VALUES ($id, $compromise, $attack);";
                my $sth = $dbh->prepare($query);
                $sth->execute();
            }
        }
        $compromise = 0;
        $attack = 0;
    }
    sleep($SLEEP);
}

$dbh->disconnect;
exit 0;

