#!/usr/bin/perl
$ret = `ps ax|grep -v grep|awk '\$2 !~ /'\$\$'/{print}' | grep -v nocount|grep generate_stats.pl|wc -l`;
$ret =~ s/\s*//g;
exit(0) if ($ret>=1);

if(!$ARGV[0]){
print "Accepts folder with *log files\n";
exit;
}
$debug = 1; # 1 for debuging info
$folder = $ARGV[0];
$folder =~ s/\/$//;
$qfolder = quotemeta $folder;
$force = ($ARGV[1] eq "force") ? 1 : 0;
$param = ($ARGV[1] ne "") ? $ARGV[1] : "nocount";
$wehavedata = 0;

%stats = ();
%already = ();
open (F,"find $folder | grep \".log\$\" |");
LOG: while ($file=<F>) {
	chomp($file);
	my $dir = $file; $dir =~ s/$qfolder//;
	my @fields = split(/\//,$dir);
	if ($fields[1] =~ /(^\d+$)/) { # not an hour directory, recurse inside
		my $val = $1;
		if ($val>200) { # root log directory
			my $subfolder = $folder."/".$val."/".$fields[2]."/".$fields[3]."/".$fields[4];
			if (!$already{$subfolder}++) {
				print "Recursive into $subfolder\n" if ($debug);
				system ("perl generate_stats.pl \"$subfolder\" $param");
			}
		}
		elsif ($val>=1 && $val<=12) { # year log directory
			my $subfolder = $folder."/".$val."/".$fields[2]."/".$fields[3];
			if (!$already{$subfolder}++) {
				print "Recursive into $subfolder\n" if ($debug);
				system ("perl generate_stats.pl \"$subfolder\" $param");
			}
		}
		elsif ($val>=1 && $val<=31 && $fields[2] =~ /^\d+$/) { # month log directory
			my $subfolder = $folder."/".$val."/".$fields[2];
			if (!$already{$subfolder}++) {
				print "Recursive into $subfolder\n" if ($debug);
				system ("perl generate_stats.pl \"$subfolder\" $param");
			}
		}
		else { # day directory 
			my $subfolder = $folder."/".$val;
			if (!$already{$subfolder}++) {
				print "Recursive into $subfolder\n" if ($debug);
				system ("perl generate_stats.pl \"$subfolder\" $param");
			}
		}
	} else {
		print "Reading $file\n" if ($debug);
		my $filet = $file; $filet =~ s/log$/ind/;
		if (-e $filet && !$force) {
			print "Skipping $file. Already exists\n";
			next LOG;
		}
		%rangos = ();
		$lasttime = -1;
		$lastdate = 4102444800;
		$line = 0;
		open (G,"tac '$file' |");
		while (<G>) {
			chomp;
			#if (/ id='(\d+)' .* date='(\d+)' plugin_id='([^']+)' sensor='([^']+)' src_ip='([^']+)' dst_ip='([^']+)' src_port='([^']+)' dst_port='([^']+)' tzone='([^']+)' data='([^']+)'/) {
			if (/ id='(\d+)' .* date='(\d+)' plugin_id='([^']+)' sensor='([^']+)' src_ip='([^']+)' dst_ip='([^']+)' src_port='([^']+)' dst_port='([^']+)'/) {
				$line++; $id = $1; $fecha = $2;
				my @timeData = localtime($fecha);
				if ($timeData[1] != $lasttime && $fecha<$lastdate) {
					$lasttime = $timeData[1];
					$lastdate = $fecha;
					$rangos{$fecha} = "$line:$id";
				}
				$stats{"plugin_id"}{$3}++;
				$stats{"sensor"}{$4}++;
				$stats{"src_ip"}{$5}++;
				$stats{"dst_ip"}{$6}++;
				$stats{"src_port"}{$7}++;
				$stats{"dst_port"}{$8}++;
				#$stats{"time_zone"}{$9}++;
				#$stats{"data"}{$10}++;
				$wehavedata++;
			}
		}
		close G;
		print "\tGenerate Index $filet\n" if ($debug);
		open (S,">$filet");
		foreach $date (sort {$b<=>$a} keys (%rangos)) {
			print S "$date:$rangos{$date}\n";
		}
		print S "lines:$line\n";
		close S;
	}
}
close F;

# sort stats
@arr = ("plugin_id","sensor","src_ip","dst_ip","src_port","dst_port");
if ($wehavedata>0) {
	print "Writing $folder/data.stats\n" if ($debug);
	open(F,">$folder/data.stats");
	foreach $type (@arr) {
		foreach $value (sort {$stats{$type}{$b}<=>$stats{$type}{$a}} keys (%{$stats{$type}})) {
			print F $type.":".$value.":".$stats{$type}{$value}."\n";
		}
	}
	close F;
}
print "Done.\n" if ($debug);
