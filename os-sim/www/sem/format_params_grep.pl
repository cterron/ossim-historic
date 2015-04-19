#!/usr/bin/perl

sub complex(){

$a = shift;

if($a =~ /^\s*(.*)!=(.*)$/){
return " grep -v -i \"$1='$2'\" ";
} elsif ($a =~ /^\s*(.*)=(.*)$/){
return " grep -i \"$1='$2'\" ";
}

}

if(!$ARGV[0]){
exit;
}

$grep_str = "";

@args = split(/\s+/, $ARGV[0]);

$first = 1;

$negation = 0;

foreach $arg (@args){
	if($arg eq "and") {next;}
	if($arg eq " ") {next;}
	if($arg eq "") {next;}
  
	if($arg =~ /=/){ 
		$ret = &complex($arg);
		$grep_str .= "|" unless $first == 1;
		$first = 0;
    $grep_str .= $ret; 
		next;
	}

	if($arg eq "not"){
		$negation = 1;
	} else {
		$grep_str .= "|" unless $first == 1;
		$first = 0;
		 if($negation){
			 $grep_str .= " grep -i -v '$arg' ";
		 } else {
			 $grep_str .= " grep -i '$arg' ";
		 }
		$negation = 0;
	}
}

$grep_str .= "\n";

print $grep_str;
