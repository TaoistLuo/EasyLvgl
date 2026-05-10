#include "commands/UndoCommands.hpp"

namespace designer {

namespace {

void clamp_stacked_current_index(LuiDocument* doc, const std::string& parentId, int preferredIndex = -1) {
    if (!doc || parentId.empty()) return;

    auto* parent = doc->findById(parentId);
    if (!parent || parent->type != "LStackedLayout") return;

    const int childCount = static_cast<int>(parent->children.size());
    if (childCount <= 0) {
        parent->props["current_index"] = QVariant(0);
        return;
    }

    int index = preferredIndex;
    if (index < 0 || index >= childCount) {
        auto it = parent->props.find("current_index");
        index = (it != parent->props.end()) ? it->second.toInt() : 0;
        if (index < 0) index = 0;
        if (index >= childCount) index = childCount - 1;
    }

    parent->props["current_index"] = QVariant(index);
}

}

void AddWidgetCommand::redo() {
    if (m_doc) {
        std::string pid = m_node.parentId;
        if (pid.empty()) m_doc->addChild(WidgetNode(m_node));
        else {
            m_doc->addChildTo(pid, WidgetNode(m_node));
            auto* parent = m_doc->findById(pid);
            if (parent) {
                clamp_stacked_current_index(m_doc, pid, static_cast<int>(parent->children.size()) - 1);
            }
        }
        if (m_rebuild) m_rebuild();
    }
}

void AddWidgetCommand::undo() {
    if (m_doc) {
        auto* parent = m_doc->findParentOf(m_node.id);
        const std::string parentId = parent ? parent->id : std::string();
        m_doc->removeById(m_node.id);
        clamp_stacked_current_index(m_doc, parentId);
        if (m_rebuild) m_rebuild();
    }
}

void DeleteWidgetCommand::redo() {
    if (m_doc) {
        auto* node = m_doc->findById(m_id);
        if (node) {
            m_backup = *node;
            auto* parent = m_doc->findParentOf(m_id);
            if (parent) {
                m_parentId = parent->id;
                auto& siblings = parent->children;
                for (int i = 0; i < (int)siblings.size(); i++) if (siblings[i].id == m_id) { m_childIndex = i; break; }
            } else {
                m_parentId.clear();
                auto& roots = m_doc->children();
                for (int i = 0; i < (int)roots.size(); i++) if (roots[i].id == m_id) { m_childIndex = i; break; }
            }
        }
        m_doc->removeById(m_id);
        clamp_stacked_current_index(m_doc, m_parentId);
        if (m_rebuild) m_rebuild();
    }
}

void DeleteWidgetCommand::undo() {
    if (m_doc) {
        m_doc->insertChildAt(m_parentId, m_childIndex, WidgetNode(m_backup));
        clamp_stacked_current_index(m_doc, m_parentId, m_childIndex);
        if (m_rebuild) m_rebuild();
    }
}

void MoveResizeCommand::redo() {
    if (m_doc) {
        auto* node = m_doc->findById(m_id);
        if (node) {
            node->setPos(m_newPos);
            node->setSize(m_newSize);
            if (m_rebuild) m_rebuild();
        }
    }
}

void MoveResizeCommand::undo() {
    if (m_doc) {
        auto* node = m_doc->findById(m_id);
        if (node) {
            node->setPos(m_oldPos);
            node->setSize(m_oldSize);
            if (m_rebuild) m_rebuild();
        }
    }
}

void PropertyChangeCommand::redo() {
    if (m_doc) {
        auto* node = m_doc->findById(m_id);
        if (node) {
            node->props[m_key] = m_newVal;
            if (m_rebuild) m_rebuild();
        }
    }
}

void PropertyChangeCommand::undo() {
    if (m_doc) {
        auto* node = m_doc->findById(m_id);
        if (node) {
            node->props[m_key] = m_oldVal;
            if (m_rebuild) m_rebuild();
        }
    }
}

void ReparentCommand::redo() {
    if (!m_doc) return;
    auto* node = m_doc->findById(m_id);
    if (!node) return;

    m_backup = *node;
    auto* oldParent = m_doc->findParentOf(m_id);
    if (oldParent) {
        m_oldParentId = oldParent->id;
        auto& siblings = oldParent->children;
        for (int i = 0; i < (int)siblings.size(); i++) if (siblings[i].id == m_id) { m_oldIndex = i; break; }
    } else {
        m_oldParentId.clear();
        auto& roots = m_doc->children();
        for (int i = 0; i < (int)roots.size(); i++) if (roots[i].id == m_id) { m_oldIndex = i; break; }
    }

    m_doc->removeById(m_id);
    clamp_stacked_current_index(m_doc, m_oldParentId);
    if (m_newParentId.empty()) {
        m_newIndex = (int)m_doc->children().size();
        m_doc->addChild(m_backup);
    } else {
        auto* newParent = m_doc->findById(m_newParentId);
        if (newParent) {
            m_newIndex = (int)newParent->children.size();
            m_doc->addChildTo(m_newParentId, WidgetNode(m_backup));
            clamp_stacked_current_index(m_doc, m_newParentId, m_newIndex);
        } else {
            m_newIndex = (int)m_doc->children().size();
            m_doc->addChild(m_backup);
        }
    }
    if (m_rebuild) m_rebuild();
}

void ReparentCommand::undo() {
    if (!m_doc) return;
    m_doc->removeById(m_id);
    clamp_stacked_current_index(m_doc, m_newParentId);
    m_doc->insertChildAt(m_oldParentId, m_oldIndex, WidgetNode(m_backup));
    clamp_stacked_current_index(m_doc, m_oldParentId, m_oldIndex);
    if (m_rebuild) m_rebuild();
}

}
