//
// Created by capting on 2025/11/26.
//

#include "unpack_task.h"
#include "MPUxxx_handler.h"
#include "elog.h"
#undef LOG_TAG
#define LOG_TAG "unpack"

extern bsp_mpuxxx_handler_t handler_instance;

void unpack_task(void* argument)
{
    log_i("unpack task start");
    mpuxxx_status_t ret = MPUxxx_OK;
    uint8_t data = 0;
    int16_t temp = 0;
    mpuxxx_data_t mpu6050_data;

    for (;;)
    {
        if (NULL != handler_instance.pUnpack_queue_handle)
        {
            ret = handler_instance.pInput_API->pOS->os_queue_get(
                                        handler_instance.pUnpack_queue_handle,
                                        &data,
                                        0xffffffff);
            if (MPUxxx_OK == ret)
            {
                log_i("unpack task data = [%d]",data);
            }
            uint8_t* addr = mpuxxx_buf.pfget_rbuffer_addr(&mpuxxx_buf);
            log_i("unpack task addr = [%p]",addr);
            temp = (int16_t)(*(addr+6)<<8 | *(addr + 7));
            mpu6050_data.temperature = 36.53 + temp/340.0;

            mpu6050_data.accel_x_raw=(int16_t)(*(addr + 0) << 8 | *(addr + 1));
            mpu6050_data.accel_y_raw=(int16_t)(*(addr + 2) << 8 | *(addr + 3));
            mpu6050_data.accel_z_raw=(int16_t)(*(addr + 4) << 8 | *(addr + 5));

            mpu6050_data.ax = mpu6050_data.accel_x_raw / 16384.0;
            mpu6050_data.ay = mpu6050_data.accel_y_raw / 16384.0;
            mpu6050_data.az = mpu6050_data.accel_z_raw / 14418.0;

            mpu6050_data.gyro_x_raw=(int16_t)(*(addr +  8) << 8 | *(addr +  9));
            mpu6050_data.gyro_y_raw=(int16_t)(*(addr + 10) << 8 | *(addr + 11));
            mpu6050_data.gyro_z_raw=(int16_t)(*(addr + 12) << 8 | *(addr + 13));

            mpu6050_data.gx = mpu6050_data.gyro_x_raw / 131.0;
            mpu6050_data.gy = mpu6050_data.gyro_y_raw / 131.0;
            mpu6050_data.gz = mpu6050_data.gyro_z_raw / 131.0;

            log_i("UnpackThread temp=%f", mpu6050_data.temperature);
            log_i("UnpackThread ax=%f", mpu6050_data.ax);
            log_i("UnpackThread ay=%f", mpu6050_data.ay);
            log_i("UnpackThread az=%f", mpu6050_data.az);
            log_i("UnpackThread gx=%f", mpu6050_data.gx);
            log_i("UnpackThread gy=%f", mpu6050_data.gy);
            log_i("UnpackThread gz=%f", mpu6050_data.gz);

            mpuxxx_buf.pfdata_readed(&mpuxxx_buf);
        }
    }
}