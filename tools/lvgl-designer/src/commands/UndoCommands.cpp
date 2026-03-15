#include "commands/UndoCommands.hpp"
#include "canvas/DesignerScene.hpp"

namespace designer {

void AddWidgetCommand::redo() {
    if (auto* doc = m_scene->document()) {
        doc->addChild(m_node);
        m_scene->scheduleRebuild();
    }
}

void AddWidgetCommand::undo() {
    if (auto* doc = m_scene->document()) {
        doc->removeById(m_node.id);
        m_scene->scheduleRebuild();
    }
}

void DeleteWidgetCommand::redo() {
    if (auto* doc = m_scene->document()) {
        auto* node = doc->findById(m_id);
        if (node) m_backup = *node;
        doc->removeById(m_id);
        m_scene->scheduleRebuild();
    }
}

void DeleteWidgetCommand::undo() {
    if (auto* doc = m_scene->document()) {
        doc->addChild(m_backup);
        m_scene->scheduleRebuild();
    }
}

void MoveResizeCommand::redo() {
    if (auto* doc = m_scene->document()) {
        auto* node = doc->findById(m_id);
        if (node) {
            node->setPos(m_newPos);
            node->setSize(m_newSize);
            m_scene->scheduleRebuild();
        }
    }
}

void MoveResizeCommand::undo() {
    if (auto* doc = m_scene->document()) {
        auto* node = doc->findById(m_id);
        if (node) {
            node->setPos(m_oldPos);
            node->setSize(m_oldSize);
            m_scene->scheduleRebuild();
        }
    }
}

void PropertyChangeCommand::redo() {
    if (auto* doc = m_scene->document()) {
        auto* node = doc->findById(m_id);
        if (node) {
            node->props[m_key] = m_newVal;
            m_scene->scheduleRebuild();
        }
    }
}

void PropertyChangeCommand::undo() {
    if (auto* doc = m_scene->document()) {
        auto* node = doc->findById(m_id);
        if (node) {
            node->props[m_key] = m_oldVal;
            m_scene->scheduleRebuild();
        }
    }
}

}
