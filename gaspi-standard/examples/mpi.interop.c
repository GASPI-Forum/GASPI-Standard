mpi_startup;

/* MPI part, no ongoing GASPI communication... */

/* ...finish all ongoing MPI communication */

mpi_barrier;

/* no ongoing MPI communication */

gaspi_proc_init;

while (!done) {

  /* GASPI part, no ongoing MPI communication... */

  /* ...finish all ongoing GASPI communication */

  gaspi_barrier;

  /* MPI part, no ongoing GASPI communication... */

  /* ...finish all ongoing MPI communication */

  mpi_barrier;
}

gaspi_proc_term;

/* MPI part, no ongoing GASPI communication */

mpi_shutdown;
