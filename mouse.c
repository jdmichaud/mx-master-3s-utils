// cc -I/usr/include/hidapi/ -ggdb3 mouse.c -lhidapi-hidraw -o mouse
#include <stdio.h> // printf
#include <wchar.h> // wchar_t

#include <hidapi.h>
#include <string.h>

#define MAX_STR 255

int main(int argc, char* argv[])
{
  int res;
  unsigned char buf[65];
  wchar_t wstr[MAX_STR];
  hid_device *handle;
  int i;

  // Initialize the hidapi library
  res = hid_init();

  // Open the device using the VID, PID,
  // and optionally the Serial number.
  // handle = hid_open(0x046d, 0xc548, NULL);
  // if (!handle) {
  //   printf("Unable to open device\n");
  //   hid_exit();
  //   return 1;
  // }

  struct hid_device_info *device_info = hid_enumerate(0x046d, 0xc548);
  struct hid_device_info *current_device_info = device_info;
  struct hid_device_info *bolt = NULL;
  while (current_device_info != NULL) {
    printf("path %s\n", current_device_info->path);
    printf("  vendor_id %i\n", current_device_info->vendor_id);
    printf("  product_id %i\n", current_device_info->product_id);
    printf("  serial_number %ls\n", current_device_info->serial_number);
    printf("  release_number %i\n", current_device_info->release_number);
    printf("  manufacturer_string %ls\n", current_device_info->manufacturer_string);
    printf("  product_string %ls\n", current_device_info->product_string);
    printf("  usage_page %i\n", current_device_info->usage_page);
    printf("  usage %i\n", current_device_info->usage);
    printf("  interface_number %i\n", current_device_info->interface_number);
    if (current_device_info->interface_number == 2) {
      bolt = current_device_info;
    }
    current_device_info = current_device_info->next;
  }
  handle = hid_open_path(bolt->path);

  // Read the Manufacturer String
  res = hid_get_manufacturer_string(handle, wstr, MAX_STR);
  printf("Manufacturer String: %ls\n", wstr);

  // Read the Product String
  res = hid_get_product_string(handle, wstr, MAX_STR);
  printf("Product String: %ls\n", wstr);

  // Read the Serial Number String
  res = hid_get_serial_number_string(handle, wstr, MAX_STR);
  printf("Serial Number String: (%d) ", wstr[0]);
  for (int i = 0; wstr[i] != 0; ++i) printf("%d %c", i, wstr[i]);
  printf("\n");
  // Read Indexed String 1
  res = hid_get_indexed_string(handle, 1, wstr, MAX_STR);
  printf("Indexed String 1: %ls\n", wstr);

  buf[0] = 0x10;
  buf[1] = 0xff;
  buf[2] = 0x83;
  buf[3] = 0xfb;
  res = hid_write(handle, buf, 7);
  if (res != 7) {
    fprintf(stderr, "res %d: ", res);
    perror("could not write to device");
    return 1;
  }

  // Read requested state
  res = hid_read(handle, buf, 65);
  if (res == -1) {
    fprintf(stderr, "res %d: ", res);
    perror("could not read from device");
    return 1;
  }

  // Print out the returned buffer.
  printf("-- response -- \n");
  for (i = 0; i < res; i++) {
    printf("0x%x ", buf[i]);
    if ((i + 1) % 8 == 0) {
      printf("\n");
    }
  }
  printf("\n");

  printf("set Ratcheted\n");
  memset(buf, 0, sizeof(buf));
  buf[0] = 0x10;
  buf[1] = 0x02;
  buf[2] = 0x0e;
  buf[3] = 0x1e;
  buf[4] = 0x02;
  res = hid_write(handle, buf, 20);
  if (res != 20) {
    fprintf(stderr, "res %d: ", res);
    perror("could not write to device");
    return 1;
  }

  // Read requested state
  res = hid_read(handle, buf, 65);
  if (res == -1) {
    fprintf(stderr, "res %d: ", res);
    perror("could not read from device");
    return 1;
  }

  // Print out the returned buffer.
  printf("-- response -- \n");
  for (i = 0; i < res; i++) {
    printf("0x%x ", buf[i]);
    if ((i + 1) % 8 == 0) {
      printf("\n");
    }
  }
  printf("\n");

  // Close the device
  hid_close(handle);

  // Finalize the hidapi library
  res = hid_exit();

  return 0;
}
