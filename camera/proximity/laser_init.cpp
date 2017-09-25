/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <paths.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/mount.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>

#include <mtd/mtd-user.h>

#include <selinux/selinux.h>
#include <selinux/label.h>
#include <selinux/android.h>

#include <cutils/android_reboot.h>
#include <cutils/fs.h>
#include <cutils/iosched_policy.h>
#include <cutils/list.h>
#include <cutils/sockets.h>
#include <private/android_filesystem_config.h>

#include <memory>
#include <cutils/log.h>

#if 0
#include "devices.h"
#include "init.h"
#include "log.h"
#include "property_service.h"
#include "bootchart.h"
#include "signal_handler.h"
#include "keychords.h"
#include "init_parser.h"
#include "util.h"
#include "ueventd.h"
#include "watchdogd.h"
#endif

int restorecon(const char* pathname)
{
    return selinux_android_restorecon(pathname, 0);
}


int main(int argc, char** argv) {

//added by likelong@camera 2015.5.18 to change the authority of Laser sensor's device node.
  /* Buffer to hold input line by line */
  FILE *fp = fopen("/proc/bus/input/devices","r");
  char line[1024];
  bool found_stm = false;

  /* Device name */
  char devname [PATH_MAX] = "/dev/input";
  char *filename = NULL;
  char * handler = NULL;

  //strcpy(devname, "/dev/input");
  filename = devname + strlen(devname);
  *filename++ = '/';

  while (fgets(line,1024,fp) != NULL){
    if(strstr(line, "STM VL6180 proximity sensor") != NULL) {
    ALOGE("found stm sensor\n");
    found_stm = true;
    }

    if (found_stm) {
      char* res =  strstr(line, "Handlers=");
      if (res) {
        /* iterate through all handlers */
        handler =  strtok (res," ,.-=");\
        while (handler != NULL) {
          /* find eventX */
          if (strncmp (handler, "event", strlen ("event")) == 0) {
	   int len = strlen(handler);
           strlcpy (filename,handler, len+1);
           break;
          }
          handler = strtok (NULL, " ,.-=");
        }

        int res =  link  (devname, "/dev/stm_sensor");
        if (res != 0) {
          ALOGE("cannot create link to stm sensor %s %d:%s\n", devname, res, strerror(errno));
        }
        restorecon ("/dev/stm_sensor");
        restorecon ("/sys/devices/soc.0/fda0c000.qcom,cci/29.qcom,proximity/enable_ps_sensor");
        restorecon ("/sys/devices/soc.0/fda0c000.qcom,cci/29.qcom,proximity/set_delay_ms");
        restorecon ("/sys/devices/soc.0/f9928000.i2c/i2c-6/6-0029/enable_ps_sensor");
        restorecon ("/sys/devices/soc.0/f9928000.i2c/i2c-6/6-0029/set_delay_ms");		
//        restorecon ("/sys/bus/platform/devices/29.qcom,proximity/enable_ps_sensor");
//        restorecon ("/sys/bus/platform/devices/29.qcom,proximity/set_delay_ms");
//        restorecon ("/sys/bus/i2c/devices/i2c-6/6-0029/enable_ps_sensor");
//        restorecon ("/sys/bus/i2c/devices/i2c-6/6-0029/set_delay_ms");
//        restorecon ("/sys/bus/i2c/devices/6-0029/enable_ps_sensor");

        break;
      }
    }
  }


    return 0;
}
