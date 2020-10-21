#include "Parallelization/OpenCLInterface.h"
#include <iostream>
#include <regex>

namespace LiPhEn {

	// OPENCL BUFFER
	OpenCLBuffer::OpenCLBuffer()
	{
		m_clBuffer = NULL;
	}

	OpenCLBuffer::~OpenCLBuffer()
	{
		delete m_clBuffer;
	}

	cl::Buffer* OpenCLBuffer::getBuffer()
	{
		return m_clBuffer;
	}

	void OpenCLBuffer::setBuffer(cl::Buffer* buffer)
	{
		m_clBuffer = buffer;
	}

	// OPENCL KERNEL
	OpenCLKernel::OpenCLKernel()
	{
		m_clKernel = NULL;
	}

	OpenCLKernel::~OpenCLKernel()
	{
		delete m_clKernel;
	}

	cl::Kernel* OpenCLKernel::getKernel()
	{
		return m_clKernel;
	}

	void OpenCLKernel::setKernel(cl::Kernel* kernel)
	{
		m_clKernel = kernel;
	}

	void OpenCLKernel::setArgument(unsigned int index, ParallelBuffer* data)
	{
		OpenCLBuffer* clBuffer = dynamic_cast<OpenCLBuffer*>(data);
		m_clKernel->setArg(index, *clBuffer->getBuffer());
	}

	void OpenCLKernel::setArgument(unsigned int index, unsigned int size, void* data)
	{
		m_clKernel->setArg(index, size, data);
	}

	// OPENCL INTERFACE
	OpenCLInterface::OpenCLInterface()
	{
	}

	void OpenCLInterface::initialize(bool usePrint)
	{
		cl::Platform::get(&m_clPlatforms);
		for (cl::Platform platform : m_clPlatforms)
		{
			std::vector<cl::Device> devicesCPU;
			platform.getDevices(CL_DEVICE_TYPE_CPU, &devicesCPU);

			std::vector<cl::Device> devicesGPU;
			platform.getDevices(CL_DEVICE_TYPE_GPU, &devicesGPU);

			std::vector<cl::Device> devices;
			devices.insert(devices.end(), devicesCPU.begin(), devicesCPU.end());
			devices.insert(devices.end(), devicesGPU.begin(), devicesGPU.end());

			if (usePrint)
			{
				std::cout << "Platform: " << platform.getInfo<CL_PLATFORM_NAME>() << std::endl;

				for (cl::Device device : devices)
				{
					std::cout << "Device: " << std::regex_replace(device.getInfo<CL_DEVICE_NAME>(), std::regex("^ +"), "") << std::endl;
					std::cout << "Supported Version: " << device.getInfo<CL_DEVICE_OPENCL_C_VERSION>() << std::endl;
					std::cout << "Vendor: " << device.getInfo<CL_DEVICE_VENDOR>() << std::endl;
					std::cout << "Global Memory: " << device.getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>() << std::endl;
					std::cout << "Local Memory: " << device.getInfo<CL_DEVICE_LOCAL_MEM_SIZE>() << std::endl;
					std::cout << "Max. Compute Units: " << device.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>() << std::endl;
					std::cout << "Max. Workgroup Size: " << device.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>() << std::endl;
				}
				std::cout << std::endl;
			}

			m_clDevicesCPU.insert(m_clDevicesCPU.end(), devicesCPU.begin(), devicesCPU.end());
			m_clDevicesGPU.insert(m_clDevicesGPU.end(), devicesGPU.begin(), devicesGPU.end());
		}

		if (m_clDevicesCPU.size() > 0)
		{
			m_hasCPU = true;
			m_isValid = true;
		}
			
		if (m_clDevicesGPU.size() > 0)
		{
			m_hasGPU = true;
			m_isValid = true;
		}
	}

