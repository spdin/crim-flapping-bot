#include "xbee/flymaple_packet_handler.h"

using namespace crim;
using namespace std;

FlymaplePacketHandler::FlymaplePacketHandler(std::string port, size_t baud): baud_(baud) {
  if (port == "SerialUSB") {
    port_ = SERIAL_USB;
  } else if( port == "Serial1") {
    port_ = SERIAL_1;
    Serial1.begin(baud_);
  } else if( port == "Serial2") {
    port_ = SERIAL_2;
    Serial2.begin(baud_);
  } else if( port == "Serial3") {
    port_ = SERIAL_3;
    Serial3.begin(baud_);
  }
}

bool FlymaplePacketHandler::send() {
  bool status = false;
  if (port_==SERIAL_USB) {
    status = send_SerialUSB();
  } else {
    status = send_SerialX();
  }
  return status;
}

bool FlymaplePacketHandler::send_SerialUSB() {
  SerialUSB.println(packet_.c_str());
  return true;
}

bool FlymaplePacketHandler::send_SerialX() {
  switch (port_) {
    case SERIAL_1: {
      Serial1.print(packet_.c_str());
      break;
    }
    case SERIAL_2: {
      Serial2.print(packet_.c_str());
      break;
    }
    case SERIAL_3: {
      Serial3.print(packet_.c_str());
      break;
    }
    default: {
      return false;
    }
  }
  return true;
}

void FlymaplePacketHandler::wrap(const StringData& data) {
  //
  std::string f_data;// f(ormatted)_data
  size_t n_field = data.content.size();

  for(size_t i=0; i<n_field; ++i) {
    size_t n_subfield = data.content.at(i).size();
    for(size_t j=0; j<n_subfield; ++j) {
      f_data += data.content.at(i).at(j);

      if(j != n_subfield-1)
        f_data += PacketHandler::kDataSubfieldDelimiter;
    }
    if(i != n_field-1)
        f_data += PacketHandler::kDataFieldDelimiter;
  }

  // For now (Dec 9, 2013), we rely on the built-in checksum on zigbee/xbee
  //hashwrapper* h = new md5wrapper();
  //h->test();

  std::string checksum;
  //checksum = h->getHashFromString(f_data);

  //
  wrap_final(f_data, checksum);
  //delete h;
}
