#define LILYGO_T5_V213
#define LILYGO_EPD_DISPLAY

#include <boards.h>
#include <GxEPD.h>
#include <SD.h>
#include <FS.h>
#include <Wire.h>
#include <SensirionI2CScd4x.h>
// #include <GxGDEH0213B72/GxGDEH0213B72.h> // Include the display driver for your specific display
#include <Fonts/FreeMonoBold9pt7b.h> // Include a font

// #include <GxGDE0213B1/GxGDE0213B1.h>      // 2.13" b/w  old panel , form GoodDisplay
//  #include <GxGDEH0213B72/GxGDEH0213B72.h>  // 2.13" b/w  old panel , form GoodDisplay
//  #include <GxGDEH0213B73/GxGDEH0213B73.h>  // 2.13" b/w  old panel , form GoodDisplay
//  #include <GxGDEM0213B74/GxGDEM0213B74.h>  // 2.13" b/w  form GoodDisplay 4-color
//  #include <GxGDEW0213M21/GxGDEW0213M21.h>  // 2.13"  b/w Ultra wide temperature , form GoodDisplay
#include <GxDEPG0213BN/GxDEPG0213BN.h> // 2.13" b/w  form DKE GROUP

#include GxEPD_BitmapExamples

// FreeFonts from Adafruit_GFX
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>
#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>
#include <WiFi.h>

// Initialize the SCD4x sensor
SensirionI2CScd4x scd4x;

// Initialize the display
GxIO_Class io(SPI, /*CS=*/5, /*DC=*/17, /*RST=*/16); // Adjust pins as needed
GxEPD_Class display(io, /*RST=*/16, /*BUSY=*/4);     // Adjust pins as needed

void showFont(const char name[], const GFXfont *f);
void drawCornerTest(void);

void setup()
{
    bool rlst = false;
    Serial.begin(115200);
    Serial.println();
    Serial.println("setup");

    // Initialize the SCD4x sensor
#define SDA_PIN 33
#define SCL_PIN 32
    Wire.begin(SDA_PIN, SCL_PIN);
    uint16_t error;
    char errorMessage[256];

    scd4x.begin(Wire);
    error = scd4x.stopPeriodicMeasurement();
    if (error)
    {
        Serial.print("Error trying to execute stopPeriodicMeasurement(): ");
        // errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    }
    SPI.begin(EPD_SCLK, EPD_MISO, EPD_MOSI);

    display.init();
    display.fillScreen(GxEPD_WHITE);
    display.update();

    uint16_t serial0;
    uint16_t serial1;
    uint16_t serial2;
    error = scd4x.getSerialNumber(serial0, serial1, serial2);
    if (error)
    {
        Serial.print("Error trying to execute getSerialNumber(): ");
        // errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    }
    else
    {
        // printSerialNumber(serial0, serial1, serial2);
        Serial.print("Serial: 0x");
        // printUint16Hex(serial0);
        // printUint16Hex(serial1);
        // printUint16Hex(serial2);
        Serial.println();
    }

    // Start Measurement
    error = scd4x.startPeriodicMeasurement();
    if (error)
    {
        Serial.print("Error trying to execute startPeriodicMeasurement(): ");
        // errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    }

    Serial.println("Waiting for first measurement... (5 sec)");
}

void loop()
{
    uint16_t co2;
    float temperature;
    float humidity;
    uint16_t error;
    char errorMessage[256];

    // Read the CO2 level from the sensor
    error = scd4x.readMeasurement(co2, temperature, humidity);
    if (error)
    {
        Serial.print("Error trying to execute readMeasurement(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    }
    else if (co2 == 0)
    {
        Serial.println("Invalid sample detected, skipping.");
    }
    else
    {
        Serial.print("CO2: ");
        Serial.print(co2);
        Serial.print(" ppm, Temperature: ");
        Serial.print(temperature);
        Serial.print(" °C, Humidity: ");
        Serial.print(humidity);
        Serial.println(" %RH");

        // Display the CO2 level on the screen using partial update
        display.setRotation(1);
        display.setTextColor(GxEPD_BLACK);
        display.setTextSize(2.5);
        display.setCursor(0, 30);
        display.fillRect(0, 30, 250, 50, GxEPD_WHITE); // Clear the area where the CO2 value is displayed
        display.print("CO2: ");
        display.print(co2);
        display.println(" ppm");
        display.updateWindow(0, 30, 250, 50, true); // Perform partial update on the specified area
    }

    delay(5000); // Wait for 5 seconds before the next reading
}

// void showFont(const char name[], const GFXfont *f)
// {
//     display.fillScreen(GxEPD_WHITE);
//     display.setTextColor(GxEPD_BLACK);
//     display.setFont(f);
//     display.setCursor(0, 0);
//     display.println();
//     display.println(name);
//     display.println(" !\"#$%&'()*+,-./");
//     display.println("0123456789:;<=>?");
//     display.println("@ABCDEFGHIJKLMNO");
//     display.println("PQRSTUVWXYZ[\\]^_");
//     display.println("`abcdefghijklmno");
//     display.println("pqrstuvwxyz{|}~ ");
//     display.update();
//     delay(5000);
// }
