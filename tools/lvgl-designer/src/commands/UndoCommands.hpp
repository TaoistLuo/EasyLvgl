#pragma once
#include <QUndoCommand>
#include <QPointF>
#include <QSizeF>
#include <string>
#include <functional>
#include "model/LuiDocument.hpp"

namespace designer {

using RebuildCallback = std::function<void()>;

class AddWidgetCommand : public QUndoCommand {
    LuiDocument* m_doc;
    RebuildCallback m_rebuild;
    WidgetNode m_node;
public:
    AddWidgetCommand(LuiDocument* doc, RebuildCallback rebuild, WidgetNode node)
        : QUndoCommand("添加组件"), m_doc(doc), m_rebuild(std::move(rebuild)), m_node(std::move(node)) {}
    void redo() override;
    void undo() override;
};

class DeleteWidgetCommand : public QUndoCommand {
    LuiDocument* m_doc;
    RebuildCallback m_rebuild;
    std::string m_id;
    WidgetNode m_backup;
    std::string m_parentId;
    int m_childIndex;
public:
    DeleteWidgetCommand(LuiDocument* doc, RebuildCallback rebuild, const std::string& id)
        : QUndoCommand("删除组件"), m_doc(doc), m_rebuild(std::move(rebuild)), m_id(id), m_childIndex(-1) {}
    void redo() override;
    void undo() override;
};

class MoveResizeCommand : public QUndoCommand {
    LuiDocument* m_doc;
    RebuildCallback m_rebuild;
    std::string m_id;
    QPointF m_oldPos, m_newPos;
    QSizeF m_oldSize, m_newSize;
public:
    MoveResizeCommand(LuiDocument* doc, RebuildCallback rebuild, const std::string& id,
                      QPointF oldPos, QPointF newPos, QSizeF oldSize, QSizeF newSize)
        : QUndoCommand("移动/缩放"), m_doc(doc), m_rebuild(std::move(rebuild)), m_id(id),
          m_oldPos(oldPos), m_newPos(newPos), m_oldSize(oldSize), m_newSize(newSize) {}
    void redo() override;
    void undo() override;
};

class PropertyChangeCommand : public QUndoCommand {
    LuiDocument* m_doc;
    RebuildCallback m_rebuild;
    std::string m_id;
    std::string m_key;
    QVariant m_oldVal, m_newVal;
public:
    PropertyChangeCommand(LuiDocument* doc, RebuildCallback rebuild, const std::string& id,
                          const std::string& key, QVariant oldVal, QVariant newVal)
        : QUndoCommand("修改属性"), m_doc(doc), m_rebuild(std::move(rebuild)), m_id(id),
          m_key(key), m_oldVal(std::move(oldVal)), m_newVal(std::move(newVal)) {}
    void redo() override;
    void undo() override;
};

class ReparentCommand : public QUndoCommand {
    LuiDocument* m_doc;
    RebuildCallback m_rebuild;
    std::string m_id;
    std::string m_oldParentId;
    int m_oldIndex;
    std::string m_newParentId;
    int m_newIndex;
    WidgetNode m_backup;
public:
    ReparentCommand(LuiDocument* doc, RebuildCallback rebuild, const std::string& id,
                    const std::string& newParentId)
        : QUndoCommand("改变父级"), m_doc(doc), m_rebuild(std::move(rebuild)), m_id(id),
          m_oldIndex(-1), m_newParentId(newParentId), m_newIndex(-1) {}
    void redo() override;
    void undo() override;
};

}
