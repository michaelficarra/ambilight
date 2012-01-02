This is a small project for creating an ambilight clone using an Arduino, a
string of LEDs and WS2801s, and boblight. Installation instructions are
provided for the Arduino Uno and Fedora 16, though they won't likely differ
much for any Arduino/Unix-like combination.


## Parts List

* Arduino Uno
* [12mm Diffused Digital RGB LED Pixels (Strand of 25) - WS2801](http://www.adafruit.com/products/322)
* 4 jumpers


## Installation

Make sure you have a `libraries` directory in the directory configured as your
sketchbook in your arduino preferences.

```bash
SKETCHBOOK_PATH=$(grep ^sketchbook.path ~/.arduino/preferences.txt | cut -d = -f 2)
mkdir -p "$SKETCHBOOK_PATH/libraries"
```

Make sure you have the SPI library installed.

```bash
cd /tmp
git clone git://github.com/arduino/Arduino.git arduino
mv arduino/libraries/SPI "$SKETCHBOOK_PATH/libraries"
rm -rf arduino
```

Get the WS2801 library.

```bash
cd "$SKETCHBOOK_PATH/libraries"
git clone git://github.com/adafruit/WS2801-Library.git WS2801
```

Clone this project.

```bash
cd "$SKETCHBOOK_PATH"
git clone git://github.com/michaelficarra/ambilight.git ambilight
```

Boblight requires libavcodec. The Fedora build of ffmpeg didn't seem to come
with libavcodec, so we'll have to get it ourselves.

```bash
cd /tmp
wget http://libav.org/releases/libav-0.7.3.tar.gz -O - | tar xz
cd libav-0.7.3
./configure && make && sudo make install
```

Download and install boblight.

```bash
cd /tmp
svn checkout http://boblight.googlecode.com/svn/trunk/ boblight
cd boblight
./configure && make && sudo make install
```

Install the boblight configuration file.

```bash
sudo cp "$SKETCHBOOK_PATH/ambilight/boblight.conf" /etc/
```

In the ardruino software, `File -> sketchbook -> ambilight`. Configure the
`dataPin`, `clockPin`, `stripLength`, `serialRate`, and `prefix` values.
`Ctrl-u` to compile and upload to your Arduino. Unplug your Arduino and attach
the LED strip to your Arduino as in the following pictures.

![connection to Arduino](http://i.imgur.com/gBIbp.jpg)

![connection to WS2801](http://i.imgur.com/stw2o.jpg)

![WS2801 pins: from top to bottom: data, clock, ground, +5V](http://i.imgur.com/HDW4i.jpg)


## Configuration

There's a few configurable values in `ambilight.ino`.

0. `dataPin`: the number of the Arduino pin connected to the WS2801 data pin
0. `clockPin`: the number of the Arduino pin connected to the WS2801 clock pin
0. `stripLength`: the number of WS2801-LED pairs, must be greater than zero
0. `serialRate`: `rate` value from `[device]` section of `/etc/boblight.conf`
0. `prefix`: `prefix` value from `[device]` section of `/etc/boblight.conf`

`/etc/boblight.conf` has to be configured with the regions (boblight calls
them "lights") that are sampled for each individual LED. Below is a diagram of
my configuration.

![boblight.conf setup, with image of light positions](http://i.imgur.com/g9RV9.png)


## Running

Connect the arduino to USB. The first LED should be red, indicating it is
waiting for a client.

Start the boblight daemon with `boblightd`. To detach it, instead run `nohup
boblightd &>/dev/null &`. `boblight-x11` connects to the `boblightd` daemon and
sends the screen image data. I pass in some options with `-o`.

    boblight-X11 -s 127.0.0.1:19333 -o saturation=1.7 -o value=0.5 -o threshold=60

Again, `nohup` may be used to detach it.
