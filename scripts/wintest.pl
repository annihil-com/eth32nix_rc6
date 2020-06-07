#!/bin/perl
use strict;
use Time::HiRes qw(usleep);

$|=1;

#create our windows
#cwin <id> <x> <y> <font scale>
print "et_str<cwin 0 20 20 1.0>et_end\n";
print "et_str<cwin 1 500 100 0.8>et_end\n";

our $i = 0;
our $j = 0;
our $k = 0;

while (1) {
	usleep (500000);

	#update the window with new text
	print "et_str<uwin 0 ^2Window 1::^3Count = ^d$i>et_end\n";

	$j++;
	$i++;
	if (($j % 3) == 0) {
		$k++;
		$j = 0;

		# update 
		print "et_str<uwin 1 ^2Window 2::^3Count = ^d$k>et_end\n";

		if ($k == 5) {
			#destroy window 1
			print "et_str<dwin 1>et_end\n";
		}
	}
}
