#include "KnWiFiManager.h"

KnWiFiManager::KnWiFiManager()
{
    readSetting();
}

KnWiFiManager::~KnWiFiManager()
{
}

char *KnWiFiManager::getblynkServer()
{
    return _knsettings.blynkServer;
}

char *KnWiFiManager::getblynkToken()
{
    readSetting();
    return _knsettings.blynkToken;
}

int KnWiFiManager::getblynkPort()
{
    return atoi(_knsettings.blynkPort);
}

void KnWiFiManager::readSetting()
{
    EEPROM.begin(512);
    EEPROM.get(0, _knsettings);
    EEPROM.end();

    Serial.println(F("Read setting from EEPROM"));
    Serial.print(F("Boot: "));
    Serial.println(_knsettings.bootAP);
    Serial.print(F("Token: "));
    Serial.println(_knsettings.blynkToken);
    Serial.print(F("Server: "));
    Serial.println(_knsettings.blynkServer);
    Serial.print(F("Port: "));
    Serial.println(_knsettings.blynkPort);
}

void KnWiFiManager::writeSetting()
{
    EEPROM.begin(512);
    EEPROM.put(0, _knsettings);
    delay(500);
    EEPROM.commit();
    //EEPROM.end();
    delay(500);
    Serial.println(F("write setting to EEPROM"));
}

boolean KnWiFiManager::isBootAP()
{
    readSetting();
    return _knsettings.bootAP;
}

void KnWiFiManager::setBootAP(boolean bootAP)
{
    _knsettings.bootAP = bootAP;
    writeSetting();
}

void KnWiFiManager::run()
{
    server.handleClient();
}

void KnWiFiManager::on(const String &uri, THandlerFunction handler)
{
    on(uri, handler, uri);
}

void KnWiFiManager::on(const String &uri, THandlerFunction handler, const String &title)
{
    server.on(uri, handler);
    _linkuri.push_back(uri);
    _linktitle.push_back(title);
}

void KnWiFiManager::send(int code, char *content_type, const String &content)
{
    server.send(code, content_type, content);
}

void KnWiFiManager::handle_Blynk()
{
    String page = FPSTR(HTTP_HEADER);
    page.replace("{v}", "Blynk");
    page += FPSTR(HTTP_SCRIPT);
    page += FPSTR(HTTP_STYLE);
    //page += _customHeadElement;
    page += FPSTR(HTTP_HEADER_END);
    page += String(F("<h1>"));
    page += _apName;
    page += String(F("</h1>"));
    page += String(F("<h3>Blynk Config</h3>"));

    readSetting();
    String item = FPSTR(AHTTP_FORM_BLYNK);
    item.replace("{t}", _knsettings.blynkToken);
    item.replace("{s}", _knsettings.blynkServer);
    item.replace("{p}", _knsettings.blynkPort);
    page += item;

    page += FPSTR(AHTTP_FORM_PIN);
    page += FPSTR(HTTP_FORM_END);
    page += FPSTR(HTTP_END);

    server.sendHeader("Content-Length", String(page.length()));
    server.send(200, "text/html", page);
}

void KnWiFiManager::handle_BlynkSave()
{
    String msg = F("<center><br>");
    if (server.arg("bpin") != PIN_NUMBER)
    {
        msg += F("Oops...<br>PIN salah !");
    }
    else
    {
        String s;
        s = server.arg("bsrv"); // Srv
        s.toCharArray(_knsettings.blynkServer, s.length() + 1);
        s = server.arg("btoken"); // token
        s.toCharArray(_knsettings.blynkToken, 33);
        s = server.arg("bport"); //
        s.toCharArray(_knsettings.blynkPort, 6);
        s.trim();
        writeSetting();
        //
        readSetting();
        msg += F("Konfigurasi Tersimpan<br>Lakukan Reboot");
    }

    msg += F("<br><a href='/blynk'>back</a>");
    server.sendHeader("Content-Length", String(msg.length()));
    server.send(200, "text/html", msg); //Send web page
}

