#ifndef CHOICEDATA_H
#define CHOICEDATA_H

#include <NodeDataModel>

using QtNodes::NodeDataType;
using QtNodes::NodeData;

/// The class can potentially incapsulate any user data which
/// need to be transferred within the Node Editor graph
class ChoiceData : public NodeData
{
private:

    int index_;
    QString string_;

public:

  ChoiceData()
    : index_(0), string_("")
  {}

  ChoiceData(int const index, QString const string)
      : index_(index), string_(string)
  {}

  NodeDataType type() const override
  {
    return NodeDataType {"choice",
                         "Choice"};
  }

  int index() const
  { return index_; }

  QString string() const
  { return string_; }

  void setIndexAndString(int const index, QString const string)
  {
      index_ = index;
      string_ = string;
  }

};

#endif // CHOICEDATA_H
