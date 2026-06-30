# Orange Pi Board Inventory

- Collected at: 2026-06-30T18:40:59
- SSH IP: `192.168.0.104`
- SSH user: `ubuntu`
- Password: `<redacted>`

## whoami

Command: `whoami`

```text
ubuntu
```

## hostname

Command: `hostname`

```text
ubuntu
```

## hostname -I

Command: `hostname -I`

```text
192.168.0.104
```

## uname -a

Command: `uname -a`

```text
Linux ubuntu 5.10.0-1012-rockchip #12-Ubuntu SMP Wed Aug 14 22:22:22 UTC 2024 aarch64 aarch64 aarch64 GNU/Linux
```

## os-release

Command: `cat /etc/os-release`

```text
PRETTY_NAME="Ubuntu 22.04.5 LTS"
NAME="Ubuntu"
VERSION_ID="22.04"
VERSION="22.04.5 LTS (Jammy Jellyfish)"
VERSION_CODENAME=jammy
ID=ubuntu
ID_LIKE=debian
HOME_URL="https://www.ubuntu.com/"
SUPPORT_URL="https://help.ubuntu.com/"
BUG_REPORT_URL="https://bugs.launchpad.net/ubuntu/"
PRIVACY_POLICY_URL="https://www.ubuntu.com/legal/terms-and-policies/privacy-policy"
UBUNTU_CODENAME=jammy
```

## lscpu

Command: `lscpu`

```text
Architecture:                       aarch64
CPU op-mode(s):                     32-bit, 64-bit
Byte Order:                         Little Endian
CPU(s):                             8
On-line CPU(s) list:                0-7
Vendor ID:                          ARM
Model name:                         Cortex-A55
Model:                              0
Thread(s) per core:                 1
Core(s) per socket:                 4
Socket(s):                          1
Stepping:                           r2p0
CPU max MHz:                        1800.0000
CPU min MHz:                        408.0000
BogoMIPS:                           48.00
Flags:                              fp asimd evtstrm aes pmull sha1 sha2 crc32 atomics fphp asimdhp cpuid asimdrdm lrcpc dcpop asimddp
Model name:                         Cortex-A76
Model:                              0
Thread(s) per core:                 1
Core(s) per socket:                 2
Socket(s):                          2
Stepping:                           r4p0
CPU max MHz:                        2304.0000
CPU min MHz:                        408.0000
BogoMIPS:                           48.00
Flags:                              fp asimd evtstrm aes pmull sha1 sha2 crc32 atomics fphp asimdhp cpuid asimdrdm lrcpc dcpop asimddp
L1d cache:                          384 KiB (8 instances)
L1i cache:                          384 KiB (8 instances)
L2 cache:                           2.5 MiB (8 instances)
L3 cache:                           3 MiB (1 instance)
Vulnerability Gather data sampling: Not affected
Vulnerability Itlb multihit:        Not affected
Vulnerability L1tf:                 Not affected
Vulnerability Mds:                  Not affected
Vulnerability Meltdown:             Not affected
Vulnerability Mmio stale data:      Not affected
Vulnerability Retbleed:             Not affected
Vulnerability Spec rstack overflow: Not affected
Vulnerability Spec store bypass:    Mitigation; Speculative Store Bypass disabled via prctl
Vulnerability Spectre v1:           Mitigation; __user pointer sanitization
Vulnerability Spectre v2:           Vulnerable: Unprivileged eBPF enabled
Vulnerability Srbds:                Not affected
Vulnerability Tsx async abort:      Not affected
```

## free -h

Command: `free -h`

```text
               total        used        free      shared  buff/cache   available
Mem:           3.8Gi       195Mi       2.6Gi       2.0Mi       1.0Gi       3.6Gi
Swap:             0B          0B          0B
```

## lsblk

Command: `lsblk`

