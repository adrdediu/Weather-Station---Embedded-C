/**************************************************************Sensors Functions*********************************************************************************************/
#include <xc.h>
#define DHT11_PIN             RA1
#define DHT11_PIN_TRIS  TRISA1



#define _XTAL_FREQ 8000000
////////////////////////////***********Read data from ADC***********//////////////////

unsigned int read_data_ADC(char analog_pin)
{
	ADCON0bits.CHS = analog_pin;
   __delay_ms(10); // wait time otherwise the adc conversion does not function
	GO=1; // start conversion
    while(GO);// go turns 
    return (ADRESH<<8)+ADRESL;
}

///////offlimits
//DHT11 DIGITAL TEMP  & HUMID sensor

void start_dht11()
{
	DHT11_PIN_TRIS = 0; // make it as output
	DHT11_PIN = 0;			 // clear dht11_pin
	
    __delay_ms(25);
	DHT11_PIN = 1;//keep 1 logic level for 30us
	__delay_us(40);
	DHT11_PIN_TRIS =1; // make RB4 input

}

char Check_Response_from_DHT11()
{
	TMR1H=0;
	TMR1L=0;
	TMR1ON=1;
	
	while(!DHT11_PIN&&TMR1L <100); // wait till dht becomes high approximatelly 80us
    
	if(TMR1L > 90) return 0 ; // if it took longer than that response error
	else
		{  TMR1H=0;TMR1L=0;
			while(DHT11_PIN&&TMR1L<100);
			if(TMR1L> 90) return 0 ; // if it took longer than 90us then response error
			else return 1;
		}

}

char read_data_from_dht11(unsigned char *data_byte_dht)
{
	*data_byte_dht = 0 ;//initializez byte-ul cu 0
	
	for(char bit_dht_byte = 0; bit_dht_byte < 8 ; bit_dht_byte++)
    {
		TMR1H=0;
		TMR1L=0;
		while(!DHT11_PIN)  // 50 us of gnd lvl signals the transmission
			if(TMR1L > 90) return 1; // timeout error it takes 50us to begin transmission
		
		TMR1H=0;
		TMR1L=0;
		
		while(DHT11_PIN) //waiting for next transmission signal
			if(TMR1L>90) return 1;// timeout error , it takes 26-28us for 0 and 70 for 1 logic level
		if(TMR1L > 49) // 70+ 28= 98/2 = 49uS
			*data_byte_dht | = (1<<(7-bit_dht_byte)); //add to the data_byte_dht ,1 shifted by the number of it's position since dht transmits msb first

   }
return 0 ; // everything ok
}

//HCZ H8 A analog resistive humidity sensor

float hcz_h8a_calculate_humidity(unsigned int adc_data_from_hczh8a , float temp)
{
	// Calibrating temperature approximation according to a threshold
	 const static char temp_values[12] ={5,10,15,20,25,30,35,40,45,50,55,60};
    char new_designated_temp=0;
	for(char temp_value_index = 1; temp_value_index <=11; temp_value_index++) {
		if(temp<=temp_values[temp_value_index]) {
			float temp_treshold = (temp_values[temp_value_index]+temp_values[temp_value_index-1])/2.0;
			if(temp<temp_treshold) new_designated_temp = temp_values[temp_value_index-1];
			else new_designated_temp = temp_values[temp_value_index];
			temp_value_index +=11;
		}
	}
	const static unsigned int hczh8a_adc_values[12][15]={
				/* 5 C - 14*/			{0, 1008,993,956,890,783,644,480,339,223,138,83,48,27,15},
				/*10 C - 15*/		    {1013,1003,981,932,845,720,577,414,286,183,117,71,43,25,15},
				/*15 C - 15*/			{1010,995,964,902,798,654,499,361,244,159,100,62,39,24,14},
				/*20 C - 15*/		    {1005,986,946,867,748,590,438,310,206,133,89,55,35,22,14},
				/*25 C - 15*/			{1000,975,926,829,694,533,380,256,178,117,76,50,32,20,14},
				/*30 C - 15*/           {993,962,902,787,634,460,343,232,154,100,66,45,30,20,13},
				/*35 C - 15*/           {987,946,875,743,577,414,300,201,133,89,60,41,28,19,12},
				/*40 C - 15*/           {979,932,848,701,533,378,264,173,117,77,54,38,26,18,12},
				/*45 C - 15*/           {972,919,822,662,480,339,236,154,106,71,50,35,24,17,12},
				/*50 C - 15*/           {966,902,794,624,438,307,210,138,95,65,46,33,23,16,12},
				/*55 C - 15*/           {959,892,771,590,414,279,192,128,89,60,43,31,22,16,12},
				/*60 C - 15*/           {956,882,753,563,391,260,173,117,83,57,41,30,22,16,11}
    };
    char hczh8a_adc_values_row =new_designated_temp/5-1;
    for(char hczh8a_adc_values_column = 0;hczh8a_adc_values_column<=13;hczh8a_adc_values_column++) {
    	if(adc_data_from_hczh8a>=hczh8a_adc_values[hczh8a_adc_values_row][hczh8a_adc_values_column+1]) {   
			int x1 = hczh8a_adc_values[hczh8a_adc_values_row][hczh8a_adc_values_column+1];
            int x2 =hczh8a_adc_values[hczh8a_adc_values_row][hczh8a_adc_values_column];
            int y1=(hczh8a_adc_values_column+1)*5+20;
            return (-5.0*(adc_data_from_hczh8a-x1)/(x2-x1)+y1);
        }
    }
 return 0;
}

