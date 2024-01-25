/* Includes **************************************************************** */
#include <Arduino.h>
#include "algosound.h"
#include "GoAlgo.h"
#include "systim.h"
/* #include "comHandler.h" */

/* Private constants ******************************************************* */


/* Private macros ********************************************************** */

/* Private types *********************************************************** */

/* Private variables ******************************************************* */
AlgoSound soundPlayer(SOUND_TX_PIN, SOUND_STATE_PIN);

const char c_volume_char[10] = {'B', 'D', 'E', 'I', 'K', 'L', 'M', 'P', 'T', 't'}; // 'B'(LOWEST)...'t'(HIGHEST-DEFAULT)
/* Private function prototypes ********************************************* */

/* Exported functions ****************************************************** */
AlgoSound::AlgoSound(const byte txPin, const byte statePin) 
    : soundSerial(txPin) 
{
    pinMode(txPin, OUTPUT);
    pinMode(statePin, INPUT);
    soundSerial.begin(115200);
    setVolume(10);
}
void AlgoSound::play(byte trackId) 
{
    // Create the trackId that we got.
    if(trackId != 255) 
    {
        if(getStatus() == ALGOSOUND_STATE_IDLE) 
        {
            String trackCommand = getTrackCommand(trackId);
            this->currentTrack = trackId;
            this->timer = getSYSTIM();
            this->state = ALGOSOUND_STATE_PLAYING;
            this->soundSerial.println(trackCommand);
        }
    }
    else 
    {
        stop();
    }
}
void AlgoSound::stop(void) 
{

    String trackCommand = getTrackCommand(this->currentTrack);
	this->state = ALGOSOUND_STATE_IDLE;
	delay(10);
    if(getStatus() == ALGOSOUND_STATE_PLAYING) 
    {
        this->soundSerial.println(trackCommand); // If a track is playing, we send it again to stop it.
        // this->soundSerial.println(trackCommand); // If a track is playing, we send it again to stop it.
    }
    // this->currentTrack = 0;
}

void AlgoSound::pause(void) 
{
    /* String trackCommand = {0x7e, 0x02, 0x02, 0xef}; */
	delay(10);
    if(getStatus() == ALGOSOUND_STATE_PLAYING) 
    {
        /* this->soundSerial.println(trackCommand); */ 
    }
}

void AlgoSound::setVolume(int volumeLevel) 
{
    soundSerial.println(c_volume_char[volumeLevel]);
}

String AlgoSound::getTrackCommand(byte trackNumber) 
{
    // The command that we get is:
    // p 00xx --> xx = number of the file
    String command = "p ";
    // Create starting 0's
    // Example: trackId is 10 (size of 2 chars\digits) the command will be "p 0010"
    for(int i = String(trackNumber).length(); i < 4; i++) 
    {
        command += "0";
    }
    command += String(trackNumber);
    return command;
}
uint8_t AlgoSound::getStatus(void) 
{
    // Short delay to let the state pin go high
    if (chk4TimeoutSYSTIM(timer,100) == SYSTIM_TIMEOUT) 
    {
        state  = (digitalRead(SOUND_STATE_PIN) == LOW) ? ALGOSOUND_STATE_PLAYING : ALGOSOUND_STATE_IDLE;
    }
    return state;
}

void playSound(System name,int sound,int power,bool isBlocking) 
{
	while(g_ALGOBOT_INFO.state == ALGOBOT_STATE_PAUSE)
	{
		yield();
	}

	if(name.cthread.sequance != name.sequance)
	{
		return;
	}

	if(g_ALGOBOT_INFO.state != ALGOBOT_STATE_RUN)
	{
		return;
	}

	switch(soundPlayer.status)
	{
		case (ALGOSOUND_STATUS_INIT):
		{
			Serial.print(F("Playing the sound ["));
			Serial.print(sound);
			Serial.print(F("] on line ["));
			Serial.print(name.line);
			Serial.print(F("] with Power ["));
			Serial.print(power);
			Serial.println(F("]"));
			soundPlayer.setVolume(power);
			soundPlayer.play(sound);
			soundPlayer.state = ALGOSOUND_STATE_PLAYING;
			soundPlayer.timer = getSYSTIM();
			if(isBlocking == OP_STATUS_BLOCKING)
			{
				if(&name.cthread == &threadAlgoC)
				{
					while(soundPlayer.getStatus() == ALGOSOUND_STATE_PLAYING)
					{
						yield();
						if(g_ALGOBOT_INFO.state != ALGOBOT_STATE_RUN)
						{
							stopActuators();
							name.cthread.sequance++;
							soundPlayer.status = ALGOSOUND_STATUS_INIT;
							return;
						}
					}
					soundPlayer.status = ALGOSOUND_STATUS_INIT;
					name.cthread.sequance++;
					return;
				}
				else
				{
					soundPlayer.status = ALGOSOUND_STATUS_RUNNING;
					return;
				}
			}
			else
			{
				name.cthread.sequance++;
				return;
			}
			break;

		}
		case (ALGOSOUND_STATUS_RUNNING):
		{
			if(soundPlayer.getStatus() == ALGOSOUND_STATE_IDLE)
			{
				soundPlayer.status = ALGOSOUND_STATUS_INIT;
				name.cthread.sequance++;
				return;
			}
			return;
			break;
		}
	}

}

void stopSound(System name)
{
    soundPlayer.stop();
}

void stopSound(void)
{
    soundPlayer.stop();
}

bool isSoundBusy(System name)
{
	if(name.cthread.sequance != name.sequance)
	{
		return false;
	}

	name.cthread.sequance++;
	soundPlayer.state = soundPlayer.getStatus();
	if(soundPlayer.state != ALGOSOUND_STATE_PLAYING)
	{
		return false;
	}
	else
	{
		return true;

	}
	return false;

}

/* Private functions ******************************************************* */

/* ***************************** END OF FILE ******************************* */
