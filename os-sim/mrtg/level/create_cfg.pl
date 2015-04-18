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

my $OUTPUT_FILE = "level_qualification.cfg";
open CFG, ">$OUTPUT_FILE" or die "Can't open file: $!";

my $query = "SELECT * FROM users;";
my $sth = $dbh->prepare($query);
$sth->execute();
if ($sth->rows > 0) {
    while (my $row = $sth->fetchrow_hashref)
    {
        my $user = $row->{"login"};

print CFG <<"EOF";

Target[level_$user]: `$ossim_conf::ossim_data->{data_dir}/mrtg/level/read_data.pl "$user"`
Title[level_$user]: OSSIM Security Level graphics
Background[level_$user]: #ffffff
PageTop[level_$user]: <H1>Security level</H1>
PageFoot[level_$user]: Test Pie
WithPeak[level_$user]: wmy
Directory[level_$user]: level_qualification
MaxBytes[level_$user]: 50000
AbsMax[level_$user]: 1000000
YLegend[level_$user]: Level
ShortLegend[level_$user]: &nbsp; level &nbsp; &nbsp;
Legend1[level_$user]: Average Compromise level
Legend2[level_$user]: Average Attack level
Legend3[level_$user]: Maximum Compromise level
Legend4[level_$user]: Maximum Attack level
LegendI[level_$user]: Compromise level:
LegendO[level_$user]: Attack level:

EOF
    }
}
close(CFG);

