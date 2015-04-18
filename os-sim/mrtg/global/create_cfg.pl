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

my $OUTPUT_FILE = "global_qualification.cfg";
open CFG, ">$OUTPUT_FILE" or die "Can't open file: $!";

my $query = "SELECT * FROM users;";
my $sth = $dbh->prepare($query);
$sth->execute();
if ($sth->rows > 0) {
    while (my $row = $sth->fetchrow_hashref)
    {
        my $user = $row->{"login"};

print CFG <<"EOF";

Target[global_$user]: `$ossim_conf::ossim_data->{data_dir}/mrtg/global/read_data.pl "$user"`
Title[global_$user]: OSSIM Level graphics
Background[global_$user]: #ffffff
PageTop[global_$user]: <H1>Level for global</H1>
PageFoot[global_$user]: Test Pie
WithPeak[global_$user]: wmy
Directory[global_$user]: global_qualification
MaxBytes[global_$user]: 50000
AbsMax[global_$user]: 1000000
YLegend[global_$user]: Level
ShortLegend[global_$user]: &nbsp; level &nbsp; &nbsp;
Legend1[global_$user]: Average Compromise level
Legend2[global_$user]: Average Attack level
Legend3[global_$user]: Maximum Compromise level
Legend4[global_$user]: Maximum Attack level
LegendI[global_$user]: Compromise level:
LegendO[global_$user]: Attack level:

EOF
    }
}
close(CFG);

