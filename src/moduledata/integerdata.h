#ifndef INTEGERDATA_H
#define INTEGERDATA_H

#include <NodeDataModel>

using QtNodes::NodeDataType;
using QtNodes::NodeData;

/// The class can potentially incapsulate any user data which
/// need to be transferred within the Node Editor graph
class IntegerData : public NodeData
{
private:

  int number_;
public:

  IntegerData()
    : number_(0)
  {}

  IntegerData(int const number)
    : number_(number)
  {}

  NodeDataType type() const override
  {
    return NodeDataType {"integer",
                         "Integer"};
  }

  int number() const
  { return number_; }

  void setNumber(int const number)
  { number_ = number; }

};

#endif // INTEGERDATA_H
