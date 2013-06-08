#ifndef __COMMON_H__
#define __COMMON_H__

#define MAX_NAME 32

typedef enum GoldenCharIoctl
{
	GOLDENCHAR_IOCTL_REQUEST,
	GOLDENCHAR_IOCTL_DEBUG,
} GoldenCharIoctl;

typedef enum GoldenCharRequestType
{
	GOLDENCHAR_REQUEST_NEW_DEVICE,
	GOLDENCHAR_REQUEST_REMOVE_DEVICE,
} GoldenCharRequestType;

typedef struct GoldenRequest
{
	GoldenCharRequestType request_type;
	union sel
	{
		struct NewDeviceRequest
		{
			char device_name[MAX_NAME];
			int capacity; //In sector units
			int minors;
		} NewDeviceRequest;

		struct RemoveDeviceRequest
		{
			int fd;
		} RemoveDeviceRequest;
	} sel;
} GoldenRequest;

#endif
