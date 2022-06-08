#include <wrl/client.h>
#include <d3d12.h>
#include <dxcapi.h>

#include <string>
#include <sstream>
#include <vector>

using Microsoft::WRL::ComPtr;

extern ComPtr<ID3D12Debug>  gDebug;
extern ComPtr<ID3D12Device> gDevice;
extern ComPtr<IDxcLibrary>  gDxcLibrary;
extern ComPtr<IDxcCompiler> gDxcCompiler;

int InitializeD3D12();

HRESULT CompileShader(
    const std::string& source,
    const std::string& entryPoint,
    const std::string& profile,
    std::vector<char>* pResult,
    std::string*       pErrorMsg);

#define ConsoleWrite(MSG)                                   \
    {                                                       \
        std::stringstream ss_console_write;                 \
        ss_console_write << MSG;                            \
        OutputDebugStringA(ss_console_write.str().c_str()); \
    }
