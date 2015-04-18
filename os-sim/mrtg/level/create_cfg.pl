#!/usr/bin/perl

use strict;
use warnings;

use ossim_conf;

my $OUTPUT_FILE = "level_qualification.cfg";
open CFG, ">$OUTPUT_FILE" or die "Can't open file: $!";

print CFG <<"EOF";

Target[level]: `$ossim_conf::ossim_data->{data_dir}/mrtg/level/read_data.pl`
Title[level]: OSSIM Security Level graphics
Background[level]: #ffffff
PageTop[level]: <H1>Security level</H1>
PageFoot[level]: Test Pie
WithPeak[level]: wmy
Directory[level]: level_qualification
MaxBytes[level]: 50000
AbsMax[level]: 1000000
YLegend[level]: Level
ShortLegend[level]: &nbsp; level &nbsp; &nbsp;
Legend1[level]: Average Compromise level
Legend2[level]: Average Attack level
Legend3[level]: Maximum Compromise level
Legend4[level]: Maximum Attack level
LegendI[level]: Compromise level:
LegendO[level]: Attack level:

EOF

close(CFG);

