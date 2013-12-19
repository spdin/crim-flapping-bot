#include <stdlib.h> // for Adafruit_GPS lib, and this _must_ be on top of all #include
#include "wirish.h"
#include "gps/Adafruit_GPS.h"
#include "xbee/flymaple_packet_handler.h"
#include "data-format/gps_data.h"

static const int SERIAL1_BAUD_RATE = 9600;
static const int SERIAL2_BAUD_RATE = 9600;
static const int SERIAL3_BAUD_RATE = 9600;
static size_t TIMER_PERIODE = 1000; // in microseconds

HardwareTimer timer(2);
Adafruit_GPS GPS(&Serial2);

void timer_int_handler(void) {
  GPS.read();
}

void setup_timer_int() {
    // Pause the timer while we're configuring it
    timer.pause();

    // Set up period
    timer.setPeriod(TIMER_PERIODE); // in microseconds

    // Set up an interrupt on channel 1
    timer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
    timer.setCompare(TIMER_CH1, 1);  // Interrupt 1 count after each update
    timer.attachCompare1Interrupt(timer_int_handler);

    // Refresh the timer's count, prescale, and overflow
    timer.refresh();

    // Start the timer counting
    timer.resume();
}

void setup()
{
  setup_timer_int();

  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);

  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data for high update rates!
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // uncomment this line to turn on all the available data - for 9600 baud you'll want 1 Hz rate
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_ALLDATA);

  // Set the update rate
  // 1 Hz update rate
  //GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
  // 5 Hz update rate- for 9600 baud you'll have to set the output to RMC or RMCGGA only (see above)
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_5HZ);
  // 10 Hz update rate - for 9600 baud you'll have to set the output to RMC only (see above)
  //GPS.sendCommand(PMTK_SET_NMEA_UPDATE_10HZ);

  // Request updates on antenna status, comment out to keep quiet
  GPS.sendCommand(PGCMD_ANTENNA);

  delay(1000);
}

void loop()
{
  //// Using this, instead of timer_interrupt
  //if (Serial1.available()) {
    //GPS.read();
  //}

  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences!
    // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
    //SerialUSB.println(GPS.lastNMEA());   // this also sets the newNMEAreceived() flag to false

    if (!GPS.parse(GPS.lastNMEA())) {   // this also sets the newNMEAreceived() flag to false
      SerialUSB.println("parsed: FAILED");
      return;// continue;  // we can fail to parse a sentence in which case we should just wait for another
    }
    else {
      // Format the data
      crim::GPSData gps_data;
      gps_data.set_time(GPS.hour, GPS.minute, GPS.seconds, GPS.milliseconds);
      gps_data.set_date(GPS.year, GPS.month, GPS.day);
      gps_data.set_pose(GPS.latitude, GPS.lat, GPS.longitude, GPS.lon, GPS.altitude);
      gps_data.set_misc(GPS.speed, GPS.angle);
      gps_data.set_note(GPS.fix, GPS.fixquality, GPS.satellites);

      crim::FlymaplePacketHandler packet_handler("Serial1", SERIAL1_BAUD_RATE);
      packet_handler.wrap(gps_data);
      packet_handler.send();
    }
  }
  else {
    SerialUSB.println("!GPS.newNMEAreceived()");
  }
}

// Force init to be called *first*, i.e. before static object allocation.
// Otherwise, statically allocated objects that need libmaple may fail.
__attribute__((constructor)) void premain() {
    init();
}

int main() {
  setup();

  while(true) {
    SerialUSB.println("looping");
    loop();
    delay(100);
  }

  return 0;
}
