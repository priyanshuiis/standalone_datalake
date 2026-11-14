# standalone_datalake

A high-performance telemetry and data-collection service for 5G RAN Physical Layer (L1)
processing, targeting the NVIDIA Aerial / cuPHY environment. It captures PUSCH pipeline
data and persists it to ClickHouse, with optional real-time streaming to external dApps
via an E3 Agent over ZeroMQ.

The repo ships "stub" headers (`include/cuphy_types.h`, `include/slot_command_types.h`)
that replicate NVIDIA Aerial SDK types, so the library can be built and tested without
a GPU-enabled 5G stack.

## Data Streams

| Stream | Contents |
|---|---|
| FH (Fronthaul) | Raw IQ samples |
| PUSCH | Decode results: RNTI, MCS, RSRP, CQI, CRC status, etc. |
| HEST | Channel H-estimate matrices |

## Architecture

- **`libdata_lake.so`**: core library. `DataLake::notify()` is the entry point called
  once per slot by the L1 chain. It appends rows to ping-pong buffers; when a buffer
  reaches its row threshold, the buffers swap and an insert task is submitted to an
  internal thread pool that writes to ClickHouse, so the producer thread never blocks
  on DB I/O.
- **`E3Agent`**: optional subsystem (enabled via a constructor flag) that exposes
  telemetry over ZeroMQ: a `REP` socket for manager handshakes, a `PUB` socket for
  streaming, and a `SUB` socket for control commands. dApps subscribe to
  `e3::StreamType` bitfields. Buffers are shared through a POSIX shared-memory segment
  (`/e3_ran_buffers`).
- **`standalone_datalake`**: demo executable that simulates slot indications and PUSCH
  decoder output and feeds them to the library in a loop.

## Requirements

- CMake >= 3.20 and a C++20 compiler
- `fmt`, `nlohmann_json`, `libzmq`, Threads
- `clickhouse-cpp-lib` plus its transitive libraries, expected in `/usr/local/lib`:
  `cityhash`, `lz4`, `zstdstatic`, `absl_int128`
- A running ClickHouse server (default address `localhost`, native port `9000`)

## Build

```bash
cmake -S . -B build
cmake --build build -j
```

## Run

```bash
./build/standalone_datalake
```

The demo constructs `DataLake` with:

| Parameter | Value |
|---|---|
| `enableDbInsert` | `true` |
| `dbAddress` | `"localhost"` |
| `dbEngine` | `"Memory"` |
| `dataTypes` | `{"fh", "pusch", "hest"}` |
| `e3AgentEnabled` | `false` |

It then loops over SFN 0-1023 with 20 slots per SFN, calling `dl.notify(...)` once per
slot (1 ms apart), and waits 5 seconds at the end so pending inserts can drain.

To point the demo at a different ClickHouse instance or to enable the E3 Agent, change
the constructor arguments in `main.cpp`.

## Docker

```bash
docker build -t standalone_datalake .
docker run standalone_datalake
```

> **Note:** The current Dockerfile (Ubuntu 24.04) installs only a basic toolchain plus
> `libfmt-dev` and `nlohmann-json3-dev`. It does not install `libzmq` or build/install
> `clickhouse-cpp` and the other `/usr/local/lib` libraries, and it does not run the
> CMake build. You will need to extend it before the image can build and run the demo.

## Integrating with L1 code

1. Link against `libdata_lake.so`.
2. Construct a `DataLake` instance.
3. Call it once per slot:

   ```cpp
   dl.notify(cell_idx, &slot_ind, &pusch_params, &data_out, &stat_prms);
   ```

4. Replace the stub headers with the real Aerial SDK headers (`cuphy.h`,
   `slot_command/slot_command.hpp`, etc.). The real includes are already present,
   commented out, in `src/data_lake.hpp`.

## License

Apache-2.0 (per the SPDX headers in the source files).
