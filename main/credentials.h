#ifndef _CREDENTIALS_H_
#define _CREDENTIALS_H_

#include <EEPROM.h>
#include <Arduino.h>

extern void getAPIs(String &apis);
extern void setAPIs(String &apis);
extern void getSsId(String &ssid);
extern void setSsId(String &ssid);
extern void getPssk(String &pssk);
extern void setPssk(String &pssk);


#endif /* _CREDENTIALS_H_ */