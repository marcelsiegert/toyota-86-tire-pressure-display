# Toyota 86 tire pressure display

An Arduino sketch for displaying tire pressures and temperatures on any
first-generation Subaru BRZ, Toyota 86, or Scion FR-S equipped with TPMS.

It polls TPMS data via the car's CAN bus and shows each tire's readings in the
corresponding corner of the display. Pressures are given in kilopascals with
2 kPa resolution, and temperatures in degrees Celsius with 1 °C resolution. The
display is dimmed when either the headlights or the parking lights are on.
Readings are highlighted in blue or red if the pressure is too low or high.

It's designed for an Arduino UNO R4 Minima and any ST7789 SPI display.

Thanks to [geraldjust] and [timurrrr] on FT86Club.com for figuring out the
CAN frames used to query TPMS data.

## Setup

1. Copy `config.dist.h` to `config.h` and edit the latter as needed.
2. Flash the sketch using Arduino CLI.
3. Connect your display to the Arduino board using the hardware SPI pins.
4. Connect a CAN transceiver to the `CTX0` and `CRX0` pins of the Arduino board.
5. Connect the CAN transceiver to the car's `CANH` and `CANL` lines.

## Disclaimer

This project sends diagnostic requests on the car's CAN bus. Use it at your own
risk. No responsibility is taken for damage to the car or tires, bricked ECUs,
malfunctioning ABS/ESC, non-deploying airbags, fires, etc.

## License

[MIT].


[geraldjust]: https://www.ft86club.com/forums/member.php?u=154285
[MIT]: ./LICENSE
[timurrrr]: https://www.ft86club.com/forums/member.php?u=167739