/** Handle the info page */
void KnWiFiManager::handle_Info()
{
    String page = FPSTR(HTTP_HEADER);
    page.replace("{v}", "Info");
    page += FPSTR(HTTP_SCRIPT);
    page += FPSTR(HTTP_STYLE);
    //page += _customHeadElement;
    page += FPSTR(HTTP_HEADER_END);
    page += String(F("<h1>MULTINET</h1>"));
    page += String(F("<h3>Hardware Information</h3>"));

    page += F("<dl>");
    page += F("<dt>Chip ID</dt><dd>");
    page += ESP.getChipId();
    page += F("</dd>");
    page += F("<dt>Flash Chip ID</dt><dd>");
    page += ESP.getFlashChipId();
    page += F("</dd>");
    page += F("<dt>IDE Flash Size</dt><dd>");
    page += ESP.getFlashChipSize();
    page += F(" bytes</dd>");
    page += F("<dt>Real Flash Size</dt><dd>");
    page += ESP.getFlashChipRealSize();
    page += F(" bytes</dd>");
    page += F("<dt>Soft AP IP</dt><dd>");
    page += WiFi.softAPIP().toString();
    page += F("</dd>");
    page += F("<dt>STA IP</dt><dd>");
    page += WiFi.localIP().toString();
    page += F("</dd>");
    page += F("<dt>Soft AP MAC</dt><dd>");
    page += WiFi.softAPmacAddress();
    page += F("</dd>");
    page += F("<dt>Station MAC</dt><dd>");
    page += WiFi.macAddress();
    page += F("</dd>");
    page += F("</dl>");
    page += F("<br/><form action=\"/\" method=\"get\"><button>Main Menu</button></form>");
    page += FPSTR(HTTP_END);

    server.sendHeader("Content-Length", String(page.length()));
    server.send(200, "text/html", page);
}

void KnWiFiManager::handle_Reset()
{
    String page = FPSTR(HTTP_HEADER);
    page.replace("{v}", "Blynk");
    page += FPSTR(HTTP_SCRIPT);
    page += FPSTR(HTTP_STYLE);
    //page += _customHeadElement;
    page += FPSTR(HTTP_HEADER_END);
    page += String(F("<h1>"));
    page += _apName;
    page += String(F("</h1>"));
    page += String(F("<h3>Restart Hardware</h3>"));

    page += FPSTR(AHTTP_FORM_RESET);
    page += FPSTR(AHTTP_FORM_PIN);
    page += FPSTR(HTTP_FORM_END);
    page += FPSTR(HTTP_END);

    server.sendHeader("Content-Length", String(page.length()));
    server.send(200, "text/html", page);
}

void KnWiFiManager::handle_DoReset()
{
    String page = FPSTR(HTTP_HEADER);
    page.replace("{v}", "Restart");
    page += F("<meta http-equiv=\"Refresh\" content=\"10; url=/\" />");
    page += FPSTR(HTTP_HEADER_END);
    if (server.arg("bpin") != PIN_NUMBER)
    {
        page += F("<br/>Oops...<br>PIN salah !<br><br><a href='/'>back</a>");
        page += FPSTR(HTTP_END);
        server.sendHeader("Content-Length", String(page.length()));
        server.send(200, "text/html", page);
    }
    else
    {
        Serial.println(F("Reset ESP.."));
        page += F("<br/>Restarting...<br><br><a href='/'>back</a>");
        page += FPSTR(HTTP_END);
        server.sendHeader("Content-Length", String(page.length()));
        server.send(200, "text/html", page);
        delay(200);
        ESP.reset();
    }
}

void KnWiFiManager::handle_BootOpt()
{
    String page = FPSTR(HTTP_HEADER);
    page.replace("{v}", "Boot");
    page += FPSTR(HTTP_SCRIPT);
    page += FPSTR(HTTP_STYLE);
    page += FPSTR(HTTP_HEADER_END);
    page += String(F("<h1>MULTINET</h1>"));
    page += String(F("<h3>Next Boot Config</h3>"));

    String item = FPSTR(AHTTP_FORM_BOOT);
    if (_knsettings.bootAP)
    {
        item.replace("{ap}", " selected");
        item.replace("{sta}", "");
    }
    else
    {
        item.replace("{ap}", "");
        item.replace("{sta}", " selected");
    }
    page += item;

    page += FPSTR(AHTTP_FORM_PIN);
    page += FPSTR(HTTP_FORM_END);
    page += FPSTR(HTTP_END);

    server.sendHeader("Content-Length", String(page.length()));
    server.send(200, "text/html", page);
}

