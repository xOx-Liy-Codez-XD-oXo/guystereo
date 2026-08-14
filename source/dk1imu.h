#ifndef DK1IMU_H
#define DK1IMU_H

/* 
To include function definitions, define DK1IMU_IMPLEMENTATON before including dk1imu.h
You should do this in only one c file to prevent duplicate definition */

#include <gccore.h>

int DK1IMU_Open(void);
void DK1IMU_Close(void);
int DK1IMU_Dataplease(void * buffer);
void DK1IMU_SendKeepalive(void);
void DK1IMU_Decodesample(const unsigned char* buffer, int32_t* smp);
void DK1IMU_vec_to_vecf(s32 * svec, float * vecf);

#ifdef DK1IMU_IMPLEMENTATION

#include <unistd.h>
#include <math.h>

s32 dk1imu_devicehandle;
usb_devdesc dk1imu_devicedescriptor;

s16 swap_s16(s16 val) 
{
    return (val << 8) | ((val >> 8) & 0xFF);
}

int DK1IMU_Open(void) {
	usb_device_entry devicelist[32];
	memset(devicelist, 0, sizeof(usb_device_entry)*32);
	u8 devcnt = 0;
	if(USB_GetDeviceList(devicelist, 32, 0x00, &devcnt) < 0) {
		return -1;
	}

	//only one interface, only 1 device
	for(int d = 0; d < devcnt; d++) {
		if(devicelist[d].vid != 0x2833 || devicelist[d].pid != 0x0001) //rift dk1
			continue;

		if (USB_OpenDevice(devicelist[d].device_id, 0x2833, 0x0001, &(dk1imu_devicehandle)) < 0) { //rift dk1
			//printf("Couldn't open device %d\n", d);
			return -2;
		}

		USB_GetDescriptors(dk1imu_devicehandle, &(dk1imu_devicedescriptor));
		USB_SetConfiguration(dk1imu_devicehandle, dk1imu_devicedescriptor.configurations[0].bConfigurationValue);

		return 1;
	}

	return 0;
}

int datapleasefinish;
int datapleaseresult;
int _dk1imu_funny_ahh_pingas_type_callback(int result, void * shit) {
	datapleasefinish = 1;
	datapleaseresult = result;
	return result;
}

int DK1IMU_Dataplease(void * buffer) {
	if(!dk1imu_devicehandle) return 0;
	datapleasefinish = 0;
	USB_ReadIntrMsgAsync(dk1imu_devicehandle, 0x81, 64, buffer, _dk1imu_funny_ahh_pingas_type_callback, NULL); //returns num bytes
	int timeswaited = 0;
	int succeeded = 0;
	while(1) {
		if(timeswaited == 20) {break;} //2ms
		if(datapleasefinish) {succeeded = 1; break;}
		usleep(100);
		timeswaited++;
	}
	if(!succeeded) USB_ClearHalt(dk1imu_devicehandle, 0x81);
	return datapleaseresult;
}

void DK1IMU_Datatosamples(unsigned char * imudat, float * samplevecs) { //accel, gyro, magnet
	int decodedsamples[9];
	DK1IMU_Decodesample((unsigned char *)&(imudat[8]), &(decodedsamples[0]));
	DK1IMU_Decodesample((unsigned char *)&(imudat[16]), &(decodedsamples[3]));
	decodedsamples[6] = swap_s16(*(signed short *)&(imudat[56]));//Values unusable without endian swap
	decodedsamples[7] = swap_s16(*(signed short *)&(imudat[58]));
	//decodedsamples[8] = swap_s16(*(signed short *)&(imudat[60]));	
	decodedsamples[8] = 0; //This axis is broken on my magnetometer, so it will be broken on every magnetometer

	DK1IMU_vec_to_vecf(&(decodedsamples[0]), &(samplevecs[0]));
	DK1IMU_vec_to_vecf(&(decodedsamples[3]), &(samplevecs[3]));
	DK1IMU_vec_to_vecf(&(decodedsamples[6]), &(samplevecs[6]));
	return;
} 

