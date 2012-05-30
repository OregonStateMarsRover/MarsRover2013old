/*************************************************
 *
 * ATXMega A3 SPI Driver
 *
 * Written by Cody Hyman.
 * Copyright (C) 2011 OSURC.
 *************************************************/
 
 void SPI_InitPortStructs(void){
	
 }
 
 void SPI_Open(SPI * spi, unsigned char port, unsigned short baud, unsigned short tx_buf, unsigned short rx_buf,){
	//Initialize Structs
	spi->spi->CTRL |= SPI_ENABLE_bm;	//Enable SPI Port
	//Set Clock Prescalers
	spi->spi->CTRL |= SPI_MASTER_bm;	//Set SPI as Master
	RingBufferInit(&serial->tx_buffer, tx_buf);
	RingBufferInit(&serial->rx_buffer, rx_buf);
	
	SPI_Table[port]=spi;
 }
 
 void SPI_SetBaud(SPI * spi, unsigned short baud){
 
}
 