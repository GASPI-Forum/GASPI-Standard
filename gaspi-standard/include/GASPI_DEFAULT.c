#include <GASPI.h>

gaspi_configuration_t GASPI_CONFIGURATION_DEFAULT_VALUE (void)
{
  gaspi_configuration_t c;

  c.segment_num = 1 << 3;
  c.group_num = 1 << 3;
  c.queue_num = 1 << 3;
  c.queue_size_max = 1 << 10;
  c.message_size_max = 1 << 30;
  c.passive_queue_size_max = 1 << 10;
  c.passive_message_size_max = 1 << 10;
  c.counter_num = 1 << 4;
  c.network = 1;
  c.param_check = 0;
  c.user_defined = 0;

  return c;
}
