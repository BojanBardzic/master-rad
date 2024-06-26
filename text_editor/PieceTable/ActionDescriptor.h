//
// Created by bbard on 6/10/2024.
//

#ifndef TEXT_EDITOR_ACTIONDESCRIPTOR_H
#define TEXT_EDITOR_ACTIONDESCRIPTOR_H

#include <vector>
#include "PieceDescriptor.h"

enum ActionType {
    Insert,
    Delete
};

class ActionDescriptor {
public:
    friend std::ostream& operator<<(std::ostream& out, const ActionDescriptor& action);

    ActionDescriptor(ActionType actionType, std::vector<PieceDescriptor*> descriptors, size_t index);
    ~ActionDescriptor();

    static ActionType getOppositeActionType(ActionType actionType);

    ActionType getActionType() const;
    std::vector<PieceDescriptor*> getDescriptors() const;
    size_t getIndex() const;

    void setActionType(ActionType actionType);
private:
    ActionType m_actionType;
    std::vector<PieceDescriptor*> m_descriptors;
    size_t m_index;
};


#endif //TEXT_EDITOR_ACTIONDESCRIPTOR_H
