#ifndef __COMMON_H__
#define __COMMON_H__

#define MAX_NAME 32

typedef enum GoldenCharIoctl
{
GOLDENCHAR_IOCTL_REQUEST,
GOLDENCHAR_IOCTL_DEBUG,
GOLDENCHAR_READ_FROM_DEV,
GOLDENCHAR_WRITE_TO_DEV,
} GoldenCharIoctl;

typedef enum GoldenCharRequestType
{
GOLDENCHAR_REQUEST_NEW_DEVICE,
GOLDENCHAR_REQUEST_REMOVE_DEVICE,
GOLDENCHAR_DEVICE_REQUEST,
} GoldenCharRequestType;

typedef enum BlockDeviceOperation
{
	BLOCK_DEVICE_READ,
	BLOCK_DEVICE_WRITE,
} BlockDeviceOperation;

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
		int fd;
	} NewDeviceRequest;

	struct RemoveDeviceRequest
	{
		int fd;
	} RemoveDeviceRequest;

	//This struct is filled out by the driver to represent an I/O Operation
	struct DeviceIoRequest
	{
		int fd;
		int request_id;
		BlockDeviceOperation op;
		int sector_start;
		int sector_offset;
		char* operation_buffer;
		int operation_buffer_size;
	} DeviceIoRequest;

	//This struct is filled out by the usermode app to represent a completed I/O Operation
	//Operations like writing need to pass the completed data to the driver, hence the operation_buffer parameter.
	struct DeviceIoCompleteRequest
	{
		int fd;
		int request_id;
		char* operation_buffer;
		int operation_buffer_size;
	} DeviceIoCompleteRequest;
	} sel;
} GoldenRequest;

#define MAX_SIZE_BUFFER  60000
#endif
