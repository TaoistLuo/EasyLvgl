/**************************************************************************************************
 * @file        ProjectPanel.hpp
 * @brief       EasyLVGL 设计器
 * @details     EasyLVGL —— 让 LVGL 开发像 Qt 一样简单：C++ 封装 + 可视化设计器 + 代码生成。
 *
 * @author      taoist.luo
 * @version     1.0.0
 * @date        2026-10-18
 *
 * Copyright (c) 2026 taoist.luo (https://github.com/TaoistLuo/EasyLVGL)
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *************************************************************************************************/

#pragma once
#include <QDir>
#include <QFileInfo>
#include <QTreeWidget>
#include <QMenu>
#include <QFont>
#include <QIcon>
#include <algorithm>
#include <functional>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace designer {

inline bool isTextFile(const QString& path) {
    const QFileInfo info(path);
    const QString suffix = info.suffix().toLower();
    const QString name = info.fileName().toLower();
    return suffix == "txt" || suffix == "md" || suffix == "cmake" ||
           suffix == "lpro" || suffix == "json" || suffix == "yaml" ||
           suffix == "yml" || suffix == "ini" || suffix == "sh" ||
           suffix == "ps1" || suffix == "csv" || name == "cmakelists.txt";
}

inline bool isCodeFile(const QString& path) {
    const QString suffix = QFileInfo(path).suffix().toLower();
    return suffix == "h" || suffix == "hpp" || suffix == "hh" || suffix == "hxx" ||
           suffix == "c" || suffix == "cpp" || suffix == "cc" || suffix == "cxx";
}

inline QString projectIconForPath(const QString& path) {
    if (path.startsWith("res:")) return ":/icons/file_image.svg";
    const QFileInfo info(path);
    const QString suffix = info.suffix().toLower();
    const QString name = info.fileName().toLower();
    if (suffix == "lui") return ":/icons/file_ui.svg";
    if (suffix == "h" || suffix == "hpp" || suffix == "hh" || suffix == "hxx" ||
        suffix == "c" || suffix == "cpp" || suffix == "cc" || suffix == "cxx" ||
        suffix == "py") {
        return ":/icons/file_source.svg";
    }
    if (suffix == "png" || suffix == "jpg" || suffix == "jpeg" || suffix == "bmp" ||
        suffix == "gif" || suffix == "svg" || suffix == "webp") {
        return ":/icons/file_image.svg";
    }
    if (suffix == "ttf" || suffix == "otf" || suffix == "woff" || suffix == "woff2") {
        return ":/icons/file_font.svg";
    }
    if (suffix == "xlsx" || suffix == "xls" || suffix == "csv") {
        return ":/icons/file_sheet.svg";
    }
    if (suffix == "json" || suffix == "lpro" || suffix == "cmake" ||
        suffix == "yaml" || suffix == "yml" || suffix == "ini" ||
        name == "cmakelists.txt") {
        return ":/icons/file_config.svg";
    }
    if (suffix == "md" || suffix == "txt" || suffix == "ofl" ||
        name.startsWith("readme") || name.startsWith("license")) {
        return ":/icons/file_doc.svg";
    }
    return ":/icons/file_other.svg";
}

struct ProjectFile {
    std::string name;      // .lui 里的 name
    QString path;
    bool isComponent = false;
};

/** 工程树（参照 Qt Creator 的项目视图）：屏幕 / 组件 两组，双击打开，右键管理 */
class ProjectPanel : public QTreeWidget {
    Q_OBJECT
public:
    explicit ProjectPanel(QWidget* parent = nullptr) : QTreeWidget(parent) {
        setHeaderHidden(true);
        setContextMenuPolicy(Qt::CustomContextMenu);
        setIndentation(16);
        setIconSize(QSize(16, 16));
        connect(this, &QWidget::customContextMenuRequested, this, &ProjectPanel::onMenu);
        connect(this, &QTreeWidget::itemDoubleClicked, [this](QTreeWidgetItem* it, int) {
            QString d = it->data(0, Qt::UserRole).toString();
            if (d.startsWith("res:")) emit previewResourceRequested(d.mid(4));
            else if (d.endsWith(".lui") || isCodeFile(d) || isTextFile(d))
                emit openRequested(d);
        });
        showEmpty();
    }

    void showEmpty() {
        clear();
        auto* it = new QTreeWidgetItem(this);
        it->setText(0, "单文件模式（未打开工程）");
        it->setFlags(Qt::ItemIsEnabled);
    }

