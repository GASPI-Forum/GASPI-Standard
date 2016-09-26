#!/usr/bin/perl
#
# christian.simmendinger@t-systems.com
#
#   GASPI - header creation script
#           parse_tex_GASPI.pl
#
#
#  current version            : 0.1
#  last release version       :
#  date of delivery           :
#  status (since last version): 
#
#  last modification by       : C. Simmendinger, T-Systems SfR
#  date                       : 1.9.2016
#
# FUNCTION
#
#  Parses the GASPI tex file, creates part of the GASPI header 
#  files for C, Fortran.
#
#  The script includes all GASPI functions text ranging from 
#    the subsubsections which point to some \gaspifunction{}
#    until the end of the function definition (\end{FDef}).
#
#  Text between start of subsubsection and beginning of
#    function definition is included as part of description
#
#  Text describing in/out parameters is included as part of description.
#
#  Const correctness is applied depending on in/out/inout parameter.
#  
#  Includes timeout return value., if available
#    
#  Output is integrated into the GASPI.h.raw template.
#
#  Intermediate files not (yet) removed.
#

use strict;
use warnings;

my $line;
my $key;
my $num_functions=0;

if (! $ARGV[0] ){
    die "ERROR: Usage: perl ./$0 GASPI_standard.tex\n\n";
}

my $filename = $ARGV[0];

# clean up, remove comments, tex style, empty lines, etc.
open INFILE,"<$filename";
open OUTFILE,">$filename.tmp";
while ($line = <INFILE>) {
    if ( $line !~ /^\%.*$/) {
	$line =~ s/\\_/_/g;
	$line =~ s/\\gaspisemantic\{(.*?)\}/$1/g;
	$line =~ s/\\gaspitype\{(.*?)\}/$1/g;
	$line =~ s/\\parameter\{(.*?)\}/$1/g;
	$line =~ s/\\label\{(.*?)\}//g;
	$line =~ s/\\left\[/[/g;
	$line =~ s/\\right\)/)/g;
	$line =~ s/\\leq/<=/g;
	$line =~ s/\\GASPI\{\}/GASPI/g;
	$line =~ s/\$(.*?)\$/$1/g;
	$line =~ s/^\s*$//g;
	print OUTFILE $line;
    }
}
close OUTFILE;
close INFILE;


open INFILE,"<$filename.tmp";
my $string = do { local $/; <INFILE> };
close INFILE;


open OUTFILE,">GASPI.h.in";

# match from subsubsection to end of FDef
my @functions     = $string 
    =~ m/\\subsubsection\{\\gaspifunction\{(.*?)\}\}.*?\\begin\{FDef\}.*?\\end\{FDef\}/sg;
my %comments      = $string 
    =~ m/\\subsubsection\{\\gaspifunction\{(.*?)\}\}(.*?)\\begin\{FDef\}.*?\\end\{FDef\}/sg;
my %declarations  = $string 
    =~ m/\\subsubsection\{\\gaspifunction\{(.*?)\}\}.*?\\begin\{FDef\}(.*?)\\end\{FDef\}/sg;

foreach my $function (@functions) {

    my $comment;
    my $declaration;
    my $param;
    my $prototype;
    my $fortran;    
    my $c;
    
# function    
    print OUTFILE "/\*\n \* FUNCTION:\n \*   $function\n \*\n";

# line wrap comments/description
    $comment = $comments{$function};
    my @comment_words = split(/\s+/,$comment);
    $comment = " \*  ";
    my $len = 0;
    my $max = 60;    
    foreach (@comment_words) {
	$len = $len + 1 + length $_;
	$comment = $comment . "$_ ";
	if ( $len > $max )  {	    
	    $comment = $comment . "\n \*   ";
	    $len = 0;
	}
    }
    $comment =~ s/\\gaspifunction\{(.*?)\}/gaspi_$1/g;
    $comment =~ s/\\begin\{listing\}\[.*?\]\{.*?\}/\n \*\n \*  /g;
    $comment =~ s/\\end\{listing\}/\n \*  /g;
    print OUTFILE " \* DESCRIPTION:\n$comment\n";

# prototype, c, fortran
    $declaration = $declarations{$function};
    $declaration =~ s/\\gaspifunction\{(.*?)\}/gaspi_$1/sg;    
    ($prototype, $c, $fortran) = $declaration 
	=~ m/\\begin\{FDefSign\}.*?\\begin\{verbatim\}(.*?)\\end\{verbatim\}.*?\\end\{FDefSign\}/sg;    
    $prototype =~ s/^\s*\n//mg;
    $prototype =~ s/^(.*)$/ \*   $1/mg;    
    print OUTFILE " \*\n \* FUNCTION PROTOTYPE:\n$prototype";

# parameter values    
    $param = $declarations{$function};
    $param =~ s/\\gaspifunction\{(.*?)\}/gaspi_$1/sg;
    my @params = $param 
	=~ m/(\\parameterlistitem\{\s*.*?\s*\}\{\s*.*?\s*\}\{\s*.*?\s*\})/sg;    
    print OUTFILE " \*\n \* PARAMETER:\n";

    my $timeout=0;
    foreach (@params) {
	my $in;
	my $parameter;
	my $description;    
	$string = $_;
	($in, $parameter, $description) = $string 
	    =~ m/\\parameterlistitem\{\s*(.*?)\s*\}\{\s*(.*?)\s*\}\{\s*(.*?)\s*\}/sg;

	my @description_words = split(/\s+/,$description);
	$description = "";
	my $len = 0;
	my $max = 50;

# const correctness 
	if ( $in =~ /in/ ) {
	    $c =~ s/\s+$parameter/ const $parameter/;
	    $c =~ s/const const/const/;
	}

# line wrap parameter description
	foreach (@description_words) {
	    $len = $len + 1 + length $_;
	    $description = $description . "$_ ";
	    if ( $len > $max )  {	    
		$description = $description . "\n \*          ";
		$len = 0;
	    }
	}
	if ( $parameter =~ /timeout/i ) {
	    $timeout=1;
	}	    
	print OUTFILE " \*   \@param $parameter $description ($in)\n";

    }
    
# return values
    print OUTFILE " \*\n \* RETURN VALUE:\n";
    print OUTFILE " \*   \@return GASPI_SUCCESS in case of success.\n";
    print OUTFILE " \*           GASPI_ERROR in case of error.\n";
    if ( $timeout != 0 ) {
	print OUTFILE " \*           GASPI_TIMEOUT in case of timeout.\n";
    }	
    
# include actual c forward declaration     
    $c =~ s/^(.*)\n$/$1;/s;        
    print OUTFILE " \*/\n";
    print OUTFILE "$c\n\n";   

}
close OUTFILE;

# assemble everyting 
open INFILE,"<GASPI.h.in";
$string = do { local $/; <INFILE> };
close INFILE;

open OUTFILE,">GASPI.h";
open INFILE,"<GASPI.h.raw";
while ($line = <INFILE>) {
    if ( $line =~ /^\/\* HERE \*\/$/) {
	print OUTFILE $string;
    } else {
	print OUTFILE $line;
    }
}
close INFILE;
close OUTFILE;
