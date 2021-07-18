#include "dht11.h"
#include "gpio.h"
#include "tim.h"
#include <math.h>

#define DHT11_PORT GPIOA
#define DHT11_PIN GPIO_PIN_1

void delay(uint16_t cycles){
	/*Tempo está configurado para trabalhar em microsegundos*/
	__HAL_TIM_SET_COUNTER(&htim6, 0);
	while(__HAL_TIM_GET_COUNTER(&htim6) < cycles){
	}
}

void test_time_delay(){
	for (int i=0; i<100; i++)
	{
		HAL_GPIO_TogglePin(DHT11_PORT, DHT11_PIN);
		for (int j=0; j<20; j++)
		{
			delay(50000); //com freq 32mhz/32 = 1 MhZ. Um ciclo vai durar 10us.
			uint32_t clock = HAL_RCC_GetPCLK1Freq();
			htim6.Init.ClockDivision;
			htim6.Init.Prescaler;
			//delay(1000); //com freq 32mhz/32000 = 1000 HZ. Um ciclo vai durar 1ms.

			//HAL_Delay(100);
		}
	}
}

void GPIO_Set_Output(GPIO_TypeDef *GPIOx, uint16_t GPIO_pin){
	  GPIO_InitTypeDef GPIO_InitStruct = {0};
	  GPIO_InitStruct.Pin = GPIO_pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void GPIO_Set_Input(GPIO_TypeDef *GPIOx, uint16_t GPIO_pin){
	  GPIO_InitTypeDef GPIO_InitStruct = {0};
	  GPIO_InitStruct.Pin = GPIO_pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

// Sinal do MCU
void DHT11_signal_start(void){
	GPIO_Set_Output(DHT11_PORT, DHT11_PIN);
	HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, 0);
	delay(18000); // espera 18ms
	HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, 1);
	delay(20); //espera 20 us
	GPIO_Set_Input(DHT11_PORT, DHT11_PIN);
}

//Parte que recebe o sinal do HDT11
uint8_t DHT11_check_response(void){
	uint8_t response = 0;
	delay(40);
	uint8_t signal = HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN);
	if(!signal){
		delay(80);
		response = (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN)) ? 1 : -1;
	}
	//espera o pino ir para LOW
	while(HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN));
	return response;
}

uint8_t DHT11_read_byte(){
	uint8_t byte_data, i;
	uint8_t data[8];
	for(i = 0; i< 8; i++){ //receber cada bit enviado. Repita até um byte ser completado
			while(!(HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN))); // Aguarda o pin ir para HIGH (receber dado de sensor). O sensor DHT11 deve levar 50un nessa parte.

			delay(40); //aguarda 40 us. É tempo suficiente para passar por envio de um "0"(26-28us) e verificar se o sensor ainda está enviando "1" (70us).

			if(!(HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN))){
				byte_data &= ~(1 << (7-i)); //escreve 0 ao byte
				data[i] = 0;
			} else{
				byte_data |= (1 << (7-i)); //escreve 1 ao byte
				data[i] = 1;
			}

			while((HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN))); // Aguarda o pin ir para LOW (encerra o envio de um bit)
	}

	return byte_data;
}

void DHT11_read_data(DHT11_Data *data){
//	uint8_t count = 0;
//	while(1){
//		while(!HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN)){
//			if(count >= 40) {
//				HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN);
//			}
//		}
//		while(HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN)){
//			if(count >= 40) {
//				HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN);
//			}
//		}
//		count++;
//	}
	//Deve ser inserido na ordem de envio dos bits pelo DHT11
	data->RH_integral = DHT11_read_byte();
	data->RH_decimal = DHT11_read_byte();
	data->T_integral = DHT11_read_byte();
	data->T_decimal = DHT11_read_byte();
	data->checksum = DHT11_read_byte();
}

