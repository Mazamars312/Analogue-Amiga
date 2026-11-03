# Amiga model profiles and accelerated A1200 support

Status: Plan A (named model profiles) is implemented - resolver in `src/MPUBIOS/app/machine_profiles.h`, menu entry id 140, host tests in `tests/test_machine_profiles.cpp`, field audit in PROFILE_FIELD_MAPPING.md. Plan B (accelerated A1200) remains a proposed implementation plan.

The work has two deliverables:

1. Named machine profiles that apply a documented combination of existing hardware settings.
2. An accelerated A1200-compatible configuration with measured performance improvements and hardware validation.

Profiles make configurations easier to select. Acceleration requires improvements to CPU execution, memory access, or both. The two projects can share configuration infrastructure, but have separate acceptance criteria.

## 1. Current codebase and constraints

The repository already contains a configurable Minimig-derived chipset, a TG68K CPU implementation, an fx68k implementation, SDRAM and cache logic, and an MPU that translates Pocket settings into Amiga configuration commands.

### Relevant source files

| Area | Source | Why it matters |
|---|---|---|
| Pocket menu and persisted choices | [interact.json](dist/Cores/Mazamars312.Amiga/interact.json) | Exposes CPU, chipset, memory, reset, and user preferences |
| Pocket register bridge | [substitute_mcu_apf.v](src/fpga/MPU/substitute_mcu_apf.v) | Receives menu settings and exposes them to the MPU |
| MPU configuration | [minimig_config.cpp](src/MPUBIOS/app/minimig_config.cpp) | Sends configuration commands on initial boot and subsequent reset |
| FPGA configuration receiver | [userio.v](src/fpga/core/amiga/userio.v) | Receives settings and latches most machine configuration during reset |
| Chipset integration | [minimig.v](src/fpga/core/amiga/minimig.v) | Decodes chipset settings into OCS/ECS/AGA behavior |
| CPU selection and memory mapping | [cpu_wrapper.v](src/fpga/core/amiga/cpu_wrapper.v) | Instantiates CPU cores, routes accesses, and implements RAM autoconfiguration |
| CPU implementation | [TG68KdotC_Kernel.vhd](src/fpga/core/amiga/tg68k/TG68KdotC_Kernel.vhd) | Implements the current 020-class execution path and control registers |
| SDRAM controller | [sdram_ctrl.v](src/fpga/core/amiga/sdram_ctrl.v) | Arbitrates memory access and connects CPU/cache traffic to SDRAM |
| Cache implementation | [cpu_cache_new.v](src/fpga/core/amiga/cpu_cache_new.v) | Candidate for cache behavior and performance analysis; confirm active instantiation before editing |
| Top-level integration | [core_top.v](src/fpga/core/core_top.v) | Connects clocks, reset, CPU, memory, and MPU |
| FPGA target and source selection | [ap_core.qsf](src/fpga/ap_core.qsf) | Targets Cyclone V 5CEBA4F23C8 and selects compiled sources |

### Findings that affect the plans

- The menu offers 68000, 68010, and 68020 options, with cache enabled or disabled for 68020.
- CPU selection needs an audit: the wrapper selects TG68K when `cpucfg[1]` is set and fx68k otherwise. The 68010 menu entry alone does not prove that a distinct 68010 implementation is active.
- The menu offers up to 2 MB Chip RAM, 1.5 MB Slow RAM, and 32 MB Fast RAM. Supported combinations and actual address maps need validation.
- Chipset labels need an audit. The menu labels values with bit 3 clear as ECS, while `minimig.v` derives ECS enable from `|chipset_config[4:3]`. Trace the complete path before assigning profile encodings or changing existing labels.
- CPU requests currently use a 16-bit data path through the wrapper. A faster CPU can still spend substantial time waiting for memory.
- The existing CACR interface is four bits wide. It does not establish 030 or 040 architectural cache compatibility.
- `userio.v` latches CPU, chipset, and memory configuration during reset, but updates cache configuration outside that reset block. Configuration changes therefore require care even before the final reset.
- Initial boot and subsequent reset currently have separate command sequences in the MPU. Both must use the same profile resolution rules.
- No FPGA utilization/timing reports or dedicated CPU correctness suite were established during this assessment. Hardware headroom and current performance remain unmeasured.

## 2. Plan A: named model profiles

### Objective and scope

Add a Machine Profile selector that produces a repeatable machine configuration when Reset Amiga is applied. Existing installations retain their current custom settings.

The first release covers configurations achievable with the existing CPU and chipset implementations. Profile names describe compatibility targets; documentation must identify differences from original machines, including CPU speed, expansion mapping, storage interfaces, and unsupported display modes.

Exact A1000 boot hardware, A3000 SCSI/MMU behavior, A4000 CPU support, CDTV, and CD32 peripherals are separate projects. A profile must not imply that those features have been implemented.

