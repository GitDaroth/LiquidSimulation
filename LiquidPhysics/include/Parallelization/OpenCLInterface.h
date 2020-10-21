#pragma once

#define CL_HPP_MINIMUM_OPENCL_VERSION 110
#define CL_HPP_TARGET_OPENCL_VERSION 120
#define CL_HPP_CL_1_2_DEFAULT_BUILD

#include "Parallelization/ParallelComputationInterface.h"
#include <CL/cl.hpp>

namespace LiPhEn {
	class OpenCLBuffer : public ParallelBuffer
	{
	public:
		OpenCLBuffer();
		virtual ~OpenCLBuffer();

		cl::Buffer* getBuffer();
		void setBuffer(cl::Buffer* buffer);

	private:
		cl::Buffer* m_clBuffer;
	};

	class OpenCLKernel : public ParallelKernel
	{
	public:
		OpenCLKernel();
		virtual ~OpenCLKernel();

		cl::Kernel* getKernel();
		void setKernel(cl::Kernel* kernel);

		virtual void setArgument(unsigned int index, ParallelBuffer* data);
		virtual void setArgument(unsigned int index, unsigned int size, void* data);

	private:
		cl::Kernel* m_clKernel;
	};

	class OpenCLInterface : public ParallelComputationInterface
	{
	public:
		OpenCLInterface();

		virtual void initialize(bool usePrint);
		virtual void reinitContext(ParallelSources sources, ParallelDeviceType deviceType, bool usePrint);
		virtual ParallelKernel* createKernel(const char* name);
		virtual ParallelBuffer* createBuffer(ParallelBufferType type, unsigned int size);
		virtual void executeKernel(ParallelKernel* kernel, unsigned int globalSize, unsigned int localSize = 0);
		virtual void writeToBuffer(ParallelBuffer* targetData, void* sourceData, unsigned int bufferSize, bool isBlocking);
		virtual void readFromBuffer(ParallelBuffer* sourceData, void* targetData, unsigned int bufferSize, bool isBlocking);
		virtual void fillBuffer(ParallelBuffer* targetData, int pattern, unsigned int bufferSize);
		virtual void waitUntilFinished();

	private:
		std::vector<cl::Platform> m_clPlatforms;
		std::vector<cl::Device> m_clDevicesCPU;
		std::vector<cl::Device> m_clDevicesGPU;
		cl::Context m_clContext;
		cl::Program m_clProgram;
		cl::Device m_clDefaultDevice;
		cl::CommandQueue m_clQueue;
	};
}