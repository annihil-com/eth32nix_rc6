#!/usr/bin/perl
# Enemy Territory (Q3) server query tool.
# server query tool for ET(q3)
# 2007 (c) nixCoders.org

use warnings FATAL => 'all';
use IO::Socket;
use strict;

if ($#ARGV != 1) {
    print "\n^d[!] Server query tool for ET(Q3)\n^d[!] 2007 (c) nixCoders.org\n\n";
    print "^d[!] usage: perl $0 server port\n\n";
exit;
}
$|=1;
my $server = $ARGV[0];
my $port = $ARGV[1];
my $query = IO::Socket::INET->new(PeerAddr=>$server,PeerPort=>$port,Proto=>'udp',Timeout=>'3') or die $!;
$query->send("\xff\xff\xff\xff\x67\x65\x74\x73\x74\x61\x74\x75\x73\x0a\r\n");
$query->recv(my $pars,2048);
my @newdata = split( /\n/, $pars );
my $players =@newdata -2;
print "\nInformation for\-\>\($server:$port\)\n\n";
if ($pars =~ 
/\x73\x76\x5f\x68\x6f\x73\x74\x6e\x61\x6d\x65\\(.*)\\\x73\x76\x5f\x70\x72\x69\x76\x61\x74\x65\x43\x6c\x69\x65\x6e\x74\x73/) 
{print "Server: $1\n"}
if ($pars =~ /(\x45\x54\x54\x56|\x45\x54)\s(\d\.\d+\w|\d\.\d+)/) { print "Version: $1 $2\n" }
if ($pars =~ 
/\x73\x76\x5f\x70\x72\x69\x76\x61\x74\x65\x43\x6c\x69\x65\x6e\x74\x73\\(\d+).*\\\x73\x76\x5f\x6d\x61\x78\x63\x6c\x69\x65\x6e\x74\x73\\(\d+)/) 
{ my $real1 = $2 -$1;print "Players: $players/$real1\n" }
if ($pars =~ 
/\x73\x76\x5f\x6d\x61\x78\x63\x6c\x69\x65\x6e\x74\x73\\(\d+).*\\\x73\x76\x5f\x70\x72\x69\x76\x61\x74\x65\x43\x6c\x69\x65\x6e\x74\x73\\(\d+)/) 
{ my $real = $1 -$2;print "Players: $players/$real\n" }
if ($pars =~ /\x6d\x61\x70\x6e\x61\x6d\x65\\(.+?)\\/) { print "MAP: $1\n" }
if ($pars =~ /\x73\x76\x5f\x6d\x61\x78\x52\x61\x74\x65\\(\d+)/) { print "Rate: $1\n" }
if ($pars =~ /\x73\x76\x5f\x70\x75\x6e\x6b\x62\x75\x73\x74\x65\x72\\(\d)/) { if ($1 == '1') { print 
"Punkbuster: on\n"}  else { print "Punkbuster: off\n" } }
