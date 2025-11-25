//
// Created by capting on 2025/11/25.
//

#ifndef USER_MPUXXX_CIRCULAR_BUFFER_H
#define USER_MPUXXX_CIRCULAR_BUFFER_H

#include <stdint.h>
#include "elog.h"

typedef struct circular_buffer_struct
{
    uint8_t* buffer;
    uint8_t  size;
    uint8_t  rflag;
    uint8_t  wflag;
    uint8_t* (*pf_get_wbuffer_addr)(struct circular_buffer_struct*);
    uint8_t* (*pf_get_rbuffer_addr)(struct circular_buffer_struct*);
    void     (*pf_data_write)      (struct circular_buffer_struct*);
    void     (*pf_data_read)       (struct circular_buffer_struct*);
}
circular_buffer_struct_t;

void buffer_init(circular_buffer_struct_t* buffer,uint8_t size);

#endif //USER_MPUXXX_CIRCULAR_BUFFER_H