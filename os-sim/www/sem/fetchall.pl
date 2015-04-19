#!/usr/bin/perl
$|=1;
use Time::Local;

if(!$ARGV[6]){
print "Expecting: start_date end_date query start_line num_lines order_by operation cache_file\n";
print "Don't forget to escape the strings\n";
exit;
}


$start = $ARGV[0];
$end = $ARGV[1];
$query = $ARGV[2];
$start_line = $ARGV[3];
$num_lines = $ARGV[4];
$order_by = $ARGV[5];
$operation = $ARGV[6];
$cache_file = $ARGV[7];
$idsesion = $ARGV[8];

$cache_file = "" if ($cache_file !~ "/var/ossim/cache/.*cache.*");

# Possible values for operation: logs or a parameter to group on: date, fdate, src_ip, dst_ip, src_port, dst_port, data

# Possible values for order_by: date, date_desc, none

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


#$grep_str = `perl format_params.pl "$query"`;
#chop($grep_str);
#$grep_str =~ s/^ *| *$//g;


$loc_db = "/var/ossim/logs/locate.index";

$common_date = `perl return_sub_dates_locate.pl \"$start\" \"$end\"`;
#print "perl return_sub_dates.pl $start $end`;
chop($common_date);


if (!$cache_file) {
	#$swish = "for i in `locate -d $loc_db $common_date | grep \".log\$\"`; do cat \$i; done";
	$sort = ($order_by eq "date") ? "sort" : "sort -r";
	$swish = "locate -d $loc_db $common_date | grep \".log\$\" | $sort";
} else {
	$swish = "echo $cache_file";
}


############
###### Start stuff
############

if($operation eq "logs") {
	# Call swish-e for a list of the files
	# cat the files
	# grep them
	# filter on epoch
	# order them

	# debug, missing swish and part
	$cmd = "$swish | perl filter_range_and_sort.pl $start_epoch $end_epoch $start_line $num_lines \"$query\" $order_by $idsesion";
	print "$cmd\n" if ($idsesion eq "debug");
	system($cmd);
} else {
	$cmd = "$swish | perl extract_stats.pl $operation $start_epoch $end_epoch $idsesion";
	print "$cmd\n" if ($idsesion eq "debug");
	system($cmd);
}
