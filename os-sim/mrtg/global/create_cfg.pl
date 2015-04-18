#!/usr/bin/perl

use strict;
use warnings;

use DBI;
use ossim_conf;

my $OUTPUT_FILE = "global_qualification.cfg";
open CFG, ">$OUTPUT_FILE" or die "Can't open file: $!";

my $dsn = 'dbi:mysql:'.$ossim_conf::base.':'.$ossim_conf::host.':'.
            $ossim_conf::port;
my $dbh = DBI->connect($dsn, $ossim_conf::user, $ossim_conf::pass) 
    or die "Can't connect to DBI\n";


print CFG <<"EOF";

Target[global]: `$ossim_conf::base_dir/mrtg/global/read_data.pl`
Title[global]: OSSIM Level graphics
Background[global]: #ffffff
PageTop[global]: <H1>Level for global</H1>
PageFoot[global]: Test Pie
WithPeak[global]: wmy
Directory[global]: global_qualification
MaxBytes[global]: 50000
AbsMax[global]: 1000000
YLegend[global]: Level
ShortLegend[global]: &nbsp; level &nbsp; &nbsp;
Legend1[global]: Average Compromise level
Legend2[global]: Average Attack level
Legend3[global]: Maximum Compromise level
Legend4[global]: Maximum Attack level
LegendI[global]: Compromise level:
LegendO[global]: Attack level:

EOF

close(CFG);

