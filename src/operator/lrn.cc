/*!
 * Copyright (c) 2015 by Contributors
 * \file lrn.cc
 * \brief
 * \author Bing Xu
*/

#include "./lrn-inl.h"
#if MXNET_USE_CUDNN == 1
#include "./cudnn_lrn-inl.h"
#endif
#if MXNET_USE_MKL2017 == 1
#include <mkl_memory.h>
#include "./mkl/mkl_memory-inl.h"
#include "./mkl/mkl_lrn-inl.h"
#endif
#if USE_ACL == 1
#include "./acl/acl_lrn-inl.h"
#endif  // USE_ACL

namespace mxnet {
namespace op {
#if USE_ACL == 1
template<>
Operator *CreateOp<cpu>(LRNParam param, int dtype,Context & ctx) {
#else
template<>
Operator* CreateOp<cpu>(LRNParam param, int dtype) {
#endif
#if MXNET_USE_MKL2017 == 1
  return new MKLLRNOp<cpu, float>(param);
#endif
#if USE_ACL == 1
  if (dtype==mshadow::kFloat32) 
    return new ACLLocalResponseNormOp<cpu, float>(ctx,param);
#endif
  return new LocalResponseNormOp<cpu>(param);
}

// DO_BIND_DISPATCH comes from operator_common.h
Operator* LocalResponseNormProp::CreateOperatorEx(Context ctx, std::vector<TShape> *in_shape,
    std::vector<int> *in_type) const {
    std::vector<TShape> out_shape, aux_shape;
    std::vector<int> out_type, aux_type;
    CHECK(InferType(in_type, &out_type, &aux_type));
    CHECK(InferShape(in_shape, &out_shape, &aux_shape));
#if USE_ACL == 1
  DO_BIND_DISPATCH(CreateOp, param_, (*in_type)[0],ctx);
#else
   DO_BIND_DISPATCH(CreateOp, param_, (*in_type)[0]);
#endif
}

DMLC_REGISTER_PARAMETER(LRNParam);

MXNET_REGISTER_OP_PROPERTY(LRN, LocalResponseNormProp)
.add_argument("data", "NDArray-or-Symbol", "Input data.")
.add_arguments(LRNParam::__FIELDS__())
.describe(R"code(Applies local response normalization to the input.

The local response normalization layer performs "lateral inhibition" by normalizing 
over local input regions. 

If :math:`a_{x,y}^{i}` is the activity of a neuron computed by applying kernel :math:`i` at position
:math:`(x, y)` and then applying the ReLU nonlinearity, the response-normalized 
activity :math:`b_{x,y}^{i}` is given by the expression: 

.. math::   
   b_{x,y}^{i} = \frac{a_{x,y}^{i}}{\Bigg({k + \alpha \sum_{j=max(0, i-\frac{n}{2})}^{min(N-1, i+\frac{n}{2})} (a_{x,y}^{j})^{2}}\Bigg)^{\beta}}

where the sum runs over :math:`n` "adjacent" kernel maps at the same spatial position, and :math:`N` is the total
number of kernels in the layer.

)code" ADD_FILELINE);

}  // namespace op
}  // namespace mxnet
