/*******************************************************************************
 * HID device listing
 * (c) VL 2018
 *  
 * Licensed under the BSD license (see ../../LICENSE-bsd.txt)
 *******************************************************************************/
#include <iostream>
#include "hidapi.h"


int main(int argc, char* argv[])
{

  int i = 0;
  struct hid_device_info *devs, *cur_dev;

  devs = hid_enumerate(0,0);
  cur_dev = devs;	
  while (cur_dev) {
    std::cout << "====================\n";
    std::cout << std::dec;
    std::cout << "device #" << i << std::endl;
    std::cout << std::hex;
    std::cout << "path: " << cur_dev->path << std::endl;
    std::cout << "VID: 0x" << cur_dev->vendor_id << std::endl;
    std::cout << "PID: 0x" << cur_dev->product_id << std::endl;
    std::wcout << "SN: " << cur_dev->serial_number << std::endl;
    std::wcout << "manufacturer: "
     << cur_dev->manufacturer_string << std::endl;
    std::wcout << "product: "
     << cur_dev->product_string << std::endl;
    std::cout << "usage page: 0x" << cur_dev->usage_page << std::endl;
    std::cout << "usage: 0x" << cur_dev->usage << std::endl;
    cur_dev = cur_dev->next;
    i++;
  }

  hid_free_enumeration(devs);
  hid_exit();
  return 0;
}
