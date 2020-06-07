#!/usr/bin/perl
use warnings FATAL => 'all';
use strict;

$|=1;

chop(my $b = `cat /proc/cpuinfo`);
if ($b =~ m/model name.*:(.*)/) { print "et_str<say CPU:$1>et_end" }

chop(my $a = `glxinfo |grep "OpenGL renderer string"`);
if ($a =~ m/OpenGL renderer string:(.*)/) { 
	print "et_str<say $1>et_end";
}

chop(my $uname = `uname -r -o`);
print "et_str<say $uname>et_end";


my $hdd = `df | awk '{ sum+=\$2/1024^2 }; END { printf (\"%d GB \", sum )}'`;
my $hdd_used = `df | awk '{ sum+=\$3/1024^2 }; END { printf (\"%d\", sum )}'`;
my $hdd_free = `df | awk '{ sum+=\$4/1024^2 }; END { printf (\"%d GB \", sum )}'`;
print "et_str<say HDD: $hdd_used/$hdd free:$hdd_free>et_end\n";

my $ram = `free | grep Mem | awk '{printf (\"%d MB\",\$2/1000)}'`;
my $ram_used = `free | grep Mem | awk '{printf (\"%d\",\$3/1000)}'`;
my $ram_free = `free | grep Mem | awk '{printf (\"%d MB\",\$4/1000)}'`;
print "et_str<say Ram: $ram_used/$ram Free:$ram_free>et_end\n";
