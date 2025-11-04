#include <ftd2xx/ftd2xx.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
  FT_HANDLE ftHandle;
  FT_STATUS ftStatus;
  int iport = 0;
  static FT_DEVICE ftDevice;

  printf("Opening port %d\n", iport);

  ftStatus = FT_Open(iport, &ftHandle);
  if (ftStatus != FT_OK) {
    printf("FT_Open(%d) failed\n", iport);
    return 1;
  }
  printf("FT_Open succeeded. Handle is %p\n", ftHandle);

  ftStatus = FT_GetDeviceInfo(ftHandle, &ftDevice, NULL, NULL, NULL, NULL);
  if (ftStatus != FT_OK) {
    printf("FT_GetDeviceType failed\n");
    return 1;
  }
  printf("FT_GetDeviceInfo succeeded. Device is type %d.\n", (int)ftDevice);

  return 0;
}
