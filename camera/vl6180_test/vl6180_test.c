

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "vl6180x_def.h"

#define MODE_RANGE		0
#define MODE_XTAKCALIB	1
#define MODE_OFFCALIB	2
#define MODE_HELP		3

//******************************** IOCTL definitions
#define VL6180_IOCTL_INIT 			_IO('p', 0x01)
#define VL6180_IOCTL_XTALKCALB		_IO('p', 0x02)
#define VL6180_IOCTL_OFFCALB		_IO('p', 0x03)
#define VL6180_IOCTL_STOP			_IO('p', 0x05)
#define VL6180_IOCTL_SETXTALK		_IOW('p', 0x06, unsigned int)
#define VL6180_IOCTL_SETOFFSET		_IOW('p', 0x07, int8_t)
#define VL6180_IOCTL_GETDATA 		_IOR('p', 0x0a, unsigned long)
//#define VL6180_IOCTL_GETDATAS 		_IOR('p', 0x0b, RangeData)
#define VL6180_IOCTL_GETDATAS 		_IOR('p', 0x0b, VL6180x_RangeData_t)
/* for ver< 1.0 */

static void help(void)
{
	fprintf(stderr,
		"Usage: vl6180_test [-c] [-h]\n"
		" -h for usage\n"
		" -c for crosstalk calibration\n"
		" -o for offset calibration\n"
		" default for ranging\n"
		);
	exit(1);
}

