# Board NPU Probe

- Collected at: 2026-06-30T18:43:11
- SSH IP: `192.168.0.104`
- SSH user: `ubuntu`
- Password: `<redacted>`

```text
===== model =====
$ cat /proc/device-tree/model 2>/dev/null | tr '\0' '\n' || true
Orange Pi 5 Plus

===== cpuinfo-tail =====
$ cat /proc/cpuinfo | tail -30
CPU revision	: 0

processor	: 5
BogoMIPS	: 48.00
Features	: fp asimd evtstrm aes pmull sha1 sha2 crc32 atomics fphp asimdhp cpuid asimdrdm lrcpc dcpop asimddp
CPU implementer	: 0x41
CPU architecture: 8
CPU variant	: 0x4
CPU part	: 0xd0b
CPU revision	: 0

processor	: 6
BogoMIPS	: 48.00
Features	: fp asimd evtstrm aes pmull sha1 sha2 crc32 atomics fphp asimdhp cpuid asimdrdm lrcpc dcpop asimddp
CPU implementer	: 0x41
CPU architecture: 8
CPU variant	: 0x4
CPU part	: 0xd0b
CPU revision	: 0

processor	: 7
BogoMIPS	: 48.00
Features	: fp asimd evtstrm aes pmull sha1 sha2 crc32 atomics fphp asimdhp cpuid asimdrdm lrcpc dcpop asimddp
CPU implementer	: 0x41
CPU architecture: 8
CPU variant	: 0x4
CPU part	: 0xd0b
CPU revision	: 0

Serial		: 0f9cf9b8f01903b9

===== npu-dev =====
$ ls -l /dev/rknpu* /dev/dri/* 2>/dev/null || true
crw-rw---- 1 root video  226,   0 Jun 24 12:44 /dev/dri/card0
crw-rw---- 1 root video  226,   1 Jun 24 12:44 /dev/dri/card1
crw-rw---- 1 root render 226, 128 Jun 24 12:44 /dev/dri/renderD128
crw-rw---- 1 root render 226, 129 Jun 24 12:44 /dev/dri/renderD129

/dev/dri/by-path:
total 0
lrwxrwxrwx 1 root root  8 Jun 24 12:44 platform-display-subsystem-card -> ../card0
lrwxrwxrwx 1 root root 13 Jun 24 12:44 platform-display-subsystem-render -> ../renderD128
lrwxrwxrwx 1 root root  8 Jun 24 12:44 platform-fdab0000.npu-card -> ../card1
lrwxrwxrwx 1 root root 13 Jun 24 12:44 platform-fdab0000.npu-render -> ../renderD129

===== rknn-libs =====
$ ls -l /usr/lib/librknnrt.so /usr/local/include/rknn/* 2>/dev/null || true
-rw-r--r-- 1 root root 5241256 Jun 24 06:19 /usr/lib/librknnrt.so
-rwxr-xr-x 1 root root   31505 Jun 24 06:19 /usr/local/include/rknn/rknn_api.h
-rw-r--r-- 1 root root    8175 Jun 24 06:19 /usr/local/include/rknn/rknn_matmul_api.h

===== rknpu-dmesg =====
$ dmesg | grep -i -E 'rknpu|npu|rknn|rk3588' | tail -120 || true
[   19.136565] input: febf0030.pwm as /devices/platform/febf0030.pwm/input/input0
[   19.137911] input: rk805 pwrkey as /devices/platform/feb20000.spi/spi_master/spi2/spi2.0/rk805-pwrkey.10.auto/input/input1
[   19.169247] vdd_npu_s0: supplied by vcc5v0_sys
[   19.173383] vdd_npu_s0: 550 <--> 950 mV at 800 mV, enabled
[   19.195291] rkisp_hw fdcb0000.rkisp: max input:0x0@0fps
[   19.277063] input: rockchip-dp0 rockchip-dp0 as /devices/platform/dp0-sound/sound/card0/input2
[   19.278131] input: rockchip-hdmi0 rockchip-hdmi0 as /devices/platform/hdmi0-sound/sound/card1/input3
[   19.279278] input: rockchip-hdmi1 rockchip-hdmi1 as /devices/platform/hdmi1-sound/sound/card2/input4
[   19.280178] input: headset-keys as /devices/platform/es8388-sound/input/input5
[   19.343105] input: rockchip-es8388 Headset as /devices/platform/es8388-sound/sound/card3/input6
[   19.348814] input: bt-powerkey as /devices/platform/wireless-bluetooth/input/input7
[   19.404620] input: adc-keys as /devices/platform/adc-keys/input/input8
[   19.405669] rockchip-pm-domain fd8d8000.power-management:power-controller: Looking up npu-supply from device tree
[   19.405703] rockchip-pm-domain fd8d8000.power-management:power-controller: Looking up npu-supply property in node /power-management@fd8d8000/power-controller failed
[   19.409187] RKNPU fdab0000.npu: Adding to iommu group 0
[   19.409364] RKNPU fdab0000.npu: RKNPU: rknpu iommu is enabled, using iommu mode
[   19.409503] RKNPU fdab0000.npu: Looking up rknpu-supply from device tree
[   19.410258] RKNPU fdab0000.npu: Looking up mem-supply from device tree
[   19.410932] RKNPU fdab0000.npu: can't request region for resource [mem 0xfdab0000-0xfdabffff]
[   19.410975] RKNPU fdab0000.npu: can't request region for resource [mem 0xfdac0000-0xfdacffff]
[   19.411004] RKNPU fdab0000.npu: can't request region for resource [mem 0xfdad0000-0xfdadffff]
[   19.411679] [drm] Initialized rknpu 0.9.6 20240322 for fdab0000.npu on minor 1
[   19.412283] rockchip-pm-domain fd8d8000.power-management:power-controller: Looking up nputop-supply from device tree
[   19.412322] rockchip-pm-domain fd8d8000.power-management:power-controller: Looking up nputop-supply property in node /power-management@fd8d8000/power-controller failed
[   19.412370] rockchip-pm-domain fd8d8000.power-management:power-controller: Looking up npu1-supply from device tree
[   19.412404] rockchip-pm-domain fd8d8000.power-management:power-controller: Looking up npu1-supply property in node /power-management@fd8d8000/power-controller failed
[   19.412442] rockchip-pm-domain fd8d8000.power-management:power-controller: Looking up npu2-supply from device tree
[   19.412475] rockchip-pm-domain fd8d8000.power-management:power-controller: Looking up npu2-supply property in node /power-management@fd8d8000/power-controller failed
[   19.412594] RKNPU fdab0000.npu: Looking up rknpu-supply from device tree
[   19.412625] vdd_npu_s0: could not add device link fdab0000.npu: -EEXIST
[   19.412630] vdd_npu_s0: Failed to create debugfs directory
[   19.413229] RKNPU fdab0000.npu: Looking up mem-supply from device tree
[   19.415368] vdd_npu_s0: could not add device link fdab0000.npu: -EEXIST
[   19.415373] vdd_npu_s0: Failed to create debugfs directory
[   19.415951] RKNPU fdab0000.npu: Looking up rknpu-supply from device tree
[   19.415973] vdd_npu_s0: could not add device link fdab0000.npu: -EEXIST
[   19.415976] vdd_npu_s0: Failed to create debugfs directory
[   19.421149] RKNPU fdab0000.npu: RKNPU: bin=0
[   19.421369] RKNPU fdab0000.npu: leakage=6
[   19.421410] RKNPU fdab0000.npu: Looking up rknpu-supply from device tree
[   19.421433] debugfs: Directory 'fdab0000.npu-rknpu' with parent 'vdd_npu_s0' already present!
[   19.421449] vdd_npu_s0: Failed to create debugfs directory
[   19.431733] RKNPU fdab0000.npu: pvtm=826
[   19.440436] RKNPU fdab0000.npu: pvtm-volt-sel=1
[   19.442191] RKNPU fdab0000.npu: avs=0
[   19.442466] RKNPU fdab0000.npu: l=15000 h=85000 hyst=5000 l_limit=0 h_limit=800000000 h_table=0
[   19.454140] RKNPU fdab0000.npu: failed to find power_model node
[   19.454191] RKNPU fdab0000.npu: RKNPU: failed to initialize power model
[   19.454215] RKNPU fdab0000.npu: RKNPU: failed to get dynamic-coefficient
[   19.813838] input: Logitech USB Receiver as /devices/platform/fc8c0000.usb/usb4/4-1/4-1:1.0/0003:046D:C534.0001/input/input9
[   19.871210] hid-generic 0003:046D:C534.0001: input,hidraw0: USB HID v1.11 Keyboard [Logitech USB Receiver] on usb-fc8c0000.usb-1/input0
[   19.877829] input: Logitech USB Receiver Mouse as /devices/platform/fc8c0000.usb/usb4/4-1/4-1:1.1/0003:046D:C534.0002/input/input10
[   19.934089] input: Logitech USB Receiver Consumer Control as /devices/platform/fc8c0000.usb/usb4/4-1/4-1:1.1/0003:046D:C534.0002/input/input11
[   19.934422] input: Logitech USB Receiver System Control as /devices/platform/fc8c0000.usb/usb4/4-1/4-1:1.1/0003:046D:C534.0002/input/input12
[   19.934873] hid-generic 0003:046D:C534.0002: input,hiddev96,hidraw1: USB HID v1.11 Mouse [Logitech USB Receiver] on usb-fc8c0000.usb-1/input1

===== python-packages =====
$ python3 -c "import importlib.util; mods=['rknnlite','rknn','rknn_toolkit_lite2','numpy','cv2']; [print(m, bool(importlib.util.find_spec(m))) for m in mods]"
rknnlite False
rknn False
rknn_toolkit_lite2 False
numpy False
cv2 False
```
