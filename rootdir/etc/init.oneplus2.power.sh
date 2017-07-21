#!/system/bin/sh

################################################################################
# helper functions to allow Android init like script

function write() {
    echo -n $2 > $1
}

function copy() {
    cat $1 > $2
}

function get-set-forall() {
    for f in $1 ; do
        cat $f
        write $f $2
    done
}

################################################################################

# disable thermal bcl hotplug to switch governor
write /sys/module/msm_thermal/core_control/enabled 0
get-set-forall /sys/devices/soc.0/qcom,bcl.*/mode disable
bcl_hotplug_mask=`get-set-forall /sys/devices/soc.0/qcom,bcl.*/hotplug_mask 0`
bcl_hotplug_soc_mask=`get-set-forall /sys/devices/soc.0/qcom,bcl.*/hotplug_soc_mask 0`
get-set-forall /sys/devices/soc.0/qcom,bcl.*/mode enable

# some files in /sys/devices/system/cpu are created after the restorecon of
# /sys/. These files receive the default label "sysfs".
restorecon -R /sys/devices/system/cpu

# ensure at most one A57 is online when thermal hotplug is disabled
write /sys/devices/system/cpu/cpu4/online 1
write /sys/devices/system/cpu/cpu5/online 0
write /sys/devices/system/cpu/cpu6/online 0
write /sys/devices/system/cpu/cpu7/online 0

# files in /sys/devices/system/cpu4 are created after enabling cpu4.
# These files receive the default label "sysfs".
# Restorecon again to give new files the correct label.
restorecon -R /sys/devices/system/cpu

# Best effort limiting for first time boot if msm_performance module is absent
write /sys/devices/system/cpu/cpu4/cpufreq/scaling_max_freq 960000

# some files in /sysmodule/msm_performance/parameters are created after the restorecon of
# /sys/. These files receive the default label "sysfs".
restorecon -R /sys/module/msm_performance/parameters

# Limit A57 max freq from msm_perf module in case CPU 4 is offline
write /sys/module/msm_performance/parameters/cpu_max_freq "4:960000 5:960000 6:960000 7:960000"

# configure governor settings for little cluster
write /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor interactive
restorecon -R /sys/devices/system/cpu # must restore after interactive
write /sys/devices/system/cpu/cpu0/cpufreq/interactive/use_sched_load 1
write /sys/devices/system/cpu/cpu0/cpufreq/interactive/use_migration_notif 1
write /sys/devices/system/cpu/cpu0/cpufreq/interactive/above_hispeed_delay "19000 960000:39000 1248000:29000"
write /sys/devices/system/cpu/cpu0/cpufreq/interactive/go_hispeed_load 95
write /sys/devices/system/cpu/cpu0/cpufreq/interactive/timer_rate 20000
write /sys/devices/system/cpu/cpu0/cpufreq/interactive/hispeed_freq 960000
write /sys/devices/system/cpu/cpu0/cpufreq/interactive/io_is_busy 1
write /sys/devices/system/cpu/cpu0/cpufreq/interactive/target_loads "80 960000:95 1248000:99"
write /sys/devices/system/cpu/cpu0/cpufreq/interactive/min_sample_time 40000
write /sys/devices/system/cpu/cpu0/cpufreq/interactive/max_freq_hysteresis 80000
write /sys/devices/system/cpu/cpu0/cpufreq/scaling_min_freq 384000

# configure governor settings for big cluster
write /sys/devices/system/cpu/cpu4/cpufreq/scaling_governor interactive
restorecon -R /sys/devices/system/cpu # must restore after interactive
write /sys/devices/system/cpu/cpu4/cpufreq/interactive/use_sched_load 1
write /sys/devices/system/cpu/cpu4/cpufreq/interactive/use_migration_notif 1
write /sys/devices/system/cpu/cpu4/cpufreq/interactive/above_hispeed_delay "19000 1400000:39000 1700000:19000"
write /sys/devices/system/cpu/cpu4/cpufreq/interactive/go_hispeed_load 90
write /sys/devices/system/cpu/cpu4/cpufreq/interactive/timer_rate 20000
write /sys/devices/system/cpu/cpu4/cpufreq/interactive/hispeed_freq 1248000
write /sys/devices/system/cpu/cpu4/cpufreq/interactive/io_is_busy 1
write /sys/devices/system/cpu/cpu4/cpufreq/interactive/target_loads "90 1248000:95 1800000:70"
write /sys/devices/system/cpu/cpu4/cpufreq/interactive/min_sample_time 40000
write /sys/devices/system/cpu/cpu4/cpufreq/interactive/max_freq_hysteresis 80000
write /sys/devices/system/cpu/cpu4/cpufreq/scaling_min_freq 384000

