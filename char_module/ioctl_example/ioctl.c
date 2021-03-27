#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

//Define macro for the ioctl in user space
#define RD_DATA _IOR('a', 'a', int32_t*)
#define WR_DATA _IOW('a', 'b', int32_t*)

int main(void) {
	int file_fd;
	int32_t val, num;

	file_fd = open("/dev/chrdrv3", O_RDWR);
	if(file_fd < 0) {
		printf("Error in opening the device file.\n");
		return file_fd;
	}
	printf("Enter the data you want to write: ");
	scanf("%d", &num);

	ioctl(file_fd, WR_DATA, (int32_t*)&num);
	printf("Wrote data to device.\n");

	ioctl(file_fd, RD_DATA, (int32_t*)&val);
	printf("Read data from the device %d\n", val);

	close(file_fd);
	return 0;
}