//
// Created by bbard on 6/10/2024.
//

#include "ActionDescriptor.h"

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
