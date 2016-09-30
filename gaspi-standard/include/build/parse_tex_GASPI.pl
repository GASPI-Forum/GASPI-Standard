#!/usr/bin/perl
#
# christian.simmendinger@t-systems.com
#
#   GASPI - header creation script
#           parse_tex_GASPI.pl
#
#
#  current version            : 0.2
#  last release version       :
#  date of delivery           :
#  status (since last version): 
#
#  last modification by       : C. Simmendinger, T-Systems SfR
#  date                       : 26.9.2016
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
#  Output is integrated into the GASPI.h.raw, GASPI.f90.raw templates.
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
open OUTFILE,">TEX.tmp";
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


open INFILE,"<TEX.tmp";
my $string = do { local $/; <INFILE> };
close INFILE;


open F90_FILE,">GASPI.f90.in";
open C_FILE,">GASPI.h.in";

# match from subsubsection to end of FDef
my @functions     = $string 
    =~ m/\\subsubsection\{\\gaspifunction\{(.*?)\}\}.*?\\begin\{FDef\}.*?\\end\{FDef\}/sg;
my %comments      = $string 
    =~ m/\\subsubsection\{\\gaspifunction\{(.*?)\}\}(.*?)\\begin\{FDef\}.*?\\end\{FDef\}/sg;
my %declarations  = $string 
    =~ m/\\subsubsection\{\\gaspifunction\{(.*?)\}\}.*?\\begin\{FDef\}(.*?)\\end\{FDef\}/sg;

