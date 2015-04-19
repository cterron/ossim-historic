#!/usr/bin/perl

#
# Execute this script to build plugins python code 
# reading from the database
#

use strict;
use warnings;

use DBI;

$| = 1;

#### Configuration ####
my $base = 'ossim';
my $host = 'localhost';
my $user = 'root';
my $pass = '';
my $port = 3306;
#### End configuration ####

my $OUTPUT = 'plugins.py';

#
#  Database connection
#
my $dsn = 'dbi:mysql:' . $base . ':' . $host. ':' .  $port;
my $dbh = DBI->connect($dsn, $user, $pass) or die "Can't connect to DBI\n";

#
# Output code
#
open FILE, ">$OUTPUT" || die "Error opening $OUTPUT\n";

#
# fetch plugins sid info and write it 
# into $OUTPUT file in a python hash
# 
my $query = "SELECT DISTINCT plugin_id FROM plugin_sid;";
my $sth = $dbh->prepare($query);
$sth->execute();

print FILE "plugins = {\n";
while (my $row = $sth->fetchrow_hashref) {
    my $plugin_id = $row->{plugin_id};
    print FILE "\t'$plugin_id':\t{\n";
    
    $query = "SELECT * FROM plugin_sid WHERE plugin_id = $plugin_id";
    my $sth2 = $dbh->prepare($query);
    $sth2->execute();
    while ($row = $sth2->fetchrow_hashref) {
        my $sid  = $row->{sid};
        my $name = $row->{name};
        print FILE "\t\t'$sid':\t'$name',\n";
    }
    print FILE "\t},\n"
}
print FILE "}\n";

#
# finished
# 
close FILE;
$dbh->disconnect;
exit 0;

