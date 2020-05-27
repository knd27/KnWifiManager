/****************************************************************************************************************************
 * KnBlynk.h
 * For ESP8266 boards
 *
 * Wifi & Blynk Manager
 * + Ketika tidak bisa konek wifi (STA) maka akan mengktifkan captivePortal (AP) agar bisa menyeting Wifi & Blynk credential
 * + Bisa menambahkan Menu / Link pada root /
 * 
 * Fiur:
 *  1. Setup Wifi credential
 *  2. Select Next Boot : Normal (STA mode), AP (Portal mode) u/ seting Wifi
 *  3. Setting Blynk credential Saving
 *  4. Chip Info
 *  5. Update firmware
 *  6. Reset Hw
 *  7. Add Custom Link on Root /
 * 
 * Files: KnWiFiManager.h, KnWiFiManager.cpp
 * 
 * 05.02.2020   : Create
 *
 * 
 * 
******************************************************************************************************************************/

#ifndef KnWiFiManager_h
#define KnWiFiManager_h

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <memory>
#include <EEPROM.h>
#include <ESP8266HTTPUpdateServer.h>

extern "C"
{
#include "user_interface.h"
}

const char HTTP_HEADER[] PROGMEM = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><title>{v}</title>";
//const char HTTP_STYLE[] PROGMEM = "<style>.c{text-align: center;} div,input{padding:5px;font-size:1em;} input{width:95%;} body{text-align: center;font-family:verdana;} button{border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;} .q{float: right;width: 64px;text-align: right;} .l{background: url(\"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAMAAABEpIrGAAAALVBMVEX///8EBwfBwsLw8PAzNjaCg4NTVVUjJiZDRUUUFxdiZGSho6OSk5Pg4eFydHTCjaf3AAAAZElEQVQ4je2NSw7AIAhEBamKn97/uMXEGBvozkWb9C2Zx4xzWykBhFAeYp9gkLyZE0zIMno9n4g19hmdY39scwqVkOXaxph0ZCXQcqxSpgQpONa59wkRDOL93eAXvimwlbPbwwVAegLS1HGfZAAAAABJRU5ErkJggg==\") no-repeat left center;background-size: 1em;}</style>";
const char HTTP_STYLE[] PROGMEM = "<style>.c{text-align: center;} div,input{padding:5px;font-size:1em;} input{width:95%;} body{text-align: center;font-family:verdana;} button{border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;} .q{float: right;width: 64px;text-align: right;} .l{background: url(\"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAMAAABEpIrGAAAALVBMVEX///8EBwfBwsLw8PAzNjaCg4NTVVUjJiZDRUUUFxdiZGSho6OSk5Pg4eFydHTCjaf3AAAAZElEQVQ4je2NSw7AIAhEBamKn97/uMXEGBvozkWb9C2Zx4xzWykBhFAeYp9gkLyZE0zIMno9n4g19hmdY39scwqVkOXaxph0ZCXQcqxSpgQpONa59wkRDOL93eAXvimwlbPbwwVAegLS1HGfZAAAAABJRU5ErkJggg==\") no-repeat left center;background-size: 1em;} .btnx{border:0;border-radius:0.3rem;background-color:#89c403;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;}</style>";
const char HTTP_SCRIPT[] PROGMEM = "<script>function c(l){document.getElementById('s').value=l.innerText||l.textContent;document.getElementById('p').focus();}</script>";
const char HTTP_HEADER_END[] PROGMEM = "</head><body><div style='text-align:center;display:inline-block;min-width:360px;'>";
//const char HTTP_PORTAL_OPTIONS[] PROGMEM = "<form action=\"/wifi\" method=\"get\"><button>Configure WiFi</button></form><br/><form action=\"/0wifi\" method=\"get\"><button>Configure WiFi (No Scan)</button></form><br/><form action=\"/blynk\" method=\"get\"><button>Blynk</button></form><br/><form action=\"/i\" method=\"get\"><button>Info</button></form><br/><form action=\"/r\" method=\"post\"><button>Reset</button></form>";
const char HTTP_PORTAL_OPTIONS[] PROGMEM = "<form action=\"/wifi\" method=\"get\"><button>Configure WiFi</button></form><br/><form action=\"/blynk\" method=\"get\"><button>Configure Blynk</button></form><br/><form action=\"/i\" method=\"get\"><button>Info</button></form><br/><form action=\"/r\" method=\"post\"><button>Reset</button></form>";
const char HTTP_ITEM[] PROGMEM = "<div><a href='#p' onclick='c(this)'>{v}</a>&nbsp;<span class='q {i}'>{r}%</span></div>";
const char HTTP_FORM_START[] PROGMEM = "<form method='get' action='wifisave'><input id='s' name='s' length=32 placeholder='SSID'><br/><input id='p' name='p' length=64 type='password' placeholder='password'><br/>";
const char HTTP_FORM_PARAM[] PROGMEM = "<br/><input id='{i}' name='{n}' maxlength={l} placeholder='{p}' value='{v}' {c}>";
const char HTTP_SCAN_LINK[] PROGMEM = "<br/><div class=\"c\"><a href=\"/wifi\">Scan</a></div>";
const char HTTP_SAVED[] PROGMEM = "<div>Credentials Saved<br />Trying to connect ESP to network.<br />If it fails reconnect to AP to try again</div>";
const char HTTP_END[] PROGMEM = "</div></body></html>";

