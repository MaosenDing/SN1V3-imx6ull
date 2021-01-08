
int set_gain_expose(int fd, int gain, int expose)
{
	static int old_gain, old_expose;

	if (gain == old_gain && expose == old_expose) {
		return 0;
	}
	old_expose = expose;
	old_gain = gain;
	printf("set gain = %d ,expose = %d \n", gain, expose);
	struct v4l2_control  Setting;

	Setting.id = V4L2_CID_EXPOSURE_AUTO;
	Setting.value = V4L2_EXPOSURE_MANUAL;
	if (0 > ioctl(fd, VIDIOC_S_CTRL, &Setting)) {
		printf("V4L2_CID_EXPOSURE_AUTO error = %d \n", errno);
	}

	Setting.id = V4L2_CID_EXPOSURE_ABSOLUTE;
	Setting.value = expose;
	if (0 > ioctl(fd, VIDIOC_S_CTRL, &Setting)) {
		printf("V4L2_CID_EXPOSURE error = %d \n", errno);
	}

	Setting.id = V4L2_CID_GAIN;
	Setting.value = gain;
	if (0 > ioctl(fd, VIDIOC_S_CTRL, &Setting)) {
		printf("V4L2_CID_GAIN error = %d \n", errno);
	}
}
