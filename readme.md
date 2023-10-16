# SAMEthing

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

SAMEthing is a [Specific Area Message Encoding (SAME)](https://en.wikipedia.org/wiki/Specific_Area_Message_Encoding) header generator.

**WARNING**: The author is not responsible for the misuse of this software. The
output of this software has the potential to activate EAS decoder systems in the
continental United States of America and its territories, along with various
offshore marine areas, if transmitted.

Do not transmit the output of this software over any radio communication unless
so authorized.

Do not play the output of this software in public areas.

If you are under U.S. jurisdiction, please read 
[Misuse of the Emergency Alert System (EAS) Sound](https://www.fcc.gov/enforcement/areas/misuse-eas-sound) for more information.

## Features

* Highly portable core functionality:
    - Written in the [C programming language](https://en.wikipedia.org/wiki/C_(programming_language)),
      targeting the [C17 standard](https://en.wikipedia.org/wiki/C17_(C_standard_revision)).
      - However, it should work just fine using [C99](https://en.wikipedia.org/wiki/C99),
        too.
    - No dynamic memory allocation
    - Single-precision floating point only
    - Consists of only 4 files

* [Qt](https://qt.io) based GUI frontend for
  [Windows](https://www.microsoft.com/en-us/windows?r=1),
  [macOS](https://www.apple.com/macos/), and
  [Linux](https://www.linux.org/) based systems.

* Fully unit tested with [GoogleTest](https://github.com/google/googletest).

### Credits
  * INI parser for frontends: [inih by Ben Hoyt](https://github.com/benhoyt/inih)
  * GUI framework for Windows/Linux/macOS: [Qt](https://qt.io)
  * Audio processing for Windows/Linux/macOS: [SDL](https://libsdl.org)
