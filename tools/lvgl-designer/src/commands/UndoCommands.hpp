#pragma once
#include <QUndoCommand>
#include <QPointF>
#include <QSizeF>
#include <string>
#include "model/LuiDocument.hpp"

namespace designer {

class DesignerScene;

class AddWidgetCommand : public QUndoCommand {
    DesignerScene* m_scene;
    WidgetNode m_node;
public:
    AddWidgetCommand(DesignerScene* scene, WidgetNode node)
        : QUndoCommand("Add Widget"), m_scene(scene), m_node(std::move(node)) {}
    void redo() override;
    void undo() override;
};

class DeleteWidgetCommand : public QUndoCommand {
    DesignerScene* m_scene;
    std::string m_id;
    WidgetNode m_backup;
public:
    DeleteWidgetCommand(DesignerScene* scene, const std::string& id)
        : QUndoCommand("Delete Widget"), m_scene(scene), m_id(id) {}
    void redo() override;
    void undo() override;
};

class MoveResizeCommand : public QUndoCommand {
    DesignerScene* m_scene;
    std::string m_id;
    QPointF m_oldPos, m_newPos;
    QSizeF m_oldSize, m_newSize;
public:
    MoveResizeCommand(DesignerScene* scene, const std::string& id,
                      QPointF oldPos, QPointF newPos,
                      QSizeF oldSize, QSizeF newSize)
        : QUndoCommand("Move/Resize"), m_scene(scene), m_id(id),
          m_oldPos(oldPos), m_newPos(newPos), m_oldSize(oldSize), m_newSize(newSize) {}
    void redo() override;
    void undo() override;
};

class PropertyChangeCommand : public QUndoCommand {
    DesignerScene* m_scene;
    std::string m_id;
    std::string m_key;
    QVariant m_oldVal, m_newVal;
public:
    PropertyChangeCommand(DesignerScene* scene, const std::string& id,
                          const std::string& key, QVariant oldVal, QVariant newVal)
        : QUndoCommand("Change Property"), m_scene(scene), m_id(id),
          m_key(key), m_oldVal(std::move(oldVal)), m_newVal(std::move(newVal)) {}
    void redo() override;
    void undo() override;
};

}
