#include <fcntl.h>
#include <linux/videodev2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
int main(int argc, char const* argv[]) {
	// 初始化变量
	int fd;
	struct v4l2_capability cap;
	struct v4l2_format fmt;
	struct v4l2_requestbuffers req;
	struct v4l2_buffer buf;
	void* buffer;
	int type;
	// 打开设备
	const char* dev = (argc >= 2) ? argv[1] : "/dev/video0";
	fd = open(dev, O_RDWR);
	if (fd < 0) {
		perror("open");
		return -1;
	}
	// 查询能力
	ioctl(fd, VIDIOC_QUERYCAP, &cap);
	printf("Driver : %s\nCard  : %s\n\n", cap.driver, cap.card);
	// 设置模式
	memset(&fmt, 0, sizeof(fmt));
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	// 640x480
	fmt.fmt.pix.width = 640;
	fmt.fmt.pix.height = 480;
	// 320x240
	// fmt.fmt.pix.width = 320;
	// fmt.fmt.pix.height = 240;
	// 1280x720
	// fmt.fmt.pix.width = 1280;
	// fmt.fmt.pix.height = 720;
	// YUYV
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
	// MJPEG
	// fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
	fmt.fmt.pix.field = V4L2_FIELD_ANY;
	if (ioctl(fd, VIDIOC_S_FMT, &fmt) < 0) {
		perror("VIDIOC_S_FMT");
		close(fd);
		return -1;
	}
	printf("Format: %dx%d, %d bytes/frame\n\n", fmt.fmt.pix.width, fmt.fmt.pix.height, fmt.fmt.pix.sizeimage);
	// 申请4个缓存区
	memset(&req, 0, sizeof(req));
	req.count = 4;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;
	if (ioctl(fd, VIDIOC_REQBUFS, &req) < 0) {
		perror("VIDIOC_REQBUFS");
		close(fd);
		return -1;
	}
	// 映射第0号缓存到用户空间
	memset(&buf, 0, sizeof(buf));
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	buf.index = 0;
	ioctl(fd, VIDIOC_QUERYBUF, &buf);
	buffer = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
	printf("Buffer mapped: %d bytes\n\n", buf.length);
	// 缓冲入队
	ioctl(fd, VIDIOC_QBUF, &buf);
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	// 开启流
	ioctl(fd, VIDIOC_STREAMON, &type);
	printf("Stream ON, capturing...\n");
	// 填充一帧
	memset(&buf, 0, sizeof(buf));
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	// 取出一帧
	ioctl(fd, VIDIOC_DQBUF, &buf);
	printf("Got frame: %d bytes\n", buf.bytesused);
	// YUYV
	FILE* fp = fopen("frame.raw", "wb");
	// MJPEG
	// FILE* fp = fopen("frame.mjpg", "wb");
	fwrite(buffer, 1, buf.bytesused, fp);
	fclose(fp);
	printf("Saved to frame.raw\n");

	ioctl(fd, VIDIOC_QBUF, &buf);
	ioctl(fd, VIDIOC_STREAMOFF, &type);

	munmap(buffer, buf.length);
	close(fd);
	printf("\nDone! View with:\n");
	printf(
		"ffplay -f rawvideo -pixel_format yuyv422 -video_size %dx%d frame.raw\n\n", fmt.fmt.pix.width, fmt.fmt.pix.height
	);
	return 0;
}
