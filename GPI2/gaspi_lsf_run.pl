#!/usr/bin/perl

use 5.10.0;   
use strict;

my $lsf_pe_var    = "LSB_MCPU_HOSTS";
my $machine_file  = "/home/cherold/machinefile";
my $gaspi_run     = "gaspi_run";
my %host_cpu;
my $master;

sub generateMachineFileLSF($)
{
  my $FILE;
  my @pes = split(/ /, shift);
  
  for( my $i = 0 ; $i < scalar( @pes ) ; $i += 2 ) {
    $host_cpu{$pes[$i]} = $pes[$i + 1];
  }
  open( $FILE, ">$machine_file") or die "Can't open machinefile:\n $!";
  
  foreach my $key ( sort( keys %host_cpu ) ) { 
    print $FILE "$key\n" x $host_cpu{$key};
  }
  
  close($FILE);
  return (sort(keys %host_cpu))[0];
}

sub generateMachineFileSLURM($)
{
  my $nodes = shift;
  my $FILE;
  
  open( $FILE, ">$machine_file") or die "Can't open machinefile:\n $!";
  
  print $FILE "$nodes";
  
  close($FILE);
  
  return ( split(/\n/, $nodes) )[0] ;
}

if ((grep /--help/, @ARGV) || (scalar( @ARGV ) == 0) )
{
  print "\nNo arguments given !!\n\n\n";
  print "#############################################################################\n";
  print "#                   Execution of an gaspi application:                      #\n";
  print "#############################################################################\n";
  print "<batch_system> [batchsystem_opt] /path/to/gaspi_run [gaspi_opt] ./application\n\n";
  print "#############################################################################\n";
  print "#                            on atlas with lsf                              #\n";
  print "#############################################################################\n\n";
  print "   bsub -Is -n 2 -R \"span[hosts=1]\" ./gaspi_lsf_run.pl -n 2 /home/user/main\n\n";
  print "#############################################################################\n";
  print "#                            on taurus with slurm                           #\n";
  print "#############################################################################\n\n";
  print "         salloc -n 2 /home/user/gaspi_lsf_run.pl -n 2 /home/user/main\n\n";
  print "#############################################################################\n\n";
  exit 1 ;
}

if( defined $ENV{$lsf_pe_var} ) 
{
  $master = generateMachineFileLSF( $ENV{$lsf_pe_var} );
}
elsif ( $ENV{"HOSTNAME"} =~ /taurus/ ) 
{
  $master = generateMachineFileSLURM( `srun hostname | sort` );
}
else 
{
  print "Wrong batchsystem variable or no execution with batchsystem !\n";
  exit 1;
}

my $exec = "ssh ".$master." $gaspi_run -m $machine_file ";
$exec .= join(' ', @ARGV);
print "<< execution on master node ",$master," >>\n";
exec $exec;
