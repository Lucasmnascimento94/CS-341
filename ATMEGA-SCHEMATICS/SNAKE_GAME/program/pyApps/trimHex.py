import sys

def parseFIle(file, file_out):
    page_count = 0
    byte_count = 0
    page_array = [[]]

    block_count = 0
    byte = ""
    out = open("file_out", "w")
    full_out = open("trimmed_out", "w")

    for s in file:
        s = s.strip()
        length = "0x"
        addr = "0x"
        data_type = "0x"
        data = ""
        cc = "0x"

        for i in range(1, 3):
            length+=(s[i])
        for i in range(3, 7):
            addr+=(s[i])
        for i in range(7, 9):
            data_type+=(s[i])

        if len(s) > 10:
            for i in range(9, len(s)-2):
                byte += s[i]
                byte_count += 1

                if(byte_count == 4):
                    hi_lo = byte[2:4] + byte[0:2]
                    out.write("0x" + hi_lo.upper() + ", ")
                    data+=("0x" + hi_lo.upper() + ", ")
                    byte = ""
                    byte_count = 0
                    block_count+=1
                    page_count+=1
                if(block_count == 8):
                    out.write("\n")
                    block_count = 0

                if(page_count == 64):
                    page_count = 0
                    out.write("\n\n******\n\n")

        for i in range(len(s)-2, len(s)):
            cc+=(s[i])
        
        full_out.write("LL: "+ length + " <> Addr: " + addr + " <> Type: " + data_type + " <> Data: " + data + " <> CheckSum: " + cc + "\n")
    
    out.close()
    full_out.close()
    
def main():
    if len(sys.argv) > 1:
        filePath = sys.argv[1]
        fileOut = sys.argv[2]

        file = open(filePath, 'r')
        parseFIle(file, fileOut)

    else:
        print("Arguments: file_target --- output_name")

if __name__ == "__main__":
    main()