# restore A57's max
copy /sys/devices/system/cpu/cpu4/cpufreq/cpuinfo_max_freq /sys/devices/system/cpu/cpu4/cpufreq/scaling_max_freq

# plugin remaining A57s
write /sys/devices/system/cpu/cpu5/online 1
write /sys/devices/system/cpu/cpu6/online 1
write /sys/devices/system/cpu/cpu7/online 1

# Restore CPU 4 max freq from msm_performance
write /sys/module/msm_performance/parameters/cpu_max_freq "4:4294967295 5:4294967295 6:4294967295 7:4294967295"

# input boost configuration
write /sys/module/cpu_boost/parameters/input_boost_freq "0:1344000"
write /sys/module/cpu_boost/parameters/input_boost_ms 40

# Configure core_ctl module parameters
write /sys/devices/system/cpu/cpu4/core_ctl/max_cpus 4
write /sys/devices/system/cpu/cpu4/core_ctl/min_cpus 1
write /sys/devices/system/cpu/cpu4/core_ctl/busy_up_thres 70
write /sys/devices/system/cpu/cpu4/core_ctl/busy_down_thres 20
write /sys/devices/system/cpu/cpu4/core_ctl/offline_delay_ms  100
write /sys/devices/system/cpu/cpu4/core_ctl/task_thres 4
write /sys/devices/system/cpu/cpu4/core_ctl/is_big_cluster 1
write /sys/devices/system/cpu/cpu0/core_ctl/max_cpus 4
write /sys/devices/system/cpu/cpu0/core_ctl/min_cpus 0
write /sys/devices/system/cpu/cpu0/core_ctl/busy_up_thres 20
write /sys/devices/system/cpu/cpu0/core_ctl/busy_down_thres 5
write /sys/devices/system/cpu/cpu0/core_ctl/offline_delay_ms 5000
write /sys/devices/system/cpu/cpu0/core_ctl/not_preferred 1
chown system:system /sys/devices/system/cpu/cpu4/core_ctl/min_cpus
chown system:system /sys/devices/system/cpu/cpu4/core_ctl/max_cpus

# Setting B.L scheduler parameters
write /proc/sys/kernel/sched_migration_fixup 1
write /proc/sys/kernel/sched_small_task 30
write /proc/sys/kernel/sched_upmigrate 99
write /proc/sys/kernel/sched_downmigrate 85
write /proc/sys/kernel/sched_freq_inc_notify 400000
write /proc/sys/kernel/sched_freq_dec_notify 400000

# Enable rps static configuration
write /sys/class/net/rmnet_ipa0/queues/rx-0/rps_cpus 8

# Devfreq
get-set-forall  /sys/class/devfreq/qcom,cpubw*/governor bw_hwmon
restorecon -R /sys/class/devfreq/qcom,cpubw*
get-set-forall  /sys/class/devfreq/qcom,mincpubw.*/governor cpufreq

# Disable sched_boost
write /proc/sys/kernel/sched_boost 0

# change GPU initial power level from 305MHz(level 4) to 180MHz(level 5) for power savings
write /sys/class/kgsl/kgsl-3d0/default_pwrlevel 5

# set GPU default governor to msm-adreno-tz
write /sys/class/devfreq/fdb00000.qcom,kgsl-3d0/governor msm-adreno-tz

# re-enable thermal and BCL hotplug
write /sys/module/msm_thermal/core_control/enabled 1
get-set-forall /sys/devices/soc.0/qcom,bcl.*/mode disable
get-set-forall /sys/devices/soc.0/qcom,bcl.*/hotplug_mask $bcl_hotplug_mask
get-set-forall /sys/devices/soc.0/qcom,bcl.*/hotplug_soc_mask $bcl_hotplug_soc_mask
get-set-forall /sys/devices/soc.0/qcom,bcl.*/mode enable
