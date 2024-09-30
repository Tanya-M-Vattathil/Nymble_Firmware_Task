//#include <stdio.h>
#define CLKFREQ 16000000UL // clock frequency of 16 MHz
#define BAUD 2400
#define MYUBRR CLKFREQ/16/BAUD-1

// Define addresses for USART
#define UDR0    (*(volatile unsigned char *)0xC6)  // USART I/O data register
#define UBRR0H  (*(volatile unsigned char *)0xC5)  // USART baud rate register high
#define UBRR0L  (*(volatile unsigned char *)0xC4)  // USART baud rate register low
#define UCSR0C  (*(volatile unsigned char *)0xC2)  // Character Size, Stop Bit Select
#define UCSR0B  (*(volatile unsigned char *)0xC1)  // Enable Transmitter and Receiver
#define UCSR0A  (*(volatile unsigned char *)0xC0)  // Flag to indicate ready for reception
#define RXEN0    4      // Receiver Enable - bit 4
#define TXEN0    3      // Transmitter Enable - bit 3
#define USBS0    3      // Stop Bit Select - bit 3
#define UCSZ0    1      // Character Size - bits 1:0
#define UDRE0    5      // USART Data Register Empty - bit 5

// Define addresses for EEPROM
#define EEARH (*(volatile unsigned char *)0x42)    // EEPROM address register high byte (memory mapped address)
#define EEARL (*(volatile unsigned char *)0x41)    // EEPROM address register low byte
#define EEDR  (*(volatile unsigned char *)0x40)    // EEPROM data register
#define EECR  (*(volatile unsigned char *)0x3F)    //Control Register 
#define EEMPE  2        // EEPROM Master Write Enable
#define EEPE   1        // EEPROM Write Enable
#define EERE   0        // EEPROM Read Enable


int main(void)
{
    unsigned int i = 0;
    unsigned char data_receive;
    USART_Init(MYUBRR);

    //USART_SendString("Initializing USART...\r\n");

    while(1) {
        data_receive = USART_Receive();
        //USART_Debug(data_receive);
        EEPROM_write(i, data_receive);
        if (data_receive == '\0') { // Stop when null character is received
            i = 0; // Reset index to start reading from beginning
            Transmit_EEPROM_String(); // Call function to transmit stored string
        } else {
            i++; // Increment index for next write
        }
    }
   //Transmit_EEPROM_String();
    return 0;
}

void USART_Init(unsigned int ubrr)
{
    /* Set baud rate */
    UBRR0H = (unsigned char)(ubrr>>8);
    UBRR0L = (unsigned char)ubrr;
    /* Enable receiver and transmitter */
    UCSR0B = (1<<RXEN0)|(1<<TXEN0);
    /* Set frame format: 2 stop bits, 8 bit data */
    UCSR0C = (1<<USBS0)|(3<<UCSZ0);
}

void USART_Transmit(unsigned char data)
{
    /* Wait for empty transmit buffer */
    while (!(UCSR0A & (1<<UDRE0)));
    /* Put data into buffer, sends the data */
    UDR0 = data;
}

unsigned char USART_Receive(void)
{
    /* Wait for data to be received */
    while (!(UCSR0A & (1<<RXC0)));
    /* Get and return received data from buffer */
    return UDR0;
}

void EEPROM_write(unsigned int uiAddress, unsigned char ucData)
{
    /* Wait for completion of previous write */
    while(EECR & (1<<EEPE));
    /* Set up address and Data Registers */
    EEARH = (unsigned char)(uiAddress>>8);
    EEARL = (unsigned char) uiAddress;
    EEDR = ucData;
    /* Write logical one to EEMPE */
    EECR |= (1<<EEMPE);
    /* Start EEPROM write by setting EEPE */
    EECR |= (1<<EEPE);
    // USART_Transmit(ucData);
    //USART_Transmit(EEDR);
    for(int j = 1000; j>0; j--);
}

unsigned char EEPROM_read(unsigned int uiAddress)
{
    /* Wait for completion of previous write */
    while(EECR & (1<<EEPE));
    /* Set up address and Data Registers */
    EEARH = (unsigned char)(uiAddress>>8);
    EEARL = (unsigned char) uiAddress;
    /* Start EEPROM read by setting EERE */
    EECR |= (1<<EERE);
    //USART_Transmit(EEDR);
    return EEDR;

}

void Transmit_EEPROM_String(void) {
    unsigned int i = 0;
    unsigned char data;  
    while (1) {
        data = EEPROM_read(i);  // Read from EEPROM
        USART_Transmit(data);   // Transmit via USART  
        if (data == '\0') {     // Stop if null character is reached
            break;
        }  
        i++;
    }
}
