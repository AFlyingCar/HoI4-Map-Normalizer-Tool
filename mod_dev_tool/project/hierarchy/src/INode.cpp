
#include "INode.h"

auto HMDT::Project::Hierarchy::IGroupNode::operator[](const std::string& name) const noexcept
    -> Maybe<ConstChildNode>
{
    if(getChildren().count(name) != 0) {
        return getChildren().at(name);
    } else {
        RETURN_ERROR(STATUS_VALUE_NOT_FOUND);
    }
}

auto HMDT::Project::Hierarchy::IGroupNode::operator[](const std::string& name) noexcept
    -> Maybe<ChildNode>
{
    if(getChildren().count(name) != 0) {
        return getChildren().at(name);
    } else {
        RETURN_ERROR(STATUS_VALUE_NOT_FOUND);
    }
}

std::string std::to_string(const HMDT::Project::Hierarchy::Node::Type& type) {
    switch(type) {
        case HMDT::Project::Hierarchy::Node::Type::GROUP:
            return "Group";
        case HMDT::Project::Hierarchy::Node::Type::PROJECT:
            return "Project";
        case HMDT::Project::Hierarchy::Node::Type::PROPERTY:
            return "Property";
        case HMDT::Project::Hierarchy::Node::Type::CONST_PROPERTY:
            return "ConstProperty";
        case HMDT::Project::Hierarchy::Node::Type::PROVINCE:
            return "Province";
        case HMDT::Project::Hierarchy::Node::Type::STATE:
            return "State";
        case HMDT::Project::Hierarchy::Node::Type::LINK:
            return "Link";
    }
}

std::string std::to_string(const HMDT::Project::Hierarchy::INode& node) {
    return node.getName() + " [" + std::to_string(node.getType()) + "]";
}