void KnWiFiManager::handle_BootOptSave()
{
    String msg = F("<center><br>");
    if (server.arg("bpin") != PIN_NUMBER)
    {
        msg += F("Oops...<br>PIN salah !");
    }
    else
    {
        String s = server.arg("cboot");
        s.trim();

        boolean nextbootap = s == "ap" ? 1 : 0;
        if (nextbootap != _knsettings.bootAP)
        {
            _knsettings.bootAP = nextbootap;
            writeSetting();
            readSetting();
            msg += F("Konfigurasi Tersimpan<br>Lakukan Reboot");
        }
        else
        {
            msg += F("Tidak ada perubahan seting");
        }
    }

    msg += F("<br><a href='/'>back</a>");
    server.sendHeader("Content-Length", String(msg.length()));
    server.send(200, "text/html", msg); //Send web page
}

/*********************************************************************************/
void KnWiFiManager::setTimeout(unsigned long seconds)
{
    setConfigPortalTimeout(seconds);
}

void KnWiFiManager::setConfigPortalTimeout(unsigned long seconds)
{
    _configPortalTimeout = seconds * 1000;
}

void KnWiFiManager::setupConfigPortal()
{
    dnsServer.reset(new DNSServer());

    DEBUG_WM(F(""));
    _configPortalStart = millis();

    DEBUG_WM(F("Configuring access point... "));
    DEBUG_WM(_apName);
    if (_apPassword != NULL)
    {
        if (strlen(_apPassword) < 8 || strlen(_apPassword) > 63)
        {
            // fail passphrase to short or long!
            DEBUG_WM(F("Invalid AccessPoint password. Ignoring"));
            _apPassword = NULL;
        }
        DEBUG_WM(_apPassword);
    }

    if (_apPassword != NULL)
    {
        WiFi.softAP(_apName, _apPassword); //password option
    }
    else
    {
        WiFi.softAP(_apName);
    }

    delay(500); // Without delay I've seen the IP address blank
    DEBUG_WM(F("AP IP address: "));
    DEBUG_WM(WiFi.softAPIP());

    /* Setup the DNS server redirecting all the domains to the apIP */
    dnsServer->setErrorReplyCode(DNSReplyCode::NoError);
    dnsServer->start(DNS_PORT, "*", WiFi.softAPIP());

    /* Setup web pages: root, wifi config pages, SO captive portal detectors and not found. */
    prepareserver();
}

void KnWiFiManager::prepareserver()
{
    httpUpdater.setup(&server);
    server.on("/", [this]() { handleRoot(); });
    server.on("/wifi", [this]() { handleWifi(true); });
    server.on("/wifisave", [this]() { handleWifiSave(); });
    server.on("/boot", [this]() { handle_BootOpt(); });
    server.on("/bootsave", [this]() { handle_BootOptSave(); });
    server.on("/blynk", [this]() { handle_Blynk(); });
    server.on("/blynksave", [this]() { handle_BlynkSave(); });
    server.on("/i", [this]() { handle_Info(); });
    server.on("/r", [this]() { handle_Reset(); });
    server.on("/doreset", [this]() { handle_DoReset(); });
    server.onNotFound([this]() { handleNotFound(); });
    server.begin(); //Start httpServer

    DEBUG_WM(F("HTTP server started"));
}

boolean KnWiFiManager::autoConnect()
{
    if (isBootAP())
    {
        setBootAP(0);
        startConfigPortal();
    }
    /*********************************************/
    String ssid = "ESP" + String(ESP.getChipId());
    return autoConnect(ssid.c_str(), NULL);
}

boolean KnWiFiManager::autoConnect(char const *apName, char const *apPassword)
{
    DEBUG_WM(F(""));
    DEBUG_WM(F("AutoConnect"));

    // attempt to connect; should it fail, fall back to AP
    WiFi.mode(WIFI_STA);

    if (connectWifi("", "") == WL_CONNECTED)
    {
        DEBUG_WM(F("IP Address:"));
        DEBUG_WM(WiFi.localIP());
        //connected
        prepareserver();
        return true;
    }

    return startConfigPortal(apName, apPassword);
}

