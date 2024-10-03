#include <Adafruit_ST7789.h>
#include <Arduino_CAN.h>

static int8_t constexpr PIN_LCD_CS = 10;
static int8_t constexpr PIN_LCD_DC = 7;
static int8_t constexpr PIN_LCD_RST = 8;

static Adafruit_ST7789 LCD(PIN_LCD_CS, PIN_LCD_DC, PIN_LCD_RST);

void setup()
{
  LCD.init(172, 320);
  LCD.enableDisplay(false);
  LCD.fillScreen(ST77XX_BLACK);
  LCD.enableDisplay(true);

  CAN.setFilterMask_Standard(0x1FFC0000);
  for (size_t mailbox = 0; mailbox < R7FA4M1_CAN::CAN_MAX_NO_STANDARD_MAILBOXES; mailbox++)
  {
    CAN.setFilterId_Standard(mailbox, 0x758);
  }
  if (!CAN.begin(CanBitRate::BR_500k))
  {
    LCD.print("Error initializing CAN");
    for (;;)
      ;
  }
}

void loop()
{
}
