//
// Created by capting on 2025/11/25.
//

#ifndef USER_MPUXXX_CIRCULAR_BUFFER_H
#define USER_MPUXXX_CIRCULAR_BUFFER_H

#include <stdint.h>
#include "elog.h"

#define DEBUG_LOG(x,...)  log_d(x,##__VA_ARGS__);

typedef struct circular_buffer_struct
{
    uint8_t* buffer;
    uint8_t  size;
    uint8_t  rflag;
    uint8_t  wflag;
    uint8_t* (*pf_get_wbuffer_addr)(struct circular_buffer_struct*);
    uint8_t* (*pf_get_rbuffer_addr)(struct circular_buffer_struct*);
    void     (*pf_data_write_addr_updata)      (struct circular_buffer_struct*);
    void     (*pf_data_read_addr_updata)       (struct circular_buffer_struct*);
}
circular_buffer_t;

void buffer_init(circular_buffer_t* buffer,uint8_t size);

#endif //USER_MPUXXX_CIRCULAR_BUFFER_H