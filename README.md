# Hot Plate Magnetic Stirrer

Hot Plate Magnetic Stirrer with ESP8266, SSD, MAX31855 and thermocouple

[![GitHub version](https://img.shields.io/github/release/ldab/hot-plate-magnetic-stirrer.svg)](https://github.com/ldab/hot-plate-magnetic-stirrer/releases/latest)
[![Build Status](https://travis-ci.org/ldab/hot-plate-magnetic-stirrer.svg?branch=master)](https://travis-ci.org/ldab/hot-plate-magnetic-stirrer)
[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](https://github.com/ldab/hot-plate-magnetic-stirrer/blob/master/LICENSE)

[![GitHub last commit](https://img.shields.io/github/last-commit/ldab/hot-plate-magnetic-stirrer.svg?style=social)](https://github.com/ldab/hot-plate-magnetic-stirrer)

## TODO

- [ ] `returnError` Handler
- [ ] Update Keywords

###  Current consumption of operating modes μA

Operating mode (HZ) | Low Power | High Resolution
----------------|-------------------|-----------
0.781|1.543|
1.563|1.635|
3.125|1.922|
6.25|2.488|
12.5|3.431|
25|5.784|
50|9.821|
100|18.15|
200|34.72|
400||156
800||156
1600||156

## Interrupt Threshold

Interrupt threshold sensitivity is compared to the top 12bits of the accelerometer 8g output value regardless of the resolution chosen:

> This value can be anything from 1 to 4095

* i.e 0.0039 (1/256) to 16 g (4095/256)

## Known Limitations

* 

## Credits

Github Shields and Badges created with [Shields.io](https://github.com/badges/shields/)
Adafruit Thermocouple Amplifier [MAX31855](https://www.adafruit.com/product/269)