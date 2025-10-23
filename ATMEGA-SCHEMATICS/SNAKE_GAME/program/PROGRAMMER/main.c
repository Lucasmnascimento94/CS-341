#include "main.h"
#include "ft_status_utils.h"

FT_EEPROM_232R ee;

unsigned int n_devices;
unsigned int devPID, devVID;

FT_STATUS status;

// FT_STATUS FT_CreateDeviceInfoList (LPDWORD lpdwNumDevs)

int main(int argc, char **argv) {

    // test_1
    status = FT_CreateDeviceInfoList(&n_devices);
    char msg[100];
    sprintf(msg, "n_devices: %d  |   status: %s", n_devices, getStatusName(status));
    printf("%s\n", msg);

    //test_2

    status = FT_GetVIDPID(&devPID, &devVID);
    sprintf(msg, "vid: %d  |   pid: %d   |     status:  %s", devVID, devPID, getStatusName(status));
    printf("%s\n", msg);
    return 0;
}