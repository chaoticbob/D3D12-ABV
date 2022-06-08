#include "device.h"

std::string gShader = R"hlsl(

Texture2D<float4>   Inputs[8]  : register(t2);
RWTexture2D<float4> Outputs[8] : register(u2);

[numthreads(1, 1, 1)]
void main(uint3 tid : SV_DispatchThreadId)
{
    for (uint i = 0; i < 8; ++i) {
        Outputs[i][tid.xy] = Inputs[i][tid.xy];  
    }
    Outputs[7][tid.xy] += Inputs[7][tid.xy];  
}

)hlsl";

int main(int argc, char** argv)
{
    if (InitializeD3D12() != EXIT_SUCCESS) {
        return EXIT_FAILURE;
    }

    std::vector<char> CS;
    std::string       errorMsg;

    HRESULT hr = CompileShader(gShader, "main", "cs_6_1", &CS, &errorMsg);
    if (FAILED(hr)) {
        ConsoleWrite("Shader compile failed: " << errorMsg << "\n");
        return EXIT_FAILURE;
    }

    D3D12_DESCRIPTOR_RANGE1 ranges[3] = {};
    // SRV [2, 3, 4, 5]
    ranges[0].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    ranges[0].BaseShaderRegister                = 2;
    ranges[0].NumDescriptors                    = 4;
    ranges[0].RegisterSpace                     = 0;
    ranges[0].Flags                             = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
    ranges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    // SRV [6, 7, 8, 9]
    ranges[1].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    ranges[1].BaseShaderRegister                = 6;
    ranges[1].NumDescriptors                    = 4;
    ranges[1].RegisterSpace                     = 0;
    ranges[1].Flags                             = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
    ranges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    // UAV [2, 3, 4, 5, 6, 7, 8, 9]
    ranges[2].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
    ranges[2].BaseShaderRegister                = 2;
    ranges[2].NumDescriptors                    = 8;
    ranges[2].RegisterSpace                     = 0;
    ranges[2].Flags                             = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
    ranges[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    D3D12_ROOT_PARAMETER1 rootParams[1]               = {};
    rootParams[0].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParams[0].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_ALL;
    rootParams[0].DescriptorTable.NumDescriptorRanges = 3;
    rootParams[0].DescriptorTable.pDescriptorRanges   = ranges;

    D3D12_ROOT_SIGNATURE_DESC1 rootSigDesc = {};
    rootSigDesc.NumParameters              = 1;
    rootSigDesc.pParameters                = rootParams;

    D3D12_VERSIONED_ROOT_SIGNATURE_DESC versionedDesc = {};
    versionedDesc.Version                             = D3D_ROOT_SIGNATURE_VERSION_1_1;
    versionedDesc.Desc_1_1                            = rootSigDesc;

    //
    // This will return an error message in a blob if it fails.
    //
    ComPtr<ID3DBlob> signatureBlob;
    ComPtr<ID3DBlob> errorBlob;
    hr = D3D12SerializeVersionedRootSignature(&versionedDesc, &signatureBlob, &errorBlob);
    if (FAILED(hr)) {
        const char* errorMsg = static_cast<const char*>(errorBlob->GetBufferPointer());
        ConsoleWrite("Serialize root signature failed: " << errorMsg << "\n");
        return EXIT_FAILURE;
    }

    ComPtr<ID3D12RootSignature> rootSig;
    hr = gDevice->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSig));
    if (FAILED(hr)) {
        ConsoleWrite("Create root signature failed" << "\n");
        return EXIT_FAILURE;
    }

    D3D12_COMPUTE_PIPELINE_STATE_DESC pipelineDesc = {};
    pipelineDesc.pRootSignature                    = rootSig.Get();
    pipelineDesc.CS.pShaderBytecode                = static_cast<const void*>(CS.data());
    pipelineDesc.CS.BytecodeLength                 = static_cast<SIZE_T>(CS.size());
    pipelineDesc.NodeMask                          = 0;
    pipelineDesc.CachedPSO                         = {};
    pipelineDesc.Flags                             = D3D12_PIPELINE_STATE_FLAG_NONE;

    //
    // Debug layers will write an error message to the console if this fails.
    //
    ComPtr<ID3D12PipelineState> PSO;
    hr = gDevice->CreateComputePipelineState(&pipelineDesc, IID_PPV_ARGS(&PSO));
    if (FAILED(hr)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
