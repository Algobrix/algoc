/* Define to prevent recursive inclusion *********************************** */
#ifndef __SOUNDPLAYER_H
#define __SOUNDPLAYER_H

/* Includes **************************************************************** */
#include <Arduino.h>
#include <softserial.h>
#include <algothread.h>
#include <system.h>

/* Exported constants ****************************************************** */
enum ALGOSOUND_STATE
{
    ALGOSOUND_STATE_IDLE = 0x00,
    ALGOSOUND_STATE_PLAYING,
};

enum ALGOSOUND_STATUS
{
	ALGOSOUND_STATUS_INIT = 0x00,
	ALGOSOUND_STATUS_RUNNING = 0x01,
	ALGOSOUND_STATUS_COMPLETED = 0x00
};
/* Exported macros ********************************************************* */

/* Exported types ********************************************************** */
class AlgoSound 
{
    private:
        SoftSerial soundSerial;
        String getTrackCommand(byte trackNumber);

    public:
        byte scriptRowId = 0;
        uint8_t status;
		uint8_t state;
        uint32_t timer;
        byte currentTrack = 0;
        AlgoSound(const byte txPin, const byte statePin);
        void play(byte trackId);
        void stop();
        void setVolume(int volumeLevel);
        void pause();
        uint8_t getStatus(void);
};


/* Exported variables ****************************************************** */
extern AlgoSound soundPlayer;

/* Exported functions ****************************************************** */
void playSound(System name,int sound,int power,bool isBlocking);
void stopSound(System name);
void stopSound();
bool isSoundBusy(System name);
#endif 
/* ***************************** END OF FILE ******************************* */

