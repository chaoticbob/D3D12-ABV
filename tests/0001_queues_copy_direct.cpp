#include "device.h"

int main(int argc, char** argv)
{
    if (InitializeD3D12() != EXIT_SUCCESS) {
        return EXIT_FAILURE;
    }



    return EXIT_SUCCESS;
}