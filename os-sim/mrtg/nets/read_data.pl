#!/usr/bin/perl

use strict;
use warnings;

use DBI;
use ossim_conf;

my $net_name = "";
if (!$ARGV[0]) {
    print("Usage: ./read-data.pl <net_name>\n");
    exit 1;
} else {
    $net_name = $ARGV[0];
}


my $dsn = 'dbi:mysql:'.$ossim_conf::base.':'.$ossim_conf::host.':'.
    $ossim_conf::port;
my $dbh = DBI->connect($dsn, $ossim_conf::user, $ossim_conf::pass) or die "Can't connecto to DBI\n";

my $query = "SELECT * FROM net_qualification where net_name = '$net_name';";
my $sth = $dbh->prepare($query);
$sth->execute();
if ($sth->rows > 0) {
    my $row = $sth->fetchrow_hashref;
    my $compromise = $row->{compromise}; 
    my $attack = $row->{attack};
    
    print "$compromise\n$attack\n\n";
    print "Stats from $net_name\n\n";
} else {
    print "0\n0\n0\nNo current stats available\n\n";
}

exit 0;

