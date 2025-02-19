// Copyright (C) 2018-2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "cum_sum_kernel_partial_sum.h"
#include "kernel_selector_utils.h"
#include <string>
#include <vector>
#include <iostream>

namespace kernel_selector {

static constexpr size_t simd = 16;
static constexpr size_t BLOCK_SIZE = 16;

ParamsKey CumSumKernelPartialSum::GetSupportedKey() const {
    ParamsKey k;
    k.EnableInputDataType(Datatype::F16);
    k.EnableInputDataType(Datatype::F32);
    k.EnableOutputDataType(Datatype::F16);
    k.EnableOutputDataType(Datatype::F32);

    k.EnableInputLayout(DataLayout::bfyx);
    k.EnableInputLayout(DataLayout::bfzyx);
    k.EnableInputLayout(DataLayout::bfwzyx);
    k.EnableOutputLayout(DataLayout::bfyx);
    k.EnableOutputLayout(DataLayout::bfzyx);
    k.EnableOutputLayout(DataLayout::bfwzyx);

    k.EnableTensorOffset();
    k.EnableTensorPitches();
    k.EnableBatching();
    return k;
}

JitConstants CumSumKernelPartialSum::GetJitConstants(const cum_sum_params& params, DispatchData dispatchData) const {
    auto jits = CumSumKernelBase::GetJitConstants(params, dispatchData);

    auto activation_dt = GetActivationType(params);
    jits.Merge(MakeTypeJitConstants(activation_dt, "PARTIAL"));
    jits.AddConstant(MakeJitConstant("SIMD", simd));
    jits.AddConstant(MakeJitConstant("LWS", dispatchData.lws[0]));
    jits.AddConstant(MakeJitConstant("BLOCK_SIZE", BLOCK_SIZE));
    jits.AddConstant(MakeJitConstant("SUM_ITEMS_NUM", dispatchData.sum_items_num));

    return jits;
}

KernelsData CumSumKernelPartialSum::GetMultiStageKernelsData(const Params& params,
                                                             const optional_params& options) const {
    if (!Validate(params, options))
        return {};

    constexpr size_t kernels_num = 2;
    KernelData kd = KernelData::Default<cum_sum_params>(params, kernels_num);
    const cum_sum_params& newParams = *static_cast<cum_sum_params*>(kd.params.get());

    auto dispatchData = SetDefaultForMulti(newParams);
    {
        // partial sum
        auto cldnn_jit = GetJitConstants(newParams, dispatchData.stage_1);
        cldnn_jit.AddConstant(MakeJitConstant("CUM_SUM_PARTIAL_SUM", 1));
        auto entry_point = GetEntryPoint(kernelName, newParams.layerID, params, options);
        auto jit = CreateJit(kernelName, cldnn_jit, entry_point);
        auto& kernel = kd.kernels[0];
        FillCLKernelData(kernel, dispatchData.stage_1, params.engineInfo, kernelName, jit, entry_point);
        kernel.params.arguments.clear();  // Clear original output argument
        kernel.params.arguments.push_back({ArgumentDescriptor::Types::INPUT, 0});
        kernel.params.arguments.push_back({ArgumentDescriptor::Types::INTERNAL_BUFFER, 0});
        kd.internalBufferSizes.push_back(newParams.output.PhysicalSizeInBytes());
    }
    {
        // Final
        auto entry_point = GetEntryPoint(kernelName, newParams.layerID, params, options, 1);
        auto cldnn_jit = GetJitConstants(newParams, dispatchData.stage_final);
        auto jit = CreateJit(kernelName, cldnn_jit, entry_point);

        auto& kernel = kd.kernels[1];

        FillCLKernelData(kernel, dispatchData.stage_final, params.engineInfo, kernelName, jit, entry_point);

        kernel.params.arguments.clear();  // Clear original output argument
        kernel.params.arguments.push_back({ArgumentDescriptor::Types::INTERNAL_BUFFER, 0});
        kernel.params.arguments.push_back({ArgumentDescriptor::Types::OUTPUT, 0});
    }
    kd.internalBufferDataType = Datatype::F32;

    return {kd};
}

CumSumKernelPartialSum::MultiDispatchData CumSumKernelPartialSum::SetDefaultForMulti(const cum_sum_params& params) const {
    MultiDispatchData dispatchData;
    std::vector<size_t> dims = {params.output.Batch().v,
                                params.output.Feature().v,
                                params.output.W().v,
                                params.output.Z().v,
                                params.output.Y().v,
                                params.output.X().v};

    size_t index = GetRealAxisIndex(params);
    auto items_num = dims[index];

    std::vector<size_t> gws(3, 0);
    gws[0] = dims[index];
    for (size_t i = 0, gws_i = 1; i < dims.size(); ++i) {
        if (i == index)
            continue;
        if (gws[gws_i] == 0) {
            gws[gws_i] = dims[i];
        } else {
            gws[gws_i] *= dims[i];
            if (gws_i == 1)
                ++gws_i;
        }
    }

    dispatchData.stage_1.gws[0] = Align(gws[0], BLOCK_SIZE);
    dispatchData.stage_1.gws[1] = gws[1];
    dispatchData.stage_1.gws[2] = gws[2];
    dispatchData.stage_1.lws[0] = BLOCK_SIZE;
    dispatchData.stage_1.lws[1] = 1;
    dispatchData.stage_1.lws[2] = 1;
    dispatchData.stage_1.sum_items_num = items_num;

    dispatchData.stage_final.gws = gws;
    dispatchData.stage_final.lws = { 1, 1, 1 };
    dispatchData.stage_final.sum_items_num = Align(items_num, BLOCK_SIZE);

    return dispatchData;
}

KernelsData CumSumKernelPartialSum::GetKernelsData(const Params& params, const optional_params& options) const {
    return GetMultiStageKernelsData(params, options);
}

KernelsPriority CumSumKernelPartialSum::GetKernelsPriority(const Params& /*params*/, const optional_params& /*options*/) const {
    return FORCE_PRIORITY_7;
}
}  // namespace kernel_selector
