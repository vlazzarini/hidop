/*******************************************************************************
 * Sony game controller HID capture test
 * (c) VL 2018
 *
 * joystick [item] [bitmask]
 *
 * Licensed under the BSD license (see ../../LICENSE-bsd.txt)
 *******************************************************************************/
#include "hidapi.h"
#include <csignal>
#include <cstdlib>
#include <iostream>
#define MAX_STR 255

static bool running = true;

void handler(int t) { running = false; }

int main(int argc, char *argv[]) {
  int res = 0;
  unsigned char buf[256];
  hid_device *handle;
  int i = 0;
  struct hid_device_info *devs, *cur_dev;
  std::signal(SIGINT, handler);
  int arg1 = 0, arg2 = 0;

  switch(argc) {
   case 3:
     arg2 = std::atoi(argv[2]);
   case 2:
     arg1 = std::atoi(argv[1]);
  }
  

  devs = hid_enumerate(0, 0);
  cur_dev = devs;
  while (cur_dev) {
    // USAGE = 1  (general desktop)
    // USAGE PAGE = 5 (game controls)
    // VENDOR ID = 0x54C
    // PID = 0x5C4 
    if (cur_dev->usage_page == 1 && cur_dev->usage == 5 &&
        cur_dev->vendor_id == 0x54C && cur_dev->product_id == 0x5C4) {
      break;
    }
    cur_dev = cur_dev->next;
    i++;
  }

  // if the device was found
  if (cur_dev) {
    handle = hid_open_path(cur_dev->path);
    if (!handle) {
      printf("unable to open device\n");
      return 1;
    }

    hid_set_nonblocking(handle, 1);
    memset(buf, 0, sizeof(buf));
    while (res >= 0 && running) {
      res = hid_read(handle, buf, sizeof(buf));
      if (res > 0) {
        int val;
        // if arg2 is given, use it as a bitmask
        if(arg2) val = buf[arg1] & arg2 ? 1 : 0;
        // else just retrieve the value
        else val = buf[arg1];
        std::cout << val << std::endl;
      }
    }
    hid_close(handle);
  }
  hid_free_enumeration(devs);
  hid_exit();
  std::cout << "\ngoodbye\n";
  return 0;
}
