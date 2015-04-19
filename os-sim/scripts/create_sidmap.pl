#!/usr/bin/perl -w

# $Id: create_sidmap.pl,v 1.14 2008/01/10 16:48:43 dvgil Exp $ #

# Copyright (C) 2004 Andreas Östling <andreaso@it.su.se>

# 2004-05-05 David Gil <dgil@ossim.net>
#  CHANGES: creates a sql file instead of a sid map in order to 
#  update OSSIM database
# 2007-04-19 DK <dk@ossim.net
# Revert that change, write by default and dump on "-d".

use strict;

sub get_next_entry($ $ $ $ $ $);
sub parse_singleline_rule($ $ $);
sub update_ossim_db();


# Regexp to match the start of a multi-line rule.
# %ACTIONS% will be replaced with content of $config{actions} later.
my $MULTILINE_RULE_REGEXP  = '^\s*#*\s*(?:%ACTIONS%)'.
                             '\s.*\\\\\s*\n$'; # ';

# Regexp to match a single-line rule.
my $SINGLELINE_RULE_REGEXP = '^\s*#*\s*(?:%ACTIONS%)'.
                             '\s.+;\s*\)\s*$'; # ';

my $USAGE = << "RTFM";

Parse active rules in *.rules in one or more directories and create a SID 
map. Result is sent to standard output, which can be redirected to a 
sid-msg.map file.

Usage: $0 <rulesdir> [rulesdir2, ...]
    Optional -d dump sql to stdout.
    Optional -q forces quiet operation.

RTFM

my $verbose = 1;

my (%sidmap, %sidinfo, %config);

my @rulesdirs = @ARGV;

