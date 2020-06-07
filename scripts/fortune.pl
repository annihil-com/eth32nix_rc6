#!/usr/bin/perl
#use strict;

$|=1;

our $max;
our $min;
our $n = @ARGV;

if ($n == 1){
	$max = $ARGV[0];
	$min = $max/10;
}

if ($n > 1) {
	$max = $ARGV[1];
	$min = $ARGV[0];
}

sub sayit {
	print "et_str<say @_[0]>et_end\n";
	sleep(0.2);
}

sub print_fortune {
	my @lines = split(/\n/, `fortune -a -e -n 240 -s`);
	
	foreach (@lines) {
		$_ =~ s/^\s*//;
		$_ =~ s/\t/ /;
		if ($_ =~ m/^[^-]/) {
			if (length > 75) {
				print_fortune;
				return;
			}
			#print "$_\n";
			sayit $_;
		}
	}
}

if ($n == 0) { print_fortune }

while ( $n > 0 ) {
	my $int = $min + rand ($max-$min);
	print_fortune;
	sleep $int
}

