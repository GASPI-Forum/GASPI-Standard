#ifndef NEGOTIATE_REMOTE_OFFSET_H
#define NEGOTIATE_REMOTE_OFFSET_H

typedef struct offset_entry_t
{
  gaspi_size_t   local_send_len; // num send int
  gaspi_offset_t local_send_offset;
  volatile gaspi_size_t local_recv_len; // num recv int
  volatile gaspi_offset_t local_recv_offset;
  gaspi_offset_t remote_recv_offset;
} offset_entry;

typedef enum handler_t
  {
    SAY_HELLO,
    RETURN_OFFSET
  } handler;

typedef struct packet_t
{
  handler type;   
  gaspi_rank_t rank;
  gaspi_size_t len;
  gaspi_offset_t offset;
} packet;


#endif
