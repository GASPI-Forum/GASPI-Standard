#!/usr/bin/perl

use 5.10.0;   
use strict;

my $lsf_pe_var    = "LSB_MCPU_HOSTS";
my $machine_file  = "/home/cherold/GPI2-rc1/machinefile";
my $gaspi_run     = "/home/cherold/GPI2-rc1/bin/gaspi_run";
my %host_cpu;

sub generateMachineFile($)
{
  my $FILE;
  my @pes = split(/ /, shift);
  
  open( $FILE, ">$machine_file") or die "Can't open machinefile:\n $!";
  
  for( my $i = 0 ; $i < scalar( @pes ) ; $i += 2 ) {
    print $FILE "$pes[$i]\n" x $pes[$i + 1];
    $host_cpu{$pes[$i]} = $pes[$i + 1];
  }
  
  close($FILE);
}

if( scalar( @ARGV ) == 0 ) {
  print "No arguments given !!\n";
  exit 1;
}
if (grep /--help/, @ARGV) {
  print "#############################################################################\n";
  print "#                   Execution of an gaspi application:                      #\n";
  print "#############################################################################\n\n";
  print "<batch_system> [batchsystem_opt] /path/to/gaspi_run [gaspi_opt] ./application\n\n";
  print "e.g.:\n";
  print "   bsub -Is -n 2 -R \"span[hosts=1]\" ./gaspi_lsf_run.pl -n 2 ~/gpi2/bin/main\n\n";
  print "#############################################################################\n";
  exit 1 ;
}

if( ! defined $ENV{$lsf_pe_var} ) {
  print "Wrong batchsystem variable or no execution with batchsystem !\n";
  exit 1;
}

generateMachineFile( $ENV{$lsf_pe_var} );

my $exec = "ssh ".(keys %host_cpu)[0]." $gaspi_run -m $machine_file ";
$exec .= join(' ', @ARGV);
print "<< execution on master node ",(keys %host_cpu)[0]," >>\n";
exec $exec;
