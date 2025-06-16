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

const char CERTIFICATE[] PROGMEM = R"CERT(
-----BEGIN CERTIFICATE-----
MIIDpTCCA0ugAwIBAgIQfAwp5EhN0tkNy410DD/sjzAKBggqhkjOPQQDAjA7MQsw
CQYDVQQGEwJVUzEeMBwGA1UEChMVR29vZ2xlIFRydXN0IFNlcnZpY2VzMQwwCgYD
VQQDEwNXRTEwHhcNMjUwNjA2MTgwMjA0WhcNMjUwOTA0MTkwMjAxWjAXMRUwEwYD
VQQDEwxvbnJlbmRlci5jb20wWTATBgcqhkjOPQIBBggqhkjOPQMBBwNCAASqhq19
niZeSEXkw+fHQNdI+0Kb8+HTZ0cZ1v12VR4L4BsePBuFVv0Hrld7JwsONcS6LNpU
Skt0ueRT4AC32Un3o4ICUzCCAk8wDgYDVR0PAQH/BAQDAgeAMBMGA1UdJQQMMAoG
CCsGAQUFBwMBMAwGA1UdEwEB/wQCMAAwHQYDVR0OBBYEFCCpL0lsHK+mqnL3OpMH
JLh13h9TMB8GA1UdIwQYMBaAFJB3kjVnxP+ozKnme9mAeXvMk/k4MF4GCCsGAQUF
BwEBBFIwUDAnBggrBgEFBQcwAYYbaHR0cDovL28ucGtpLmdvb2cvcy93ZTEvZkF3
MCUGCCsGAQUFBzAChhlodHRwOi8vaS5wa2kuZ29vZy93ZTEuY3J0MCcGA1UdEQQg
MB6CDG9ucmVuZGVyLmNvbYIOKi5vbnJlbmRlci5jb20wEwYDVR0gBAwwCjAIBgZn
gQwBAgEwNgYDVR0fBC8wLTAroCmgJ4YlaHR0cDovL2MucGtpLmdvb2cvd2UxL3Qz
TEpiWmlCdHNVLmNybDCCAQIGCisGAQQB1nkCBAIEgfMEgfAA7gB0AN3cyjSV1+EW
BeeVMvrHn/g9HFDf2wA6FBJ2Ciysu8gqAAABl0afKbUAAAQDAEUwQwIfJXjIhpwc
EAcu/D5sIgq6ZJytHJtfqyaXIfZeZZAiiQIgaaNOuYUNaHBoynZk+v1JLfjvcOa3
Vlo1vtR3Ss3CGK4AdgAaBP9J0FQdQK/2oMO/8djEZy9O7O4jQGiYaxdALtyJfQAA
AZdGnyrkAAAEAwBHMEUCIQCxnabwRsrPpfAuj9npD4qGTEwXA8wpkVKslfxp+mEj
SAIgXeDsCV64uJo71hG0ueYnX9U22fybRJUfzmBDAnh9F0QwCgYIKoZIzj0EAwID
SAAwRQIhAPdm2rxGNZ72Lz8vf0w/JgBno1yGlTX6fRsuvZIKMLMQAiBI+28KH8Rh
4NVk/tjADsXiTmxKBWiFxgrAOKOmMUIOvQ==
-----END CERTIFICATE-----
)CERT";
