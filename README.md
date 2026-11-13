# U2C4_security_demo
U2C4_security_demo

## U2C4 clock configuration

Target: external 20 MHz crystal, CPU 240 MHz, `CLK_SBUS` 120 MHz.

The CPU clock mode is selected by flash Option Bytes, not by a runtime PLL
multiplier. Program the following settings with the startup project or flash
programmer, then reset the device. Building or downloading the demo application
alone does not change these Option Bytes.

| Setting | Required field | Value |
| --- | --- | --- |
| OPBT10 | `MOSC_EXCLKINPUT[28] = 1`, `MOSC_FREQ[26:24] = 010b` | Normal 20 MHz crystal |
| OPBT11 | `CKDIVMD[31:29] = 100b` | 480 MHz SSCG, Mode B |
| OPBT11 | `STARTUPPLL[28] = 0` | Start Main OSC and PLL after reset |

With the other bits at their shipping values, OPBT10 is `0xFAFD288A` and OPBT11
is **`0x8FFFFFFF`**. Preserve board-specific oscillator trimming and unrelated
Option Byte fields; do not overwrite them with shipping defaults. To update only
the clock mode and startup selection, use
`new_OPBT11 = (old_OPBT11 & 0x0FFFFFFF) | 0x80000000`.

The shared [clock initialization](icum/target/u2cx/include/driver/u2c4_clock.h)
is used by both ICU-M and PE initialization. It reads OPBT11, performs the clock
gear-up sequence, and finishes with `CKD_PLLC = 0`, `CKD_SSCGC = 0`,
`CKS_CLEANC = 0`, and `CKS_SSCGC = 0`. No runtime divider change is needed for
Mode B. Nonzero PLL/SSCG divider settings are only permitted during clock
gear-up/down, not as a permanent frequency setting.

After initialization with the required Option Bytes:

| Clock | Frequency |
| --- | --- |
| `CLK_CPU` | 240 MHz |
| `CLK_SBUS` | 120 MHz |
| `CLK_HBUS`, `CLK_HSB` | 60 MHz |
| `CLK_UHSB` | 120 MHz |
| `CLK_LSB` | 30 MHz |
| `CLKC_HSB` (Clean domain) | 80 MHz |
| RLIN31 and OSTM | 80 MHz (unchanged) |

Keep the UART and timer frequency values at 80 MHz: RLIN uses `CKS_RLINC = 1`
(`CLKC_HSB`), and OSTM also uses `CLKC_HSB`.

For comparison, OPBT11 `0xEFFFFFFF` selects 640 MHz SSCG and produces CPU
320 MHz / `CLK_SBUS` 160 MHz after the existing initialization. The actual board
frequency depends on the Option Bytes already programmed.

### Debugger checks after reset and initialization

- OPBT10 at `0xFF320BC8`: `(value & 0x17000000) == 0x12000000`.
- OPBT11 at `0xFF320BCC`: `(value & 0xF0000000) == 0x80000000`.
- `CKS_CLEANS` at `0xFF980108` and `CKS_SSCGS` at `0xFF980114`: bit 0 is 0.
- `CKD_PLLC` at `0xFF980120` and `CKD_SSCGC` at `0xFF980130`: both are 0.
- `CKD_PLLS` at `0xFF980128` and `CKD_SSCGS` at `0xFF980134`: bit 1 is 1.

Reference: RH850/U2C Hardware Manual R01UH1018EJ0100, Table 13.7 (pages
917-918), sections 13.5.5.4-13.5.5.6 (pages 965-972), section 13.6.4 (page
1005), and sections 53.12.15-53.12.16 (pages 6785-6787).
