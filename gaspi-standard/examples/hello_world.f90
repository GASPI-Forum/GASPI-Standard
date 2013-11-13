program hello_world

  use gaspi_c_binding
  implicit none
  integer(gaspi_return_t) :: res
  integer(gaspi_rank_t) :: rank, num
  integer(gaspi_timeout_t) :: timeout

  timeout = GASPI_BLOCK

  res = gaspi_proc_init(timeout)
  res = gaspi_proc_rank(rank)
  res = gaspi_proc_num(num)

  print *,"Hello world from rank ",rank

  res = gaspi_proc_term(timeout)

end program hello_world
