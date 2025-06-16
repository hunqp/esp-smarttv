#include "credentials.h"

#define DEFAULT_SSID_ADDR (0)
#define DEFAULT_PSSK_ADDR (20)
#define DEFAULT_UUID_ADDR (40)

void getUuId(String &uuid)
{
    char val;
    int i = DEFAULT_UUID_ADDR;

    do
    {
        val = EEPROM.read(i);
        if (val != 0)
        {
            uuid += char(val);
        }
        i++;
    } while (val != 0 && i < 20);
}

void setUuId(String &uuid)
{
    for (int i = 0; i < uuid.length(); ++i)
    {
        EEPROM.write(DEFAULT_UUID_ADDR + i, uuid[i]);
    }
    EEPROM.write(DEFAULT_UUID_ADDR + uuid.length(), 0);
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
    } while (val != 0 && i < 20);
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
    } while (val != 0 && i < 20);
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