void DK1IMU_SendKeepalive(void) {
	if(!dk1imu_devicehandle) return;
	unsigned char keepalivedata[6] ATTRIBUTE_ALIGN(32) = {8, 0, 0, 0xFF, 0xFF, 0x00}; //setting keepalive to maximum whatever
	USB_WriteCtrlMsg(dk1imu_devicehandle, USB_REQTYPE_INTERFACE_SET, USB_REQ_SETREPORT, (3 << 8) | 1, 0, 5, keepalivedata);
}

void DK1IMU_Close(void) {
	if(!dk1imu_devicehandle) return;
	USB_CloseDevice(&(dk1imu_devicehandle));
	dk1imu_devicehandle = 0;
}

void DK1IMU_Decodesample(const unsigned char* buffer, int32_t* smp) {
	/*
	 * Decode 3 tightly packed 21 bit values from EIIIGHTTTT bytes not 4 EEIIIIGHHT bytes.
	 * We unpack them in the higher 21 bit values first and then shift
	 * them down to the lower in order to get the sign bits correct.
	 */

	int x = (buffer[0] << 24)          | (buffer[1] << 16) | ((buffer[2] & 0xF8) << 8);
	int y = ((buffer[2] & 0x07) << 29) | (buffer[3] << 21) | (buffer[4] << 13) | ((buffer[5] & 0xC0) << 5);
	int z = ((buffer[5] & 0x3F) << 26) | (buffer[6] << 18) | (buffer[7] << 10);

	smp[0] = x >> 11;
	smp[1] = y >> 11;
	smp[2] = z >> 11;
}

void DK1IMU_vec_to_vecf(s32 * svec, float * vecf) {
	vecf[0] = (float)svec[0] * 0.0001f;
	vecf[1] = (float)svec[1] * 0.0001f;
	vecf[2] = (float)svec[2] * 0.0001f;
}

void DK1IMU_vecnormalize(float * a) {
	float len = sqrtf((a[0]*a[0]) + (a[1]*a[1]) + (a[2]*a[2]));
	if(len < 0.000001f) {
		a[0] = 0;
		a[1] = 0;
		a[2] = 1.0;
		return;
	}
	a[0] /= len;
	a[1] /= len;
	a[2] /= len;
}

void DK1IMU_veclerp(float * a, float * b, float * c, float t) {
	for(int i = 0; i < 3; i++)
		c[i] = (a[i] * (1.0f - t)) + (b[i] * t);
}

float DK1IMU_vecdot(float * a, float * b) {
	return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}

void DK1IMU_veccross(float * a, float * b, float * out) {
	out[0] = a[1]*b[2] - a[2]*b[1];
	out[1] = a[2]*b[0] - a[0]*b[2];
	out[2] = a[0]*b[1] - a[1]*b[0];
}

//our quats will be float * xyzw
void DK1IMU_quatmul(float * a, float * b, float * c) {
	float out[4];
	out[0] = a[3] * b[0] + a[0] * b[3] + a[1] * b[2] - a[2] * b[1];
	out[1] = a[3] * b[1] - a[0] * b[2] + a[1] * b[3] + a[2] * b[0];
	out[2] = a[3] * b[2] + a[0] * b[1] - a[1] * b[0] + a[2] * b[3];
	out[3] = a[3] * b[3] - a[0] * b[0] - a[1] * b[1] - a[2] * b[2];
	memcpy(c, out, 12);
}

void DK1IMU_quatnormalize(float * a) {
	float len = sqrtf(a[0] * a[0] + a[1] * a[1] + a[2] * a[2] + a[3] * a[3]);
	if(len < 0.000001f) {
		a[0] = 0;
		a[1] = 0;
		a[2] = 0;
		a[3] = 1.0f;
		return;
	}
	a[0] /= len;
	a[1] /= len;
	a[2] /= len;
	a[3] /= len;
}

void DK1IMU_eulertoquat(float * euler, float * quat) {
	float hx = euler[0] * 0.5f;
	float hy = euler[1] * 0.5f;
	float hz = euler[2] * 0.5f;

	float cx = cosf(hx);
	float sx = sinf(hx);
	float cy = cosf(hy);
	float sy = sinf(hy);
	float cz = cosf(hz);
	float sz = sinf(hz);

	quat[0] = sx * cy * cz - cx * sy * sz;
	quat[1] = cx * sy * cz + sx * cy * sz;
	quat[2] = cx * cy * sz - sx * sy * cz;
	quat[3] = cx * cy * cz + sx * sy * sz;
}

