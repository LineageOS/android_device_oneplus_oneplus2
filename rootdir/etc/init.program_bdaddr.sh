#!/vendor/bin/sh

btnvtool -O
# Convert '11.22.33.44.55.66' to '66:55:44:33:22:11'
btaddr=$(btnvtool -p 2>&1 | \
    awk '/--board-address/ {split($2, mac, "."); for (i=1;i<=6;i++) {if (length(mac[i]) == 1) mac[i] = "0" mac[i]; }; printf("%s:%s:%s:%s:%s:%s\n", mac[6], mac[5], mac[4], mac[3], mac[2], mac[1]);} ')
setprop ro.boot.btmacaddr ${btaddr}
