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
	strcpy(req.sel.NewDeviceRequest.device_name, "Hello");
	req.sel.NewDeviceRequest.capacity = 10;
	req.sel.NewDeviceRequest.minors = 1;
	
	if (ioctl(fd, GOLDENCHAR_IOCTL_REQUEST, &req) != 0)
		printf("No can do...");

	printf("Got back fd: %d", req.sel.NewDeviceRequest.fd);

	return 0;
}
