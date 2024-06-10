use strict;
use warnings;                      # always use these

for my $file (@ARGV) {             # loop over all file names
    my $out = "$file.deduped.csv";     # create output file name
    open my $infh,  "<", $file or die "$file: $!";
    open my $outfh, ">", $out  or die "$out: $!";
    my %seen;
    while (<$infh>) {
        print $outfh $_ if !$seen{$_}++;   # print if a line is never seen before
    }
}