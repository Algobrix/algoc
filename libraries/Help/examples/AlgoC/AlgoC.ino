/**
 * This documents gives brief list of all available functions.
 * 
 * For a full documentation please visit: https://youngeng.sharepoint.com/:f:/s/Franchise-Portal/En-N2A3VoMJLh0E5fzskb3UBG1fbPSWls3hZd_X4-FQsQA?e=GCDSbN
 */
/**
 * Motor related functions
 */
void move(System name, char motorPort, float seconds, float power, int direction, bool isBlocking);
void moveAB(System name, float seconds, float power, int direction, bool isBlocking);
void moveABC(System name, float seconds, float power, int direction, bool isBlocking);
void rotations(System name, char motorPort, float rotations, float power, int direction, bool isBlocking);
void rotationsAB(System name, float rotations, float power, int direction, bool isBlocking);
void rotationsABC(System name, float rotations, float power, int direction, bool isBlocking);
void stopMotor(System name,char motorPort);
void startCounting(System name,char motorPort, Number & parameter);
void stopCounting(System name, char motorPort);
bool isMotorBusy(System name, char motorPort);
void resistanceToStop(System name, char motorPort, float  threshold);

/**
 * Light related functions
 */
void light(System Name,int lightPort, float seconds,int power,char * color,bool isBlocking);
void light12(System name,float seconds,int power,char * color,bool isBlocking);
void RGB(System name,int lightPort, float seconds,int power,int R,int G,int B,bool isBlocking);
void RGB12(System name,int lightPort, float seconds,int power,int R,int G,int B,bool isBlocking);
void stopLight(System name, int lightPort);
bool isLightBusy(System name, int lightPort);

/**
 * Wait related functions
 */
void wait(System name,float seconds);

/**
 * Sensor related functions
 */
int waitSensor(System name,int sensorPort, int minSignalValue, int maxSignalValue);
void waitForPressSensor(System name,int sensorPort, bool logicState);
int getSensor(System name,int sensorPort);

/**
 * Sound related functions
 */
void playSound(System name,int sound,int power,bool isBlocking);
void stopSound(System name);
void listAvailableSounds(System name);
bool isSoundBusy(System name);

/**
 * Serial related functions
 */
void write(System name,Var line);
void writeLine(System name,Var line);
float enterNumber(System name);
char * enterString(System name);
char enterChar(System name);

/**
 * Play button related functions
 */
void playButtonReset(System name);
int waitPlayButtonPress(System name,int numberOfPresses);
bool getPlayButtonState(System name);

/**
 * Break point related functions
 */
uint8_t breakPoint(System name,char breakChar);

/**
 * Repeat syntax
 */
repeat(system,condition)
{
    // do something
}