### A1. Establish a configuration specification

Audit each field from menu register to FPGA consumer. Record:

- Register address, field mask, default, and persistence behavior.
- Meaning of menu `defaultval` and whether it selects an option index.
- CPU and cache encodings, including the actual behavior of the 68010 choice.
- OCS/ECS/AGA encoding and compatibility implications of correcting existing labels.
- PAL/NTSC and turbo semantics. Do not assume the existing Turbo choice is a general CPU acceleration control.
- Chip/Slow/Fast RAM encodings and valid combinations.
- IDE enablement, floppy settings, and reset sequencing.
- Which controls take effect immediately and which wait for reset.

Deliverable: a field mapping and a reviewed profile table. Resolve the chipset-label discrepancy before releasing named profiles.

### A2. Define the first profile set

| Profile | CPU target | Chipset target | Chip RAM | Slow RAM | Fast RAM | Release condition |
|---|---|---|---:|---:|---:|---|
| Custom / Existing settings | User choice | User choice | User choice | User choice | User choice | Preserve existing behavior |
| A500 compatibility | 68000 | OCS | 512 KB | 512 KB | None | Verify OCS path and memory placement |
| A500 Plus compatibility | 68000 | ECS | 1 MB | None | None | Verify ECS selection and boot/software behavior |
| A600 compatibility | 68000 | ECS | 1 MB | None | None | Verify IDE and relevant ROM behavior |
| A1200 compatibility | 020-compatible | AGA | 2 MB | None | None | Document differences from stock EC020 timing/address behavior |
| AmigaVision | 020-compatible | AGA | 2 MB | None | 32 MB | Preserve the documented recommended setup, including cache disabled |

Keep PAL/NTSC independent of model selection. Specify recommended Kickstart families and ROM requirements for every profile, retaining manual ROM loading. Do not infer full ROM compatibility from file size alone.

Use one initial A500 configuration rather than proliferating variants. Additional memory variants can be introduced after the profile behavior is established.

### A3. Add a central profile resolver

Define stable profile identifiers, reserving zero for Custom. Store the authoritative profile definitions in MPU firmware, near the existing configuration code.

The resolver accepts a snapshot of the profile identifier and relevant menu settings, then returns the effective CPU, cache, chipset, memory, and required peripheral fields.

Rules:

1. Custom passes through the existing machine controls.
2. A named profile supplies its defined machine fields.
3. PAL/NTSC is merged explicitly from the independent video-standard choice.
4. Controller mappings, mouse preferences, audio preferences, and mounted images remain independent.
5. Preserve unrelated bits in shared registers; do not replace whole register values indiscriminately.
6. Unknown identifiers fall back to Custom and produce a diagnostic.
7. Use stable identifiers when adding future profiles; do not repurpose released values.

Share the resolver between initial boot and Reset Amiga. Keep control profiles and bootblock-based game recommendations separate from machine profiles.

### A4. Extend the menu and register bridge

Inventory available menu entries, register fields, and bridge decoding before allocating a profile field. The register bridge currently exposes a finite set of registers; a new menu address may require RTL changes.

Recommended interaction:

- Machine Profile is persisted and defaults to Custom for compatibility.
- Individual CPU/chipset/memory settings are identified as Custom settings.
- Named profiles override those settings at configuration resolution.
- Profile selection is staged until Reset Amiga.
- Returning to Custom restores the saved custom choices.

Check the Pocket menu's capabilities before depending on dynamic hiding, disabling, or updating other controls. If those features are unavailable, use clear static labels and help text. The menu must make it understandable which settings will actually apply.

Distinguish the new profile default from the existing Reset All to Default behavior. Preserve the current AmigaVision-oriented default configuration unless a separate change is explicitly agreed.

### A5. Apply the effective configuration safely

Trace the actual reset controller and implement one shared application sequence:

1. Snapshot the selected configuration.
2. Resolve and validate all effective fields.
3. Hold the Amiga in reset using the existing mechanism.
4. Send the complete effective configuration.
5. Ensure the FPGA has received and latched it before releasing reset.
6. Log the effective profile, CPU/cache settings, chipset, video standard, and memory sizes.

Audit live cache updates so a menu selection cannot change part of a staged machine configuration before reset. Do not add arbitrary delays where command completion or the existing handshake can establish ordering.

Preserve mounted images and user preferences across model changes. A different profile may require a different user-supplied ROM; document that requirement rather than silently substituting a ROM.

### A6. Validation and release criteria

Use focused resolver tests for profile encodings, bit preservation, unknown identifiers, and Custom pass-through. Verify the bridge and reset sequence with simulation where feasible, then validate on Pocket.

