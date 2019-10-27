# Arduino temperature controller

Read a temperature sensor and control a heater and cooler to try to maintain
a specific temperature within a set tolerance.

Provides protection from overruns which might occur if the environment can't
be brought to the desired temperature for any reason.

LEDs display a startup sequence to indicate successful initialisation, then
light individually to indicate heating or cooling. Both LEDs light together in
the safety period after an over-run, and both flash in sync in the permanent error
mode which is triggered if there are too many consecutive over-runs.

Keep the poll interval reasonably long to prevent jitter, especially if controlling
a peltier junction as they can be damaged by rapid reversal.

Merlyn Kline arduino@binary.co.uk

Connection is simple, something like this:

```
-- +5V --------+---------------------+------------------
               |                     |
  +------+     |                     |
  |      |     |                     |
  |  Vcc |-----+                     |
  |      |                           |  +-------------+
  |    7 |-----                      +--+             +------\
  | A    |                              |             |       => Heater
  | r  4 |------------------------------+   Relay     +------/
  | d    |                              |             |
  | u  6 |------------------------------+   Module    +------\
  | i    |      1K   LED Cool           |             |       => Cooler
  | n  2 |----/\/\/\/----|>|--+      +--+             +------/
  | o    |      1K            |      |  +-------------+
  |    8 |----/\/\/\/----|>|--+      |
  |      |           LED Heat |      |
  |  GND |-----+              |      |
  |      |     |              |      |
  +------+     |              |      |
               |              |      |
-- GND --------+--------------+------+-----------------
```
