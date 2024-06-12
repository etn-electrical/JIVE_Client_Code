#!/usr/bin/perl

use strict;
use warnings;

open(my $in, '<', 'input.txt')
    or die "Cannot open input.txt: $!";

open(my $out, '>', 'output.txt')
    or die "Cannot open output.txt: $!";

while (<$in>) {
  print $out $_ unless /script/;
}

close($in);
close($out);