void DK1IMU_quatfromaxisangle(float * axis, float angle, float * quat) {
	float half = angle * 0.5f;
	float s = sinf(half);
	quat[0] = axis[0] * s;
	quat[1] = axis[1] * s;
	quat[2] = axis[2] * s;
	quat[3] = cosf(half);
}

void DK1IMU_quatrotatevec(float * q, float * v, float * out) {
	float qv[4] = {v[0], v[1], v[2], 0.0f};
	float qinv[4] = {-q[0], -q[1], -q[2], q[3]};
	float tmp[4], res[4];
	DK1IMU_quatmul(q, qv, tmp);
	DK1IMU_quatmul(tmp, qinv, res);
	out[0] = res[0];
	out[1] = res[1];
	out[2] = res[2];
}

void DK1IMU_quattoeuler(float * quat, float * euler)
{
	float x = quat[0], y = quat[1], z = quat[2], w = quat[3];

	float sinr_cosp = 2.0f * (w * x + y * z);
	float cosr_cosp = 1.0f - 2.0f * (x * x + y * y);
	euler[0] = atan2f(sinr_cosp, cosr_cosp);

	float sinp = 2.0f * (w * y - z * x);
	if (fabsf(sinp) >= 1.0f)
		euler[1] = copysignf((float)(M_PI * 0.5), sinp);
	else
		euler[1] = asinf(sinp);

	float siny_cosp = 2.0f * (w * z + x * y);
	float cosy_cosp = 1.0f - 2.0f * (y * y + z * z);
	euler[2] = atan2f(siny_cosp, cosy_cosp);
}

int resetacceltimer = 100;
int resetaccelaccum = 1;
float accelsmooth[3] = {0.5, 0.5, 0.5};
float accelsmoothing = 0.9;
float lastdowntilted[3] = {0, 0, 0};
float downvector[3] = {0, 0, 0};
void DK1IMU_DownvecFusion(float * accel, float * gyro, int numcollects, float * outvector) { //use relative (collective) gyro as input
	float dt = (float)numcollects / 1000.0f;
	//Smooth accel then get lenngthhhhh
	float accelnormal[3];
	memcpy(accelnormal, accel, 12);
	DK1IMU_vecnormalize(accelnormal);
	if(resetaccelaccum) 
		for(int i = 0; i < 3; i++) {
			accelsmooth[i] = accelnormal[i];
			downvector[i] = accelsmooth[i];
		}
	if(resetacceltimer) {
		for(int i = 0; i < 3; i++) downvector[i] = accelsmooth[i];
		resetacceltimer--;
	}
	float accellength = 0;
	for(int i = 0; i < 3; i++) {
		accelsmooth[i] = (accelnormal[i] * (1.0f - accelsmoothing)) + (accelsmooth[i] * accelsmoothing);
		accellength += accelsmooth[i] * accelsmooth[i];
	}
	accellength = sqrtf(accellength);

	//averaging smoothed accel results into down vec... thats okay
	float acceltrust = 0.008f;

	float gyromag = sqrtf(gyro[0]*gyro[0] + gyro[1]*gyro[1] + gyro[2]*gyro[2]);
	float downtilted[3];
	memcpy(downtilted, downvector, 12);
	if(gyromag > 0.0000001f) {
		float gyroaxis[3] = { gyro[0]/gyromag, gyro[1]/gyromag, gyro[2]/gyromag };
		float gyroangle = gyromag * dt;
		float gyrodeltaq[4];
		gyroangle *= -6.2831853072f;
		DK1IMU_quatfromaxisangle(gyroaxis, gyroangle, gyrodeltaq);
		DK1IMU_quatrotatevec(gyrodeltaq, downtilted, downtilted);
		DK1IMU_vecnormalize(downtilted);
	}
	int anynan = 0;
	for(int i = 0; i < 3; i++) {if(isnan(downtilted[i])) anynan = 1;
	if(anynan) downtilted[i] = lastdowntilted[i]; else lastdowntilted[i] = downtilted[i];}

	DK1IMU_veclerp(downtilted, accelsmooth, downvector, acceltrust);

	for(int i = 0; i < 3; i++) outvector[i] = downvector[i];
	resetaccelaccum = 0;
	//printf("%7.4f %7.4f ", accellength, acceltrust);
}

