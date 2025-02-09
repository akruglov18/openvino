// Copyright (C) 2018-2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <functional>
#include <memory>
#include <ngraph/pass/graph_rewrite.hpp>

namespace ngraph {
namespace pass {

class NGRAPH_API ReshapeAMatMul;
class NGRAPH_API ReshapeBMatMul;
class NGRAPH_API TransposeMatMul;

}  // namespace pass
}  // namespace ngraph

/**
 * @ingroup ie_transformation_common_api
 * @brief ReshapeAMatMul and ReshapeBMatMul transformations relax hard-coded Reshape followed by MatMul operation
 * For 2D Reshape search patterns are:
 *  - MatMul(Reshape(any_input, any_input), any_input)
 *  - MatMul(any_input, Reshape(any_input, any_input))
 */

class ngraph::pass::ReshapeAMatMul : public ngraph::pass::MatcherPass {
public:
    NGRAPH_RTTI_DECLARATION;
    ReshapeAMatMul();
};
class ngraph::pass::ReshapeBMatMul : public ngraph::pass::MatcherPass {
public:
    NGRAPH_RTTI_DECLARATION;
    ReshapeBMatMul();
};
class ngraph::pass::TransposeMatMul : public ngraph::pass::MatcherPass {
public:
    NGRAPH_RTTI_DECLARATION;
    TransposeMatMul();
};