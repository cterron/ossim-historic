#!/usr/bin/perl

use strict;

use DBI;
use ossim_conf;

my $ip = "";
if (!$ARGV[0]) {
    print("Usage: ./read-data.pl <ip>\n");
    exit 1;
} else {
    $ip = $ARGV[0];
}



my $dsn = 'dbi:mysql:'.$ossim_conf::base.':'.$ossim_conf::host.':'.
            $ossim_conf::port;
my $dbh = DBI->connect($dsn, $ossim_conf::user, $ossim_conf::pass) 
    or die "Can't connecto to DBI\n";

my $query = "SELECT * FROM host_qualification where host_ip = '$ip';";
my $sth = $dbh->prepare($query);
$sth->execute();
if ($sth->rows > 0) {
    my $row = $sth->fetchrow_hashref;
    my $compromise = $row->{compromise}; 
    my $attack = $row->{attack};
    print "$compromise\n$attack\n0\n";
    print "Stats from $ip\n\n";
} else {
    print "0\n0\n0\nNo current stats available\n\n";
}

exit 0;

