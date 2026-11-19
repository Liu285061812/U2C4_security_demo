# U2C4_security_demo
U2C4_security_demo

## Project guides

- [ICUM 工程说明](ICUM_PROJECT_GUIDE.md)
- [PE 工程说明](PE_PROJECT_GUIDE.md)

## U2C4 clock configuration

Target: external 20 MHz crystal, CPU 320 MHz, `CLK_SBUS` 160 MHz.

The CPU clock mode is selected by flash Option Bytes, not by a runtime PLL
multiplier. Program the following settings with the startup project or flash
programmer, then reset the device. Building or downloading the demo application
alone does not change these Option Bytes.

| Setting | Required field | Value |
| --- | --- | --- |
| OPBT10 | `MOSC_EXCLKINPUT[28] = 1`, `MOSC_FREQ[26:24] = 010b` | Normal 20 MHz crystal |
| OPBT11 | `CKDIVMD[31:29] = 111b` | 640 MHz SSCG |
| OPBT11 | `STARTUPPLL[28] = 0` | Start Main OSC and PLL after reset |

With the other bits at their shipping values, OPBT10 is `0xFAFD288A` and OPBT11
is **`0xEFFFFFFF`**. Preserve board-specific oscillator trimming and unrelated
Option Byte fields; do not overwrite them with shipping defaults. To update only
the clock mode and startup selection, use
`new_OPBT11 = (old_OPBT11 & 0x0FFFFFFF) | 0xE0000000`.

The shared [clock initialization](icum/target/u2cx/include/driver/u2c4_clock.h)
is used by both ICU-M and PE initialization. It reads OPBT11, performs the clock
gear-up sequence, and finishes with `CKD_PLLC = 0`, `CKD_SSCGC = 0`,
`CKS_CLEANC = 0`, and `CKS_SSCGC = 0`. The startup helper performs the required
gear-up; no additional permanent divider setting is needed. Nonzero PLL/SSCG
divider settings are only used during clock gear-up/down.

After initialization with the required Option Bytes:

| Clock | Frequency |
| --- | --- |
| `CLK_CPU` | 320 MHz |
| `CLK_SBUS` | 160 MHz |

Keep the UART and timer frequency values at 80 MHz: RLIN uses `CKS_RLINC = 1`
(`CLKC_HSB`), and OSTM also uses `CLKC_HSB`.

The actual board frequency depends on the Option Bytes programmed into the
device; changing the CSA definition alone does not affect a device that has not
been programmed with the updated Option Bytes.

### Debugger checks after reset and initialization

- OPBT10 at `0xFF320BC8`: `(value & 0x17000000) == 0x12000000`.
- OPBT11 at `0xFF320BCC`: `(value & 0xF0000000) == 0xE0000000`.
- `CKS_CLEANS` at `0xFF980108` and `CKS_SSCGS` at `0xFF980114`: bit 0 is 0.
- `CKD_PLLC` at `0xFF980120` and `CKD_SSCGC` at `0xFF980130`: both are 0.
- `CKD_PLLS` at `0xFF980128` and `CKD_SSCGS` at `0xFF980134`: bit 1 is 1.

Reference: RH850/U2C Hardware Manual R01UH1018EJ0100, Table 13.7 (pages
917-918), sections 13.5.5.4-13.5.5.6 (pages 965-972), section 13.6.4 (page
1005), and sections 53.12.15-53.12.16 (pages 6785-6787).

## PE ACEU engine tests

The hardware name is **ACEU** (the `aecu` spelling is also accepted by UART).
Program the matching PE and ICUM images, then enter `aceu 0` to run all modes,
or `aceu` to open the menu. Existing `icum` commands are unchanged.

- ECB, CBC, CTR, CFB-128, OFB, GCM, XTS, CCM, CMAC, GMAC, MP compression and PRNG.
- The PE driver is entirely in
  [aceu.c](pe/target/u2cx/src/driver/aceu/aceu.c);
  its API is [r_aceu.h](pe/generic/icumif/r_aceu.h).
- ICUM service `0x02` provisions public demo keys into protected ACEU_KEYRAM
  and initializes PRNG context #3; PE never receives key bytes.
