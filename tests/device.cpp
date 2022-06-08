#include "device.h"
#include "utf8.h"

ComPtr<ID3D12Debug>  gDebug;
ComPtr<ID3D12Device> gDevice;
ComPtr<IDxcLibrary>  gDxcLibrary;
ComPtr<IDxcCompiler> gDxcCompiler;

int InitializeD3D12()
{
    //
    // Enable debug layer
    //
    HRESULT hr = D3D12GetDebugInterface(__uuidof(ID3D12Debug), &gDebug);
    if (FAILED(hr)) {
        return EXIT_FAILURE;
    }
    gDebug->EnableDebugLayer();

    //
    // Enable experimental shader models in case we're on an old version of Windows
    //
    UUID experimentalFeatures[] = {
        D3D12ExperimentalShaderModels,
    };
    hr = D3D12EnableExperimentalFeatures(1, experimentalFeatures, NULL, NULL);
    if (FAILED(hr)) {
        return EXIT_FAILURE;
    }

    //
    // Create device using first adapter
    //
    ComPtr<ID3D12Device5> device;
    hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&gDevice));
    if (FAILED(hr)) {
        return EXIT_FAILURE;
    }

    //
    // Create DXC library
    //
    hr = DxcCreateInstance(CLSID_DxcLibrary, __uuidof(IDxcLibrary), &gDxcLibrary);
    if (FAILED(hr)) {
        return EXIT_FAILURE;
    }

    //
    // Create DXC compiler
    //
    hr = DxcCreateInstance(CLSID_DxcCompiler, __uuidof(IDxcCompiler), &gDxcCompiler);
    if (FAILED(hr)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

HRESULT CompileShader(
    const std::string& source,
    const std::string& entryPoint,
    const std::string& profile,
    std::vector<char>* pResult,
    std::string*       pErrorMsg)
{
    if (!gDxcCompiler) {
        return E_FAIL;
    }

    //
    // Clear error msg so there isn't any confusion if it's getting reused.
    //
    if (pErrorMsg != nullptr) {
        pErrorMsg->clear();
    }

    ComPtr<IDxcBlobEncoding> sourceBlob;
    HRESULT                  hr = gDxcLibrary->CreateBlobWithEncodingFromPinned(
        (LPVOID)source.c_str(),
        static_cast<UINT32>(source.length()),
        CP_ACP,
        &sourceBlob);

    std::u16string entryPointU16 = utf8::utf8to16(entryPoint);
    std::u16string profileU16    = utf8::utf8to16(profile);

    std::vector<LPCWSTR>   arguments;
    std::vector<DxcDefine> defines;

    ComPtr<IDxcOperationResult> operationResult;
    hr = gDxcCompiler->Compile(
        sourceBlob.Get(),
        L"DxcCompileHlsl",
        (LPCWSTR)entryPointU16.c_str(),
        (LPCWSTR)profileU16.c_str(),
        arguments.data(),
        static_cast<UINT32>(arguments.size()),
        defines.empty() ? nullptr : defines.data(),
        static_cast<UINT32>(defines.size()),
        nullptr,
        &operationResult);
    if (FAILED(hr)) {
        return hr;
    }

    HRESULT status = S_OK;
    hr             = operationResult->GetStatus(&status);
    if (FAILED(hr) || FAILED(status)) {
        if (pErrorMsg != nullptr) {
            ComPtr<IDxcBlobEncoding> errorBlob;
            hr = operationResult->GetErrorBuffer(&errorBlob);
            if (SUCCEEDED(hr)) {
                BOOL   known    = FALSE;
                UINT32 codePage = 0;
                // This may not be necessary
                hr = errorBlob->GetEncoding(&known, &codePage);
                if (SUCCEEDED(hr)) {
                    SIZE_T n = errorBlob->GetBufferSize();
                    if (n > 0) {
                        *pErrorMsg = std::string((const char*)errorBlob->GetBufferPointer(), n);
                    }
                }
            }
        }
        return status;
    }

    ComPtr<IDxcBlob> resultBlob;
    hr = operationResult->GetResult(&resultBlob);
    if (FAILED(hr)) {
        return hr;
    }

    SIZE_T n = resultBlob->GetBufferSize();
    if ((n > 0) && (pResult != nullptr)) {
        const char* pData = (const char*)resultBlob->GetBufferPointer();
        *pResult          = std::vector<char>(pData, pData + n);
    }

    return S_OK;
}