const char AHTTP_FORM_BOOT[] PROGMEM = "<form method='post' action='/bootsave'><select name='cboot'><option value='sta' {sta}>Normal Mode</option><option value='ap' {ap}>AP Mode : Wifi Manager</option></select><br/><br/>";
const char AHTTP_FORM_BLYNK[] PROGMEM = "<form method='post' action='blynksave'><input id='btoken' name='btoken' length=32 value='{t}' placeholder='Blynk Token'><br/><input id='bsrv' name='bsrv' length=32  value='{s}' placeholder='Blynk Server'><br/><input id='bport' name='bport' length=4 type='number'  value='{p}' placeholder='Port'><br/>";
const char AHTTP_FORM_RESET[] PROGMEM = "<form method='post' action='/doreset'><br/>";
const char AHTTP_FORM_PIN[] PROGMEM = "<input id='bpin' name='bpin' length=6 type='password'  placeholder='6 digit PIN'><br/>";
const char AHTTP_FORM_PARAM[] PROGMEM = "<br/><input id='{i}' name='{n}' maxlength={l} placeholder='{p}' value='{v}' type='{t}' {c}>";
const char HTTP_FORM_END[] PROGMEM = "<br/><button type='submit'>Save</button></form><br/><form action=\"/\" method=\"get\"><button>Main Menu</button></form>";
const char AHTTP_ADD_MENU[] PROGMEM = "<br/><form action=\"{l}\" method=\"get\"><button class=\"btnx\">{t}</button></form>";

const String PIN_NUMBER = "272727";

#define EEPROM_SALT 270279

typedef struct
{
    boolean bootAP = 0;
    char blynkToken[33] = "";
    char blynkServer[33] = "multinet.id";
    char blynkPort[6] = "8080";
    int salt = EEPROM_SALT;
} KnSettings;

class KnWiFiManager
{
public:
    KnWiFiManager();
    ~KnWiFiManager();

    boolean isBootAP();
    char *getblynkServer();
    char *getblynkToken();
    int getblynkPort();
    void setBootAP(boolean bootAP);
    void run();

    typedef std::function<void(void)> THandlerFunction;
    void on(const String &uri, THandlerFunction handler);
    void on(const String &uri, THandlerFunction handler, const String &title);
    void send(int code, char *content_type, const String &content);

    /***************************************************************************/
    boolean autoConnect();
    boolean autoConnect(char const *apName, char const *apPassword = NULL);

    //if you want to always start the config portal, without trying to connect first
    boolean startConfigPortal();
    boolean startConfigPortal(char const *apName, char const *apPassword = NULL);

    //sets timeout before webserver loop ends and exits even if there has been no setup.
    //useful for devices that failed to connect at some point and got stuck in a webserver loop
    //in seconds setConfigPortalTimeout is a new name for setTimeout
    void setConfigPortalTimeout(unsigned long seconds);
    void setTimeout(unsigned long seconds);
    void setAppName(const String &val);

    //called when AP mode and config portal is started
    void setAPCallback(void (*func)(KnWiFiManager *));
    //called when settings have been changed and connection was successful
    void setSaveConfigCallback(void (*func)(void));

private:
    std::unique_ptr<DNSServer> dnsServer;
    ESP8266WebServer server;
    ESP8266HTTPUpdateServer httpUpdater;

    KnSettings _knsettings;
    std::vector<String> _linkuri;
    std::vector<String> _linktitle;

    void readSetting();
    void writeSetting();
    boolean applyNewWifiConfig();

    /************************************************************************/
    void setupConfigPortal();
    void prepareserver();

    String _AppName = "~ MULTINET ~";
    const char *_apName = "MULTINET";
    const char *_apPassword = NULL;
    String _ssid = "";
    String _pass = "";
    unsigned long _configPortalTimeout = 0;
    unsigned long _connectTimeout = 0;
    unsigned long _configPortalStart = 0;

    int _paramsCount = 0;
    int _minimumQuality = -1;
    boolean _removeDuplicateAPs = true;
    boolean _shouldBreakAfterConfig = false;

    int connectWifi(String ssid, String pass);
    uint8_t waitForConnectResult();

    const char *_customHeadElement = "";

    void handleRoot();
    void handleWifi(boolean scan);
    void handleWifiSave();
    void handleNotFound();
    void handle_Blynk();
    void handle_BlynkSave();
    void handle_Info();
    void handle_Reset();
    void handle_DoReset();
    void handle_BootOpt();
    void handle_BootOptSave();

    boolean captivePortal();
    boolean configPortalHasTimeout();

    // DNS server
    const byte DNS_PORT = 53;

    //helpers
    int getRSSIasQuality(int RSSI);
    boolean isIp(String str);
    String toStringIp(IPAddress ip);

    boolean connect;
    boolean _debug = true;

    void (*_apcallback)(KnWiFiManager *) = NULL;
    void (*_savecallback)(void) = NULL;

    template <typename Generic>
    void DEBUG_WM(Generic text);

    template <class T>
    auto optionalIPFromString(T *obj, const char *s) -> decltype(obj->fromString(s))
    {
        return obj->fromString(s);
    }
    auto optionalIPFromString(...) -> bool
    {
        DEBUG_WM("NO fromString METHOD ON IPAddress, you need ESP8266 core 2.1.0 or newer for Custom IP configuration to work.");
        return false;
    }
};

#endif