boolean KnWiFiManager::configPortalHasTimeout()
{
    if (_configPortalTimeout == 0 || wifi_softap_get_station_num() > 0)
    {
        _configPortalStart = millis(); // kludge, bump configportal start time to skew timeouts
        return false;
    }
    return (millis() > _configPortalStart + _configPortalTimeout);
}

boolean KnWiFiManager::startConfigPortal()
{
    String ssid = "ESP" + String(ESP.getChipId());
    return startConfigPortal(ssid.c_str(), NULL);
}

boolean KnWiFiManager::startConfigPortal(char const *apName, char const *apPassword)
{

    if (!WiFi.isConnected())
    {
        WiFi.persistent(false);
        // disconnect sta, start ap
        WiFi.disconnect(); //  this alone is not enough to stop the autoconnecter
        WiFi.mode(WIFI_AP);
        WiFi.persistent(true);
    }
    else
    {
        //setup AP
        WiFi.mode(WIFI_AP_STA);
        DEBUG_WM(F("SET AP STA"));
    }

    _apName = apName;
    _apPassword = apPassword;

    //notify we entered AP mode
    if (_apcallback != NULL)
    {
        _apcallback(this);
    }

    connect = false;
    setupConfigPortal();

    while (1)
    {

        // check if timeout
        if (configPortalHasTimeout())
            break;

        //DNS
        dnsServer->processNextRequest();
        //HTTP
        server.handleClient();

        if (connect)
        {
            if (applyNewWifiConfig())
            {
                break;
            }
        }
        yield();
    }

    //server.reset();
    dnsServer.reset();

    return WiFi.status() == WL_CONNECTED;
}

boolean KnWiFiManager::applyNewWifiConfig()
{
    connect = false;
    delay(2000);
    DEBUG_WM(F("Connecting to new AP"));

    // using user-provided  _ssid, _pass in place of system-stored ssid and pass
    if (connectWifi(_ssid, _pass) != WL_CONNECTED)
    {
        DEBUG_WM(F("Failed to connect."));
        return false;
    }
    else
    {
        //connected
        WiFi.mode(WIFI_STA);
        //notify that configuration has changed and any optional parameters should be saved
        if (_savecallback != NULL)
        {
            //todo: check if any custom parameters actually exist, and check if they really changed maybe
            _savecallback();
        }
        DEBUG_WM(F("Restarting.."));
        delay(200);
        ESP.reset();
        return true; //break;
    }

    if (_shouldBreakAfterConfig)
    {
        //flag set to exit after config after trying to connect
        //notify that configuration has changed and any optional parameters should be saved
        if (_savecallback != NULL)
        {
            //todo: check if any custom parameters actually exist, and check if they really changed maybe
            _savecallback();
        }
        return true; //break;
    }
}

int KnWiFiManager::connectWifi(String ssid, String pass)
{
    DEBUG_WM(F("Connecting as wifi client..."));

    //fix for auto connect racing issue
    if (WiFi.status() == WL_CONNECTED && (WiFi.SSID() == ssid))
    {
        DEBUG_WM(F("Already connected. Bailing out."));
        return WL_CONNECTED;
    }
    //check if we have ssid and pass and force those, if not, try with last saved values
    if (ssid != "")
    {
        WiFi.begin(ssid.c_str(), pass.c_str());
    }
    else
    {
        if (WiFi.SSID() != "")
        {
            DEBUG_WM(F("Using last saved values, should be faster"));
            DEBUG_WM(F("kn connectWifi | SSID: "));
            DEBUG_WM(WiFi.SSID());

            //trying to fix connection in progress hanging
            ETS_UART_INTR_DISABLE();
            wifi_station_disconnect();
            ETS_UART_INTR_ENABLE();

            WiFi.begin();
        }
        else
        {
            DEBUG_WM(F("No saved credentials"));
        }
    }

    int connRes = waitForConnectResult();
    DEBUG_WM("Connection result: ");
    DEBUG_WM(connRes);

    return connRes;
}

