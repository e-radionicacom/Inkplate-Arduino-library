/*
   Weather station example for e-radionica.com Inkplate 6COLOR
   For this example you will need only USB cable and Inkplate 6COLOR.
   Select "Inkplate 6COLOR(ESP32)" from Tools -> Board menu.
   Don't have "Inkplate 6COLOR(ESP32)" option? Follow our tutorial and add it:
   https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

   This example will show you how you can use Inkplate 6COLOR to display API data,
   e.g. Metaweather public weather API

   IMPORTANT:
   Make sure to change your desired city, timezone and wifi credentials below
   Also have ArduinoJSON installed in your Arduino libraries

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
   28 July 2020 by e-radionica.com
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATECOLOR
#error "Wrong board selection for this example, please select Inkplate color in the boards menu."
#endif

// ---------- CHANGE HERE  -------------:

// Time zone for adding hours
int timeZone = 2;

// City search query
char city[128] = "ZAGREB";

// Change to your wifi ssid and password
char ssid[] = "e-radionica.com";
char pass[] = "croduino";

// ----------------------------------

// Include Inkplate library to the sketch
#include "Inkplate.h"

// Header file for easier code readability
#include "Network.h"

// Including fonts used
#include "Fonts/FreeMonoBold12pt7b.h"
#include "Fonts/Roboto_Light_120.h"
#include "Fonts/Roboto_Light_36.h"
#include "Fonts/Roboto_Light_48.h"

// Including icons generated by the py file
#include "icons.h"

// Delay between API calls
#define DELAY_MS 1000 * 60 * 10 // 10 minute time

// Inkplate object
Inkplate display;

// All our network functions are in this object, see Network.h
Network network;

// Contants used for drawing icons
char abbrs[32][16] = {"sn", "sl", "h", "t", "hr", "lr", "s", "hc", "lc", "c"};
const uint8_t *logos[16] = {icon_sn, icon_sl, icon_h, icon_t, icon_hr, icon_lr, icon_s, icon_hc, icon_lc, icon_c};
const uint8_t *s_logos[16] = {icon_s_sn, icon_s_sl, icon_s_h,  icon_s_t,  icon_s_hr,
                              icon_s_lr, icon_s_s,  icon_s_hc, icon_s_lc, icon_s_c};

char abbr1[16];
char abbr2[16];
char abbr3[16];
char abbr4[16];

// Variables for storing temperature
char temps[8][4] = {
    "0F",
    "0F",
    "0F",
    "0F",
};

// Variables for storing days of the week
char days[8][4] = {
    "",
    "",
    "",
    "",
};

// Variable for counting partial refreshes
long refreshes = 0;

// Constant to determine when to full update
const int fullRefresh = 10;

// Variables for storing current time and weather info
char currentTemp[16] = "0F";
char currentWind[16] = "0m/s";

char currentTime[16] = "9:41";

char currentWeather[32] = "-";
char currentWeatherAbbr[8] = "th";

// functions defined below
void drawWeather();
void drawCurrent();
void drawTemps();
void drawCity();
void drawTime();

void setup()
{
    // Begin serial and display
    Serial.begin(115200);
    display.begin();

    // Initial cleaning of buffer and physical screen
    display.clearDisplay();
    display.display();

    // Calling our begin from network.h file
    network.begin(city);

    // If city not found, do nothing
    if (network.location == -1)
    {
        display.setCursor(50, 290);
        display.setTextSize(2);
        display.print(F("City not in Metaweather Database"));
        display.display();
        while (1)
            ;
    }
    else
    {
        // Clear display
        display.clearDisplay();

        // Get all relevant data, see Network.cpp for info
        network.getTime(currentTime);
        network.getDays(days[0], days[1], days[2], days[3]);
        network.getData(city, temps[0], temps[1], temps[2], temps[3], currentTemp, currentWind, currentTime,
                        currentWeather, currentWeatherAbbr, abbr1, abbr2, abbr3, abbr4);

        // Draw data, see functions below for info
        drawWeather();
        drawCurrent();
        drawTemps();
        drawCity();
        drawTime();

        // Refresh full screen every fullRefresh times, defined above
        display.display();

        // Go to sleep before checking again
        esp_sleep_enable_timer_wakeup(1000L * DELAY_MS);
        esp_deep_sleep_start();
    }
}

void loop()
{
}

// Function for drawing weather info
void drawWeather()
{
    // Searching for weather state abbreviation
    for (int i = 0; i < 10; ++i)
    {
        // If found draw specified icon
        if (strcmp(abbrs[i], currentWeatherAbbr) == 0)
            display.drawBitmap(10, 10, logos[i], 152, 152, INKPLATE_ORANGE);
    }

    // Draw weather state
    display.setTextColor(INKPLATE_BLUE);
    display.setFont(&Roboto_Light_36);
    display.setTextSize(1);
    display.setCursor(150, 50);
    display.println(currentWeather);
}

// Function for drawing current time
void drawTime()
{
    // Drawing current time
    display.setTextColor(INKPLATE_BLUE);
    display.setFont(&Roboto_Light_36);
    display.setTextSize(1);

    display.setCursor(600 - 20 * strlen(currentTime), 35);
    display.println(currentTime);
}

// Function for drawing city name
void drawCity()
{
    // Drawing city name
    display.setTextColor(INKPLATE_BLUE);
    display.setFont(&Roboto_Light_36);
    display.setTextSize(1);

    display.setCursor(300 - 9 * strlen(city), 430);
    display.println(city);
}

// Function for drawing temperatures
void drawTemps()
{
    // Drawing 4 black rectangles in which temperatures will be written
    int rectWidth = 100;
    int rectSpacing = (600 - rectWidth * 4) / 5;
    int yRectangleOffset = 200;
    int rectHeight = 200;

    display.fillRect(1 * rectSpacing + 0 * rectWidth, yRectangleOffset, rectWidth, rectHeight, BLACK);
    display.fillRect(2 * rectSpacing + 1 * rectWidth, yRectangleOffset, rectWidth, rectHeight, BLACK);
    display.fillRect(3 * rectSpacing + 2 * rectWidth, yRectangleOffset, rectWidth, rectHeight, BLACK);
    display.fillRect(4 * rectSpacing + 3 * rectWidth, yRectangleOffset, rectWidth, rectHeight, BLACK);

    int textMargin = 6;

    display.setFont(&Roboto_Light_36);
    display.setTextSize(1);
    display.setTextColor(INKPLATE_BLUE);

    int dayOffset = 40;

    display.setCursor(1 * rectSpacing + 0 * rectWidth + textMargin, yRectangleOffset + textMargin + dayOffset);
    display.println(days[0]);

    display.setCursor(2 * rectSpacing + 1 * rectWidth + textMargin, yRectangleOffset + textMargin + dayOffset);
    display.println(days[1]);

    display.setCursor(3 * rectSpacing + 2 * rectWidth + textMargin, yRectangleOffset + textMargin + dayOffset);
    display.println(days[2]);

    display.setCursor(4 * rectSpacing + 3 * rectWidth + textMargin, yRectangleOffset + textMargin + dayOffset);
    display.println(days[3]);

    int tempOffset = 120;

    // Drawing temperature values into black rectangles
    display.setFont(&Roboto_Light_36);
    display.setTextSize(1);
    display.setTextColor(INKPLATE_BLUE);

    display.setCursor(1 * rectSpacing + 0 * rectWidth + textMargin, yRectangleOffset + textMargin + tempOffset);
    display.print(temps[0]);
    display.println(F("C"));

    display.setCursor(2 * rectSpacing + 1 * rectWidth + textMargin, yRectangleOffset + textMargin + tempOffset);
    display.print(temps[1]);
    display.println(F("C"));

    display.setCursor(3 * rectSpacing + 2 * rectWidth + textMargin, yRectangleOffset + textMargin + tempOffset);
    display.print(temps[2]);
    display.println(F("C"));

    display.setCursor(4 * rectSpacing + 3 * rectWidth + textMargin, yRectangleOffset + textMargin + tempOffset);
    display.print(temps[3]);
    display.println(F("C"));

    int iconOffset = 130;

    for (int i = 0; i < 18; ++i)
    {
        // If found draw specified icon
        if (strcmp(abbr1, abbrs[i]) == 0)
            display.drawBitmap(1 * rectSpacing + 0 * rectWidth + textMargin, yRectangleOffset + textMargin + iconOffset,
                               s_logos[i], 48, 48, INKPLATE_WHITE, INKPLATE_BLACK);
    }

    for (int i = 0; i < 18; ++i)
    {
        // If found draw specified icon
        if (strcmp(abbr2, abbrs[i]) == 0)
            display.drawBitmap(2 * rectSpacing + 1 * rectWidth + textMargin, yRectangleOffset + textMargin + iconOffset,
                               s_logos[i], 48, 48, INKPLATE_WHITE, INKPLATE_BLACK);
    }

    for (int i = 0; i < 18; ++i)
    {
        // If found draw specified icon
        if (strcmp(abbr3, abbrs[i]) == 0)
            display.drawBitmap(3 * rectSpacing + 2 * rectWidth + textMargin, yRectangleOffset + textMargin + iconOffset,
                               s_logos[i], 48, 48, INKPLATE_WHITE, INKPLATE_BLACK);
    }

    for (int i = 0; i < 18; ++i)
    {
        // If found draw specified icon
        if (strcmp(abbr4, abbrs[i]) == 0)
            display.drawBitmap(4 * rectSpacing + 3 * rectWidth + textMargin, yRectangleOffset + textMargin + iconOffset,
                               s_logos[i], 48, 48, INKPLATE_WHITE, INKPLATE_BLACK);
    }
}

// Current weather drawing function
void drawCurrent()
{
    // Drawing current information

    // Temperature:
    display.setFont(&Roboto_Light_120);
    display.setTextSize(1);
    display.setTextColor(INKPLATE_BLUE);

    display.setCursor(150, 150);
    display.print(currentTemp);

    int x = display.getCursorX();
    int y = display.getCursorY();

    display.setFont(&Roboto_Light_48);
    display.setTextSize(1);

    display.setCursor(x, y);
    display.println(F("C"));

    // Wind:
    display.setFont(&Roboto_Light_120);
    display.setTextSize(1);
    display.setTextColor(INKPLATE_BLUE);

    display.setCursor(350, 150);
    display.print(currentWind);

    x = display.getCursorX();
    y = display.getCursorY();

    display.setFont(&Roboto_Light_48);
    display.setTextSize(1);

    display.setCursor(x, y);
    display.println(F("m/s"));

    // Labels underneath
    display.setFont(&FreeMonoBold12pt7b);
    display.setTextSize(1);

    display.setCursor(165, 190);
    display.println(F("TEMPERATURE"));

    display.setCursor(390, 190);
    display.println(F("WIND SPEED"));
}
