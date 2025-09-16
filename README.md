# Logging

A simple single header log library written as a personal exercise.

## Basic Usage

Include the header file and assign a reference variable to `Logging::Logger::getLogger("<logger name>")`. Logging is done by accessing the various semantic methods: *log*, *debug*, *info*, *warning* and *error*.

```c++
#include "logging.h"

static Logging::Logger& logger = Logging::Logger::getLogger("my_script");

int main()
{
    logger.log("Hello world!");
    logger.debug("Debug message");
    logger.info("Some information");
    logger.warning("Uh oh something's not right...");
    logger.error("Something went very wrong!");
}
```

Unformatted output:

```
Hello world!
DEBUG:   Debug message
INFO:    Some information
WARNING: Uh oh something's not right...
ERROR:   Something went very wrong!
```

By default any log level above *info* will be written a log file in `/logs`.

## Styling

The file includes a helper function (`Styling::style()`) for writing ASCI escape codes to change console output text appearance.

For example, to style some text in bold and bright red:

```c++
using namespace Styling;

std::cout << style(bold|brightRed) << "Hello world!" << style() << std::endl;
```

Using `Styling::style()` without any arguments will reset the currently applied style.
