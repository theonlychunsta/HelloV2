#pragma once
#include <CL\cl.h>
#include <vector>
#include "Kernel_TrumpBuildsAWallButLovesMexicans.h"
#include <iostream>
#include <string.h>
#include "Position_PikachuHatedAshButBefriendedHim.h"


cl_int clError;

std::string getPlatformInformation(cl_platform_id platform, cl_platform_info type)
{
	size_t valueSize = 0;
	clError = clGetPlatformInfo(platform, type, 0, nullptr, &valueSize);

	char* valueSizeChar = (char*)malloc(valueSize);
	clError = clGetPlatformInfo(platform, type, valueSize, valueSizeChar, nullptr);

	std::string info(valueSizeChar);
	free(valueSizeChar);

	return info;
}

std::vector<cl_platform_id> getPlatforms(cl_int type = CL_DEVICE_TYPE_ALL)
{
	cl_uint numPlatforms = 0;
	clError = clGetPlatformIDs(0, nullptr, &numPlatforms);

	std::vector<cl_platform_id> platforms(numPlatforms);
	std::vector<cl_platform_id> openclPlatforms;
	clError = clGetPlatformIDs(numPlatforms, platforms.data(), nullptr);

	for (auto platform : platforms)
	{
		std::string nameOfPlatform = getPlatformInformation(platform, CL_PLATFORM_NAME);
		std::cout << nameOfPlatform << std::endl;

		if (type == CL_DEVICE_TYPE_CPU && nameOfPlatform.find("OpenCL") != std::string::npos) {
			openclPlatforms.push_back(platform);
		}
		if (type == CL_DEVICE_TYPE_GPU && nameOfPlatform.find("CUDA") != std::string::npos) {
			openclPlatforms.push_back(platform);
		}
	}

	return openclPlatforms;
}

std::string getDeviceInformation(cl_device_id device, cl_device_info type)
{
	size_t valueSize = 0;
	clError = clGetDeviceInfo(device, type, 0, nullptr, &valueSize);

	char* valueSizeChar = (char*)malloc(valueSize);
	clError = clGetDeviceInfo(device, type, valueSize, valueSizeChar, nullptr);

	std::string info(valueSizeChar);
	free(valueSizeChar);

	return info;
}

std::vector<cl_device_id> getDevicesOfType(cl_int type)
{
	auto platforms = getPlatforms(type);
	std::vector<cl_device_id> devices;

	for (auto platform : platforms)
	{
		cl_uint numberOfDevices = 0;
		clError = clGetDeviceIDs(platform, type, 0, nullptr, &numberOfDevices);

		if (numberOfDevices) {
			std::vector<cl_device_id> platformDevices(numberOfDevices);
			clError = clGetDeviceIDs(platform, type, numberOfDevices, platformDevices.data(), nullptr);

			for (auto device : platformDevices)
			{
				std::string name = getDeviceInformation(device, CL_DEVICE_NAME);
				std::cout  << name << ", " << device << std::endl;

				devices.push_back(device);
			}
		}
	}
	return devices;
}

cl_program createProgramWithSource(cl_context context, std::string kernelCode)
{
	const size_t sizeOfKernelString = kernelCode.size();
	const char* cPtr = kernelCode.c_str();
	cl_program program = clCreateProgramWithSource(context, 1, &cPtr, &sizeOfKernelString, &clError);
	return program;
}

void enqueueForDataParallelism(cl_command_queue queue, cl_kernel kernel, size_t numberOfBlocks, size_t numberOfThreads)
{
	size_t globalWorkSize = numberOfBlocks * numberOfThreads;
	clError = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &globalWorkSize, &numberOfThreads, 0, NULL, NULL);
}


void doSomeGPUComputing(Boid* boids, int numberOfFlocks, int numberOfBoidsPerFlock)
{
	//Make it static to avoid calling again
	static auto devices = getDevicesOfType(CL_DEVICE_TYPE_GPU);
	static auto context = clCreateContext(nullptr, devices.size(), devices.data(), nullptr, nullptr, &clError);
	static auto queue = clCreateCommandQueue(context, devices[0], 0, &clError);
	static auto program = createProgramWithSource(context, updateBoidPosition());
	static auto program_built = clBuildProgram(program, devices.size(), devices.data(), NULL, NULL, NULL);
	static auto kernel_updateBoids = clCreateKernel(program, "updatePosition", &clError);

	auto memorySizeForBoids = sizeof(Boid) * numberOfFlocks * numberOfBoidsPerFlock;

	static auto memoryForBoids = clCreateBuffer(context, CL_MEM_READ_WRITE, memorySizeForBoids, NULL, &clError);

	static auto kernelParameterOne = clSetKernelArg(kernel_updateBoids, 0, sizeof(cl_mem), &memoryForBoids);
	static auto kernelParameterTwo = clSetKernelArg(kernel_updateBoids, 1, sizeof(int), &numberOfFlocks);
	static auto kernelParameterThree = clSetKernelArg(kernel_updateBoids, 2, sizeof(int), &numberOfBoidsPerFlock);

	clEnqueueWriteBuffer(queue, memoryForBoids, CL_TRUE, 0, memorySizeForBoids, boids, 0, NULL, NULL);
	clFinish(queue);

	enqueueForDataParallelism(queue, kernel_updateBoids, numberOfFlocks, numberOfBoidsPerFlock);
	clFinish(queue);

	clEnqueueReadBuffer(queue, memoryForBoids, CL_TRUE, 0, memorySizeForBoids, boids, 0, NULL, NULL);
	clFinish(queue);
}