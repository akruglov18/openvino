// Copyright (C) 2018-2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <functional>
#include <memory>
#include <ngraph/pass/graph_rewrite.hpp>
#include <transformations_visibility.hpp>

namespace ngraph {
namespace pass {

class TRANSFORMATIONS_API ConvolutionMultiplyFusion;
class TRANSFORMATIONS_API GroupConvolutionMultiplyFusion;
class TRANSFORMATIONS_API ConvolutionBackpropDataMultiplyFusion;
class TRANSFORMATIONS_API GroupConvolutionBackpropDataMultiplyFusion;

}  // namespace pass
}  // namespace ngraph

class ngraph::pass::ConvolutionMultiplyFusion : public ngraph::pass::MatcherPass {
public:
    NGRAPH_RTTI_DECLARATION;
    ConvolutionMultiplyFusion();
};

class ngraph::pass::GroupConvolutionMultiplyFusion : public ngraph::pass::MatcherPass {
public:
    NGRAPH_RTTI_DECLARATION;
    GroupConvolutionMultiplyFusion();
};

class ngraph::pass::ConvolutionBackpropDataMultiplyFusion : public ngraph::pass::MatcherPass {
public:
    NGRAPH_RTTI_DECLARATION;
    ConvolutionBackpropDataMultiplyFusion();
};

class ngraph::pass::GroupConvolutionBackpropDataMultiplyFusion : public ngraph::pass::MatcherPass {
public:
    NGRAPH_RTTI_DECLARATION;
    GroupConvolutionBackpropDataMultiplyFusion();
};
