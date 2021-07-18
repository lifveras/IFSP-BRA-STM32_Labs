#ifndef __DHT11_H__
#define __DHT11_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
// Estrutura definida de acordo com o Datasheet
// Data de 40 bits
typedef struct{
	uint8_t RH_integral;
	uint8_t RH_decimal;
	uint8_t T_integral;
	uint8_t T_decimal;
	uint8_t checksum;
} DHT11_Data;

void DHT11_signal_start(void);

uint8_t DHT11_check_response(void);

void DHT11_read_data(DHT11_Data *data);

#ifdef __cplusplus
}
#endif
#endif /*__DHT11_H__ */
