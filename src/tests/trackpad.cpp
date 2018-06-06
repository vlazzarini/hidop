/*******************************************************************************
 * MacOS Trackpad HID capture test
 * (c) VL 2018
 *  
 * Licensed under the BSD license (see ../../LICENSE-bsd.txt)
 *******************************************************************************/
#include <iostream>
#include "hidapi.h"
#include <csignal>
#define MAX_STR 255

static bool running = true;

void handler(int t) {
  running = false;
}

int main(int argc, char* argv[])
{
  int res = 0;
  unsigned char buf[256];
  hid_device *handle;
  int i = 0;
  struct hid_device_info *devs, *cur_dev;
  std::signal(SIGINT, handler);

  devs = hid_enumerate(0,0);
  cur_dev = devs;	
  while (cur_dev) {
    // USAGE = 1  (general desktop) 
    // USAGE PAGE = 2 (mouse)
    // VENDOR ID = 0x5AC 
    // PID = 0x278 (Apple Internal Trackpad and keyboard)
    if(cur_dev->usage_page == 1 &&
       cur_dev->usage == 2  &&
       cur_dev->vendor_id == 0x5AC &&
       cur_dev->product_id ==  0x278) {
      break;
    }
    cur_dev = cur_dev->next;
    i++;
  }

  // if the device was found
  if(cur_dev) {    
    handle = hid_open_path(cur_dev->path);
    if (!handle) {
      printf("unable to open device\n");
      return 1;
    }

    hid_set_nonblocking(handle, 1);
    memset(buf,0,sizeof(buf));
    while (res >= 0 && running) {
      res = hid_read(handle, buf, sizeof(buf));
      if(res > 0) {
        for (i = 0; i < res; i++) {
          std::cout << (int) buf[i] << " ";
        }
        std::cout << std::endl;
      }
    }
    hid_close(handle);
  }      
  hid_free_enumeration(devs);
  hid_exit();
  std::cout << "\ngoodbye\n";
  return 0;
}
