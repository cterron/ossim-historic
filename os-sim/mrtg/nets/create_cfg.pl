#!/usr/bin/perl

use strict;
use warnings;

use DBI;
use ossim_conf;

my $OUTPUT_FILE = "net_qualification.cfg";
open CFG, ">$OUTPUT_FILE" or die "Can't open file: $!";

my $dsn = 'dbi:mysql:'.$ossim_conf::base.':'.$ossim_conf::host.':'.
            $ossim_conf::port;
my $dbh = DBI->connect($dsn, $ossim_conf::user, $ossim_conf::pass) 
    or die "Can't connect to DBI\n";

my $query = "SELECT net_name FROM net_qualification;";
my $sth = $dbh->prepare($query);
$sth->execute();
if ($sth->rows > 0) {
    while (my $row = $sth->fetchrow_hashref)
    {
        my $net_name = $row->{net_name};

        print CFG <<"EOF";
Target[$net_name]: `$ossim_conf::base_dir/mrtg/nets/read_data.pl $net_name`
Title[$net_name]: OSSIM Level graphics
Background[$net_name]: #ffffff
PageTop[$net_name]: <H1>Level for $net_name</H1>
WithPeak[$net_name]: wmy
Directory[$net_name]: net_qualification
MaxBytes[$net_name]: 50000
AbsMax[$net_name]: 1000000
YLegend[$net_name]: Level
ShortLegend[$net_name]: &nbsp; level &nbsp; &nbsp;
Legend1[$net_name]: Average Compromise level
Legend2[$net_name]: Average Attack level
Legend3[$net_name]: Maximum Compromise level
Legend4[$net_name]: Maximum Attack level
LegendI[$net_name]: Compromise level:
LegendO[$net_name]: Attack level:

EOF
    }
}

close(CFG);

