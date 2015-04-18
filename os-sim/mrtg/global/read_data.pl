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

my $dsn = "dbi:mysql:".$ossim_conf::ossim_data->{"ossim_base"}.":".$ossim_conf::ossim_data->{"ossim_host"}.":".$ossim_conf::ossim_data->{"ossim_port"};
my $dbh = DBI->connect($dsn, $ossim_conf::ossim_data->{"ossim_user"}, $ossim_conf::ossim_data->{"ossim_pass"})
    or die "Can't connect to DBI\n";

sub isIpInNet {

    my ($ip, $nets) = @_;

    my @net_list = split(",", $nets);
    foreach my $n (@net_list)
    {
        my ($net, $mask) = split("/", $n);

        my @a = split(/\./, $ip);
        my $val1 = $a[0]*256*256*256 + $a[1]*256*256 + $a[2]*256 + $a[3];

        @a = split(/\./, $net);
        my $val2 = $a[0]*256*256*256 + $a[1]*256*256 + $a[2]*256 + $a[3];

        if (($val1 >> (32 - $mask)) == ($val2 >> (32 - $mask))) {
            return 1;
        }
    }
    return 0;
}

sub get_qualification {

    my $nets = shift;

    my $compromise = 1;
    my $attack = 1;

    my $query = "SELECT * FROM host_qualification;";
    my $sth = $dbh->prepare($query);
    $sth->execute();
    if ($sth->rows > 0) {
        while (my $row = $sth->fetchrow_hashref)
        {
            my $ip = $row->{host_ip};
            if ((isIpInNet($ip, $nets)) or (!$nets)) {
                $compromise += $row->{"compromise"};
                $attack += $row->{"attack"};
            }
        }
    }
    my @info = ($compromise, $attack);
    return \@info;
}



my $compromise = 1;
my $attack = 1;
my $info;

my $query = "SELECT * FROM users WHERE login = '$user';";
my $sth = $dbh->prepare($query);
$sth->execute();
if ($sth->rows > 0) {
    my $row = $sth->fetchrow_hashref;
    my $nets = $row->{"allowed_nets"};
    my $info = get_qualification("$nets");
    ($compromise, $attack) = (${$info}[0], ${$info}[1]);
}

if($compromise < 1){ $compromise = 1};
if($attack < 1){ $attack = 1};

print "$compromise\n$attack\n0\n";
print "Stats from global\n\n";

exit 0;

