# libopenamp-rpu

Generic OpenAMP remote-side (RPU) helper library for AMD/Xilinx SoCs.
Wraps the AMD OpenAMP/remoteproc platform glue in a small, exception-free
C++ API (`mnc::OpenAmpPlatform`, `mnc::RpmsgEndpoint`) safe for
bare-metal / FreeRTOS use. Carries **no board-specific addresses** — every
machine supplies its channel configuration as a generated header (see
*Port contract* below).

```
include/mnc/          C++ API (openamp_platform.hpp, rpmsg_endpoint.hpp)
src/                  C++ implementation
machine/zynqmp_r5/    AMD platform glue for ZynqMP Cortex-R5 (BSD-3-Clause,
                      derived from OpenAMP/openamp-system-reference).
                      Future SoC ports (versal_r5, ...) live as siblings.
```

## Port contract (per machine, per core)

The library separates policy from hardware discovery:

- `openamp_contract.h` supplies the shared-memory layout and IPI channel
  mask that must agree with Linux.
- the BSP's XSA-generated `xparameters.h` supplies the IPI peripheral base
  address and interrupt vector.

The product build generates one `openamp_contract.h` per R5 core from a
canonical machine-readable contract. The BSP/machine-configuration build
renders its Linux domain from the same contract, preventing firmware and
kernel policy from drifting without coupling the RPU build to BSP tooling.

The consuming build must:

1. compile all library sources (src/*.cpp + machine/<soc>/*.c — the C files
   must be compiled as C, not C++),
2. add include paths: `include/`, `machine/<soc>/`, and the directory
   containing the generated `openamp_contract.h` for **this core**,
3. define `MNC_OPENAMP_CONTRACT` and build against a FreeRTOS BSP that
   supplies the matching XSA-generated `xparameters.h`.

Required contract macros (a port may instead pass them as `-D` flags):
`SHARED_MEM_PA`, `SHARED_MEM_SIZE`, `SHARED_BUF_OFFSET`, and
`IPI_CHN_BITMASK`. `IPI_IRQ_VECT_ID` and `POLL_BASE_ADDR` are derived from
the BSP's `XPAR_XIPIPSU_0_*` macros when not explicitly overridden.
A missing channel config **fails the build** (`#error`) — there are no
silent defaults.

## Consumers

- Vitis (Empty-app cmake): list the sources and include dirs in
  `UserConfig.cmake` — see `KR260Demo_RPU` for a working dual-core example.
- Yocto: fetch with `gitsm://` if this repo is consumed as a submodule.

## License

BSD-3-Clause. `machine/` retains AMD/Mentor copyright headers from the
upstream OpenAMP application sources it derives from.
