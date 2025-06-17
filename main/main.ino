#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <TFT_eSPI.h>
#include <ArduinoJson.h>
#include "wallpaper.h"
#include "credentials.h"

#define LIST_WIFIS_SIZE (64)
#define DEFAULT_AP_SSID (char *)"AP_VF_MEDUSA"
#define DEFAULT_AP_PSSK (char *)"88888888"

static String APIS = "";
static String SSID = "";
static String PSSK = "";
static ESP8266WebServer wserver(80);
static bool bInternetConnected = false;
static char ListWiFis[LIST_WIFIS_SIZE][64];
static TFT_eSPI gui = TFT_eSPI(WALLPAPER_WID, WALLPAPER_HEI);

static void WiFiSetup();
static void RenderGuiCalls();
static void TimeServicesCalls();

static void ImplSrvSideLogin();
static void ImplSrvSideLoading();
static void ImplSrvSideConnected();
static void ImplSrvSideConnecting();
static void ImplSrvSideDisconnected();

void setup(void)
{
    const int TFT_BL_Pin = 5;

    Serial.begin(115200);
    pinMode(TFT_BL_Pin, OUTPUT);
    digitalWrite(TFT_BL_Pin, LOW);
    gui.begin();
    gui.fillScreen(TFT_BLACK);
    gui.setSwapBytes(true);
    gui.pushImage(0, 0, WALLPAPER_WID, WALLPAPER_HEI, WALLPAPER_BITMAP);

    // gui.fillScreen(TFT_GREY);
    // gui.setCursor(0, 0, 2);
    // gui.setTextColor(TFT_WHITE,TFT_BLACK);  tft.setTextSize(1);
    // // We can now plot text on screen using the "print" class
    // gui.println("Hello World!");

    // gui.setTextColor(TFT_YELLOW); tft.setTextFont(7);
    // gui.println(1234.56);

    EEPROM.begin(512);
    delay(1000);
    WiFiSetup();

    wserver.on("/", ImplSrvSideLogin);
    wserver.on("/loading", HTTP_POST, ImplSrvSideLoading);
    wserver.on("/connected", ImplSrvSideConnected);
    wserver.on("/connecting", ImplSrvSideConnecting);
    wserver.on("/disconnected", ImplSrvSideDisconnected);
    wserver.begin();

    /* Set time via NTP, as required for x.509 validation */
    configTime(7 * 3600, 0, "pool.ntp.org", "time.nist.gov");
}

void loop()
{
    static uint32_t nowms = 0;
    wserver.handleClient();

    if (bInternetConnected)
    {
        RenderGuiCalls();
        TimeServicesCalls();
    }
}

static void apEntrance(const char *ssid, const char *pssk)
{
    Serial.println();
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    WiFi.mode(WIFI_AP);
    Serial.println("Entrance ACCESS POINT mode");
    Serial.printf("AP SSID: %s\r\n", ssid);
    Serial.printf("AP PSSK: %s\r\n", pssk);
    WiFi.softAP(ssid, pssk);
    Serial.print("IP address: ");
    Serial.println(WiFi.softAPIP());
}

static bool staEntrance(const char *ssid, const char *pssk)
{
    WiFi.begin(ssid, pssk);
    int i = 15;
    Serial.printf("Connecting to %s with password\n", ssid);
    while (i > 0)
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            Serial.println("WiFi connected");
            Serial.print("IP address: ");
            Serial.println(WiFi.localIP().toString());
            return true;
        }
        delay(1000);
        Serial.println(i);
        --i;
    }
    Serial.println("STA TIMEOUT");
    return false;
}

static bool IsASCII(const char *str, int strLen)
{
    for (int i = 0; i < strLen; ++i)
    {
        if (str[i] < 33 || str[i] > 127)
        {
            return false;
        }
    }
    return true;
}

static int wiFiScanner()
{
    String ssid;
    int32_t rssi;
    uint8_t encryptionType;
    uint8_t *bssid;
    int32_t channel;
    bool hidden;
    int scanResult;

    memset(ListWiFis, 0, sizeof(ListWiFis));
    scanResult = WiFi.scanNetworks(false, true);
    for (int8_t i = 0; i < scanResult; i++)
    {
        WiFi.getNetworkInfo(i, ssid, encryptionType, rssi, bssid, channel, hidden);
        yield();
        Serial.println(ssid);
        if (i < LIST_WIFIS_SIZE)
        {
            strcpy(ListWiFis[i], ssid.c_str());
        }
    }
    return scanResult;
}

void WiFiSetup()
{
    getSsId(SSID);
    getPssk(PSSK);
    getAPIs(APIS);
    Serial.printf("[PREFETCH] SSID: %s\r\n", SSID.c_str());
    Serial.printf("[PREFETCH] PSSK: %s\r\n", PSSK.c_str());
    Serial.printf("[PREFETCH] APIS: %s\r\n", APIS.c_str());

    if (IsASCII(SSID.c_str(), SSID.length()) && IsASCII(PSSK.c_str(), PSSK.length()))
    {
        if (staEntrance(SSID.c_str(), PSSK.c_str()))
        {
            bInternetConnected = true;
            return;
        }
    }

    SSID = DEFAULT_AP_SSID;
    PSSK = DEFAULT_AP_PSSK;
    apEntrance(SSID.c_str(), PSSK.c_str());
}

