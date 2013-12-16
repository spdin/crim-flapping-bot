// @author saripudin, vektor dewanto
#ifndef MAESTRO_SERVO_H
#define MAESTRO_SERVO_H

#include <stdint.h> // for uint16_t, uint8_t, etc
#include <vector>
#include "wirish.h"

namespace crim{
class MaestroServo {
 public:
  std::vector<uint16_t> star_poses;
  
  /**
   * @brief This uses Serial2 _only_ with baudrate= 9600
  */
  MaestroServo(uint8_t addr, uint16_t min_pos, uint16_t max_pos);
  /**
   * @brief This end the Serial2 comm.
  */
  ~MaestroServo();
  
  /**
   * @brief 
  */
  void go_to(uint16_t pos);

  /**
   * @brief 
  */
  void goto_star(uint16_t idx);
  
  /**
   * @brief 
  */
  void goto_max();
  
  /**
   * @brief 
  */
  void goto_min();
  
  /**
   * @brief 
  */
  uint8_t get_addr();
  
 private:
  uint8_t addr_;
     
  uint16_t min_pos_;
  uint16_t max_pos_;
  
};

}// namespace crim

#endif
