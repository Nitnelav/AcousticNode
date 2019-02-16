#ifndef FLOATDATA_H
#define FLOATDATA_H

#include <NodeDataModel>

using QtNodes::NodeDataType;
using QtNodes::NodeData;

/// The class can potentially incapsulate any user data which
/// need to be transferred within the Node Editor graph
class FloatData : public NodeData
{
private:

  float number_;
public:

  FloatData()
    : number_(0.0)
  {}

  FloatData(float const number)
    : number_(number)
  {}

  NodeDataType type() const override
  {
    return NodeDataType {"float",
                         "Float"};
  }

  float number() const
  { return number_; }

  void setNumber(float const number)
  { number_ = number; }

};

#endif // FLOATDATA_H
