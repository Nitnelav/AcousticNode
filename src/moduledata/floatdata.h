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

  double number_;
public:

  FloatData()
    : number_(0.0)
  {}

  FloatData(double const number)
    : number_(number)
  {}

  NodeDataType type() const override
  {
    return NodeDataType {"float",
                         "Float"};
  }

  double number() const
  { return number_; }

  void setNumber(double const number)
  { number_ = number; }

};

#endif // FLOATDATA_H