void ImplSrvSideLogin()
{
    Serial.println("ImplSrvSideLogin");

    int n = wiFiScanner();
    String HomePage = R"rawliteral(
        <!DOCTYPE html>
        <html>
        <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1">
            <title>Wi-Fi Setup</title>
            <style>
            body { font-family: 'Segoe UI', sans-serif; background: #f4f6f9; color: #333; margin: 0; padding: 0; }
            .container { max-width: 400px; margin: 50px auto; padding: 20px; background: #fff; border-radius: 12px; box-shadow: 0 4px 12px rgba(0,0,0,0.1); }
            h2 { text-align: center; color: #305680; }
            label { font-weight: bold; margin-top: 12px; display: block; }
            select, input { width: 100%; padding: 10px; margin-top: 5px; border-radius: 6px; border: 1px solid #ccc; font-size: 16px; box-sizing: border-box; }
            button { width: 100%; padding: 12px; margin-top: 20px; background-color: #305680; color: white; border: none; border-radius: 6px; font-size: 16px; cursor: pointer; }
            button:hover { background-color: #274666; }
            </style>
        </head>
        <body>
            <div class="container">
                <h2>Wi-Fi Setup</h2>
                <form method='POST' action='/connecting' onsubmit="saveCreds()">
                    <label for='id'>HTTP GET</label>
    )rawliteral";

    if (APIS.length() && IsASCII(APIS.c_str(), APIS.length()))
    {
        HomePage += "<input type='text' name='apis' id='apis' value='" + APIS + "'>";
    }
    else
    {
        HomePage += "<input type='text' name='apis' id='apis'>";
    }

    HomePage += R"rawliteral(
                    <label for='ssid'>Select Wi-Fi</label>
                    <select name='ssid' id='ssid'>
    )rawliteral";

    for (int i = 0; i < n; ++i)
    {
        HomePage += "<option value='" + String(ListWiFis[i]) + "'>" + String(ListWiFis[i]) + "</option>";
    }

    HomePage += R"rawliteral(
                    </select>
                    <label for="password">Password</label>
                    <div style="position: relative;">
                        <input type="password" name="password" id="password" required>
                        <span onclick="togglePassword()" style="position: absolute; right: 10px; top: 50%; transform: translateY(-50%); cursor: pointer;">👁️</span>
                    </div>
                    <button type="submit">Connect</button>
                </form>
            </div>
            <script>
                function togglePassword() {
                    const input = document.getElementById('password');
                    input.type = input.type === 'password' ? 'text' : 'password';
                }
                function saveCreds() {
                    const apis = document.getElementById('apis').value;
                    const ssid = document.getElementById('ssid').value;
                    const password = document.getElementById('password').value;
                    sessionStorage.setItem("apis", apis);
                    sessionStorage.setItem("ssid", ssid);
                    sessionStorage.setItem("password", password);
                }
            </script>
        </body></html>
    )rawliteral";

    wserver.send(200, "text/html", HomePage);
}

void ImplSrvSideConnecting()
{
    Serial.println("ImplSrvSideConnecting");

    String LoadingPage = R"rawliteral(
        <!DOCTYPE html><html><head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>WiFi Connecting</title>
            <style>
                body { font-family: 'Segoe UI', sans-serif; background: #f4f6f9; color: #305680; text-align: center; padding-top: 100px; }
                .spinner { margin: 20px auto; width: 40px; height: 40px; border: 4px solid #ccc; border-top: 4px solid #305680; border-radius: 50%; animation: spin 1s linear infinite; }
                @keyframes spin { 0% { transform: rotate(0deg); } 100% { transform: rotate(360deg); } }
            </style>
            <script>
                function checkStatus() {
                    fetch('/loading', {
                        method: 'POST',
                        headers: { 'Content-Type': 'application/json' },
                        body: JSON.stringify({
                            apis: sessionStorage.getItem("apis"),
                            ssid: sessionStorage.getItem("ssid"),
                            password: sessionStorage.getItem("password")
                        })
                    }).then(res => res.json()).then(data => {
                        if (data.connected === true) {
                            window.location.href = "/connected";
                        } else {
                            window.location.href = "/disconnected";
                        }
                    });
                }
                window.onload = function() {
                    setTimeout(checkStatus, 1000);
                };
            </script>
        </head><body>
            <h2>Connecting to Wi-Fi</h2>
            <div class="spinner"></div>
            <p>Please wait a moment</p>
        </body></html>
    )rawliteral";

    wserver.send(200, "text/html", LoadingPage);
}

void ImplSrvSideLoading()
{
    Serial.println("ImplSrvSideLoading");

    StaticJsonDocument<200> doc;
    deserializeJson(doc, wserver.arg("plain"));

    String apis = doc["apis"] | "";
    String StaSsId = doc["ssid"] | "";
    String StaPssk = doc["password"] | "";

    if (apis.length())
    {
        APIS = apis;
        setAPIs(APIS);
    }

    Serial.printf("Connecting to SSID: %s, PSSK: %s\n", StaSsId.c_str(), StaPssk.c_str());

    StaticJsonDocument<200> response;

    if (staEntrance(StaSsId.c_str(), StaPssk.c_str()))
    {
        response["connected"] = true;
        bInternetConnected = true;

        /* Save credentials */
        setSsId(StaSsId);
        setPssk(StaPssk);
        SSID = StaSsId;
        PSSK = StaPssk;
    }
    else
    {
        response["connected"] = false;
    }

    String json;
    serializeJson(response, json);
    wserver.send(200, "application/json", json);
}

void ImplSrvSideConnected()
{
    Serial.println("ImplSrvSideConnected");

    String page = R"rawliteral(
        <!DOCTYPE html><html><head>
        <meta charset="UTF-8"><meta name="viewport" content="width=device-width, initial-scale=1">
        <title>Connected</title>
        <style>
            body { font-family: 'Segoe UI', sans-serif; background: #f4f6f9; color: #305680; text-align: center; padding-top: 100px; }
            .status { font-size: 24px; font-weight: bold; }
            .ip { margin-top: 10px; color: #333; font-size: 18px; }
        </style>
        </head><body>
            <div class="status">Connected to Wi-Fi!</div>
            <div class="ip">IP Address: %IP%</div>
        </body></html>
    )rawliteral";

    page.replace("%IP%", WiFi.localIP().toString());
    if (bInternetConnected)
    {
        wserver.send(200, "text/html", page);
        delay(1000);
        WiFi.softAPdisconnect(true);
    }
}

void ImplSrvSideDisconnected()
{
    Serial.println("ImplSrvSideDisconnected");

    String page = R"rawliteral(
        <!DOCTYPE html><html><head>
        <meta charset="UTF-8"><meta name="viewport" content="width=device-width, initial-scale=1">
        <title>Connection Failed</title>
        <style>
            body { font-family: 'Segoe UI', sans-serif; background: #f4f6f9; color: #c0392b; text-align: center; padding-top: 100px; }
            .status { font-size: 24px; font-weight: bold; }
            .retry { margin-top: 20px; }
            a { text-decoration: none; color: #305680; font-weight: bold; }
        </style>
        </head><body>
            <div class="status">Failed to Connect</div>
            <div class="retry"><a href="/">Try Again</a></div>
        </body></html>
    )rawliteral";

    wserver.send(200, "text/html", page);
}

void RenderGuiCalls()
{
    static uint32_t lastTime = 0;

    if (millis() - lastTime > 10000)
    {
        lastTime = millis();

        HTTPClient https;
        std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
        client->setFingerprint("64:51:B0:DA:30:C9:FB:16:C5:4C:95:9B:49:AB:07:EE:CF:10:EB:4E"); /* See helpers.txt to get fingerprint of URL */

        Serial.printf("API Called: %s\r\n", APIS.c_str());

        if (https.begin(*client, APIS))
        {
            int ret = https.GET();
            Serial.printf("[HTTPS] Method \'GET\' return code: %d\n", ret);
            if (ret > 0)
            {
                if (ret == HTTP_CODE_OK || ret == HTTP_CODE_MOVED_PERMANENTLY)
                {
                    String payload = https.getString();
                    Serial.println(payload);
                }
            }
            https.end();
        }
        else
        {
            Serial.printf("[HTTPS] Unable to connect %s\n", APIS.c_str());
        }
    }
}

static std::string utlsTimestampToFormatted(uint32_t ts, char *formatted) {
	char chars[64];
	time_t timeStamp = (time_t)ts;
	struct tm *timin = localtime(&timeStamp);
	strftime(chars, sizeof(chars), formatted, timin);
	return std::string(chars, strlen(chars));
}

void TimeServicesCalls()
{
    static uint32_t lastTime = 0;

    if (millis() - lastTime > 5000)
    {
        lastTime = millis();

        time_t now = time(NULL);
        /* Convert to local time (assuming UTC+7) */
        struct tm *ptm = localtime(&now);
        int yy = ptm->tm_year + 1900;
        int mo = ptm->tm_mon + 1;
        int dd = ptm->tm_mday;
        int hh = ptm->tm_hour;
        int mm = ptm->tm_min;
        int ss = ptm->tm_sec;

        Serial.printf("[NTP] %02d/%02d/%04d - %02d:%02d:%02d\r\n", dd, mo, yy, hh, mm, ss);

        gui.setCursor(179, 97, 2);
        gui.setTextSize(3);
        gui.println(dd);

        char chars[16];
        memset(chars, 0, sizeof(chars));
        sprintf(chars, "%02d.%04d", mo, yy);
        gui.setCursor(171, 76, 2);
        gui.setTextSize(1);
        gui.println(chars);
    }
}
