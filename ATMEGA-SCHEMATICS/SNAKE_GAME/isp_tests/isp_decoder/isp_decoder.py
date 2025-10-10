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

    mosi: list[str] = []
    miso: list[str] = []
    len = 0
    with open(inFile, "r") as inf:
        reader = csv.DictReader(inf)
        for row in reader:
            mosi.append(row["MOSI"])
            miso.append(row["MISO"])
            len += 1

    mosi_cmd_tuples: list[tuple[str, str, str, str]] = []
    cmds: list[str] = []
    for i in range(0, len, 4):
        cmd = (mosi[i][-2:], mosi[i + 1][-2:], mosi[i + 2][-2:], mosi[i + 3][-2:])
        mosi_cmd_tuples.append(cmd)
        match cmd:
            case ("AC", "53", "00", "00"):
                cmds.append("PRG_EN")
            case ("AC", "80", "00", "00"):
                cmds.append("CHIP_ERASE")
            case ("F0", "00", "00", db_out):
                cmds.append(f"{'POLL_RDY:':<26} db_out: __ __ __ {db_out}")
            case ("4D", "00", ext_adr, "00"):
                cmds.append(f"{'LOAD_EXT_ADDR_BYTE:':<26} __ __ {ext_adr} __ ")
            case ("48", "00", adrlsb, db_in):
                cmds.append(f"{'LOAD_PRG_MEM_PAGE_HI_BYTE:':<26} __ __ {adrlsb} {db_in}")
            case ("40", "00", adrlsb, db_in):
                cmds.append(f"{'LOAD_PRG_MEM_PAGE_LO_BYTE:':<26} __ __ {adrlsb} {db_in}")
            case ("C1", "00", adr, db_in):
                cmds.append(f"{'LOAD_EEPROM_MEM_PAGE:':<26} __ __ {adr} {db_in}")
            case ("28", adrmsb, adrlsb, db_out):
                cmds.append(f"{'READ_PRG_MEM_HI_BYTE:':<26} __ {adrmsb} {adrlsb} {db_out}")
            case ("20", adrmsb, adrlsb, db_out):
                cmds.append(f"{'READ_PRG_MEM_LO_BYTE:':<26} __ {adrmsb} {adrlsb} {db_out}")
            case ("A0", adr1, adr2, db_out):
                cmds.append(f"{'READ_EEPROM_MEM:':<26} __ {adr1} {adr2} {db_out}")
            case ("58", "00", "00", db_out):
                cmds.append(f"{'READ_LOCK_BITS:':<26} __ __ __ {db_out}")
            case ("30", "00", adr, db_out):
                cmds.append(f"{'READ_SIGNATURE_BYTE:':<26} __ __ {adr} {db_out}")
            case ("50", "00", "00", db_out):
                cmds.append(f"{'READ_FUSE_BITS:':<26} __ __ __ {db_out}")
            case ("58", "08", "00", db_out):
                cmds.append(f"{'READ_FUSE_HIGH_BITS:':<26} __ __ __ {db_out}")
            case ("50", "08", "00", db_out):
                cmds.append(f"{'READ_EXT_FUSE_BITS:':<26} __ __ __ {db_out}")
            case ("38", "00", "00", db_out):
                cmds.append(f"{'READ_CAL_BYTE:':<26} __ __ __ {db_out}")
            case ("4C", adrmsb, adrlsb, "00"):
                cmds.append(f"{'WRITE_PRG_MEM_PAGE:':<26} __ {adrmsb} {adrlsb} __")
            case ("C0", adr1, adr2, db_in):
                cmds.append(f"{'WRITE_EEPROM_MEM:':<26} __ {adr1} {adr2} {db_in}")
            case ("C2", adr1, adr2, "00"):
                cmds.append(f"{'WRITE_EEPROM_MEM_PAGE:':<26} __ {adr1} {adr2}")
            case ("AC", "E0", "00", db_in):
                cmds.append(f"{'WRITE_LOCK_BITS:':<26} __ __ __ {db_in}")
            case ("AC", "A0", "00", db_in):
                cmds.append(f"{'WRITE_FUSE_BITS:':<26} __ __ __ {db_in}")
            case ("AC", "A8", "00", db_in):
                cmds.append(f"{'WRITE_FUSE_HIGH_BITS:':<26} __ __ __ {db_in}")
            case ("AC", "A4", "00", db_in):
                cmds.append(f"{'WRITE_EXT_FUSE_BITS:':<26} __ __ __ {db_in}")
            case _:
                cmds.append(f"Unknown command: {' '.join(cmd)}")

    with open(outFile, "w") as outf:
        # for cmd in mosi_cmd_tuples:
        if hex_out:
            for cmd in mosi_cmd_tuples:
                _ = outf.write(" ".join(b for b in cmd))
                _ = outf.write("\n")
        else:
            for cmd in cmds:
                _ = outf.write(str(cmd))
                _ = outf.write("\n")

    # with open(outFile, 'w') as outf:
    #     i = 0
    #     for byte in mosi:
    #         _ = outf.write(byte)
    #         _ = outf.write(' ')
    #         i += 1
    #         if i == 4:
    #             _ = outf.write('\n')
    #             i = 0


if __name__ == "__main__":
    main()
