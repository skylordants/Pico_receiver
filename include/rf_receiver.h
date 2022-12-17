#ifndef __RF_RECEIVER_H__
#define __RF_RECEIVER_H__

bool rf_receiver_init(uint32_t *aht20_t, uint32_t *aht20_h, int32_t *bmp280_t, uint32_t *bmp280_p);

bool rf_read_message();

#endif