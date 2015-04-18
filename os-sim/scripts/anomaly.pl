#!/usr/bin/perl
# RRD file anomaly detection, alpha alpha

use ossim_conf;
use strict;
use warnings;

my $rrdtool = "$ossim_conf::ossim_data->{\"rrdtool_path\"}/rrdtool";

sub usage {
    print("Usage: $0 file range\n");
    exit 1;
}

if (!$ARGV[1]) {
    usage();
}


my $file = $ARGV[0];
my $range = $ARGV[1];

print "$file\n";

open(INPUT,"$rrdtool fetch $file FAILURES -s N-$range -e N|") or die "Can't execute.."; 

while(<INPUT>){
#print;
if(/(^\d+):.*1\.0000000000e\+00.*/){
my $temp = $1;
$temp += 7200;
my $result = gmtime($temp);
print "Anomaly at $result \n";
$temp = $1;
my $temp2 = $1-1;
open(INPUT2,"$rrdtool fetch $file AVERAGE -s $temp2 -e $temp|") or die "Can't execute..";
while(<INPUT2>){
if(/^\d+:\s+(.*)\s+.*/){
#printf ("%.2f\n",$1);
}}
close(INPUT2);

}}

close(INPUT);
exit 0;
