#!/usr/bin/perl
use Time::Local;
use File::Basename; 

if(!$ARGV[1]){
print "Expecting: start_date end_date\n";
print "Don't forget to escape the strings\n";
exit;
}

$debug=0;
$start = $ARGV[0];
$end = $ARGV[1];
$debug = 1 if ($ARGV[2] eq "debug");

############
###### Convert stuff
############

$index_file = "/var/ossim/logs/forensic_storage.index";

if ($start =~ /(\d+)-(\d+)-(\d+)\s+(\d+):(\d+):(\d+)/) {
	$start_epoch = timegm($6, $5, $4, $3, $2-1, $1);
# Temporary fix until server fix
#$start_epoch += 25200;
}
if ($end =~ /(\d+)-(\d+)-(\d+)\s+(\d+):(\d+):(\d+)/) {
	$end_epoch = timegm($6, $5, $4, $3, $2-1, $1);
# Temporary fix until server fix
#$end_epoch += 25200;
}

$loc_db = "/var/ossim/logs/locate.index";

$common_date = `perl return_sub_dates_locate.pl \"$start\" \"$end\"`;
#print "perl return_sub_dates.pl $start $end`;
chop($common_date);

%already = ();
$lines = 0;
$sort = ($order_by eq "date") ? "sort" : "sort -r";
$swish = "locate -d $loc_db $common_date | grep \".log\$\" | $sort |";
open (G,$swish);
while ($file=<G>) {
	chomp($file);
	my @fields = split(/\//,$file);
	my $sdirtime = timegm(0, 0, $fields[7], $fields[6], $fields[5]-1, $fields[4]);
	my $edirtime = timegm(59, 59, $fields[7], $fields[6], $fields[5]-1, $fields[4]);
	if ($edirtime > $start_epoch && $sdirtime < $end_epoch) {
		my $sf = dirname($file)."/../.total_events"; 
		#$sf =~ s/log$/ind/;
		if (!$already{$sf}++) {
			print "Reading $sf\n" if ($debug);
			open (F,$sf);
			while (<F>) {
				#if (/^lines\:(\d+)/) {
				#	$lines += $1;
				#}
				if (/^(\d+)/) { $lines += $1; }
			}
			close F;
		}
	}
}
close G;
print "$lines\n";