    void setProject(const QString& name, const QString& manifest,
                    const std::vector<QString>& uiFiles,
                    const std::vector<QString>& sourceFiles,
                    const std::vector<std::pair<QString, QString>>& resources,
                    const std::vector<QString>& otherFiles) {
        clear();
        m_manifest = manifest;
        const QString projectDir = QFileInfo(manifest).absolutePath();

        auto* root = new QTreeWidgetItem(this);
        root->setText(0, name);
        root->setData(0, Qt::UserRole, manifest);
        root->setData(0, Qt::UserRole + 1, true);
        root->setToolTip(0, QFileInfo(manifest).fileName());
        root->setIcon(0, QIcon(":/icons/project.svg"));
        QFont bold = root->font(0);
        bold.setBold(true);
        root->setFont(0, bold);

        auto makeGroup = [&](const QString& title, const QString& icon) {
            auto* group = new QTreeWidgetItem(root);
            group->setText(0, title);
            group->setData(0, Qt::UserRole + 1, true);
            group->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            QFont groupFont = group->font(0);
            groupFont.setBold(true);
            group->setFont(0, groupFont);
            group->setIcon(0, QIcon(icon));
            return group;
        };

        std::map<QTreeWidgetItem*, std::map<QString, QTreeWidgetItem*>> dirs;
        auto addPath = [&](QTreeWidgetItem* group, const QString& path,
                           const QString& label, const QString& data) {
            QString rel = QDir(projectDir).relativeFilePath(path);
            rel.replace('\\', '/');
            const QStringList parts = rel.split('/', QString::SkipEmptyParts);
            if (parts.isEmpty()) return;
            QTreeWidgetItem* parent = group;
            QString key;
            for (int i = 0; i + 1 < parts.size(); ++i) {
                key += (key.isEmpty() ? "" : "/") + parts[i];
                auto& groupDirs = dirs[group];
                auto found = groupDirs.find(key);
                if (found == groupDirs.end()) {
                    auto* dir = new QTreeWidgetItem(parent);
                    dir->setText(0, parts[i]);
                    dir->setData(0, Qt::UserRole + 1, true);
                    dir->setToolTip(0, key);
                    dir->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
                    dir->setIcon(0, QIcon(":/icons/folder.svg"));
                    groupDirs[key] = dir;
                    parent = dir;
                } else {
                    parent = found->second;
                }
            }
            auto* leaf = new QTreeWidgetItem(parent);
            leaf->setText(0, label.isEmpty() ? parts.last() : label);
            leaf->setData(0, Qt::UserRole, data);
            leaf->setToolTip(0, rel);
            leaf->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            leaf->setIcon(0, QIcon(projectIconForPath(data)));
        };

        auto* uiGroup = makeGroup("UI 文件", ":/icons/file_ui.svg");
        auto* sourceGroup = makeGroup("源文件", ":/icons/file_source.svg");
        auto* resourceGroup = makeGroup("嵌入资源（qrc）", ":/icons/file_image.svg");
        auto* otherGroup = makeGroup("其他文件", ":/icons/file_doc.svg");

        for (const QString& path : uiFiles)
            addPath(uiGroup, path, QFileInfo(path).fileName(), path);
        for (const QString& path : sourceFiles)
            addPath(sourceGroup, path, QFileInfo(path).fileName(), path);
        for (const auto& r : resources) {
            const QString abs = QDir(projectDir).absoluteFilePath(r.second);
            addPath(resourceGroup, abs, "#" + r.first, "res:" + r.first);
        }
        for (const QString& path : otherFiles)
            addPath(otherGroup, path, QFileInfo(path).fileName(), path);

        std::function<void(QTreeWidgetItem*)> sortTree = [&](QTreeWidgetItem* parent) {
            std::vector<QTreeWidgetItem*> children;
            while (parent->childCount() > 0) children.push_back(parent->takeChild(0));
            std::sort(children.begin(), children.end(), [](QTreeWidgetItem* a, QTreeWidgetItem* b) {
                const bool aDir = a->data(0, Qt::UserRole + 1).toBool();
                const bool bDir = b->data(0, Qt::UserRole + 1).toBool();
                if (aDir != bDir) return aDir;
                return a->text(0).compare(b->text(0), Qt::CaseInsensitive) < 0;
            });
            for (auto* child : children) {
                parent->addChild(child);
                sortTree(child);
            }
        };
        for (auto* group : {uiGroup, sourceGroup, resourceGroup, otherGroup}) sortTree(group);
        expandAll();
    }

    void setCurrentFile(const QString& path) {
        setCurrentItem(nullptr);
        for (int i = 0; i < topLevelItemCount(); ++i) mark(topLevelItem(i), path);
    }

signals:
    void openRequested(QString path);
    void newComponentRequested();
    void newScreenRequested();
    void addFilesRequested();
    void addResourceRequested();
    void removeResourceRequested(QString alias);
    void previewResourceRequested(QString alias);
    void removeRequested(QString path);
    void projectSettingsRequested();

private:
    void mark(QTreeWidgetItem* it, const QString& p) {
        if (it->data(0, Qt::UserRole).toString() == p) setCurrentItem(it);
        for (int i = 0; i < it->childCount(); ++i) mark(it->child(i), p);
    }

    void onMenu(const QPoint& pos) {
        QTreeWidgetItem* it = itemAt(pos);
        QString data = it ? it->data(0, Qt::UserRole).toString() : QString();
        bool isFile = data.endsWith(".lui") || isCodeFile(data) || isTextFile(data);
        if (data == m_manifest) {
            QMenu rootMenu;
            rootMenu.addAction("编辑工程文件...", [this, data] { emit openRequested(data); });
            rootMenu.addAction("工程设置...", [this] { emit projectSettingsRequested(); });
            rootMenu.exec(mapToGlobal(pos));
            return;
        }
        bool isResource = data.startsWith("res:");
        QMenu m;
        if (isFile) m.addAction("打开", [this, data] { emit openRequested(data); });
        if (isResource) {
            QString alias = data.mid(4);
            m.addAction("删除资源", [this, alias] { emit removeResourceRequested(alias); });
            m.addSeparator();
        }
        m.addAction("新建组件…", [this] { emit newComponentRequested(); });
        m.addAction("新建屏幕…", [this] { emit newScreenRequested(); });
        m.addAction("添加已有文件…", [this] { emit addFilesRequested(); });
        m.addAction("添加图片资源…", [this] { emit addResourceRequested(); });
        if (data.endsWith(".lui") || isCodeFile(data))
            m.addAction("从工程中删除", [this, data] { emit removeRequested(data); });
        m.exec(mapToGlobal(pos));
    }

    QString m_manifest;
};

} // namespace designer
