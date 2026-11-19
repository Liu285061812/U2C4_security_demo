"""Verify the separate boot MOT and run actual ICUM/PE boot gates with modeled hardware."""
import argparse
import re
import subprocess
import tempfile
from pathlib import Path

from cryptography.hazmat.primitives import cmac
from cryptography.hazmat.primitives.ciphers import algorithms

from generate_boot_auth_data import BOOT, ROOT, INPUT_BYTES, boot_data, demo_keys


def read_mot(path):
    memory = {}
    for line in path.read_text(encoding="ascii").splitlines():
        if not line.startswith("S") or len(line) < 4:
            raise ValueError(f"Invalid Motorola record: {line}")
        record = bytes.fromhex(line[2:])
        if record[0] != len(record) - 1 or sum(record) & 255 != 255:
            raise ValueError("Invalid Motorola record length/checksum")
        kind = int(line[1])
        if kind not in (1, 2, 3):
            continue
        width = kind + 1
        address = int.from_bytes(record[1:1 + width], "big")
        for offset, value in enumerate(record[1 + width:-1]):
            target = address + offset
            if target in memory:
                raise ValueError(f"Overlapping MOT data at {target:08X}")
            memory[target] = value
    return memory


def function(source, signature):
    start = source.index(signature)
    return source[start:source.index("\n}\n", start) + 3]


def cmac_tag(key, data):
    mac = cmac.CMAC(algorithms.AES(key))
    mac.update(data)
    return mac.finalize()


def verify_image():
    parts = boot_data()
    expected = b"".join(parts)
    header = (BOOT / "boot_auth.h").read_text(encoding="ascii")
    base = int(re.search(r"BOOT_AUTH_ADDRESS\s+\(0x([0-9A-Fa-f]+)UL\)", header)[1], 16)
    memory = read_mot(BOOT / "boot_auth_data.mot")
    if sorted(memory) != list(range(base, base + len(expected))):
        raise ValueError("Boot MOT must contain only the complete dedicated data image")
    actual = bytes(memory[address] for address in sorted(memory))
    if actual != expected:
        raise ValueError("Boot MOT does not match the generated input and tags")
    for index, key in enumerate(demo_keys()):
        if cmac_tag(key, actual[:INPUT_BYTES]) != parts[index + 1]:
            raise ValueError("Independent cryptography CMAC disagrees with generated tag")
    if any(actual[i:i + 4] == b"\xFF" * 4 for i in range(0, len(actual), 4)):
        raise ValueError("Boot image contains a blank-looking programming unit")
    combined = read_mot(ROOT / r"pe\target\u2cx\build\_release\test_U2C4_Alpha.mot")
    if any(base <= address < base + 16 * 1024 for address in combined):
        raise ValueError("Main firmware MOT overlaps the reserved independent boot-data window")
    print(f"Boot MOT: {len(actual)} bytes at 0x{base:08X}, separate from firmware, two CMACs PASS")
    return parts[0]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--ghs", type=Path, default=Path(r"C:\ghs\comp_202354"))
    args = parser.parse_args()
    data = verify_image()
    with tempfile.TemporaryDirectory(prefix="boot-auth-test-") as directory:
        work = Path(directory)
        icum = (ROOT / r"icum\generic\src\middle\mh_init\icum_mh_boot_auth.c").read_text()
        start = (ROOT / r"icum\generic\src\middle\mh_init\icum_mh_init.c").read_text()
        pe = (ROOT / r"pe\generic\src\pe1\src\pe1_boot_auth.c").read_text()
        sysinfo = (ROOT / r"icum\generic\src\middle\mh_sysinfo\icum_mh_sysinfo_int.c").read_text()
        (work / "boot_icum.inc").write_text(
            function(icum, "void BootAuth_VerifyIcum(void)") +
            function(start, "uint32_t MH_INIT_StartPE(void)"), encoding="ascii")
        (work / "boot_pe.inc").write_text(
            function(pe, "void BootAuth_VerifyPe(void)"), encoding="ascii")
        (work / "boot_uart.inc").write_text(
            function(icum, "static int boot_uart_init(void)") +
            function(icum, "static int boot_uart_puts(const char *text)") +
            function(icum, "static void boot_auth_halt(const char *message, uint32_t error)"),
            encoding="ascii")
        (work / "boot_fatal.inc").write_text(
            function(sysinfo, "static uint32_t mh_SYSINFO_Interrupt(uint32_t system_err)"),
            encoding="ascii")
        inputs = [data]
        for offset in (0, INPUT_BYTES // 2, INPUT_BYTES - 1):
            changed = bytearray(data)
            changed[offset] ^= 1
            inputs.append(bytes(changed))
        lines = ["static const uint8_t calculated_tags[4][2][16] = {"]
        for sample in inputs:
            tags = ["{" + ",".join(str(v) for v in cmac_tag(key, sample)) + "}"
                    for key in demo_keys()]
            lines.append("{" + ",".join(tags) + "},")
        lines.append("};\n")
        (work / "boot_tags.inc").write_text("\n".join(lines), encoding="ascii")
        output = work / "boot_test.out"
        subprocess.run(
            [str(args.ghs / "ccrh850.exe"), "-cpu=rh850g4mh", "-lnk=-append",
             "-I" + str(ROOT / r"pe\generic\icumif"),
             "-I" + str(ROOT / r"icum\generic\include\common"),
             "-I" + str(ROOT / r"icum\generic\include\middle"),
             "-I" + str(ROOT / r"icum\target\u2cx\include\driver"),
             "-I" + str(BOOT), "-I" + str(work),
             "-o", str(output), str(ROOT / r"tools\boot_auth_test.c"),
             str(BOOT / "boot_auth_data.c")],
            cwd=work, check=True, timeout=180)
        result = subprocess.run(
            [str(args.ghs / "isimrh850.exe"), str(output)],
            cwd=work, capture_output=True, text=True, timeout=120)
        print(result.stdout, end="")
        if result.returncode or "PASS (0 failures)" not in result.stdout:
            raise SystemExit(result.stderr or "Boot authentication simulator regression failed")


if __name__ == "__main__":
    main()
