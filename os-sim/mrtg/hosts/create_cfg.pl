#!/usr/bin/perl

use strict;
use warnings;

use DBI;
use ossim_conf;

my $dsn = "dbi:mysql:" . 
    $ossim_conf::ossim_data->{"ossim_base"}. ":" . 
    $ossim_conf::ossim_data->{"ossim_host"}. ":".
    $ossim_conf::ossim_data->{"ossim_port"};

my $dbh = DBI->connect($dsn, 
                       $ossim_conf::ossim_data->{"ossim_user"}, 
                       $ossim_conf::ossim_data->{"ossim_pass"})
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

sub in_net {

    my $host_ip = shift;

    my $query = "SELECT * FROM net;";
    my $sth = $dbh->prepare($query);
    $sth->execute();
    if ($sth->rows > 0) {
        while (my $row = $sth->fetchrow_hashref) {
            my $net_ips = $row->{"ips"};
            if (isIpInNet($host_ip, $net_ips)) {
                return 1;
            }
        }
    }
    return 0;
}

sub in_host {

    my $host_ip = shift;
    my $query = "SELECT * FROM host WHERE ip = '$host_ip'";
    my $sth = $dbh->prepare($query);
    $sth->execute();
    return ($sth->rows > 0);
}


my $OUTPUT_FILE = "host_qualification.cfg";
open CFG, ">$OUTPUT_FILE" or die "Can't open file: $!";

my $query = "SELECT * FROM host_qualification hq;";
my $sth = $dbh->prepare($query);
$sth->execute();
if ($sth->rows > 0) {
    while (my $row = $sth->fetchrow_hashref)
    {
        my $host_ip = $row->{"host_ip"};

        if (in_net($host_ip) or in_host($host_ip)) {
        

            print CFG <<"EOF";

Target[$host_ip]: `$ossim_conf::ossim_data->{data_dir}/mrtg/hosts/read_data.pl "$host_ip"`
Title[$host_ip]: OSSIM Level graphics
Background[$host_ip]: #ffffff
PageTop[$host_ip]: <H1>Level for $host_ip</H1>
PageFoot[$host_ip]: Test Pie
WithPeak[$host_ip]: wmy
Directory[$host_ip]: host_qualification
MaxBytes[$host_ip]: 50000
AbsMax[$host_ip]: 1000000
YLegend[$host_ip]: Level
ShortLegend[$host_ip]: &nbsp; level &nbsp; &nbsp;
Legend1[$host_ip]: Average Compromise level
Legend2[$host_ip]: Average Attack level
Legend3[$host_ip]: Maximum Compromise level
Legend4[$host_ip]: Maximum Attack level
LegendI[$host_ip]: Compromise level:
LegendO[$host_ip]: Attack level:

EOF
        }
    }
}

close(CFG);

