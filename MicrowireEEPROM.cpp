/*
  MicrowireEEPROM
 Reads and writes a Microwire EEPROM.
 Written by Timo Schneider <timos@perlplexity.org> 
 
 This code is in the public domain.
*/

#include "Arduino.h"
#include "MicrowireEEPROM.h"

MicrowireEEPROM::MicrowireEEPROM(int cs_pin, int clk_pin, int di_pin,
                                 int do_pin, int pg_size, int addr_width,
								 int clock_period)
{
  this->CS = cs_pin;
  this->CLK = clk_pin;
  this->DI = di_pin;
  this->DO = do_pin;

  this->ADDRWIDTH = addr_width;
  this->PAGESIZE = pg_size;
  this->HALF_CLOCK_PERIOD = clock_period / 2; 

  // make CS, CLK, DI outputs
  pinMode(CS, OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(DI, OUTPUT);
  
  // make DO an input
  pinMode(DO, INPUT);
}

long MicrowireEEPROM::transmit(int data, int bits)
{
        int dout = 0;
        for (int i=(bits-1); i>=0; i--) {
                dout |= ((int) digitalRead(DO)) << i;
                if ((1 << i) & data) digitalWrite(DI, HIGH);
                else digitalWrite(DI, LOW);
                delayMicroseconds(HALF_CLOCK_PERIOD);
                digitalWrite(CLK, HIGH);
                delayMicroseconds(HALF_CLOCK_PERIOD);
                digitalWrite(CLK, LOW);
        }
        digitalWrite(DI, LOW);
        return dout;
}

void MicrowireEEPROM::send_opcode(char op)
{
        digitalWrite(CLK, HIGH);
        delayMicroseconds(HALF_CLOCK_PERIOD);
        digitalWrite(CLK, LOW);
        digitalWrite(CS, HIGH);
        digitalWrite(DI, HIGH);
        // transmit start bit and two bit opcode
        transmit((1 << 2) | op, 3);
}


int MicrowireEEPROM::read(int addr)
{
        send_opcode(2);
        transmit(addr, ADDRWIDTH);
		// when reading, a leading zero is returned
        long data = transmit(0, PAGESIZE+1);
        digitalWrite(CS, LOW);
        return data;
}

void MicrowireEEPROM::writeEnable(void)
{
        send_opcode(0);
        transmit(0xFF, ADDRWIDTH);
        digitalWrite(CS, LOW);
}

void MicrowireEEPROM::writeDisable(void)
{
        send_opcode(0);
        transmit(0x00, ADDRWIDTH);
        digitalWrite(CS, LOW);
}

void MicrowireEEPROM::write(int addr, int data)
{
        send_opcode(1);
        transmit(addr, ADDRWIDTH);
        transmit(data, PAGESIZE);
        digitalWrite(CS, LOW);
}


 
