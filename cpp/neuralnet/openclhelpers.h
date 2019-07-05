#ifndef NEURALNET_OPENCL_HELPERS_H_
#define NEURALNET_OPENCL_HELPERS_H_

#include "../core/global.h"
#include "../core/logger.h"
#include "../neuralnet/openclincludes.h"

#define CHECK_ERR(x) { OpenCLHelpers::checkErrors((x),__FILE__,#x,__LINE__); }

struct OpenCLTuneParams;

//Wrapper around cl_context for sharing initialization code
struct DevicesContext {
  cl_context context;
  std::vector<cl_platform_id> platformIds;
  std::vector<cl_device_id> deviceIds;
  std::vector<std::string> deviceNames;
  std::vector<std::string> deviceVendors;

  std::vector<int> gpuIdxsToUse;
  std::vector<cl_device_id> deviceIdsToUse;
  std::vector<cl_command_queue> commandQueues;

  DevicesContext(const std::vector<int>& gIdxs, Logger* logger, bool enableProfiling);
  ~DevicesContext();

  DevicesContext() = delete;
  DevicesContext(const DevicesContext&) = delete;
  DevicesContext& operator=(const DevicesContext&) = delete;

  //Given the gpu identifier from the system, find the index of that gpu for the *ToUse vectors
  int findWhichGpu(int gpuIdx) const;
};

namespace OpenCLHelpers {
  const char* getErrorMessage(cl_int error);
  void checkErrors(cl_int error, const char* file, const char* func, int line);
  cl_program compileProgram(const std::string& name, cl_context context, const std::vector<cl_device_id>& devices, const std::string& str, const std::string& options);

  cl_mem createReadOnlyBuffer(cl_context context, std::vector<float>& data);
  cl_mem createReadWriteBuffer(cl_context context, std::vector<float>& data);
  cl_mem createReadWriteBuffer(cl_context context, size_t numFloats);

  void blockingReadBuffer(cl_command_queue commandQueue, cl_mem srcBuf, size_t numFloats, std::vector<float>& dstBuf);

  size_t powerOf2ify(size_t size);
  size_t roundUpToMultiple(size_t size, size_t ofThis);

  cl_int doBatchedXGemm_KM_KN_MN(
    cl_kernel kernel,
    cl_command_queue commandQueue,
    const OpenCLTuneParams& tuneParams,
    int M, int N, int K,
    cl_mem A, cl_mem B, cl_mem C,
    int numBatchElts,
    cl_event* eventBuf
  );

  cl_int doStridedBatchedXGemm_KM_KN_MN(
    cl_kernel kernel,
    cl_command_queue commandQueue,
    const OpenCLTuneParams& tuneParams,
    int M, int N, int K,
    int aStride, int bStride, int cStride,
    cl_mem A, cl_mem B, cl_mem C,
    int numBatchElts,
    cl_event* eventBuf
  );

  cl_int doBatchedXGemm_MK_NK_MN(
    cl_kernel kernel,
    cl_command_queue commandQueue,
    const OpenCLTuneParams& tuneParams,
    int M, int N, int K,
    cl_mem A, cl_mem B, cl_mem C,
    int numBatchElts,
    cl_event* eventBuf
  );

  cl_int doWinogradTransform(
    cl_kernel kernel,
    cl_command_queue commandQueue,
    const OpenCLTuneParams& tuneParams,
    cl_mem input, cl_mem convWorkspace,
    int batchSize, int nnXLen, int nnYLen,
    int numTilesX, int numTilesY,
    int inChannels,
    cl_event* eventBuf
  );

  cl_int doWinogradUntransform(
    cl_kernel kernel,
    cl_command_queue commandQueue,
    const OpenCLTuneParams& tuneParams,
    cl_mem convWorkspace2, cl_mem output,
    int batchSize, int nnXLen, int nnYLen,
    int numTilesX, int numTilesY,
    int outChannels,
    cl_event* eventBuf
  );

}


#endif //NEURALNET_OPENCL_HELPERS_H_
