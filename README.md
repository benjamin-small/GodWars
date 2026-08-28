# GodWars

GodWars preserves and incrementally cleans up the historical GodWars Deluxe MUD server, a C codebase derived from DikuMUD and Merc. The source was imported from the `gw_deluxe.tgz` archive formerly published by MudBytes.

Original archive checksums:

- MD5: `5380ebef7d4fbec4ade83d5abb6284ca`
- SHA-256: `7db41f4c4693e86f8ad7a64843bd31656795349dbd7bc77cf163c2c53fc87cf7`

## Build and usage

The historical build targets a Unix-like system with GCC and `libcrypt`. The supported language version for the portable validation suite is GNU C99; the full server retains compiler-era assumptions that may require platform-specific maintenance. From the source directory:

```sh
cd src
make
cd ../area
../src/merc 1234
```

Port `1234` is an example. This legacy network service has not been hardened for exposure to untrusted networks; use an isolated environment while evaluating it. See [configuration](docs/configuration.md) for its working-directory and data-file assumptions.

## Testing

```sh
make test
```

The portable suite validates sector mapping behavior without starting the server. See [testing documentation](docs/testing.md) for scope, measured coverage, and exclusions.

## Licensing

This code carries several inherited, non-standard terms. Read [licensing documentation](docs/licensing.md) and the original files it references before use or redistribution.
