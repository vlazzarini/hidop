/*******************************************************************************
 * HID opcodes
 * (c) VL, 2018
 *
 * Licensed under the BSD license (see ../../LICENSE-bsd.txt)
 *******************************************************************************/
#include "hidapi.h"
#include <plugin.h>
#include <sstream>

// HDI device list printing
struct HIDPrint : csnd::Plugin<0, 0> {
  int init() {
    int i = 0;
    struct hid_device_info *devs, *cur_dev;
    std::stringstream sstr;
    hid_init();
    devs = hid_enumerate(0, 0);
    cur_dev = devs;
    sstr << "HID devices connected:\n";
    while (cur_dev) {
      sstr << "====================\n";
      sstr << std::dec;
      sstr << "device #" << i << std::endl;
      sstr << std::hex;
      sstr << "path: " << cur_dev->path << std::endl;
      sstr << "VID: 0x" << cur_dev->vendor_id << std::endl;
      sstr << "PID: 0x" << cur_dev->product_id << std::endl;
      sstr << "usage page: 0x" << cur_dev->usage_page << std::endl;
      sstr << "usage: 0x" << cur_dev->usage << std::endl;
      cur_dev = cur_dev->next;
      csound->message(sstr.str().c_str());
      sstr.str(std::string());
      i++;
    }
    hid_free_enumeration(devs);
    hid_exit();
    return OK;
  }
};

struct SonyJoy : csnd::Plugin<1, 2> {
   hid_device *handle;

   int deinit() {
     hid_close(handle)
       return OK;
   }

   int init() {
     int i = 0;
     struct hid_device_info *devs, *cur_dev;
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
    if (cur_dev) {
      handle = hid_open_path(cur_dev->path);
      if (!handle) {
        csound->message("unable to open device\n");
        return NOTOK;
    } 
   } else return NOTOK;
    hid_set_nonblocking(handle, 1);
    csound->plugin_deinit(this);
    hid_free_enumeration(devs);
    return OK;
   }

  int kperf() {
     int res;
     unsigned char buf[128], mask;
     res = hid_read(handle, buf, sizeof(buf));
     mask = (unsigned char) (inargs[1] > 0 ? inargs[1] : 0xFF);
     if(res)
       outargs[0] = buf[(int) inargs[0]] & mask;
     return OK;
  } 
};

  
#include <modload.h>
void csnd::on_load(Csound *csound) {
  csnd::plugin<HIDPrint>(csound, "hidprint", "", "", csnd::thread::i);
  csnd::plugin<HIDPrint>(csound, "sonyctl", "k", "kk", csnd::thread::ik);
  // tracing for debugging purposes, remove when ready.
  csound->message("HID opcode library loaded \n");
}