```text
NAME        MAJ:MIN RM  SIZE RO TYPE MOUNTPOINTS
loop0         7:0    0 69.2M  1 loop /snap/core22/1624
loop1         7:1    0   69M  1 loop /snap/core22/2412
loop2         7:2    0 94.4M  1 loop /snap/lxd/30134
loop3         7:3    0 33.7M  1 loop /snap/snapd/21761
mtdblock0    31:0    0   16M  0 disk 
mmcblk1     179:0    0 14.6G  0 disk 
├─mmcblk1p1 179:1    0    4M  0 part 
└─mmcblk1p2 179:2    0 14.6G  0 part /
```

## df -h

Command: `df -h`

```text
Filesystem      Size  Used Avail Use% Mounted on
tmpfs           391M  2.2M  389M   1% /run
/dev/mmcblk1p2   15G  4.8G  9.0G  35% /
tmpfs           2.0G     0  2.0G   0% /dev/shm
tmpfs           5.0M     0  5.0M   0% /run/lock
tmpfs           391M  4.0K  391M   1% /run/user/1000
```

## ip a

Command: `ip a`

```text
1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue state UNKNOWN group default qlen 1000
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
    inet 127.0.0.1/8 scope host lo
       valid_lft forever preferred_lft forever
    inet6 ::1/128 scope host 
       valid_lft forever preferred_lft forever
2: enP4p65s0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc mq state UP group default qlen 1000
    link/ether c0:74:2b:f9:d8:56 brd ff:ff:ff:ff:ff:ff
    inet 192.168.0.104/24 metric 100 brd 192.168.0.255 scope global dynamic enP4p65s0
       valid_lft 4671sec preferred_lft 4671sec
    inet6 fe80::c274:2bff:fef9:d856/64 scope link 
       valid_lft forever preferred_lft forever
3: enP3p49s0: <NO-CARRIER,BROADCAST,MULTICAST,UP> mtu 1500 qdisc mq state DOWN group default qlen 1000
    link/ether c0:74:2b:f9:d8:57 brd ff:ff:ff:ff:ff:ff
```

## python3 --version

Command: `python3 --version || true`

```text
Python 3.10.12
```

## pip3 --version

Command: `pip3 --version || true`

```text
<no stdout>

[stderr]
bash: line 1: pip3: command not found
```

## which git

Command: `which git || true`

```text
/usr/bin/git
```

## which python3

Command: `which python3 || true`

```text
/usr/bin/python3
```

## dev rknn

Command: `ls /dev | grep -i rknn || true`

```text
<no stdout>
```

## usr lib rknn

Command: `ls /usr/lib | grep -i rknn || true`

```text
librknnrt.so
```

## find rknn

Command: `find /usr -iname "*rknn*" 2>/dev/null | head -80`

```text
/usr/local/include/rknn
/usr/local/include/rknn/rknn_api.h
/usr/local/include/rknn/rknn_matmul_api.h
/usr/lib/librknnrt.so
```

## dmesg tail

Command: `dmesg | tail -n 120 || true`