void DK1IMU_rotatevecbygyro(float * invec, float * gyro, int numcollects, float * outvec) {
	float dt = (float)numcollects / 1000.0f;

	float gyromag = sqrtf(gyro[0]*gyro[0] + gyro[1]*gyro[1] + gyro[2]*gyro[2]);
	if(gyromag > 0.0000001f) {
		float gyroaxis[3] = { gyro[0]/gyromag, gyro[1]/gyromag, gyro[2]/gyromag };
		float gyroangle = gyromag * dt;
		float gyrodeltaq[4];
		gyroangle *= (M_PI * 2.0f);
		gyroangle *= -1.0f;
		DK1IMU_quatfromaxisangle(gyroaxis, gyroangle, gyrodeltaq);
		DK1IMU_quatrotatevec(gyrodeltaq, invec, outvec);
		DK1IMU_vecnormalize(outvec);
		for(int i = 0; i < 3; i++) {if(isnan(outvec[i])) outvec[i] = invec[i];}
	} else for(int i = 0; i < 3; i++) outvec[i] = invec[i];
}

int _dk1imu_calibrationexit = 0; //1 = cancel, 2 = confirm
void _dk1imu_cancelcalibrationcallback(uint a, void * b) {
	_dk1imu_calibrationexit = 1;
}
void _dk1imu_confirmcalibrationcallback(void) {
	_dk1imu_calibrationexit = 2;
}

