#!/usr/bin/env python3
import sys, csv
from typing import List, Dict, Any, Optional

# ---------- parsing helpers ----------
def parse_hex(s: str) -> int:
    s = s.strip()
    if not s:
        return 0
    try:
        return int(s, 16)          # works for "0xAC" or "AC"
    except ValueError:
        return int(s if s.startswith(("0x", "0X")) else ("0x"+s), 16)

def hx(b: int) -> str:
    return f"{b:02X}"

def hx4(bs: List[int]) -> str:
    return " ".join(hx(b) for b in bs)

def group4_len(n: int):
    """Yield (start_index, txn_id) for each 4-byte group up to n (multiple of 4)."""
    for i in range(0, n, 4):
        yield i, i // 4

# ---------- command table ----------
# pattern: list of 4 items. Use ints for fixed bytes, None for wildcard.
# data_out_idx: index (0..3) on **MISO** that carries returned value
# data_in_idx : index (0..3) on **MOSI** that carries payload sent by master
CMDS: List[Dict[str, Any]] = [
    # Programming control
    {"name":"PRG_EN",         "pattern":[0xAC,0x53,0x00,0x00]},
    {"name":"CHIP_ERASE",     "pattern":[0xAC,0x80,0x00,0x00]},
    {"name":"POLL_RDY",       "pattern":[0xF0,0x00,0x00,0x00]},

    # Loaders (write-side payload)
    {"name":"LOAD_EXT_ADDR_BYTE",          "pattern":[0x4D,0x00,None,0x00], "data_in_idx":2},
    {"name":"LOAD_PRG_MEM_PAGE_HIGH_BYTE", "pattern":[0x48,0x00,None,None], "data_in_idx":3},
    {"name":"LOAD_PRG_MEM_PAGE_LOW_BYTE",  "pattern":[0x40,0x00,None,None], "data_in_idx":3},
    {"name":"LOAD_EEPROM_MEM_PAGE",        "pattern":[0xC1,0x00,None,None], "data_in_idx":3},

    # Readers (read-side payload appears on MISO)
    {"name":"READ_PRG_MEM_HIGH_BYTE",      "pattern":[0x28,None,None,0x00], "data_out_idx":3},
    {"name":"READ_PRG_MEM_LOW_BYTE",       "pattern":[0x20,None,None,0x00], "data_out_idx":3},
    {"name":"READ_EEPROM_MEM",             "pattern":[0xA0,None,None,0x00], "data_out_idx":3},
    {"name":"READ_LOCK_BITS",              "pattern":[0x58,0x00,0x00,0x00], "data_out_idx":3},

    # Signature (3rd is index, result on 4th byte)
    {"name":"READ_SIGNATURE_BYTE",         "pattern":[0x30,0x00,None,0x00], "data_out_idx":3},

    # Fuses (result on 4th byte)
    {"name":"READ_FUSE_BITS",              "pattern":[0x50,0x00,0x00,0x00], "data_out_idx":3},
    {"name":"READ_FUSE_HIGH_BITS",         "pattern":[0x58,0x08,0x00,0x00], "data_out_idx":3},
    {"name":"READ_EXT_FUSE_BITS",          "pattern":[0x50,0x08,0x00,0x00], "data_out_idx":3},
    {"name":"READ_CAL_BYTE",               "pattern":[0x38,0x00,0x00,0x00], "data_out_idx":3},

    # Writers
    {"name":"WRITE_PRG_MEM_PAGE",          "pattern":[0x4C,None,None,0x00]},
    {"name":"WRITE_EEPROM_MEM",            "pattern":[0xC0,None,None,None], "data_in_idx":3},
    {"name":"WRITE_EEPROM_MEM_PAGE",       "pattern":[0xC2,None,None,0x00]},
    {"name":"WRITE_LOCK_BITS",             "pattern":[0xAC,0xEC,0x00,None], "data_in_idx":3},
    {"name":"WRITE_FUSE_BITS",             "pattern":[0xAC,0xA0,0x00,None], "data_in_idx":3},
    {"name":"WRITE_FUSE_HIGH_BITS",        "pattern":[0xAC,0xA8,0x00,None], "data_in_idx":3},
    {"name":"WRITE_EXT_FUSE_BITS",         "pattern":[0xAC,0xA4,0x00,None], "data_in_idx":3},
]

from typing import Optional

def match(pattern: List[Optional[int]], mosi4: List[int]) -> bool:
    for k, want in enumerate(pattern):
        if want is None:
            continue
        if mosi4[k] != want:
            return False
    return True

def classify(mosi4: List[int]) -> Dict[str, Any]:
    for cmd in CMDS:
        if match(cmd["pattern"], mosi4):
            return cmd
    return {"name":"UNKNOWN", "pattern":[None]*4}

# ---------- analysis over arrays ----------
def analyze_transactions(stamp: List[float], packet: List[int],
                         mosi: List[int], miso: List[int]) -> None:
    n = min(len(stamp), len(packet), len(mosi), len(miso))
    if n % 4 != 0:
        # truncate tail to full 4-byte packets
        n -= (n % 4)

    for i, txid in group4_len(n):
        m4 = mosi[i:i+4]
        s4 = miso[i:i+4]
        cmd = classify(m4)
        name = cmd["name"]
        t0   = stamp[i] if i < len(stamp) else 0.0

        print(f"txn {txid:04d}  t={t0:.9f}  {name}")
        print(f"  mosi: {hx4(m4)}")

        # If this command produces data_out (on MISO), show the MISO line and value
        data_out_idx = cmd.get("data_out_idx")
        if data_out_idx is not None:
            val = s4[data_out_idx]
            print(f"  miso: {hx4(s4)}   value=0x{hx(val)}")

        # If this command consumes data_in (on MOSI), show the payload byte
        data_in_idx = cmd.get("data_in_idx")
        if data_in_idx is not None:
            din = m4[data_in_idx]
            print(f"  data_in=0x{hx(din)}")

        print()  # blank line

# ---------- main: load CSV into arrays, then analyze ----------
if __name__ == "__main__":
    if len(sys.argv) < 2:
        sys.exit("Usage: decode_avr_isp.py <capture.csv>")

    path = sys.argv[1]

    stamp:  List[float] = []
    packet: List[int]   = []
    mosi:   List[int]   = []
    miso:   List[int]   = []

    with open(path, newline="") as f:
        r = csv.reader(f)
        first = next(r, None)
        if first is None:
            sys.exit("Empty file")

        # Detect and skip header
        header_like = ",".join(first).lower()
        if ("time" in header_like and "packet" in header_like) or ("mosi" in header_like and "miso" in header_like):
            pass  # header consumed
        else:
            # treat 'first' as data row
            row = [x.strip() for x in first]
            if len(row) >= 4:
                stamp.append(float(row[0]))
                packet.append(int(row[1]))
                mosi.append(parse_hex(row[2]))
                miso.append(parse_hex(row[3]))

        for row in r:
            if not row:
                continue
            row = [x.strip() for x in row]
            if len(row) < 4:
                continue
            try:
                stamp.append(float(row[0]))
                packet.append(int(row[1]))
                mosi.append(parse_hex(row[2]))
                miso.append(parse_hex(row[3]))
            except ValueError:
                # skip malformed
                continue

    # sanity check
    n = len(stamp)
    if not (n == len(packet) == len(mosi) == len(miso)):
        sys.exit("Column length mismatch; input not aligned")

    print(f"Loaded {n} rows, {n//4} transactions.\n")
    analyze_transactions(stamp, packet, mosi, miso)
