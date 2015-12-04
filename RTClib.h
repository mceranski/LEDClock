#include <avr/pgmspace.h>
// Code by JeeLabs http://news.jeelabs.org/code/
// Released to the public domain! Enjoy!

// Simple general-purpose date/time class (no TZ / DST / leap second handling!)
static char* const hourNames[12] PROGMEM = { "one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten", "eleven", "twelve" };
static char* const monthNames[12] PROGMEM = { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };
static char* const dateNames[31] PROGMEM = { "first", "second", "third", "fourth", "fifth", "sixth", "seventh", "eighth", "ninth", "tenth", "eleventh", "twelfth", "thirteenth", "fourteenth", "fifteenth"
                                             "sixteenth", "seventeenth", "eighteenth", "nineteenth", "twentieth", "twenty first", "twenty second", "twenty third", "twenty fourth", "twenty fifth",
				                                      "twenty sixth", "twenty seventh", "twenty eighth", "twenty ninth", "thirtieth", "thirty first" };
static char* const dayNames[7] PROGMEM = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
    
class DateTime {
public:
    DateTime (uint32_t t =0);
    DateTime (uint16_t year, uint8_t month, uint8_t day, uint8_t hour =0, uint8_t min =0, uint8_t sec =0);
    DateTime (const char* date, const char* time);
    uint16_t year() const       { return 2000 + yOff; }
    uint8_t month() const       { return m; }
    uint8_t day() const         { return d; }
    uint8_t hour() const        { return hh; }
    uint8_t minute() const      { return mm; }
    uint8_t second() const      { return ss; }
    uint8_t dayOfWeek() const;
    uint8_t twelveHourFormat() const { return hh == 0 ? 12 : hh > 12 ? hh -12 : hh; } 
    // 32-bit times as seconds since 1/1/2000
    long secondstime() const;   
    // 32-bit times as seconds since 1/1/1970
    uint32_t unixtime(void) const;
    
    const char* hourName() const { return (char*)pgm_read_word(&(hourNames[twelveHourFormat()-1]));}
    const char* monthName() const { return (char*)pgm_read_word(&(monthNames[m-1])); }
    const char* dateName() const { return (char*)pgm_read_word(&(dateNames[d-1])); }
    const char* dayName() const { return (char*)pgm_read_word(&(dayNames[dayOfWeek()])); }
     char* dateEnding() const {
      if( d == 1 || d == 21 || d == 31 ) return "st";
      if( d == 2 || d == 22 ) return "nd";
      if( d == 3 || d == 23 ) return "rd";
      return "th";
    }
protected:
    
    uint8_t yOff, m, d, hh, mm, ss;
};

// RTC based on the DS1307 chip connected via I2C and the Wire library
class RTC_DS1307 {
public:
  static uint8_t begin(void);
  static void adjust(const DateTime& dt);
  uint8_t isrunning(void);
  static DateTime now();
};

// RTC using the internal millis() clock, has to be initialized before use
// NOTE: this clock won't be correct once the millis() timer rolls over (>49d?)
class RTC_Millis {
public:
    static void begin(const DateTime& dt) { adjust(dt); }
    static void adjust(const DateTime& dt);
    static DateTime now();

protected:
    static long offset;
};

