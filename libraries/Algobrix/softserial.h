/* Define to prevent recursive inclusion *********************************** */
#ifndef __SOFTSERIAL_H
#define __SOFTSERIAL_H

/* Includes **************************************************************** */
#include <inttypes.h>
#include <Stream.h>

/* Exported constants ****************************************************** */
#ifndef GCC_VERSION
#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#endif


/* Exported macros ********************************************************* */

/* Exported types ********************************************************** */
class SoftSerial : public Stream
{
private:
  uint8_t _transmitBitMask;
  volatile uint8_t *_transmitPortRegister;
  volatile uint8_t *_pcint_maskreg;
  uint8_t _pcint_maskvalue;

  // Expressed as 4-cycle delays (must never be 0!)
  uint16_t _tx_delay;

  uint16_t _buffer_overflow:1;
  uint16_t _inverse_logic:1;

  // private methods
  void setTX(uint8_t transmitPin);

  // Return num - sub, or 1 if the result would be < 1
  static uint16_t subtract_cap(uint16_t num, uint16_t sub);

  // private static method for timing
  static inline void tunedDelay(uint16_t delay);

public:
  // public methods
  SoftSerial(uint8_t transmitPin, bool inverse_logic = false);
  ~SoftSerial();
  void begin(long speed);
  void end();
  bool overflow() { bool ret = _buffer_overflow; if (ret) _buffer_overflow = false; return ret; }
  int peek();

  virtual size_t write(uint8_t byte);
  virtual int read();
  virtual int available();
  virtual void flush();
  operator bool() { return true; }

  using Print::write;

};

/* Exported variables ****************************************************** */

/* Exported functions ****************************************************** */


// Arduino 0012 workaround
#undef int
#undef char
#undef long
#undef byte
#undef float
#undef abs
#undef round
#endif 
/* ***************************** END OF FILE ******************************* */
        