| Scenario | Required result |
|---|---|
| Upgrade with saved settings | Custom reproduces the previous effective configuration |
| Cold boot and Reset Amiga | Both apply the same profile definition |
| Change profile without reset | Machine configuration stays staged |
| Named profile with conflicting Custom settings | Named profile wins consistently |
| Return to Custom | Previously saved custom choices remain available |
| Power cycle | Selected profile and independent preferences persist correctly |
| Reset All to Default | Existing documented default behavior is retained |
| Invalid profile identifier | Safe Custom fallback |
| Each supported memory layout | Correct sizes, address placement, and no unintended aliasing |
| PAL and NTSC | Correct video timing and stable operation |

For each released profile, record ROM identity/version, settings, representative games/demos or Workbench workloads, and observed limitations. Do not bundle copyrighted ROMs or commercial disk images.

Acceptance: every released profile applies its documented configuration, survives reset and power cycling, and has evidence for its compatibility claims.

### A7. Milestones and effort

| Milestone | Output |
|---|---|
| A1–A2 | Audited field mapping, profile definitions, and explicit compatibility limits |
| A3–A5 | Resolver, menu/bridge integration, safe reset application, persistence behavior |
| A6 | Validation record, user documentation, packaged firmware/bitstream/menu changes as required |

Planning allowance: approximately 1–2 developer weeks for the mechanism and documentation, assuming working build tools and available Pocket hardware. Chipset/CPU correctness fixes, substantial menu constraints, and newly discovered model-specific bugs are additional work. Re-estimate after the encoding and bridge audit.

## 3. Plan B: accelerated A1200 support

### Objective and scope

Deliver an AGA, 020-compatible machine with 2 MB Chip RAM, validated Fast RAM, and measured improvements over this repository's existing 020 configuration.

This first accelerator does not require 030/040 identification, an MMU, or an FPU. Software needing those features remains outside its compatibility claim. CPU identification must describe the implemented instruction/control behavior honestly.

The existing AGA/020/Fast RAM combination is the starting point. Adding a named preset alone does not satisfy the performance objective.

### B1. Establish reproducible builds and hardware headroom

Build the current revision with the intended Quartus toolchain. Record source revision, tool version, build settings, FPGA resource utilization, memory-block usage, clock frequencies, and timing results.

Identify all clocks and clock-enable relationships touching the CPU, caches, memory controller, and chipset. Audit existing timing constraints before interpreting timing success as permission to increase clock speed.

Create a known-working baseline bitstream and document how to return to it during development. Do not overwrite release artifacts while experimenting.

Deliverable: a baseline build report and an initial feasibility decision about clock changes, cache growth, and wider transfers.

### B2. Measure the current configuration

Use identical ROMs, images, video settings, and software versions for comparisons. Measure cache on/off with a consistent Fast RAM size, then confirm behavior at the other advertised sizes.

| Workload | What it measures |
|---|---|
| Small arithmetic loops | CPU execution with a small working set |
| Fast RAM reads, writes, copies, and larger working sets | Memory throughput and cache miss cost |
| Chip RAM reads/writes under display DMA | CPU/chipset contention |
| CPU-driven rendering or representative demanding AGA software | Application benefit |
| Workbench interaction and application launch | User-visible responsiveness; separate storage effects |
| WHDLoad and self-modifying code workloads | Compatibility under acceleration and caching |

Repeat measurements enough to establish variation. Compare the candidate with both the existing recommended setup and the fastest validated current setup. Record whether a benchmark changes its code path based on CPU detection.

Set a numerical improvement target after the baseline. Avoid advertising a particular MHz or physical accelerator equivalent from one synthetic score.

### B3. Identify the limiting path

Inspect active instantiated logic before modifying files. Trace requests from TG68K through the wrapper, cache, SDRAM arbitration, and completion handshake.

Use simulation or temporary counters to estimate:

- Cycles spent executing versus waiting for memory.
- Cache hits, misses, and line-fill cost where observable.
- Time spent on Fast RAM versus Chip RAM and peripheral accesses.
- Arbitration delays under realistic display, audio, disk, and MPU traffic.
- Cost of split longword and unaligned accesses.

Keep temporary instrumentation out of the final release unless it has a justified diagnostic purpose.

Decision gate: choose the smallest change with a plausible measured benefit. If TG68K execution is the main limit and the FPGA has insufficient headroom, report that result and prepare a separate CPU-replacement proposal.

### B4. Implement improvements in separate stages

The following are candidates, not commitments to implement every item:

