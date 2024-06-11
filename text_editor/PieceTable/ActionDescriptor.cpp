//
// Created by bbard on 6/10/2024.
//

#include "ActionDescriptor.h"

std::ostream& operator<<(std::ostream& out, const ActionDescriptor& action) {
    out << (action.m_actionType == ActionType::Insert ? "Insert" : "Delete") << ", "
        << action.m_index << std::endl;

    for (size_t i=0; i<action.m_descriptors.size(); ++i) {
        out << *(action.m_descriptors[i]) << std::endl;
    }

    return out;
}

ActionDescriptor::ActionDescriptor(ActionType actionType, std::vector<PieceDescriptor *> descriptors, size_t index)
    : m_actionType(actionType), m_descriptors(descriptors), m_index(index)  {}

ActionDescriptor::~ActionDescriptor() {
    for (auto descriptor : m_descriptors)
        delete descriptor;
}

ActionType ActionDescriptor::getOppositeActionType(ActionType actionType) {
    return actionType == ActionType::Insert ? ActionType::Delete : ActionType::Insert;
}

ActionType ActionDescriptor::getActionType() const { return m_actionType; }

std::vector<PieceDescriptor*> ActionDescriptor::getDescriptors() const { return m_descriptors; }

size_t ActionDescriptor::getIndex() const { return m_index; }

void ActionDescriptor::setActionType(ActionType actionType) { m_actionType = actionType; }