uint8_t KnWiFiManager::waitForConnectResult()
{
    if (_connectTimeout == 0)
    {
        return WiFi.waitForConnectResult();
    }
    else
    {
        DEBUG_WM(F("Waiting for connection result with time out"));
        unsigned long start = millis();
        boolean keepConnecting = true;
        uint8_t status;
        while (keepConnecting)
        {
            status = WiFi.status();
            if (millis() > start + _connectTimeout)
            {
                keepConnecting = false;
                DEBUG_WM(F("Connection timed out"));
            }
            if (status == WL_CONNECTED)
            {
                keepConnecting = false;
            }
            delay(100);
        }
        return status;
    }
}

template <typename Generic>
void KnWiFiManager::DEBUG_WM(Generic text)
{
    if (_debug)
    {
        Serial.print("*WM: ");
        Serial.println(text);
    }
}

/** Handle root or redirect to captive portal */
void KnWiFiManager::handleRoot()
{
    DEBUG_WM(F("Handle root"));
    if (captivePortal())
    { // If caprive portal redirect instead of displaying the page.
        return;
    }

    String page = FPSTR(HTTP_HEADER);
    page.replace("{v}", "Options");
    page += FPSTR(HTTP_SCRIPT);
    page += FPSTR(HTTP_STYLE);
    //page += _customHeadElement;
    page += FPSTR(HTTP_HEADER_END);
    page += String(F("<h1>"));
    page += _apName;
    page += String(F("</h1>"));
    page += String(F("<h3>Setting</h3>"));
    page += FPSTR(HTTP_PORTAL_OPTIONS);

    if (_configPortalStart == 0)
    {
        page += String(F("<br/><form action=\"/update\" method=\"get\"><button>Update</button></form><br/><form action=\"/boot\" method=\"get\"><button>Boot Option</button></form>"));
        if (_linkuri.size() > 0)
        {
            String item;
            for (byte i = 0; i < _linkuri.size(); i++)
            {
                item += FPSTR(AHTTP_ADD_MENU);
                item.replace("{l}", _linkuri[i]);
                item.replace("{t}", _linktitle[i]);
            }
            page += item;
        }
    }

    page += FPSTR(HTTP_END);

    server.sendHeader("Content-Length", String(page.length()));
    server.send(200, "text/html", page);
}

