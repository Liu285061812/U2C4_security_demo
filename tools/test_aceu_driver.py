"""Run the real PE driver in the GHS simulator against independent AES oracles."""
from pathlib import Path
import argparse
import subprocess
import tempfile

from Crypto.Cipher import AES
from generate_aceu_vectors import ROOT, cases


def pad(data):
    return data.ljust(16, b"\0")


def bytes_c(data):
    return "{" + ",".join(str(x) for x in data) + "}"


def ccm_mac(key, iv, data, aad, tag_length):
    l_value = 15 - len(iv)
    flags = (0x40 if aad else 0) | ((tag_length - 2) // 2 << 3) | (l_value - 1)
    blocks = bytes([flags]) + iv + len(data).to_bytes(l_value, "big")
    if aad:
        encoded = len(aad).to_bytes(2, "big") + aad
        blocks += encoded + bytes((-len(encoded)) % 16)
    blocks += data + bytes((-len(data)) % 16)
    state = bytes(16)
    aes = AES.new(key, AES.MODE_ECB)
    for offset in range(0, len(blocks), 16):
        state = aes.encrypt(bytes(x ^ y for x, y in zip(state, blocks[offset:offset + 16])))
    s0 = aes.encrypt(bytes([l_value - 1]) + iv + bytes(l_value))
    return state, s0


def trace(case, decrypt=False, bad=False):
    mode, slot, length, aad_length, iv_length, tag_length, cipher, tag = case
    data = bytes(i & 255 for i in range(length))
    aad = bytes((0xA0 + i) & 255 for i in range(aad_length))
    key = bytes(range(16 + slot * 8))
    iv = bytes(range(16))
    steps = []

    def add(attr, source, output=bytes(16), last=0, valid=16, expected_slot=slot):
        steps.append((attr, last, valid, expected_slot, pad(source), pad(output)))

    def chunks(attr, source, output=b"", final=False):
        for offset in range(0, len(source), 16):
            part = source[offset:offset + 16]
            add(attr, part, output[offset:offset + 16] if output else bytes(16),
                int(final and offset + 16 >= len(source)), len(part))

    if mode <= 4:
        chunks(0, cipher if decrypt else data, data if decrypt else cipher)
    elif mode == 6:
        tweak_slot = 5 if slot == 2 else 3
        second = bytes(range(0x20, 0x20 + len(key)))
        add(1, iv, AES.new(second, AES.MODE_ECB).encrypt(iv), expected_slot=tweak_slot)
        source, output = (cipher, data) if decrypt else (data, cipher)
        remainder = length % 16
        if remainder:
            prefix = length - remainder - 16
            chunks(0, source[:prefix], output[:prefix])
            add(2 if decrypt else 0, source[prefix:prefix + 16], output[-remainder:])
            add(0, source[-remainder:], output[prefix:prefix + 16], 1, remainder)
        else:
            chunks(0, source, output, final=True)
    elif mode in (5, 10):
        # AAD always hashes full padded blocks; REM applies to GCTR only.
        for offset in range(0, len(aad), 16):
            add(0, aad[offset:offset + 16])
        chunks(1, cipher if decrypt else data, data if decrypt else cipher, final=True)
        lengths = (aad_length * 8).to_bytes(8, "big") + (length * 8).to_bytes(8, "big")
        add(2, lengths, tag)
    elif mode == 7:
        nonce = iv[:iv_length]
        yr, s0 = ccm_mac(key, nonce, data, aad, tag_length)
        b0 = (aad_length * 8).to_bytes(4, "big") + bytes(
            [0, (iv_length - 7) * 16 + (tag_length - 4) // 2, 0, 0]
        ) + (length * 8).to_bytes(8, "big")
        add(1, b0, yr)
        if decrypt:
            chunks(2, cipher, data)
        for offset in range(0, len(aad), 16):
            add(3, aad[offset:offset + 16], yr)
        source, output = (data, b"") if decrypt else (data, cipher)
        for offset in range(0, len(source), 16):
            add(4, source[offset:offset + 16],
                yr if decrypt else pad(output[offset:offset + 16]))
        if decrypt:
            received = bytearray(tag)
            if bad:
                received[0] ^= 1
            unmasked = bytes(x ^ y for x, y in zip(received, s0))
            add(13, bytes(received), unmasked)
        else:
            add(8, bytes(16), tag)
    elif mode == 8:
        if not length:
            add(1, b"", tag, 1, 0)
        else:
            for offset in range(0, length, 16):
                last = offset + 16 >= length
                add(0, data[offset:offset + 16], tag if last else bytes(16),
                    int(last), min(16, length - offset))
    elif mode == 9:
        state = bytes(16)
        for offset in range(0, length, 16):
            block = data[offset:offset + 16]
            encrypted = AES.new(state, AES.MODE_ECB).encrypt(block)
            state = bytes(x ^ y ^ z for x, y, z in zip(encrypted, block, state))
            add(0, block, state, expected_slot=4)
    else:
        chunks(0, bytes(length), cipher)
    return steps


def generate(path):
    rows, steps = [], []
    for case_index, case in enumerate(cases()):
        mode, slot, length, aad_length, iv_length, tag_length, cipher, tag = case
        directions = ((0, 0),) if mode in (9, 11) else (
            ((0, 0), (1, 0), (1, 1)) if mode in (5, 7, 8, 10) else ((0, 0), (1, 0)))
        for decrypt, bad in directions:
            sequence = trace(case, bool(decrypt), bool(bad))
            rows.append((mode, slot, length, aad_length, iv_length, tag_length,
                         decrypt, bad, len(steps), len(sequence), case_index, tag))
            steps.extend(sequence)
    lines = ["static const step_t steps[] = {"]
    for attr, last, valid, slot, source, output in steps:
        lines.append(f"{{{attr},{last},{valid},{slot},{bytes_c(source)},{bytes_c(output)}}},")
    lines.append("};\nstatic const vector_t vectors[] = {")
    for row in rows:
        lines.append("{" + ",".join(str(v) for v in row[:-1]) + "," + bytes_c(row[-1]) + "},")
    lines.append("};\n")
    path.write_text("\n".join(lines), encoding="ascii")
    return len(rows), len(steps)


def generate_provisioning(path):
    source = (ROOT / r"icum\generic\src\middle\mh_key\icum_mh_key_aes.c").read_text()
    start = source.index("static uint8_t g_demo_aes_keys[")
    keys = source[start:source.index("\n};", start) + 3]
    start = source.index("uint32_t MH_KEY_PrepareAceu(uint32_t prng_key_bits)\n{")
    function = source[start:source.index("\n}\n", start) + 3]
    path.write_text(keys + "\n" + function, encoding="ascii")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--ghs", type=Path, default=Path(r"C:\ghs\comp_202354"))
    args = parser.parse_args()
    with tempfile.TemporaryDirectory(prefix="aceu-driver-test-") as directory:
        work = Path(directory)
        count, blocks = generate(work / "aceu_trace.inc")
        generate_provisioning(work / "aceu_provision.inc")
        board_source = (ROOT / r"pe\generic\src\pe1\src\pe1_aceu_test.c").read_text()
        start = board_source.index("typedef struct\n{")
        end = board_source.index("} aceu_vector_t;", start) + len("} aceu_vector_t;")
        board_vectors = (ROOT / r"pe\generic\src\pe1\src\pe1_aceu_vectors.h").read_text()
        (work / "aceu_board_vectors.inc").write_text(
            board_source[start:end] + "\n" + board_vectors, encoding="ascii")
        output = work / "aceu_test.out"
        subprocess.run(
            [str(args.ghs / "ccrh850.exe"), "-cpu=rh850g4mh",
             "-I" + str(ROOT / r"pe\generic\icumif"),
             "-I" + str(ROOT / r"pe\target\u2cx\src\driver\aceu"),
             "-I" + str(work), "-o", str(output),
             str(ROOT / r"tools\aceu_driver_test.c")],
            cwd=work, check=True, timeout=180)
        result = subprocess.run(
            [str(args.ghs / "isimrh850.exe"), str(output)],
            cwd=work, capture_output=True, text=True, timeout=120)
        print(result.stdout, end="")
        if result.returncode or "PASS (0 failures)" not in result.stdout:
            raise SystemExit(result.stderr or "ACEU simulator regression failed")
        print(f"Independent oracles: {count} operations, {blocks} hardware blocks")


if __name__ == "__main__":
    main()