die($USAGE) unless ($#rulesdirs > -1);

$config{rule_actions} = "alert|drop|log|pass|reject|sdrop|activate|dynamic";

$SINGLELINE_RULE_REGEXP =~ s/%ACTIONS%/$config{rule_actions}/;
$MULTILINE_RULE_REGEXP  =~ s/%ACTIONS%/$config{rule_actions}/;

# Dump SQL. Default off.
# Be quiet. Default off.
my $dump = 0;
my $quiet = 0;

# Read in all rules from each rules file (*.rules) in each rules dir.
# into %sidmap.
foreach my $rulesdir (@rulesdirs) {
    if($rulesdir =~ /^-d$/){
        $dump= 1;
        next;
    }
    if($rulesdir =~ /^-q$/){
        $quiet = 1;
        next;
    }
    opendir(RULESDIR, "$rulesdir") or die("could not open \"$rulesdir\": $!\n");

    while (my $file = readdir(RULESDIR)) {
        next unless ($file =~ /\.rules$/);

        open(FILE, "$rulesdir/$file") or die("could not open \"$rulesdir/$file\": $!\n");
        my @file = <FILE>;
        close(FILE);

        my ($single, $multi, $nonrule, $msg, $sid);

        while (get_next_entry(\@file, \$single, \$multi, \$nonrule, \$msg, \$sid)) {
            if (defined($single)) {

            # Don't care about inactive rules.
                next if ($single =~ /^\s*#/);

                warn("WARNING: duplicate SID: $sid (discarding old)\n")
                  if (exists($sidmap{$sid}));

                $sidmap{$sid} = "$sid || $msg";

              # Print all references. Borrowed from Brian Caswell's regen-sidmap script.
                my $ref = $single;
                while ($ref =~ s/(.*)reference\s*:\s*([^\;]+)(.*)$/$1 $3/) {
                    $sidmap{$sid} .= " || $2"
                }

                $sidmap{$sid} .= "\n";

                my $ref2 = $single;
                if ($ref2 =~ /\(msg\s*:\s*([^\;]+)(.*)classtype\s*:\s*([^\;]+)/i) {
                    $sidinfo{$sid}{"msg"} = $1;
                    $sidinfo{$sid}{"classtype"} = $3;
                }
                
                my $category = $file;
                $category =~ s/([^\.]+)\.rules/$1/;
                $sidinfo{$sid}{"category"} = $category;
                
            }
        }
    }
}

# Print results.
#foreach my $sid (sort { $a <=> $b } keys(%sidmap)) {
#    print "$sidmap{$sid}";
#}

update_ossim_db();


# Same as in oinkmaster.pl.
sub get_next_entry($ $ $ $ $ $)
{
    my $arr_ref     = shift;
    my $single_ref  = shift;
    my $multi_ref   = shift;
    my $nonrule_ref = shift;
    my $msg_ref     = shift;
    my $sid_ref     = shift;

    undef($$single_ref);
    undef($$multi_ref);
    undef($$nonrule_ref);
    undef($$msg_ref);
    undef($$sid_ref);

    my $line = shift(@$arr_ref) || return(0);
    my $disabled = 0;
    my $broken   = 0;

  # Possible beginning of multi-line rule?
    if ($line =~ /$MULTILINE_RULE_REGEXP/oi) {
        $$single_ref = $line;
        $$multi_ref  = $line;

        $disabled = 1 if ($line =~ /^\s*#/);

      # Keep on reading as long as line ends with "\".
        while (!$broken && $line =~ /\\\s*\n$/) {

          # Remove trailing "\" and newline for single-line version.
            $$single_ref =~ s/\\\s*\n//;

          # If there are no more lines, this can not be a valid multi-line rule.
            if (!($line = shift(@$arr_ref))) {

                warn("\nWARNING: got EOF while parsing multi-line rule: $$multi_ref\n")
                  if ($config{verbose});

                @_ = split(/\n/, $$multi_ref);

                undef($$multi_ref);
                undef($$single_ref);

              # First line of broken multi-line rule will be returned as a non-rule line.
                $$nonrule_ref = shift(@_) . "\n";
                $$nonrule_ref =~ s/\s*\n$/\n/;    # remove trailing whitespaces

              # The rest is put back to the array again.
                foreach $_ (reverse((@_))) {
                    unshift(@$arr_ref, "$_\n");
                }

                return (1);   # return non-rule
            }

          # Multi-line continuation.
            $$multi_ref .= $line;

          # If there are non-comment lines in the middle of a disabled rule,
          # mark the rule as broken to return as non-rule lines.
            if ($line !~ /^\s*#/ && $disabled) {
                $broken = 1;
            } elsif ($line =~ /^\s*#/ && !$disabled) {
                # comment line (with trailing slash) in the middle of an active rule - ignore it
            } else {
                $line =~ s/^\s*#*\s*//;  # remove leading # in single-line version
                $$single_ref .= $line;
            }

        } # while line ends with "\"

      # Single-line version should now be a valid rule.
      # If not, it wasn't a valid multi-line rule after all.
        if (!$broken && parse_singleline_rule($$single_ref, $msg_ref, $sid_ref)) {

            $$single_ref =~ s/^\s*//;     # remove leading whitespaces
            $$single_ref =~ s/^#+\s*/#/;  # remove whitespaces next to leading #
            $$single_ref =~ s/\s*\n$/\n/; # remove trailing whitespaces

            $$multi_ref  =~ s/^\s*//;
            $$multi_ref  =~ s/\s*\n$/\n/;
            $$multi_ref  =~ s/^#+\s*/#/;

            return (1);   # return multi
        } else {
            warn("\nWARNING: invalid multi-line rule: $$single_ref\n")
              if ($config{verbose} && $$multi_ref !~ /^\s*#/);

            @_ = split(/\n/, $$multi_ref);

            undef($$multi_ref);
            undef($$single_ref);

          # First line of broken multi-line rule will be returned as a non-rule line.
            $$nonrule_ref = shift(@_) . "\n";
            $$nonrule_ref =~ s/\s*\n$/\n/;   # remove trailing whitespaces

          # The rest is put back to the array again.
            foreach $_ (reverse((@_))) {
                unshift(@$arr_ref, "$_\n");
            }

            return (1);   # return non-rule
        }
     } elsif (parse_singleline_rule($line, $msg_ref, $sid_ref)) {
        $$single_ref = $line;
        $$single_ref =~ s/^\s*//;
        $$single_ref =~ s/^#+\s*/#/;
        $$single_ref =~ s/\s*\n$/\n/;

        return (1);   # return single
    } else {                          # non-rule line

      # Do extra check and warn if it *might* be a rule anyway,
      # but that we just couldn't parse for some reason.
        warn("\nWARNING: line may be a rule but it could not be parsed ".
             "(missing sid or msg?): $line\n")
          if ($config{verbose} && $line =~ /^\s*alert .+msg\s*:\s*".+"\s*;/);

        $$nonrule_ref = $line;
        $$nonrule_ref =~ s/\s*\n$/\n/;

        return (1);   # return non-rule
    }
}



# Same as in oinkmaster.pl.
sub parse_singleline_rule($ $ $)
{
    my $line    = shift;
    my $msg_ref = shift;
    my $sid_ref = shift;

    if ($line =~ /$SINGLELINE_RULE_REGEXP/oi) {

        if ($line =~ /\bmsg\s*:\s*"(.+?)"\s*;/i) {
            $$msg_ref = $1;
        } else {
            return (0);
        }

        if ($line =~ /\bsid\s*:\s*(\d+)\s*;/i) {
            $$sid_ref = $1;
        } else {
            return (0);
        }

        return (1);
    }

    return (0);
}


sub get_category_id($ $)
{
    (my $conn, my $name) = @_;

    my $query = "SELECT * FROM category WHERE name = '$name'";
    my $stm = $conn->prepare($query);
    $stm->execute();

    my $row = $stm->fetchrow_hashref;
    if(!exists($row->{"id"})) {
        return 117; # misc
    }
    $stm->finish();

    return $row->{"id"};
}

sub get_class_info($ $)
{
    (my $conn, my $name) = @_;

    if(!defined($name)){
        my @info = (102,3);
        return \@info;
    } else {
        my $query = "SELECT * FROM classification WHERE name = '$name'";
        my $stm = $conn->prepare($query);
        $stm->execute();

        my $row = $stm->fetchrow_hashref;

        my @info = ($row->{"id"}, 
                    $row->{"priority"});
        $stm->finish();
        return \@info;
    }
}

sub update_ossim_db()
{
    use DBI;
    use ossim_conf;


    #
    #  OSSIM db connect
    #
    my $dsn = "dbi:" .
        $ossim_conf::ossim_data->{"ossim_type"} . ":" .
        $ossim_conf::ossim_data->{"ossim_base"} . ":" .
        $ossim_conf::ossim_data->{"ossim_host"} . ":" .
        $ossim_conf::ossim_data->{"ossim_port"} . ":";

    my $conn = DBI->connect($dsn, 
                            $ossim_conf::ossim_data->{"ossim_user"}, 
                            $ossim_conf::ossim_data->{"ossim_pass"}) 
        or die "Can't connect to Database\n";

    #
    #  get all snort rules from ossim db 
    #  and store them in %db_sids hash table
    #
    my $query = "SELECT * FROM plugin_sid 
        WHERE plugin_id = 1001 ORDER BY sid;";
    my $stm = $conn->prepare($query);
    $stm->execute();

    my %db_sids;
    while (my $row = $stm->fetchrow_hashref) {
        $db_sids{$row->{"sid"}} = $row;
    }
    $stm->finish();


    foreach my $sid (sort { $a <=> $b } keys(%sidinfo)) {
        if (not exists($db_sids{$sid})) 
        {
            my $category_id = 
                get_category_id($conn, $sidinfo{$sid}{"category"});
            my $info =
                get_class_info ($conn, $sidinfo{$sid}{"classtype"});
            my ($class_id, $priority) = (${$info}[0], ${$info}[1]);
            my $msg = $sidinfo{$sid}{"msg"};
            if(!defined($msg)){ $msg = "Undefined msg, please check"; }
            $msg =~ s/\'/\\\'/g;
            $msg =~ s/\\'/\\\\\'/g;
            $msg =~ s/\-\-/\-/g; # sql comments (s/--/-)

            my $query = "INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name, priority) VALUES (1001, $sid, $category_id, $class_id, '$msg', $priority);";

            if($dump){
                print "$query\n";
            } else {
                my $stm = $conn->prepare($query);
                $stm->execute();
                $stm->finish();
                print "Inserting $msg: [1001:$sid:$priority]\n" unless ($quiet);
            }
		 
        }
    }

    $conn->disconnect();
}

