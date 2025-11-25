//
// Created by capting on 2025/11/25.
//

#include "circular_buffer.h"
#include <string.h>
#include <stdlib.h>

static uint8_t* get_wbuffer_addr(circular_buffer_t* pBuffer)
{
    return pBuffer->buffer + pBuffer->wflag * pBuffer->size;
}

static uint8_t* get_rbuffer_addr(circular_buffer_t* pBuffer)
{
    return pBuffer->buffer + pBuffer->rflag * pBuffer->size;
}

static void data_write_addr_updata(circular_buffer_t* pBuffer)
{
    pBuffer->wflag = (pBuffer->wflag + 1) % pBuffer->size;
}

static void data_read_addr_updata(circular_buffer_t* pBuffer)
{
    pBuffer->rflag = (pBuffer->rflag + 1) % pBuffer->size;
}

void buffer_init(circular_buffer_t* buffer,uint8_t size)
{
    if (NULL == buffer)
    {
        DEBUG_LOG("buffer is NULL");
        return;
    }
    buffer->size = size;
    buffer->rflag = 0;
    buffer->wflag = 0;

    buffer->buffer = (uint8_t*)malloc(size);

    buffer->pf_get_rbuffer_addr = get_rbuffer_addr;
    buffer->pf_get_wbuffer_addr = get_wbuffer_addr;
    buffer->pf_data_read_addr_updata = data_read_addr_updata;
    buffer->pf_data_write_addr_updata = data_write_addr_updata;
}
