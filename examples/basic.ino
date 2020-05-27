#define BLYNK_PRINT Serial // Comment this out to disable prints and save space
#include <BlynkSimpleEsp8266.h>

#include "KnWiFiManager.h"
KnWiFiManager kn;

const String _AppName = "Example..";

void setup()
{
    Serial.begin(115200);
    kn.setAppName(_AppName);
    kn.setTimeout(180);
    kn.autoConnect();

    Blynk.config(kn.getblynkToken(), kn.getblynkServer(), kn.getblynkPort());

    kn.on("/a", handle_a, "Tambahan Menu 1");
    kn.on("/b", handle_b, "Tambahan Menu 2");
}

void loop()
{
    kn.run();
    Blynk.run();
}

void handle_a()
{
    String page = "<br/>iki page AAAAAA";
    kn.send(200, "text/html", page);
}

void handle_b()
{
    String page = "<br/>iki page Bee";
    kn.send(200, "text/html", page);
}
