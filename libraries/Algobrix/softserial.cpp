//
// Includes
//
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <Arduino.h>
#include <softserial.h>
#include <util/delay_basic.h>

/* Includes **************************************************************** */
#include "softserial.h"

/* Private constants ******************************************************* */


/* Private macros ********************************************************** */

/* Private types *********************************************************** */

/* Private variables ******************************************************* */

/* Private function prototypes ********************************************* */

/* Exported functions ****************************************************** */
inline void SoftSerial::tunedDelay(uint16_t delay) 
{
  _delay_loop_2(delay);
}

SoftSerial::SoftSerial(uint8_t transmitPin, bool inverse_logic /* = false */):
  _tx_delay(0),
  _inverse_logic(inverse_logic)
{
  setTX(transmitPin);
}

SoftSerial::~SoftSerial()
{
  end();
}

void SoftSerial::setTX(uint8_t tx)
{
  // First write, then set output. If we do this the other way around,
  // the pin would be output low for a short while before switching to
  // output high. Now, it is input with pullup for a short while, which
  // is fine. With inverse logic, either order is fine.
  digitalWrite(tx, _inverse_logic ? LOW : HIGH);
  pinMode(tx, OUTPUT);
  _transmitBitMask = digitalPinToBitMask(tx);
  uint8_t port = digitalPinToPort(tx);
  _transmitPortRegister = portOutputRegister(port);
}

uint16_t SoftSerial::subtract_cap(uint16_t num, uint16_t sub) 
{
  if (num > sub)
  {
    return num - sub;
  }
  else
  {
    return 1;
  }
}

void SoftSerial::begin(long speed)
{
  _tx_delay = 0;

  // Precalculate the various delays, in number of 4-cycle delays
  uint16_t bit_delay = (F_CPU / speed) / 4;

  // 12 (gcc 4.8.2) or 13 (gcc 4.3.2) cycles from start bit to first bit,
  // 15 (gcc 4.8.2) or 16 (gcc 4.3.2) cycles between bits,
  // 12 (gcc 4.8.2) or 14 (gcc 4.3.2) cycles from last bit to stop bit
  // These are all close enough to just use 15 cycles, since the inter-bit
  // timings are the most critical (deviations stack 8 times)
  _tx_delay = subtract_cap(bit_delay, 15 / 4);

}

void SoftSerial::end()
{
}

size_t SoftSerial::write(uint8_t b)
{
  if (_tx_delay == 0) 
  {
    setWriteError();
    return 0;
  }

  // By declaring these as local variables, the compiler will put them
  // in registers _before_ disabling interrupts and entering the
  // critical timing sections below, which makes it a lot easier to
  // verify the cycle timings
  volatile uint8_t *reg = _transmitPortRegister;
  uint8_t reg_mask = _transmitBitMask;
  uint8_t inv_mask = ~_transmitBitMask;
  uint8_t oldSREG = SREG;
  bool inv = _inverse_logic;
  uint16_t delay = _tx_delay;

  if (inv)
  {
    b = ~b;
  }

  cli();  // turn off interrupts for a clean txmit

  // Write the start bit
  if (inv)
  {
    *reg |= reg_mask;
  }
  else
  {
    *reg &= inv_mask;
  }

  tunedDelay(delay);

  // Write each of the 8 bits
  for (uint8_t i = 8; i > 0; --i)
  {
    if (b & 1) // choose bit
    {
        *reg |= reg_mask; // send 1
    }
    else
    {
      *reg &= inv_mask; // send 0
    }

    tunedDelay(delay);
    b >>= 1;
  }

  // restore pin to natural state
  if (inv)
  {
    *reg &= inv_mask;
  }
  else
  {
    *reg |= reg_mask;
  }

  SREG = oldSREG; // turn interrupts back on
  tunedDelay(_tx_delay);
  return 1;
}

void SoftSerial::flush()
{
  // There is no tx buffering, simply return
}

// Read data from buffer
int SoftSerial::read()
{
  return -1;
}

int SoftSerial::available()
{
  return 0;
}

int SoftSerial::peek()
{
  return -1;
}


/* Private functions ******************************************************* */

/* ***************************** END OF FILE ******************************* */
