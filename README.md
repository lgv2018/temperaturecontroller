# Arduino temperature controller

Read a temperature sensor and control a heater and cooler to try to maintain
a specific temperature within a set tolerance.

Provides protection from overruns which might occur if the environment can't
be brought to the desired temperature for any reason.

LEDs display a start-up sequence to indicate successful initialisation, then
light individually to indicate heating or cooling. Both LEDs light together in
the safety period after an over-run, and both flash in sync in the permanent error
mode which is triggered if there are too many consecutive over-runs.

Keep the poll interval reasonably long to prevent jitter, especially if controlling
a Peltier junction as they can be damaged by rapid reversal.

Merlyn Kline arduino@binary.co.uk

Connection is simple, something like this with a Peltier heater/cooler:

```plaintext
-- +5V --------+---------------------+-----------------------+-------
               |  4.7K               |                       |
  +------+     +-/\/\/\/---+---------)-----------------------)------> DS18B20
  |      |     |           |         |                       |
  |  Vcc |-----+           |         |                       |
  |      |                 |         |  +-------------+      |
  |    7 |-----------------+         +--+          o--+------+
  | A    |                              |           /-+------)--------+
  | r  4 |------------------------------+  Relay   o--+--+   |   +----+----+
  | d    |                              |             |  |   |   | Peltier |
  | u  6 |------------------------------+  Module  o--+--)---+   +----+----+
  | i    |      1K   LED Cool           |           /-+--)------------+
  | n  2 |----/\/\/\/----|>|--+      +--+          o--+--+
  | o    |      1K            |      |  +-------------+  |
  |    8 |----/\/\/\/----|>|--+      |                   |
  |      |           LED Heat |      |                   |
  |  GND |-----+              |      |                   |
  |      |     |              |      |                   |
  +------+     |              |      |                   |
               |              |      |                   |
-- GND --------+--------------+------+-------------------+-----------
```