```text
[   24.210609] rk-pcie fe170000.pcie: PCIe Linking... LTSSM is 0x3
[   24.237279] rk-pcie fe170000.pcie: PCIe Linking... LTSSM is 0x3
[   24.263970] rk-pcie fe170000.pcie: PCIe Linking... LTSSM is 0x3
[   24.290718] rk-pcie fe170000.pcie: PCIe Linking... LTSSM is 0x3
[   24.320593] rk-pcie fe170000.pcie: PCIe Linking... LTSSM is 0x3
[   24.578368] systemd[1]: Configuration file /run/systemd/system/netplan-ovs-cleanup.service is marked world-inaccessible. This has no effect as configuration data is accessible via APIs without restrictions. Proceeding anyway.
[   24.643126] systemd[1]: /lib/systemd/system/snapd.service:23: Unknown key name 'RestartMode' in section 'Service', ignoring.
[   24.888834] systemd[1]: Condition check resulted in Root Slice being skipped.
[   24.889040] systemd[1]: Condition check resulted in System Slice being skipped.
[   24.894187] systemd[1]: Queued start job for default target Graphical Interface.
[   24.900076] systemd[1]: Created slice Slice /system/modprobe.
[   24.902415] systemd[1]: Created slice Slice /system/serial-getty.
[   24.904631] systemd[1]: Created slice Slice /system/systemd-growfs.
[   24.906004] systemd[1]: Created slice User and Session Slice.
[   24.906438] systemd[1]: Started Forward Password Requests to Wall Directory Watch.
[   24.907376] systemd[1]: Set up automount Arbitrary Executable File Formats File System Automount Point.
[   24.907809] systemd[1]: Reached target Slice Units.
[   24.907983] systemd[1]: Reached target Mounting snaps.
[   24.908145] systemd[1]: Reached target Swaps.
[   24.908320] systemd[1]: Reached target Local Verity Protected Volumes.
[   24.908728] systemd[1]: Listening on Device-mapper event daemon FIFOs.
[   24.909274] systemd[1]: Listening on LVM2 poll daemon socket.
[   24.909803] systemd[1]: Listening on multipathd control socket.
[   24.910310] systemd[1]: Listening on Syslog Socket.
[   24.910901] systemd[1]: Listening on fsck to fsckd communication Socket.
[   24.911234] systemd[1]: Listening on initctl Compatibility Named Pipe.
[   24.912084] systemd[1]: Listening on Journal Audit Socket.
[   24.912566] systemd[1]: Listening on Journal Socket (/dev/log).
[   24.913124] systemd[1]: Listening on Journal Socket.
[   24.913739] systemd[1]: Listening on Network Service Netlink Socket.
[   24.914371] systemd[1]: Listening on udev Control Socket.
[   24.914816] systemd[1]: Listening on udev Kernel Socket.
[   24.918150] systemd[1]: Mounting Huge Pages File System...
[   24.920712] systemd[1]: Mounting POSIX Message Queue File System...
[   24.923161] systemd[1]: Mounting Kernel Debug File System...
[   24.925590] systemd[1]: Mounting Kernel Trace File System...
[   24.930595] systemd[1]: Starting Journal Service...
[   24.934012] systemd[1]: Starting Restore / save the current clock...
[   24.936921] systemd[1]: Starting Set the console keyboard layout...
[   24.939589] systemd[1]: Starting Create List of Static Device Nodes...
[   24.942213] systemd[1]: Starting Monitoring of LVM2 mirrors, snapshots etc. using dmeventd or progress polling...
[   24.942500] systemd[1]: Condition check resulted in LXD - agent being skipped.
[   24.945130] systemd[1]: Starting Load Kernel Module configfs...
[   24.948027] systemd[1]: Starting Load Kernel Module drm...
[   24.950573] systemd[1]: Starting Load Kernel Module efi_pstore...
[   24.953109] systemd[1]: Starting Load Kernel Module fuse...
[   24.953476] systemd[1]: Condition check resulted in OpenVSwitch configuration for cleanup being skipped.
[   24.956231] systemd[1]: Starting Enable Rockchip camera engine rkaiq...
[   24.956711] systemd[1]: Condition check resulted in File System Check on Root Device being skipped.
[   24.972676] systemd[1]: Starting Load Kernel Modules...
[   24.974623] systemd[1]: Starting Remount Root and Kernel File Systems...
[   24.976645] systemd[1]: Starting Coldplug All udev Devices...
[   24.979264] systemd[1]: Mounted Huge Pages File System.
[   24.979611] systemd[1]: Mounted POSIX Message Queue File System.
[   24.979831] systemd[1]: Mounted Kernel Debug File System.
[   24.980024] systemd[1]: Mounted Kernel Trace File System.
[   24.980854] systemd[1]: Finished Restore / save the current clock.
[   24.981822] systemd[1]: Finished Create List of Static Device Nodes.
[   24.982368] systemd[1]: modprobe@configfs.service: Deactivated successfully.
[   24.982814] systemd[1]: Finished Load Kernel Module configfs.
[   24.983409] systemd[1]: modprobe@drm.service: Deactivated successfully.
[   24.983829] systemd[1]: Finished Load Kernel Module drm.
[   24.984602] systemd[1]: modprobe@efi_pstore.service: Deactivated successfully.
[   24.985009] systemd[1]: Finished Load Kernel Module efi_pstore.
[   24.986985] systemd[1]: Mounting Kernel Configuration File System...
[   24.994009] systemd[1]: Mounted Kernel Configuration File System.
[   24.994449] EXT4-fs (mmcblk1p2): re-mounted. Opts: (null)
[   24.996839] systemd[1]: Finished Remount Root and Kernel File Systems.
[   24.999559] systemd[1]: Starting Device-Mapper Multipath Device Controller...
[   25.001384] systemd[1]: Starting Grow File System on /...
[   25.001628] systemd[1]: Condition check resulted in Platform Persistent Storage Archival being skipped.
[   25.003410] systemd[1]: Starting Load/Save Random Seed...
[   25.005634] systemd[1]: Starting Create System Users...
[   25.021526] fuse: init (API version 7.32)
[   25.023749] systemd[1]: modprobe@fuse.service: Deactivated successfully.
[   25.024368] systemd[1]: Finished Load Kernel Module fuse.
[   25.025539] systemd[1]: Finished Load Kernel Modules.
[   25.028480] systemd[1]: Mounting FUSE Control File System...
[   25.031069] systemd[1]: Starting Apply Kernel Variables...
[   25.031825] systemd[1]: Started Enable Rockchip camera engine rkaiq.
[   25.037136] systemd[1]: Mounted FUSE Control File System.
[   25.065724] systemd[1]: Finished Monitoring of LVM2 mirrors, snapshots etc. using dmeventd or progress polling.
[   25.112920] device-mapper: ioctl: 4.44.0-ioctl (2021-02-01) initialised: dm-devel@redhat.com
[   25.119252] EXT4-fs (mmcblk1p2): resizing filesystem from 3834875 to 3834875 blocks
[   25.120365] systemd[1]: Finished Set the console keyboard layout.
[   25.130637] systemd[1]: Started Journal Service.
[   25.164998] systemd-journald[486]: Received client request to flush runtime journal.
[   25.347445] systemd-journald[486]: File /var/log/journal/60d5771e66eb42109f822a13c082fe35/system.journal corrupted or uncleanly shut down, renaming and replacing.
[   25.775399] squashfs: version 4.0 (2009/01/31) Phillip Lougher
[   25.863937] rk-pcie fe170000.pcie: PCIe Link Fail, LTSSM is 0x3, hw_retries=1
[   26.243360] audit: type=1400 audit(1782305059.404:2): apparmor="STATUS" operation="profile_load" profile="unconfined" name="snap-update-ns.lxd" pid=576 comm="apparmor_parser"
[   26.275669] audit: type=1400 audit(1782305059.437:3): apparmor="STATUS" operation="profile_load" profile="unconfined" name="/snap/snapd/21761/usr/lib/snapd/snap-confine" pid=575 comm="apparmor_parser"
[   26.275690] audit: type=1400 audit(1782305059.437:4): apparmor="STATUS" operation="profile_load" profile="unconfined" name="/snap/snapd/21761/usr/lib/snapd/snap-confine//mount-namespace-capture-helper" pid=575 comm="apparmor_parser"
[   26.304293] audit: type=1400 audit(1782305059.467:5): apparmor="STATUS" operation="profile_load" profile="unconfined" name="snap.lxd.hook.connect-plug-ceph-conf" pid=582 comm="apparmor_parser"
[   26.307100] audit: type=1400 audit(1782305059.467:6): apparmor="STATUS" operation="profile_load" profile="unconfined" name="snap.lxd.check-kernel" pid=579 comm="apparmor_parser"
[   26.310230] audit: type=1400 audit(1782305059.470:7): apparmor="STATUS" operation="profile_load" profile="unconfined" name="snap.lxd.buginfo" pid=578 comm="apparmor_parser"
[   26.313355] audit: type=1400 audit(1782305059.474:8): apparmor="STATUS" operation="profile_load" profile="unconfined" name="snap.lxd.activate" pid=577 comm="apparmor_parser"
[   26.327017] audit: type=1400 audit(1782305059.487:9): apparmor="STATUS" operation="profile_load" profile="unconfined" name="snap.lxd.hook.connect-plug-ovn-certificates" pid=614 comm="apparmor_parser"
[   26.347041] audit: type=1400 audit(1782305059.507:10): apparmor="STATUS" operation="profile_load" profile="unconfined" name="snap.lxd.hook.connect-plug-ovn-chassis" pid=615 comm="apparmor_parser"
[   26.383456] audit: type=1400 audit(1782305059.544:11): apparmor="STATUS" operation="profile_load" profile="unconfined" name="snap.lxd.hook.disconnect-plug-ovn-chassis" pid=618 comm="apparmor_parser"
[   26.569490] pwm-fan pwm-fan: Looking up fan-supply from device tree
[   26.569497] pwm-fan pwm-fan: Looking up fan-supply property in node /pwm-fan failed
[   26.676153] [BT_RFKILL]: bt shut off power
[   26.944205] rk-pcie fe170000.pcie: failed to initialize host
[   29.205153] FAT-fs (mmcblk1p1): utf8 is not a recommended IO charset for FAT filesystems, filesystem will be case sensitive!
[   30.155428] enP3p49s0: 0xffff80000c710000, c0:74:2b:f9:d8:57, IRQ 196
[   30.216066] enP4p65s0: 0xffff80000c690000, c0:74:2b:f9:d8:56, IRQ 154
[   34.713816] kauditd_printk_skb: 9 callbacks suppressed
[   34.713819] audit: type=1400 audit(1782305067.874:21): apparmor="STATUS" operation="profile_load" profile="unconfined" name="/usr/lib/snapd/snap-confine" pid=927 comm="apparmor_parser"
[   34.715178] audit: type=1400 audit(1782305067.877:22): apparmor="STATUS" operation="profile_load" profile="unconfined" name="/usr/lib/snapd/snap-confine//mount-namespace-capture-helper" pid=927 comm="apparmor_parser"
[   35.189083] r8125: enP4p65s0: link up
[   35.189117] IPv6: ADDRCONF(NETDEV_CHANGE): enP4p65s0: link becomes ready
[   36.009136] ttyFIQ ttyFIQ0: tty_port_close_start: tty->count = 1 port count = 2
[   50.540580] vcc_mipicsi0: disabling
[   50.540588] vcc_mipicsi1: disabling
[   50.540590] vcc_mipicsi1: disabling
[ 3008.571120] audit: type=1400 audit(1782794895.084:23): apparmor="DENIED" operation="change_onexec" info="label not found" error=-2 profile="unconfined" name="ubuntu_pro_apt_news" pid=1212 comm="(python3)"
[ 3008.575752] audit: type=1400 audit(1782794895.091:24): apparmor="DENIED" operation="change_onexec" info="label not found" error=-2 profile="unconfined" name="ubuntu_pro_esm_cache" pid=1213 comm="(python3)"
[ 7691.124605] usb 4-1: USB disconnect, device number 2
[24132.179326] systemd-journald[486]: File /var/log/journal/60d5771e66eb42109f822a13c082fe35/user-1000.journal corrupted or uncleanly shut down, renaming and replacing.
```

