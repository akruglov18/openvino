// Copyright (C) 2018-2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <algorithm>
#include <memory>
#include <string>
#include <utility>
#include <sstream>
#include <vector>

#include <ie_api.h>

#include <ngraph/pass/graph_rewrite.hpp>

namespace ngraph {
namespace pass {

class ConvertMatMulToFCorGemm;
class ConvertMatMulToFC;
class ConvertMatMulToGemm;

}  // namespace pass
}  // namespace ngraph

class ngraph::pass::ConvertMatMulToFC: public ngraph::pass::MatcherPass {
public:
    NGRAPH_RTTI_DECLARATION;
    ConvertMatMulToFC();
};

class ngraph::pass::ConvertMatMulToGemm: public ngraph::pass::MatcherPass {
public:
    NGRAPH_RTTI_DECLARATION;
    ConvertMatMulToGemm();
};

class ngraph::pass::ConvertMatMulToFCorGemm: public ngraph::pass::GraphRewrite {
public:
    NGRAPH_RTTI_DECLARATION;
    ConvertMatMulToFCorGemm() {
        add_matcher<ngraph::pass::ConvertMatMulToFC>();
        add_matcher<ngraph::pass::ConvertMatMulToGemm>();
    }
};