	void OpenCLInterface::reinitContext(ParallelSources sources, ParallelDeviceType deviceType, bool usePrint)
	{
		if (deviceType == ParallelDeviceType::CPU)
			m_clDefaultDevice = m_clDevicesCPU[0];
		else
			m_clDefaultDevice = m_clDevicesGPU[0];

		m_clContext = cl::Context(m_clDefaultDevice);

		cl::Program::Sources clSources;
		for (std::pair<const char*, unsigned int> source : sources)
			clSources.push_back(std::make_pair(source.first, source.second));

		m_clProgram = cl::Program(m_clContext, clSources);
		m_clProgram.build("-cl-std=CL1.2");
		std::string buildInfo = m_clProgram.getBuildInfo<CL_PROGRAM_BUILD_LOG>(m_clDefaultDevice);

		m_clQueue = cl::CommandQueue(m_clContext, m_clDefaultDevice, CL_QUEUE_PROFILING_ENABLE);

		if (usePrint)
			std::cout << "Using " << std::regex_replace(m_clDefaultDevice.getInfo<CL_DEVICE_NAME>(), std::regex("^ +"), "") << std::endl << std::endl;
	}

	ParallelKernel* OpenCLInterface::createKernel(const char* name)
	{
		OpenCLKernel* kernel = new OpenCLKernel();
		kernel->setKernel(new cl::Kernel(m_clProgram, name));
		return kernel;
	}

	ParallelBuffer* OpenCLInterface::createBuffer(ParallelBufferType type, unsigned int size)
	{
		cl_mem_flags bufferType;
		switch (type)
		{
		case ParallelBufferType::READ_ONLY:
			bufferType = CL_MEM_READ_ONLY;
			break;
		case ParallelBufferType::WRITE_ONLY:
			bufferType = CL_MEM_WRITE_ONLY;
			break;
		default:
		case ParallelBufferType::READ_WRITE:
			bufferType = CL_MEM_READ_WRITE;
			break;
		}

		OpenCLBuffer* buffer = new OpenCLBuffer();
		buffer->setBuffer(new cl::Buffer(m_clContext, bufferType, size));
		return buffer;
	}

	void OpenCLInterface::executeKernel(ParallelKernel* kernel, unsigned int globalSize, unsigned int localSize)
	{
		cl::Event event;
		OpenCLKernel* clKernel = dynamic_cast<OpenCLKernel*>(kernel);
		if(localSize > 0)
			m_clQueue.enqueueNDRangeKernel(*clKernel->getKernel(), cl::NullRange, cl::NDRange(globalSize), cl::NDRange(localSize), NULL, &event);
		else
			m_clQueue.enqueueNDRangeKernel(*clKernel->getKernel(), cl::NullRange, cl::NDRange(globalSize), cl::NullRange, NULL, &event);
	}

	void OpenCLInterface::writeToBuffer(ParallelBuffer* targetData, void* sourceData, unsigned int bufferSize, bool isBlocking)
	{
		cl::Event event;
		OpenCLBuffer* clBuffer = dynamic_cast<OpenCLBuffer*>(targetData);
		m_clQueue.enqueueWriteBuffer(*clBuffer->getBuffer(), isBlocking, 0, bufferSize, sourceData, NULL, &event);
	}

	void OpenCLInterface::readFromBuffer(ParallelBuffer* sourceData, void* targetData, unsigned int bufferSize, bool isBlocking)
	{
		cl::Event event;
		OpenCLBuffer* clBuffer = dynamic_cast<OpenCLBuffer*>(sourceData);
		m_clQueue.enqueueReadBuffer(*clBuffer->getBuffer(), isBlocking, 0, bufferSize, targetData, NULL, &event);
	}

	void OpenCLInterface::fillBuffer(ParallelBuffer* targetData, int pattern, unsigned int bufferSize)
	{
		cl::Event event;
		OpenCLBuffer* clBuffer = dynamic_cast<OpenCLBuffer*>(targetData);
		m_clQueue.enqueueFillBuffer(*clBuffer->getBuffer(), pattern, 0, bufferSize, NULL, &event);
	}

	void OpenCLInterface::waitUntilFinished()
	{
		m_clQueue.finish();
	}
}