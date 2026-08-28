# Testing

Run the portable regression suite from the repository root:

```sh
make test
```

## Test scope

The tests cover all 11 supported sector name-to-number mappings, their reverse mappings, and both invalid-input fallbacks in `src/bit.c`. They do not start a network listener, load world data, or exercise gameplay.

Compiler instrumentation reports 100% line coverage (50 of 50 executable lines) for the tested `sector_number` and `sector_name` functions. Across the entire legacy `src/bit.c` translation unit, the focused suite covers 7.17% (50 of 697 executable lines). No repository-wide coverage percentage is claimed for the legacy server.