int main(int argc, char *argv[])
{
	int fd;
	unsigned long data;
	VL6180x_RangeData_t range_datas;
	int flags = 0;
	int mode = MODE_RANGE;

	/* handle (optional) flags first */
	while (1+flags < argc && argv[1+flags][0] == '-') {
		switch (argv[1+flags][1]) {
		case 'c': mode= MODE_XTAKCALIB; break;
		case 'h': mode= MODE_HELP; break;
		case 'o': mode = MODE_OFFCALIB; break;
		default:
			fprintf(stderr, "Error: Unsupported option "
				"\"%s\"!\n", argv[1+flags]);
			help();
			exit(1);
		}
		flags++;
	}
	if (mode == MODE_HELP)
	{
		help();
		exit(0);
	}

	fd = open("/dev/stmvl6180_ranging",O_RDWR | O_SYNC);
	if (fd <= 0)
	{
		fprintf(stderr,"Error open stmvl6180_ranging device: %s\n", strerror(errno));
		return -1;
	}
	fprintf(stderr,"open vl6180_ranging device successfully!\n");

	//make sure it's not started
	if (ioctl(fd, VL6180_IOCTL_STOP , NULL) < 0) {
		fprintf(stderr, "Error: Could not perform VL6180_IOCTL_STOP : %s\n", strerror(errno));
		close(fd);
		return -1;
	}	
	if (mode == MODE_XTAKCALIB)
	{
		int num_samples = 20;
		int i=0;
		int RangeSum =0;
		int RateSum = 0;
		int XtalkInt =0;
		fprintf(stderr, "xtalk Calibrate place black target at 100mm from glass===\n");
		// to xtalk calibration 
		if (ioctl(fd, VL6180_IOCTL_XTALKCALB , NULL) < 0) {
			fprintf(stderr, "Error: Could not perform VL6180_IOCTL_XTALKCALB : %s\n", strerror(errno));
			close(fd);
			return -1;
		}
		for (i=0; i< num_samples; i++)
		{

			usleep(50*1000); /*50ms*/
			ioctl(fd, VL6180_IOCTL_GETDATAS,&range_datas);
			fprintf(stderr," VL6180 Range Data:%d, error status:0x%x, Rtn Signal Rate:%d, signalRate_mcps:%d\n",
				range_datas.range_mm, range_datas.errorStatus, range_datas.rtnRate, range_datas.signalRate_mcps);	
			RangeSum += range_datas.range_mm;
			RateSum += 	range_datas.signalRate_mcps;// signalRate_mcps in 9.7 format

		}
		XtalkInt = (int)((float)RateSum/num_samples *(1-(float)RangeSum/num_samples/100));
		fprintf(stderr, "VL6180 Xtalk Calibration get Xtalk Compensation rate in 9.7 format as %d\n",XtalkInt);
		if (ioctl(fd, VL6180_IOCTL_SETXTALK,&XtalkInt) < 0) {
			fprintf(stderr, "Error: Could not perform VL6180_IOCTL_SETXTALK : %s\n", strerror(errno));
			close(fd);
			return -1;
		}
		//to stop
		close(fd);
		return -1;
	}
	else if (mode == MODE_OFFCALIB)
	{
		int num_samples = 20;
		int i=0;
		int RangeSum =0,RangeAvg=0;
		int OffsetInt =0;
		fprintf(stderr, "offset Calibrate place white target at 50mm from glass===\n");
		// to offset calibration 
		if (ioctl(fd, VL6180_IOCTL_INIT , NULL) < 0) {
			fprintf(stderr, "Error: Could not perform VL6180_IOCTL_INIT : %s\n", strerror(errno));
			close(fd);
			return -1;
		}
		for (i=0; i< num_samples; i++)
		{

			usleep(50*1000); /*50ms*/
			ioctl(fd, VL6180_IOCTL_GETDATAS,&range_datas);
			fprintf(stderr," VL6180 Range Data:%d, error status:0x%x, Rtn Signal Rate:%d, signalRate_mcps:%d\n",
				range_datas.range_mm, range_datas.errorStatus, range_datas.rtnRate, range_datas.signalRate_mcps);	
			RangeSum += range_datas.range_mm;

		}
		RangeAvg = RangeSum/num_samples;
		fprintf(stderr, "VL6180 Offset Calibration get the average Range as %d\n", RangeAvg);
		if ((RangeAvg >= (50-3)) && (RangeAvg <= (50+3)))
			fprintf(stderr, "VL6180 Offset Calibration: original offset is OK, finish offset calibration\n");
		else
		{
			int8_t offset=0;
			if (ioctl(fd, VL6180_IOCTL_STOP , NULL) < 0) {
				fprintf(stderr, "Error: Could not perform VL6180_IOCTL_STOP : %s\n", strerror(errno));
				close(fd);
				return -1;
			}
			if (ioctl(fd, VL6180_IOCTL_OFFCALB , NULL) < 0) {
				fprintf(stderr, "Error: Could not perform VL6180_IOCTL_OFFCALB : %s\n", strerror(errno));
				close(fd);
				return -1;
			}
			RangeSum = 0;
			for (i=0; i< num_samples; i++)
			{

				usleep(50*1000); /*50ms*/
				ioctl(fd, VL6180_IOCTL_GETDATAS,&range_datas);
				fprintf(stderr," VL6180 Range Data:%d, error status:0x%x, Rtn Signal Rate:%d, signalRate_mcps:%d\n",
					range_datas.range_mm, range_datas.errorStatus, range_datas.rtnRate, range_datas.signalRate_mcps);	
				RangeSum += range_datas.range_mm;

			}
			RangeAvg = RangeSum/num_samples;
			fprintf(stderr, "VL6180 Offset Calibration get the average Range as %d\n", RangeAvg);
			offset = 50 - RangeAvg;
			fprintf(stderr, "VL6180 Offset Calibration to set the offset value(pre-scaling) as %d\n",offset);
			/**now need to resset the driver state to scaling mode that is being turn off by IOCTL_OFFCALB**/ 
			if (ioctl(fd, VL6180_IOCTL_STOP , NULL) < 0) {
				fprintf(stderr, "Error: Could not perform VL6180_IOCTL_STOP : %s\n", strerror(errno));
				close(fd);
				return -1;
			}	
			if (ioctl(fd, VL6180_IOCTL_INIT , NULL) < 0) {
				fprintf(stderr, "Error: Could not perform VL6180_IOCTL_INIT : %s\n", strerror(errno));
				close(fd);
				return -1;
			}
			if (ioctl(fd, VL6180_IOCTL_SETOFFSET, &offset) < 0) {
				fprintf(stderr, "Error: Could not perform VL6180_IOCTL_SETOFFSET : %s\n", strerror(errno));
				close(fd);
				return -1;
			}

		}	

		close(fd);
		return -1;
	}	
	else
	{
		// to init 
		if (ioctl(fd, VL6180_IOCTL_INIT , NULL) < 0) {
			fprintf(stderr, "Error: Could not perform VL6180_IOCTL_INIT : %s\n", strerror(errno));
			close(fd);
			return -1;
		}
	fprintf(stderr,"init vl6180_ranging device successfully!\n");		
	}
	// get data testing
	int num = 10;
	while (num)
	{
		usleep(1000*1000); /*100ms*/
		// to get proximity value only
		/*
		ioctl(fd, VL6180_IOCTL_GETDATA , &data) ;
		*/
		// to get all range data
		fprintf(stderr, "VL6180_IOCTL_GETDATAS = %ld\n", VL6180_IOCTL_GETDATAS);		
		if(ioctl(fd, VL6180_IOCTL_GETDATAS,&range_datas) < 0)
			fprintf(stderr, "Error: Could not perform VL6180_IOCTL_GETDATAS : %s\n", strerror(errno));
		
		fprintf(stderr," VL6180 Range Data:%d, error status:0x%x, Rtn Signal Rate:%d, signalRate_mcps:%d, Rtn Amb Rate:%d, Rtn Conv Time:%d,"
			"Ref Conv Time:%d, DMax:%d, Ref Amb Rate:%d, Conv Time:%d, Rtn Signal Count:%d, Ref Signal Count:%d, Rtn Amb Count:%d,"
		"Ref Amb Count:%d, Ref Rate:%d, Cross Talk:%d, Range Offset:%d\n",	range_datas.range_mm, range_datas.errorStatus, range_datas.rtnRate,
		range_datas.signalRate_mcps, range_datas.rtnAmbRate, range_datas.rtnConvTime, range_datas.refConvTime, range_datas.DMax, 
		range_datas.m_refAmbRate, range_datas.m_convTime, range_datas.m_rtnSignalCount, range_datas.m_refSignalCount, range_datas.m_rtnAmbientCount, 
		range_datas.m_refAmbientCount, range_datas.m_refRate, range_datas.m_crossTalk, range_datas.m_rangeOffset);
		num--;

	}
	close(fd);
	return 0;
}