| Candidate | When to choose it | Main checks |
|---|---|---|
| Remove avoidable memory wait states | Handshake or arbitration delays dominate | Request lifetime, acknowledgement timing, starvation |
| Improve cache hit/fill handling | Cache behavior dominates | Invalidation, tags, partial writes, DMA visibility |
| Increase CPU execution rate | Execution dominates and timing permits | Clock enables, crossings, reset, peripheral timing |
| Increase internal transfer width or burst use | Split transfers dominate | Byte order, alignment, byte enables, backpressure |
| Change cache capacity/organization | Working-set misses dominate and resources permit | FPGA fit, timing, replacement behavior, real workload gains |

Make one architectural change at a time and repeat the relevant baseline measurements. Preserve accurate chipset and peripheral pacing while accelerating CPU/Fast RAM work. Never increase the entire chipset clock as a shortcut to CPU acceleration.

### B5. Establish memory and cache correctness

Audit these cases before enabling acceleration or cache by default:

- Cache-control writes and clears, including reset and mode transitions.
- Self-modifying code and code loaded into memory after initial execution.
- CPU writes visible to chipset DMA and DMA writes visible to the CPU.
- ROM overlay changes, ROM mirrors, and any accelerated Chip RAM paths.
- Device registers excluded from inappropriate caching or speculative behavior.
- Byte, word, longword, and unaligned access behavior.
- Memory boundaries, autoconfig, and every supported Fast RAM size.
- Interrupts, exception handling, CPU reset, and restart during active I/O.

The current cache need not reproduce a physical 020 cache internally, but its externally visible behavior must remain compatible with the claimed software environment.

Use focused RTL tests and differential CPU checks where appropriate. Hardware validation is required for the SDRAM interface, clock changes, and Pocket-specific integration.

### B6. Integrate the accelerated profile

Reuse Plan A's profile resolver for A1200 Accelerated. Define:

- AGA and 020-compatible execution.
- 2 MB Chip RAM and no Slow RAM.
- A validated default Fast RAM size; choose 32 MB only after validation.
- Validated cache and acceleration settings.
- Independent PAL/NTSC and user input preferences.

Keep cache mode and execution speed conceptually distinct, even if the first release offers one tested combination. Allocate any new acceleration control through an explicit bridge/firmware/RTL specification; do not overload the existing Turbo bit without proving its meaning throughout the design.

A standard A1200 compatibility profile remains useful for software sensitive to acceleration. Document measured behavior rather than claiming exact stock timing unless that has been established.

### B7. Hardware validation and release gate

Validate cold boot, repeated reset, sustained CPU/RAM stress, representative AGA software, WHDLoad, and concurrent display/audio/storage activity. Include both PAL and NTSC and the supported memory sizes.

Record absolute results and improvement ratios, software versions, ROM identity, bitstream revision, and any changed settings. Include a chipset-limited workload so users can see where CPU acceleration does not materially improve performance.

Release requires:

1. Successful FPGA fit with all required timing constraints met and no unexplained relevant timing exceptions.
2. Correct memory reporting and no observed corruption in the validation set.
3. Repeatable gains against the agreed baseline and performance target.
4. No unresolved regressions in the chosen compatibility set.
5. Reliable reset/profile switching and clear documentation of remaining limits.

### B8. Milestones and effort

| Milestone | Output | Decision |
|---|---|---|
| B1–B3 | Build/headroom report, benchmarks, bottleneck evidence | Select an optimization or revise scope |
| B4–B5 | Measured implementation improvement and correctness evidence | Retain, revise, or abandon each candidate |
| B6–B7 | Accelerated profile, hardware validation, documented performance | Release or resolve remaining failures |

Planning allowance: 1–2 developer weeks for characterization, followed by approximately 3–8 weeks for focused improvements and validation if the current architecture has usable headroom. These are engineering estimates, not a guaranteed speedup or delivery date. A CPU replacement or substantial SDRAM redesign requires a new estimate.

## 4. Recommended implementation order

1. Audit configuration encodings, especially chipset labels and CPU selection.
2. Implement profiles with Custom migration and consistent reset application.
3. Validate and release only profiles supported by the evidence.
4. Establish FPGA and performance baselines for the existing A1200-compatible setup.
5. Optimize the measured CPU/memory bottleneck.
6. Validate correctness and integrate A1200 Accelerated.
7. Reassess 030/040 support only when a concrete software requirement or replacement-core performance result justifies it.

## 5. Decisions to settle during implementation

- Which original-machine differences are acceptable for each compatibility profile?
- Does the Pocket menu support the desired conditional controls, or are static Custom labels needed?
- Where can the profile identifier and any acceleration setting fit in the register bridge?
- How should an OCS/ECS label or encoding correction migrate existing installations?
- What are the exact semantics of the existing Turbo setting?
- Which ROMs and software form the repeatable hardware validation set?
- What performance target is justified by the measured baseline and FPGA headroom?
- Which Fast RAM size and cache setting should be the accelerated default?

Resolve these through source inspection and measurements before promising additional CPU models or a physical accelerator equivalent.
