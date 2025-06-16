#ifndef _CREDENTIALS_H_
#define _CREDENTIALS_H_

#include <EEPROM.h>

extern void getUuId(String &uuid);
extern void setUuId(String &uuid);
extern void getSsId(String &ssid);
extern void setSsId(String &ssid);
extern void getPssk(String &pssk);
extern void setPssk(String &pssk);

#endif /* _CREDENTIALS_H_ */