#!/usr/bin/perl
use strict;
$|=1;
my $nick = $ARGV[0];
my $c = 0;
my $d = length($nick);
#my $clr = "^".int(rand(9));
for (1..length($nick)) {
my $substr = substr($nick, $c++);
print("et_str<name $substr>et_end"."\n");
sleep 1;
}
for (0..length($nick)) {
my $substr = substr($nick,$d--);
print("et_str<name $substr>et_end"."\n");
sleep 1;
}