- Aligned 10KB operations use ACEU's own DTF. Formatting, short authenticated
  phases, XTS final blocks and MP/PRNG use CPU transfers. Each printed operation
  reports its actual CPU/DTF block counts and OSTM elapsed time.
- Tests compare full ciphertext/PRNG output and tags with independent expected
  bytes, and check decryption, short/empty
  messages and plaintext erasure after failed GCM/CCM authentication.

GCM/GMAC currently accept 96-bit nonces. MP is the raw, unpadded 16-byte-block
compression function, not a padded SHE hash. PRNG uses public deterministic
test keys/seeds and **must not be used as an entropy source**.
See the [PE ACEU interface and limits](PE_PROJECT_GUIDE.md#81-pe-aceu-tests)
and [ICUM provisioning](ICUM_PROJECT_GUIDE.md#73-aceu-keyram-provisioning).

## Mandatory boot-data CMAC checks

Download **both** the usual firmware and the separate
[boot_auth_data.mot](boot_auth/boot_auth_data.mot) before resetting.
The normal top-level build also builds this data-only MOT; it is deliberately
not embedded in the firmware MCA/MOT. Its reserved nonsecure Data Flash window
is `0xFF23C000..0xFF23FFFF`.

The new, independent 10KB plaintext and two AES-128-CMAC values occupy
`0xFF23C000..0xFF23E81F`. Existing ICUM and ACEU test data are unchanged.
ICUM checks BF bit 15 in the read-only blank mirror (`0xFF43C000`), without
entering Flash P/E mode, then authenticates the plaintext using
its own key, provisions the different PE CMAC key into ACEU slot 6, and only
then releases PE cores. PE1 verifies the same plaintext with ACEU before
notifying PE2 or entering the UART interpreter. Failures print an error and
remain in `while (1)`; missing boot data therefore blocks startup.
ICUM services its watchdog during bounded error-UART waits and the halt loop;
system-error handlers cannot release PE before authentication/provisioning passes.

These are public demonstration keys, not production secrets. This authenticates
only the dedicated plaintext, not PE firmware or the existing test vectors.
See [boot-data layout and build](PE_PROJECT_GUIDE.md#82-dedicated-boot-data-authentication).

## ICUM crypto services

The service groups are organized by algorithm:

- `service_01`: AES
- `service_02`: RSA, including key generation
- `service_03`: random-number services
- `service_04`: ECC/ECDSA, ECDH, EdDSA, and ECC key generation
- `service_05`: HASH/HMAC

Demo keys are initialized in ICUM RAM; generated RSA/ECC keys are RAM-only.
Key import and update services are not dispatched.
The shared service descriptor contains the parameters for the active
AES, RSA, RNG, ECC, HASH, HMAC, and ACEU preparation services; legacy import/update and GC test
flows are excluded from the PE builds.

The PE crypto-service tests use `ISD_BUFFER` for request descriptors and
`BIG_BUFFER` for shared crypto data. The PE service wrapper reports the algorithm
and mode (for example, `AES-CBC encrypt`), result, elapsed time in microseconds,
and RSA latency in ms/ops or non-RSA payload throughput in decimal MB/s,
using the configured OSTM0 clock (80 MHz with the
required Option Bytes). Timing includes
request and response overhead and is valid for intervals shorter than one
32-bit counter period (approximately 53.7 seconds at 80 MHz).
Raw ticks are no longer printed. All RSA services, including key generation,
report milliseconds per operation rather than calculating MB/s.
Other payload throughput uses the input-message length, or output length for RNG generation;
AES authenticated-cipher measurements exclude AAD/IV/tag. Bit lengths round
up to bytes. Services without payload omit the throughput field entirely.
Failed measured services, an uninitialized
timer clock, or a zero timer delta also report `N/A`; valid empty messages report
zero MB/s. Rates use 64-bit fixed-point arithmetic with six decimal places.

Run `icum 6`, or select option 6 in the ICUM/grouped menu, to run all existing
algorithm tests in ascending service-ID order: 0x10-0x12, 0x20-0x24, 0x30-0x32,
0x40-0x45, 0x50-0x51. Preparation requests retain their dependency order, so
individual RNG/signature/AES requests inside a test need not be ascending.
This reuses the current menu coverage rather than adding all hash variants.
Key generation replaces RAM keys. The default configuration runs 19 service
entries with 29 test results; failures do not suppress later tests. The final
summary counts passed/failed tests and skipped service entries, reporting
`INCOMPLETE` if any tests were disabled and none failed.
ICUM drives P17_0 (`ICUMGPIO0`) low at startup and marks algorithm execution
directly within ICUM. PE BEGIN/END notifications and demo service `0x01`
have been removed.
Encryption/decryption, signing/verification, MAC generation/verification,
and preparation requests each produce separate pulses; asynchronous re-entry
within one request does not. The 29 top-level results are unchanged, but pulse
counts follow operations that enter the cryptographic core, not top-level tests.
Preflight failures have no pulse. Actual `ICUM_OPBT4.ICUMOPGPIO`
must be enabled; the firmware reports an error instead of silently omitting
markers. GPIO excludes service-layer preparation, output formatting, final
cleanup, inter-core notifications and UART output. Complete cryptographic
operations retain necessary internal hashing, padding, KDF and driver work;
GPIO is not a pure hardware BUSY signal. OSTM still measures the end-to-end
algorithm-request interval.
See [GPIO prerequisites and timing scope](ICUM_PROJECT_GUIDE.md#72-p17_0-逐操作标记).
Message-based menu tests use a common 10240-byte (10KB) binary data source, made by
repeating `s_test_message_pattern[32]` 320 times in shared RAM. This covers AES
cipher/authenticated cipher/CMAC, RSA signatures, ECDSA/EdDSA,
SHA-256 and HMAC-SHA256. RNG, key generation, ECDH and its auxiliary one-block
AES comparison retain their existing sizes.
AES tests retain fixed ciphertext-prefix checks (32 bytes for ordinary modes,
16 for GCM/CCM), compare the complete updated authentication tags, clear the
plaintext destination, and verify recovery of all 10240 bytes. CMAC and SHA-256
compare full reference tags/digests for the 10240-byte message.
RSA-2048 encryption takes one maximum legal plaintext directly from the start
of `BIG_BUFFER[0]`: 190 bytes for OAEP-SHA256 or 245 bytes for PKCS#1 v1.5.
Each produces one 256-byte ciphertext in `BIG_BUFFER[1]`. Decryption prepares
one ciphertext silently, decrypts into the start of `BIG_BUFFER[2]`, checks the
returned length, and compares the recovered prefix. There is no chunk loop or
PE reassembly/staging area. The encryption-only entry does not add decryption
requests. Each selected RSA operation reports `latency=... ms/ops`, excluding
preparation encryption. PSS and PKCS#1 v1.5 signature messages remain 10KB.
The RSA driver now uses the four-channel ICUM_DMAC for bulk PKCC operand/result
transfers in ordinary, protected and CRT modular exponentiation. CPU conversion
preserves the PKCC 64-bit limb layout; scalar words, internal PKCC moves and
other existing software arithmetic copies remain on the CPU. DMA uses polling,
bounded waits and watchdog-window servicing. RSA and HASH share channel leases;
AES DTF is a separate engine. See the ICUM guide for constraints and board checks.
Each of the three shared data buffers now has 16384-byte capacity. PE1 and PE2
have separate queue/buffer sections starting at `0xFE003000` and `0xFE015000`,
respectively, to avoid cross-core overlap. Both sections fit before the
synchronization region at `0xFE027800`; reflash the complete multicore image.
ECC RAM slots 0 and 2 contain distinct P-256 key pairs; slot 1 contains an
Ed25519 seed and its matching compressed public key. Initialization reserves
all three slots, including when an older key mapping is loaded, and copies
the fixed demo arrays directly into ICUM RAM without key-import services.
The 0x40/0x41 and 0x43/0x44 tests prepare RNG before constructing their shared
message/signature buffers, sign, verify, and check rejection of a modified
message. Signing tests therefore also perform verification.
For the modified-message check, `SERV_OK` means verification executed;
`verification_result` must be `VERIFICATION_FAIL`.
The 0x42 test uses ECC slots 0 and 2 as different parties, derives into the
reserved AES RAM slot 4 in each direction, and compares AES-ECB encryption
of the same block with both derived keys. It does not expose the shared key.
ECC operations retain contexts while the PKC library is still processing or
preserving a pending operation. Failed preparation or cleared PKC state
restarts from initialization and reacquires key references; EdDSA accesses
the context pointer only after successful allocation.
HMAC RAM slot 0 is initialized for each active VM with the RFC 4231 test-case-1
key (20 bytes of `0x0b`), including its length, VM/key IDs, and availability.
Older mappings reserve at least one HMAC slot before RAM layout calculation.
The 0x51 HMAC-SHA256 test uses the common 10240-byte message, compares the full
32-byte tag with an independently calculated reference, verifies it, and checks rejection of a modified
tag. No key-import service or RNG preparation is required.
The RSA key-generation, sign/verify and encrypt/decrypt tests initialize RNG
and extend its seed before preparing their shared request descriptors/buffers.
The menu and batch key-generation tests use the same preparation helper, so
0x24 can be selected immediately after a cold reset. RNG preparation failures
stop the test before key generation; the ICUM service itself still requires an
initialized, seeded RNG and reports `SERV_NO_INIT_RNG` otherwise.
PSS, PKCS#1 v1.5 signatures, and OAEP use SHA-256;
PSS uses a 32-byte salt. SHA-1 is not supported by the current ICUM hash mapping.
The initial RSA RAM key is 2048 bits (256-byte modulus and private exponent),
with the public exponent 65537 encoded as four big-endian bytes. The larger
array capacities are reserved for generated keys, not part of this key's length.
RSA key generation uses a separate aligned RAM workspace while holding the
single RSA/ECC context; it does not borrow or overwrite the active key cache.
Only a completed generation is installed into RAM key ID 0, using the driver's
output lengths and big-endian public exponent. Temporary key material is
cleared before the context is released, including generation-failure paths.
RSA contexts explicitly bind the watchdog callback used by the PKC driver's
long-running loops. If key generation yields for RNG resource arbitration,
its PKC state is cleared before the synchronous service releases its context,
so the dispatcher can retry from initialization rather than reuse freed state.
RNG initialization/seed-extension errors and PE request-submission errors are
reported as failures, not successful preparation or an indefinite response wait.
For board acceptance, cold-reset both PE and ICUM, select 0x24, and check that
RNG initialization and seed extension succeed before key generation returns
`SERV_OK`. Then run RSA sign/verify (PSS and PKCS#1 v1.5) and encrypt/decrypt
(OAEP and PKCS#1 v1.5) using generated RAM key ID 0, and repeat generation.
These board checks are still required: simulated peripheral/prime-generation
results validate software contracts and key storage, not real PKCC/TRNG operation.
These fixed demo keys must not be used in production.
Synchronous PKCC waits service the watchdog using its existing 75% request
flag. The PKCC polling limit and error returns remain active; the watchdog
is not disabled or extended for RSA key generation.
The PKC library's byte-copy helper accepts unaligned sub-buffer addresses,
as required by RSA padding; public API buffer-alignment requirements still apply.
SHA-2 and SHA-3 CPU-fed data paths treat input and digest buffers as byte arrays:
input bytes are staged into aligned register words, and output register words
are copied back as bytes. This includes PKC callback buffers inside RSA
PSS/PKCS#1/OAEP padding; aligning just the top-level PE buffers is insufficient.
Partial final input words are zero-filled without reading beyond the supplied
input length. Contexts retain their typed alignment and DMA requirements are
unchanged. An alignment exception followed by watchdog NMI is a primary MAEX
fault followed by a secondary watchdog timeout, not evidence to disable the WDT.
On U2C4, service response interrupts use
INTC2 channel 236 for PE0 and channel 540 for PE1.
The current data-set backend uses RAM, not persistent data flash. It is reset
during initialization; RAM compaction remains, while the old automatic flash
GC path is not built. Capacity warnings are not reliably propagated by the
current dispatcher. See the ICUM guide for details and known limitations.

From the PE serial console, enter `icum` and select a service group (1-5), then
select one of the listed service items. Enter `0` or `q` to return to the
previous menu. `icum 308` opens the categorized service menu directly.
