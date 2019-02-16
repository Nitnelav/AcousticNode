#ifndef BOOLEANDATA_H
#define BOOLEANDATA_H

#include <NodeDataModel>

using QtNodes::NodeDataType;
using QtNodes::NodeData;

/// The class can potentially incapsulate any user data which
/// need to be transferred within the Node Editor graph
class BooleanData : public NodeData
{
private:

  bool boolean_;

public:

  BooleanData()
    : boolean_(false)
  {}

  BooleanData(bool const boolean)
    : boolean_(boolean)
  {}

  NodeDataType type() const override
  {
    return NodeDataType {"boolean",
                         "Boolean"};
  }

  int boolean() const
  { return boolean_; }

  void setBoolean(bool const boolean)
  { boolean_ = boolean; }

};

#endif // BOOLEANDATA_H
