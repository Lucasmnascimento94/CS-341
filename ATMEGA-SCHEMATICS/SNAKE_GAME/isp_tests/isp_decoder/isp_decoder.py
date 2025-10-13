import argparse
import csv


def main():
    parser = argparse.ArgumentParser()
    _ = parser.add_argument("input", type=str, help="input csv")
    _ = parser.add_argument("output", type=str, help="output csv")
    _ = parser.add_argument("--hex", action="store_true", help="output as hex")
    args = parser.parse_args()

    inFile = args.input
    outFile = args.output
    hex_out = args.hex

    times:  list[str] = []
    mosi: list[str] = []
    miso: list[str] = []

    length = 0
    with open(inFile, "r") as inf:
        reader = csv.DictReader(inf)
        for row in reader:
            times.append(row["Time [s]"])
            mosi.append(row["MOSI"])
            miso.append(row["MISO"])
            length += 1

    mosi_cmd_tuples: list[tuple[str, str, str, str]] = []
    cmds: list[str] = []
    for i in range(0, length, 4):
        cmd = (mosi[i][-2:], mosi[i + 1][-2:], mosi[i + 2][-2:], mosi[i + 3][-2:])
        mosi_cmd_tuples.append(cmd)
        miso_bytes = (miso[i][-2:], miso[i + 1][-2:], miso[i + 2][-2:], miso[i + 3][-2:])

        timestamp = times[i]
        time_width = len(times[-1])
        cmd_str = ""
        mosi_str = ""
        miso_str = " ".join(miso_bytes)

        match cmd:
            case ("AC", "53", "00", "00"):
                cmd_str = f"{'PRG_EN':<26}"
                mosi_str = "__ __ __ __"
            case ("AC", "80", "00", "00"):
                cmd_str = f"{'CHIP_ERASE':<26}"
                mosi_str = "__ __ __ __"
            case ("F0", "00", "00", db_out):
                cmd_str = f"{'POLL_RDY:':<26}"
                mosi_str = "__ __ __ {db_out}"
            case ("4D", "00", ext_adr, "00"):
                cmd_str = f"{'LOAD_EXT_ADDR_BYTE:':<26}"
                mosi_str = f"__ __ {ext_adr} __"
            case ("48", "00", adrlsb, db_in):
                cmd_str = f"{'LOAD_PRG_MEM_PAGE_HI_BYTE:':<26}"
                mosi_str = f"__ __ {adrlsb} {db_in}"
            case ("40", "00", adrlsb, db_in):
                cmd_str = f"{'LOAD_PRG_MEM_PAGE_LO_BYTE:':<26}"
                mosi_str = f"__ __ {adrlsb} {db_in}"
            case ("C1", "00", adr, db_in):
                cmd_str = f"__ __ {adr} {db_in}"
                mosi_str = f"{'LOAD_EEPROM_MEM_PAGE:':<26}"
            case ("28", adrmsb, adrlsb, db_out):
                cmd_str = f"{'READ_PRG_MEM_HI_BYTE:':<26}"
                mosi_str = f"__ {adrmsb} {adrlsb} {db_out}"
            case ("20", adrmsb, adrlsb, db_out):
                cmd_str = f"{'READ_PRG_MEM_LO_BYTE:':<26}" 
                mosi_str = f"__ {adrmsb} {adrlsb} {db_out}"
            case ("A0", adr1, adr2, db_out):
                cmd_str = f"__ {adr1} {adr2} {db_out}"
                mosi_str = f"{'READ_EEPROM_MEM:':<26}"
            case ("58", "00", "00", db_out):
                cmd_str = f"__ __ __ {db_out}"
                mosi_str = f"{'READ_LOCK_BITS:':<26}"
            case ("30", "00", adr, db_out):
                cmd_str = f"{'READ_SIGNATURE_BYTE:':<26}"
                mosi_str = f"__ __ {adr} {db_out}"
            case ("50", "00", "00", db_out):
                cmd_str = f"__ __ __ {db_out}"
                mosi_str = f"{'READ_FUSE_BITS:':<26}"
            case ("58", "08", "00", db_out):
                cmd_str = f"__ __ __ {db_out}"
                mosi_str = f"{'READ_FUSE_HIGH_BITS:':<26}"
            case ("50", "08", "00", db_out):
                cmd_str = f"__ __ __ {db_out}"
                mosi_str = f"{'READ_EXT_FUSE_BITS:':<26}"
            case ("38", "00", "00", db_out):
                cmd_str = f"__ __ __ {db_out}"
                mosi_str = f"{'READ_CAL_BYTE:':<26}"
            case ("4C", adrmsb, adrlsb, "00"):
                cmd_str = f"{'WRITE_PRG_MEM_PAGE:':<26}"
                mosi_str = f"__ {adrmsb} {adrlsb} __"
            case ("C0", adr1, adr2, db_in):
                cmd_str = f"__ {adr1} {adr2} {db_in}"
                mosi_str = f"{'WRITE_EEPROM_MEM:':<26}"
            case ("C2", adr1, adr2, "00"):
                cmd_str = f"__ {adr1} {adr2} __"
                mosi_str = f"{'WRITE_EEPROM_MEM_PAGE:':<26}"
            case ("AC", "E0", "00", db_in):
                cmd_str = f"__ __ __ {db_in}"
                mosi_str = f"{'WRITE_LOCK_BITS:':<26}"
            case ("AC", "A0", "00", db_in):
                cmd_str = f"__ __ __ {db_in}"
                mosi_str = f"{'WRITE_FUSE_BITS:':<26}"
            case ("AC", "A8", "00", db_in):
                cmd_str = f"__ __ __ {db_in}"
                mosi_str = f"{'WRITE_FUSE_HIGH_BITS:':<26}"
            case ("AC", "A4", "00", db_in):
                cmd_str = f"__ __ __ {db_in}"
                mosi_str = f"{'WRITE_EXT_FUSE_BITS:':<26}"
            case _:
                cmds.append(f"Unknown command: {' '.join(cmd)}")
                continue

        cmds.append(f"{timestamp:>{time_width}}    {cmd_str}    {mosi_str}    {miso_str}")

    with open(outFile, "w") as outf:
        # for cmd in mosi_cmd_tuples:
        if hex_out:
            for cmd in mosi_cmd_tuples:
                _ = outf.write(" ".join(b for b in cmd))
                _ = outf.write("\n")
        else:
            for cmd in cmds:
                _ = outf.write(cmd)
                _ = outf.write("\n")

if __name__ == "__main__":
    main()
