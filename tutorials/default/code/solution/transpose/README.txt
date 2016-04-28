                       Pipelined Transpose
                          Version 0.8
                             README

                          March 2015

==============================================================================
Table of contents
==============================================================================

 1.  Overview
 
 2.  Contents of this Distribution
 3.  Hardware and Software Requirements
 4.  Configuration
 5.  Related Documentation - MPI (Message Passing Interface)
 6.  Documentation - GASPI (Global Address Space Programming Interface)
 7.  Implementation details
 8.  Results
 9.  Community involvement

==============================================================================
1. Overview
==============================================================================


This is the initial release of a pipelined transpose proxy.  This kernel is an
attempt at establishing a small but meaningful proxy/benchmark for
notification based  1-sided communication.

The kernel calculates a global transpose of a matrix for a column-based
matrix distribution. This is a hybrid implementation where a global transpose
is followed by a local transpose.

While the MPI implementation uses an MPI alltoall for global communication
(where potentially several methods are being used, e.g. linear aggreagtion,
Brucks Algorithm for latency bound message sizes, parallel send/recv with
(possibly hierarchical) communication schedules for large message sizes etc.)
the GASPI implementation uses a rather naive communication algorithm, in 
which all required communication to all target ranks is issued in a single
phase. The notification based one sided communication requests are scheduled 
(target rank sequence) such that we minimize network congestion, i.e. we
always aim for bidirectional communication even though the actual
communication is entirely one-sided.

We note that this rather naive implementation appears to scale higher than
many/most/all(?) existing implementations for MPI_Alltoall.

We note that (apart from potential network congestion) the issue and receive
rate of GASPI notifications is limited by hardware (message rate, bandwidth,
etc) rather than by the actual GASPI API. As such we believe that this 
implementation potentially can scale very high.

==============================================================================
2.  Contents of this Distribution
==============================================================================

This release includes source code for MPI and GASPI implementations 
in the ./mpi and ./gaspi directories. 
  
==============================================================================
3. Hardware and Software Requirements
==============================================================================

1) Server platform with InfiniBand HCA or ROCE Ethernet.

2) Linux operating system 

3) MPI. Download and install. This kernel requires MPI to be interoperable 
   with GASPI. So far this has been verified to work with with Openmpi, 
   Mvapich, Mvapich2, Intel MPI and IBM Platform MPI. 

4) GASPI. For installation and download instructions of GPI2 (currently 
   the only full implementation of GASPI) please see 
   https://github.com/cc-hpc-itwm/GPI-2

==============================================================================
4. Configuration
==============================================================================

1) Download and install the required software packages, MPI, GPI2.
   Note : GPI2 needs to be compiled against your MPI library of choice in 
   order to use the  MPI interoperabilty mode. 
   (./install.sh --with-mpi=/../../foo -p PREFIX)

2) Adapt the Makefiles. (MPI_DIR, GPI2_DIR, CFLAGS, etc). The MPI lib will 
   require support for MPI_THREAD_MULTIPLE.
 
3) GASPI can make use of the startup mechanisms of MPI. Start the
   hybrid MPI/GASPI executable as a regular  hybrid OpenMP/MPI application, e.g  
   mpirun -np 2 -machinefile machines -perhost 2 ./foo.exe

==============================================================================
5. MPI
==============================================================================

   For MPI Documentation
   http://www.mpi-forum.org/docs/mpi-3.0/mpi30-report.pdf

==============================================================================
6. GASPI
==============================================================================

   For GASPI Documentation
   http://www.gpi-site.com/gpi2/gaspi/

==============================================================================
7. Results
==============================================================================

See documentation.

==============================================================================
8. Community involvement
==============================================================================

We encourage the HPC community to provide new patterns or improve existing
ones. No restrictions apply. Instead we encourage you to improve
this kernel via better threadings models, and/or better communication and/or
more scalable communication API’s and/or better programming languages. Bonus
points are granted for highly scalable implementations which also feature
better programmability.



