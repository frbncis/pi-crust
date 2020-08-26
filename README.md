# Pi Crust

> THIS IS A WORK IN PROGRESS!

Use Raspberry Pi Zero W as hardware [barrier](https://github.com/debauchee/barrier) client for machines that you can't install the software barrier client on!

The Pi Zero W will connect with your barrier server and sends keyboard and mouse controls to the client machine as if it was a USB keyboard and mouse.

In theory, as long as the machine supports USB HID devices, Pi Crust should be able to control it.

## How Does this Work?

It's pretty simple.

The Pi Zero W runs the barrier client and behaves just like another barrier client.

When control changes to the Pi Zero W via barrier, the barrier client will emit X11 mouse and keyboard events. Those events are then captured and forwarded over the USB OTG interface to the end host.

## How To Run

Rough instructions at the moment.

1. Install dependencies on the Pi Zero W on fresh Raspberry Pi OS Lite installation

```
sudo apt-get install --no-install-recommends xserver-xorg
sudo apt-get install --y barrier xinit x11-xserver-utils xserver-xorg-video-dummy
```

2. Configure the Pi Zero W for keyboard emulation https://www.isticktoit.net/?p=1383 

2. Configure the dummy X11 display: https://gist.github.com/divinity76/ce210b5dbcd9ea7d0585ac403caef577

3. On the machine with the keyboard and mouse, start `barriers`.

4. On the Pi Zero W, configure `barrierc` (some steps in the barrier intsall seem to have been missed)

```bash
mkdir -p ~/.local/share/barrier/SSL/Fingerprints
nano ~/.local/share/barrier/SSL/Fingerprints/TrustedServers.txt
# Add Server Fingerprint to TrustedServers.txt
```

5. Install dev dependencies on your Pi Zero

```bash
sudo apt-get install libx11-dev
```

6. Clone the code to your Pi Zero

7. Using `tmux`, start X11 and `barrierc`

* X11
```
startx
```

* `barrierc`
```
barrierc -d DEBUG1 -f --name kvm-01 --enable-crypto --restart 10.160.0.196
```

8. Compile the code in this repository and run it to forward X11 keyboard/mouse events over to USB.

```bash
make all && ./bin/kvm /dev/hidg0
```

## References/Credits
* https://github.com/debauchee/barrier - Provides the network transport for mouse and keyboard commands between computers
* https://www.isticktoit.net/?p=1383 - Tobias Girstmair's instructions on using the Pi Zero W as a USB device, as well as his sample code for USB HID scancodes.
* https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf - USB HID Usage Tables
* https://wiki.osdev.org/USB_Human_Interface_Devices - Human readable description on how USB HID devices work
* https://gist.github.com/javiercantero/7753445 - Sample code for interfacing with X11.
* https://gist.github.com/divinity76/ce210b5dbcd9ea7d0585ac403caef577 - Setting up X11 dummy devices
* http://manpages.ubuntu.com/manpages/xenial/man8/usbhid-dump.8.html - `usbhid-dump` for viewing USB HID reports coming in for debugging
* https://eleccelerator.com/tutorial-about-usb-hid-report-descriptors/ - *amazing* tutorial on USB HID report descriptors
* https://eleccelerator.com/usbdescreqparser/ - USB HID report descriptor parser 
