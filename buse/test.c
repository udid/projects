#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "common.h"


int main()
{
	GoldenRequest req = {0};
	int fd = open("/dev/goldenchar", O_RDONLY);

	req.request_type = GOLDENCHAR_REQUEST_NEW_DEVICE;
	strcpy(req.sel.NewDeviceRequest.device_name, "HelloBD");
	req.sel.NewDeviceRequest.capacity = 10;
	req.sel.NewDeviceRequest.minors = 1;
	
	if (ioctl(fd, GOLDENCHAR_IOCTL_REQUEST, &req) != 0)
		printf("No can do...");

	printf("Got back fd: %d", req.sel.NewDeviceRequest.fd);

	int test_dev = open("/dev/HelloBD", O_RDWR);

	if (test_dev < 0)
		printf("Could not open the test device");
	else
		printf("YAY! Opened the device successfully");

	return 0;
}
