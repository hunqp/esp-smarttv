#include "credentials.h"

#define DEFAULT_SSID_ADDR (0)
#define DEFAULT_PSSK_ADDR (21)
#define DEFAULT_APIS_ADDR (45)

void getAPIs(String &apis) {
    char val;
    int i = DEFAULT_APIS_ADDR;

    do
    {
        val = EEPROM.read(i);
        if (val != 0)
        {
            apis += char(val);
        }
        i++;
    } while (val != 0 && i < (DEFAULT_APIS_ADDR + 128));
}

void setAPIs(String &apis) {
    for (int i = 0; i < apis.length(); ++i)
    {
        EEPROM.write(DEFAULT_APIS_ADDR + i, apis[i]);
    }
    EEPROM.write(DEFAULT_APIS_ADDR + apis.length(), 0);
    EEPROM.commit();
}

void getSsId(String &ssid)
{
    char val;
    int i = DEFAULT_SSID_ADDR;

    do
    {
        val = EEPROM.read(i);
        if (val != 0)
        {
            ssid += char(val);
        }
        i++;
    } while (val != 0 && i < (DEFAULT_SSID_ADDR + 20));
}

void setSsId(String &ssid)
{
    for (int i = 0; i < ssid.length(); ++i)
    {
        EEPROM.write(DEFAULT_SSID_ADDR + i, ssid[i]);
    }
    EEPROM.write(DEFAULT_SSID_ADDR + ssid.length(), 0);
    EEPROM.commit();
}

void getPssk(String &pssk)
{
    char val;
    int i = DEFAULT_PSSK_ADDR;

    do
    {
        val = EEPROM.read(i);
        if (val != 0)
        {
            pssk += char(val);
        }
        i++;
    } while (val != 0 && i < (DEFAULT_PSSK_ADDR + 20));
}

void setPssk(String &pssk)
{
    for (int i = 0; i < pssk.length(); ++i)
    {
        EEPROM.write(DEFAULT_PSSK_ADDR + i, pssk[i]);
    }
    EEPROM.write(DEFAULT_PSSK_ADDR + pssk.length(), 0);
    EEPROM.commit();
}

