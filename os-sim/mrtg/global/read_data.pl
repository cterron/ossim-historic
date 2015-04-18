#!/usr/bin/perl

use strict;

use DBI;
use ossim_conf;


my $dsn = 'dbi:mysql:'.$ossim_conf::base.':'.$ossim_conf::host.':'.
            $ossim_conf::port;
my $dbh = DBI->connect($dsn, $ossim_conf::user, $ossim_conf::pass) 
    or die "Can't connecto to DBI\n";


my $compromise = 1;
my $attack = 1;

my $query = "SELECT sum(compromise) FROM host_qualification;";
my $sth = $dbh->prepare($query);
$sth->execute();
if ($sth->rows > 0) {
    my $row = $sth->fetchrow_hashref;
    $compromise = $row->{"sum(compromise)"}; 
}

$query = "SELECT sum(attack) FROM host_qualification;";
$sth = $dbh->prepare($query);
$sth->execute();
if ($sth->rows > 0) {
    my $row = $sth->fetchrow_hashref;
    $attack = $row->{"sum(attack)"};
}

print "$compromise\n$attack\n0\n";
print "Stats from global\n\n";

exit 0;