foreach my $function (@functions) {

    my $prototype;
    my $fortran;    
    my $c;
    
# function    
    print C_FILE "/\*\n \* FUNCTION:\n \*   $function\n \*\n";
    print F90_FILE "!\n! FUNCTION:\n!   $function\n!\n";

# line wrap comments/description
    my $comment = $comments{$function};
    my @comment_words = split(/\s+/,$comment);

    my $len;
    my $max = 60;

# line wrap c
    $len = 0;
    my $c_text = " \*  ";
    foreach (@comment_words) {
	$len = $len + 1 + length $_;
	$c_text = $c_text . "$_ ";
	if ( $len > $max )  {	    
	    $c_text = $c_text . "\n \*   ";
	    $len = 0;
	}
    }

# line wrap fortran 
    $len = 0;
    my $f90_text = "!  ";
    foreach (@comment_words) {
	$len = $len + 1 + length $_;
	$f90_text = $f90_text . "$_ ";
	if ( $len > $max )  {	    
	    $f90_text = $f90_text . "\n!   ";
	    $len = 0;
	}
    }

# actual descriptions, c, fortran
    $c_text =~ s/\\gaspifunction\{(.*?)\}/gaspi_$1/g;
    $c_text =~ s/\\begin\{listing\}\[.*?\]\{.*?\}/\n \*\n \*  /g;
    $c_text =~ s/\\end\{listing\}/\n \*  /g;
    print C_FILE " \* DESCRIPTION:\n$c_text\n";

    $f90_text =~ s/\\gaspifunction\{(.*?)\}/gaspi_$1/g;
    $f90_text =~ s/\\begin\{listing\}\[.*?\]\{.*?\}/\n!\n!  /g;
    $f90_text =~ s/\\end\{listing\}/\n!  /g;
    print F90_FILE "! DESCRIPTION:\n$f90_text\n";

# prototype, c, fortran
    my $declaration = $declarations{$function};
    $declaration =~ s/\\gaspifunction\{(.*?)\}/gaspi_$1/sg;    
    ($prototype, $c, $fortran) = $declaration 
	=~ m/\\begin\{FDefSign\}.*?\\begin\{verbatim\}(.*?)\\end\{verbatim\}.*?\\end\{FDefSign\}/sg;    

    my $c_prototype = $prototype;
    $c_prototype =~ s/^\s*\n//mg;
    $c_prototype =~ s/^(.*)$/ \*   $1/mg;    
    print C_FILE " \*\n \* FUNCTION PROTOTYPE:\n$c_prototype";

    my $f90_prototype = $prototype;
    $f90_prototype =~ s/^\s*\n//mg;
    $f90_prototype =~ s/^(.*)$/!   $1/mg;    
    print F90_FILE "!\n! FUNCTION PROTOTYPE:\n$f90_prototype";

# parameter values    
    my $param;
    $param = $declarations{$function};
    $param =~ s/\\gaspifunction\{(.*?)\}/gaspi_$1/sg;
    my @params = $param
	=~ m/(\\parameterlistitem\{\s*.*?\s*\}\{\s*.*?\s*\}\{\s*.*?\s*\})/sg;    

    print C_FILE " \*\n \* PARAMETER:\n";
    print F90_FILE "!\n! PARAMETER:\n";

    my $timeout=0;
    foreach (@params) {
	my $in;
	my $parameter;
	my $description;    
	$string = $_;
	($in, $parameter, $description) = $string 
	    =~ m/\\parameterlistitem\{\s*(.*?)\s*\}\{\s*(.*?)\s*\}\{\s*(.*?)\s*\}/sg;

# const correctness 
	if ( $in !~ /out/ ) {
	    if ($parameter =~  /\[num\]/ ) {
		$parameter =~ s/(.*?)\[num\]/$1/;
	    }
	    $c =~ s/\s+([\*]*)$parameter/ const $1$parameter/;
	}

	my @description_words = split(/\s+/,$description);

	my $len;
	my $max = 50;

# line wrap c parameter description
	$len = 0;
	my $c_description = "";
	foreach (@description_words) {
	    $len = $len + 1 + length $_;
	    $c_description = $c_description . "$_ ";
	    if ( $len > $max )  {	    
		$c_description = $c_description . "\n \*          ";
		$len = 0;
	    }
	}
	print C_FILE " \*   \@param $parameter $c_description ($in)\n";

# line wrap fortran parameter description
	$len = 0;
	my $f90_description = "";
	foreach (@description_words) {
	    $len = $len + 1 + length $_;
	    $f90_description = $f90_description . "$_ ";
	    if ( $len > $max )  {	    
		$f90_description = $f90_description . "\n!          ";
		$len = 0;
	    }
	}
	print F90_FILE "!   \@param $parameter $f90_description ($in)\n";
	
	if ( $parameter =~ /timeout/i ) {
	    $timeout=1;
	}	    

    }
    
# return values c
    print C_FILE " \*\n \* RETURN VALUE:\n";
    print C_FILE " \*   \@return GASPI_SUCCESS in case of success.\n";
    print C_FILE " \*           GASPI_ERROR in case of error.\n";
    if ( $timeout != 0 ) {
	print C_FILE " \*           GASPI_TIMEOUT in case of timeout.\n";
    }	

# return values fortran
    print F90_FILE "!\n! RETURN VALUE:\n";
    print F90_FILE "!   \@return GASPI_SUCCESS in case of success.\n";
    print F90_FILE "!           GASPI_ERROR in case of error.\n";
    if ( $timeout != 0 ) {
	print F90_FILE "!           GASPI_TIMEOUT in case of timeout.\n";
    }	
    
# include actual c forward declaration     
    $c =~ s/^(.*)\n$/$1;/s;        
    print C_FILE " \*/\n";
    print C_FILE "$c\n\n";

# include actual f90 forward declaration     
    $fortran =~ s/^(.*)\n$/$1/s;

# include interface, import for fortran 
    $fortran =~ s/(\Wbind\s*\(.*?\))/$1\n  import/s;        
    print F90_FILE "!\n\ninterface";
    print F90_FILE "$fortran\nend interface\n\n";   

}
close C_FILE;
close F90_FILE;

# assemble c header
open C_INFILE,"<GASPI.h.in";
$string = do { local $/; <C_INFILE> };
close INFILE;

open C_FILE,">GASPI.h";
open C_INFILE,"<GASPI.h.raw";
while ($line = <C_INFILE>) {
    if ( $line =~ /^\/\* HERE \*\/$/) {
	print C_FILE $string;
    } else {
	print C_FILE $line;
    }
}
close C_INFILE;
close C_FILE;

# assemble fortran header
open F90_INFILE,"<GASPI.f90.in";
$string = do { local $/; <F90_INFILE> };
close F90_INFILE;

open F90_FILE,">GASPI.f90";
open F90_INFILE,"<GASPI.f90.raw";
while ($line = <F90_INFILE>) {
    if ( $line =~ /^! HERE/) {
	print F90_FILE $string;
    } else {
	print F90_FILE $line;
    }
}
close F90_INFILE;
close F90_FILE;
