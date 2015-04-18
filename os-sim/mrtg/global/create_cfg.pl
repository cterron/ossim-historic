#!/usr/bin/perl

use strict;
use warnings;

use ossim_conf;

my $OUTPUT_FILE = "global_qualification.cfg";
open CFG, ">$OUTPUT_FILE" or die "Can't open file: $!";

print CFG <<"EOF";

Target[global]: `$ossim_conf::ossim_data->{data_dir}/mrtg/global/read_data.pl`
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

