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

struct TrackPad : csnd::Plugin<2,0> {
   hid_device *handle;

   int deinit() {
       hid_close(handle);
       return OK;
   }

   int init() {
     int i = 0;
     struct hid_device_info *devs, *cur_dev;
     devs = hid_enumerate(0, 0);
     if(devs == NULL) csound->init_error("error opening hid devices\n");
     cur_dev = devs;
      while (cur_dev) {
    // USAGE = 1  (general desktop)
    // USAGE PAGE = 2 (mouse)
    // VENDOR ID = 0x5AC
    // PID = 0x278 (Apple Internal Trackpad and keyboard)
       if (cur_dev->usage_page == 1 && cur_dev->usage == 2 &&
        cur_dev->vendor_id == 0x5AC && cur_dev->product_id == 0x343) {
         printf("found %d\n" ,i);
         break;
    }
    i++;   
    cur_dev = cur_dev->next;
    printf("%d %p \n" ,i, cur_dev);
    
    }
    if (cur_dev) {
      handle = hid_open_path(cur_dev->path);
      if (!handle) {
        return csound->init_error("unable to open device\n");
    } 
    } else return csound->init_error("error opening track pad\n");
    hid_set_nonblocking(handle, 1);
    hid_free_enumeration(devs);
    return OK;
   }

  int kperf() {
     int res;
     char buf[16];
     res = hid_read(handle, (unsigned char*) buf, sizeof(buf));
     if(res) {
       outargs[0] = ((MYFLT) buf[2])/128.;
       outargs[1] = ((MYFLT) buf[3])/128.;
     }
     return OK;
  } 
};

struct TouchPad : csnd::Plugin<1,0> {
   hid_device *handle;

   int deinit() {
       hid_close(handle);
       return OK;
   }

   int init() {
     int i = 0;
     struct hid_device_info *devs, *cur_dev;
     devs = hid_enumerate(0, 0);
     cur_dev = devs;
      while (cur_dev) {
    // USAGE = 1  (general desktop)
    // USAGE PAGE = 2 (mouse)
    // VENDOR ID = 0x5AC
    // PID = 0x278 (Apple Internal Trackpad and keyboard)
       if (cur_dev->usage_page == 1 && cur_dev->usage == 2 &&
        cur_dev->vendor_id == 0x5AC && cur_dev->product_id == 0x278) {
      break;
    }
    cur_dev = cur_dev->next;
    i++;
    }
    if (cur_dev) {
      handle = hid_open_path(cur_dev->path);
      if (!handle) {
        return csound->init_error("unable to open device\n");
    } 
   } else return NOTOK;
    hid_set_nonblocking(handle, 1);
    hid_free_enumeration(devs);
    return OK;
   }

  int kperf() {
     int res;
     char buf[16];
     res = hid_read(handle, (unsigned char*) buf, sizeof(buf));
     if(res) {
       outargs[0] = buf[7] == 2 ? 0. : 1.;
     }
     return OK;
  } 
};

struct ClickPad : csnd::Plugin<1,0> {
   hid_device *handle;

   int deinit() {
       hid_close(handle);
       return OK;
   }

   int init() {
     int i = 0;
     struct hid_device_info *devs, *cur_dev;
     devs = hid_enumerate(0, 0);
     cur_dev = devs;
      while (cur_dev) {
    // USAGE = 1  (general desktop)
    // USAGE PAGE = 2 (mouse)
    // VENDOR ID = 0x5AC
    // PID = 0x278 (Apple Internal Trackpad and keyboard)
       if (cur_dev->usage_page == 1 && cur_dev->usage == 2 &&
        cur_dev->vendor_id == 0x5AC && cur_dev->product_id == 0x278) {
      break;
    }
    cur_dev = cur_dev->next;
    i++;
    }
    if (cur_dev) {
      handle = hid_open_path(cur_dev->path);
      if (!handle) {
        return csound->init_error("unable to open device\n");
    } 
   } else return NOTOK;
    hid_set_nonblocking(handle, 1);
    hid_free_enumeration(devs);
    return OK;
   }

  int kperf() {
     int res;
     char buf[16];
     res = hid_read(handle, (unsigned char*) buf, sizeof(buf));
     if(res) {
       outargs[0] = (MYFLT) buf[1];
     }
     return OK;
  } 
};


  
#include <modload.h>
void csnd::on_load(Csound *csound) {
  csnd::plugin<HIDPrint>(csound, "hidprint", "", "", csnd::thread::i);
  csnd::plugin<TrackPad>(csound, "hidtrack", "kk", "", csnd::thread::ik);
  csnd::plugin<TouchPad>(csound, "hidtouch", "k", "", csnd::thread::ik);
  csnd::plugin<ClickPad>(csound, "hidclick", "k", "", csnd::thread::ik);
  // tracing for debugging purposes, remove when ready.
  csound->message("HID opcode library loaded \n");
}
