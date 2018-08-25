#pragma once

#include <vector>

namespace LiPhEn {
	typedef std::vector<std::pair<const char*, unsigned int>> ParallelSources;

	enum class ParallelDeviceType {
		GPU,
		CPU
	};

	enum class ParallelBufferType {
		READ_WRITE,
		READ_ONLY,
		WRITE_ONLY
	};

	class ParallelBuffer
	{
	public:
		ParallelBuffer() {};
		virtual ~ParallelBuffer() {};
	};

	class ParallelKernel
	{
	public:
		ParallelKernel() {};

		virtual void setArgument(unsigned int index, ParallelBuffer* data) = 0;
		virtual void setArgument(unsigned int index, unsigned int size, void* data) = 0;
	};

	class ParallelComputationInterface
	{
	public:
		ParallelComputationInterface() {};

		virtual void initialize(bool usePrint) = 0;
		virtual void reinitContext(ParallelSources sources, ParallelDeviceType deviceType, bool usePrint) = 0;
		virtual ParallelKernel* createKernel(const char* name) = 0;
		virtual ParallelBuffer* createBuffer(ParallelBufferType type, unsigned int size) = 0;
		virtual void executeKernel(ParallelKernel* kernel, unsigned int globalSize, unsigned int localSize = 0) = 0;
		virtual void writeToBuffer(ParallelBuffer* targetData, void* sourceData, unsigned int bufferSize, bool isBlocking) = 0;
		virtual void readFromBuffer(ParallelBuffer* sourceData, void* targetData, unsigned int bufferSize, bool isBlocking) = 0;
		virtual void fillBuffer(ParallelBuffer* targetData, int pattern, unsigned int bufferSize) = 0;
		virtual void waitUntilFinished() = 0;
	};
}