# Configuration

GodWars is configured through compile-time definitions and constants in `src/merc.h`, plus the area and data files under `area/`. It does not use environment variables or a checked-in runtime secret file.

The historical server expects to start with `area/` as its working directory so relative paths such as `area.lst` and `../player/` resolve correctly. Review the inherited Diku, Merc, and GodWars license terms before operating a server.