void DK1IMU_Calibrationroutine(void * xfb, float * thecalibration) {
	if(!dk1imu_devicehandle) return;
	GX_DrawDone();
	GX_Flush();
	float newcalibration[4] = {0, 0, 0, 0}; //minmax x, minmax y
	float calibrationcenter[2]; //purely for visualization purpose
	float newcalibrationcentered[4]; //visaulization
	int firstgo = 1;

	_dk1imu_calibrationexit = 0;
	SYS_SetPowerCallback(_dk1imu_confirmcalibrationcallback);
	SYS_SetResetCallback(_dk1imu_cancelcalibrationcallback);
	while(1) {
		if(_dk1imu_calibrationexit) break;
		DK1IMU_SendKeepalive();
		unsigned char imudat[65] ATTRIBUTE_ALIGN(32) = {0, 0};
		int imudatsize;
		float magnecollective[3] = {0, 0, 0};
		float lastmags[3] = {0, 0, 0};
		int numcollects = 0;
		float samplevecs[9];
		int lastwasreused = 1;
		while(1) {
			if(imudat[1] == 2) break;
			imudatsize = DK1IMU_Dataplease(imudat);
			if(imudatsize == 0) break;
			if(imudatsize < 0) {DK1IMU_Close(); DK1IMU_Open(); break; }
			DK1IMU_Datatosamples(imudat, samplevecs);

			magnecollective[0] = samplevecs[6];
			magnecollective[1] = samplevecs[7];
			magnecollective[2] = samplevecs[8];

			int magwithinrange = 1; //Mag read will occastionally error crazy so put measurements within range eachoter whatever
			for(int i = 0; i < 2; i++) {
				if(magnecollective[i] >= (lastmags[i] + 0.05f)) magwithinrange = 0;
				if(magnecollective[i] <= (lastmags[i] - 0.05f)) magwithinrange = 0;
			}
			if(!magwithinrange && !lastwasreused) { //But lets prevent ourselves from doing it twice in a row
				for(int i = 0; i < 3; i++) magnecollective[i] = lastmags[i];
				lastwasreused = 1;
			} else {
				for(int i = 0; i < 3; i++) lastmags[i] = magnecollective[i];
				lastwasreused = 0;
			}

			if(firstgo) {
				newcalibration[0] = magnecollective[0];
				newcalibration[1] = magnecollective[0];
				newcalibration[2] = magnecollective[1];
				newcalibration[3] = magnecollective[1];
				firstgo = 0;
			} else {
				if(magwithinrange) {
					if(magnecollective[0] < newcalibration[0]) newcalibration[0] = magnecollective[0];
					if(magnecollective[0] > newcalibration[1]) newcalibration[1] = magnecollective[0];
					if(magnecollective[1] < newcalibration[2]) newcalibration[2] = magnecollective[1];
					if(magnecollective[1] > newcalibration[3]) newcalibration[3] = magnecollective[1];
				}
			}
			numcollects++;
		}
		
		if(numcollects) {
		calibrationcenter[0] = (newcalibration[0] * 0.5f) + (newcalibration[1] * 0.5f);
		calibrationcenter[1] = (newcalibration[2] * 0.5f) + (newcalibration[3] * 0.5f);
		newcalibrationcentered[0] = newcalibration[0] - calibrationcenter[0];
		newcalibrationcentered[1] = newcalibration[1] - calibrationcenter[0];
		newcalibrationcentered[2] = newcalibration[2] - calibrationcenter[1];
		newcalibrationcentered[3] = newcalibration[3] - calibrationcenter[1];
		
		
		Mtx44 persp;
		guMtx44Identity(persp);
		GX_LoadProjectionMtx(persp, GX_PERSPECTIVE);
		Mtx model;
		guMtxTrans(model, 0.0f, 0.0f, -1.0f);
		GX_LoadPosMtxImm(model, GX_PNMTX0);
		GX_ClearVtxDesc();
		GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
		GX_SetVtxAttrFmt(0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
		GX_Begin(GX_LINES, 0, 8);
		GX_Position3f32(newcalibrationcentered[0], newcalibrationcentered[3], 0.0f);
		GX_Position3f32(newcalibrationcentered[1], newcalibrationcentered[3], 0.0f);
		GX_Position3f32(newcalibrationcentered[1], newcalibrationcentered[3], 0.0f);
		GX_Position3f32(newcalibrationcentered[1], newcalibrationcentered[2], 0.0f);
		GX_Position3f32(newcalibrationcentered[1], newcalibrationcentered[2], 0.0f);
		GX_Position3f32(newcalibrationcentered[0], newcalibrationcentered[2], 0.0f);
		GX_Position3f32(newcalibrationcentered[0], newcalibrationcentered[2], 0.0f);
		GX_Position3f32(newcalibrationcentered[0], newcalibrationcentered[3], 0.0f);

		guMtxScale(model, 0.02, 0.02, 0.02);
		guMtxTransApply(model, model, (magnecollective[0] - calibrationcenter[0]), (magnecollective[1] - calibrationcenter[1]), -1.0f);
		GX_LoadPosMtxImm(model, GX_PNMTX0);
		GX_Begin(GX_TRIANGLEFAN, 0, 18);
		GX_Position3f32(0, 0, 0);
		for(int i = 0; i < 17; i++) {
			float faralong = (float)i / 16.0f;
			faralong *= 6.283;
			GX_Position3f32(sin(faralong), cos(faralong), 0);
		}

		}

		GX_DrawDone();
		GX_Flush();
		GX_PixModeSync();
		
		GX_CopyDisp(xfb, GX_TRUE);
	
		VIDEO_WaitVSync();
	}
	if(_dk1imu_calibrationexit == 2) memcpy(thecalibration, newcalibration, 16);	
}

#endif//DK1IMU_IMPLEMENTATION

/* Bits of this program were pinched off OpenHMD which follows this license
Boost Software License - Version 1.0 - August 17th, 2003

Permission is hereby granted, free of charge, to any person or organization
obtaining a copy of the software and accompanying documentation covered by
this license (the "Software") to use, reproduce, display, distribute,
execute, and transmit the Software, and to prepare derivative works of the
Software, and to permit third-parties to whom the Software is furnished to
do so, all subject to the following:

The copyright notices in the Software and this entire statement, including
the above license grant, this restriction and the following disclaimer,
must be included in all copies of the Software, in whole or in part, and
all derivative works of the Software, unless such copies or derivative
works are solely in the form of machine-executable object code generated by
a source language processor.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.*/

#endif//DK1IMU_H