void KnWiFiManager::handleNotFound()
{
    if (captivePortal())
    { // If captive portal redirect instead of displaying the error page.
        return;
    }
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";

    for (uint8_t i = 0; i < server.args(); i++)
    {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    server.sendHeader("Pragma", "no-cache");
    server.sendHeader("Expires", "-1");
    server.sendHeader("Content-Length", String(message.length()));
    server.send(404, "text/plain", message);
}

/** Handle the WLAN save form and redirect to WLAN config page again */
void KnWiFiManager::handleWifiSave()
{
    DEBUG_WM(F("WiFi save"));

    //SAVE/connect here
    _ssid = server.arg("s").c_str();
    _pass = server.arg("p").c_str();

    String page = FPSTR(HTTP_HEADER);
    page.replace("{v}", "Credentials Saved");
    page += FPSTR(HTTP_SCRIPT);
    page += FPSTR(HTTP_STYLE);
    page += _customHeadElement;
    page += FPSTR(HTTP_HEADER_END);
    page += FPSTR(HTTP_SAVED);
    page += FPSTR(HTTP_END);

    server.sendHeader("Content-Length", String(page.length()));
    server.send(200, "text/html", page);

    DEBUG_WM(F("Sent wifi save page"));

    connect = true; //signal ready to connect/reset
    applyNewWifiConfig();
}

/** Wifi config page handler */
void KnWiFiManager::handleWifi(boolean scan)
{
    String page = FPSTR(HTTP_HEADER);
    page.replace("{v}", "Config ESP");
    page += FPSTR(HTTP_SCRIPT);
    page += FPSTR(HTTP_STYLE);
    //page += _customHeadElement;
    page += FPSTR(HTTP_HEADER_END);

    if (scan)
    {
        int n = WiFi.scanNetworks();
        DEBUG_WM(F("Scan done"));
        if (n == 0)
        {
            DEBUG_WM(F("No networks found"));
            page += F("No networks found. Refresh to scan again.");
        }
        else
        {

            //sort networks
            int indices[n];
            for (int i = 0; i < n; i++)
            {
                indices[i] = i;
            }

            // RSSI SORT

            // old sort
            for (int i = 0; i < n; i++)
            {
                for (int j = i + 1; j < n; j++)
                {
                    if (WiFi.RSSI(indices[j]) > WiFi.RSSI(indices[i]))
                    {
                        std::swap(indices[i], indices[j]);
                    }
                }
            }

            /*std::sort(indices, indices + n, [](const int & a, const int & b) -> bool
        {
        return WiFi.RSSI(a) > WiFi.RSSI(b);
        });*/

            // remove duplicates ( must be RSSI sorted )
            if (_removeDuplicateAPs)
            {
                String cssid;
                for (int i = 0; i < n; i++)
                {
                    if (indices[i] == -1)
                        continue;
                    cssid = WiFi.SSID(indices[i]);
                    for (int j = i + 1; j < n; j++)
                    {
                        if (cssid == WiFi.SSID(indices[j]))
                        {
                            DEBUG_WM("DUP AP: " + WiFi.SSID(indices[j]));
                            indices[j] = -1; // set dup aps to index -1
                        }
                    }
                }
            }

            //display networks in page
            for (int i = 0; i < n; i++)
            {
                if (indices[i] == -1)
                    continue; // skip dups
                DEBUG_WM(WiFi.SSID(indices[i]));
                DEBUG_WM(WiFi.RSSI(indices[i]));
                int quality = getRSSIasQuality(WiFi.RSSI(indices[i]));

                if (_minimumQuality == -1 || _minimumQuality < quality)
                {
                    String item = FPSTR(HTTP_ITEM);
                    String rssiQ;
                    rssiQ += quality;
                    item.replace("{v}", WiFi.SSID(indices[i]));
                    item.replace("{r}", rssiQ);
                    if (WiFi.encryptionType(indices[i]) != ENC_TYPE_NONE)
                    {
                        item.replace("{i}", "l");
                    }
                    else
                    {
                        item.replace("{i}", "");
                    }
                    //DEBUG_WM(item);
                    page += item;
                    delay(0);
                }
                else
                {
                    DEBUG_WM(F("Skipping due to quality"));
                }
            }
            page += "<br/>";
        }
    }

    page += FPSTR(HTTP_FORM_START);
    char parLength[5];

    page += FPSTR(HTTP_FORM_END);
    page += FPSTR(HTTP_SCAN_LINK);

    page += FPSTR(HTTP_END);

    server.sendHeader("Content-Length", String(page.length()));
    server.send(200, "text/html", page);

    DEBUG_WM(F("Sent config page"));
}

/** Redirect to captive portal if we got a request for another domain. Return true in that case so the page handler do not try to handle the request again. */
boolean KnWiFiManager::captivePortal()
{
    if (!isIp(server.hostHeader()))
    {
        DEBUG_WM(F("Request redirected to captive portal"));
        server.sendHeader("Location", String("http://") + toStringIp(server.client().localIP()), true);
        server.send(302, "text/plain", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
        server.client().stop();             // Stop is needed because we sent no content length
        return true;
    }
    return false;
}

/** Is this an IP? */
boolean KnWiFiManager::isIp(String str)
{
    for (size_t i = 0; i < str.length(); i++)
    {
        int c = str.charAt(i);
        if (c != '.' && (c < '0' || c > '9'))
        {
            return false;
        }
    }
    return true;
}

/** IP to String? */
String KnWiFiManager::toStringIp(IPAddress ip)
{
    String res = "";
    for (int i = 0; i < 3; i++)
    {
        res += String((ip >> (8 * i)) & 0xFF) + ".";
    }
    res += String(((ip >> 8 * 3)) & 0xFF);
    return res;
}

//start up config portal callback
void KnWiFiManager::setAPCallback(void (*func)(KnWiFiManager *myWiFiManager))
{
    _apcallback = func;
}

//start up save config callback
void KnWiFiManager::setSaveConfigCallback(void (*func)(void))
{
    _savecallback = func;
}

int KnWiFiManager::getRSSIasQuality(int RSSI)
{
    int quality = 0;

    if (RSSI <= -100)
    {
        quality = 0;
    }
    else if (RSSI >= -50)
    {
        quality = 100;
    }
    else
    {
        quality = 2 * (RSSI + 100);
    }
    return quality;
}