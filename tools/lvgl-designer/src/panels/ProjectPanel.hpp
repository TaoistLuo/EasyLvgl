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
#include <QTreeWidget>
#include <QMenu>
#include <QFont>
#include <string>
#include <vector>

namespace designer {

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
        connect(this, &QWidget::customContextMenuRequested, this, &ProjectPanel::onMenu);
        connect(this, &QTreeWidget::itemDoubleClicked, [this](QTreeWidgetItem* it, int) {
            QString p = it->data(0, Qt::UserRole).toString();
            if (p.endsWith(".lui")) emit openRequested(p);
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
                    const std::vector<ProjectFile>& files) {
        clear();
        m_manifest = manifest;
        auto* root = new QTreeWidgetItem(this);
        root->setText(0, name);
        root->setData(0, Qt::UserRole, manifest);
        QFont f = root->font(0);
        f.setBold(true);
        root->setFont(0, f);
        auto* screens = new QTreeWidgetItem(root);
        screens->setText(0, "屏幕");
        auto* comps = new QTreeWidgetItem(root);
        comps->setText(0, "组件");
        for (auto& pf : files) {
            auto* it = new QTreeWidgetItem(pf.isComponent ? comps : screens);
            it->setText(0, QString::fromStdString(pf.name));
            it->setData(0, Qt::UserRole, pf.path);
            it->setToolTip(0, pf.path);
        }
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
    void removeRequested(QString path);

private:
    void mark(QTreeWidgetItem* it, const QString& p) {
        if (it->data(0, Qt::UserRole).toString() == p) setCurrentItem(it);
        for (int i = 0; i < it->childCount(); ++i) mark(it->child(i), p);
    }

    void onMenu(const QPoint& pos) {
        QTreeWidgetItem* it = itemAt(pos);
        QString path = it ? it->data(0, Qt::UserRole).toString() : QString();
        bool isFile = path.endsWith(".lui");
        QMenu m;
        if (isFile) m.addAction("打开", [this, path] { emit openRequested(path); });
        m.addAction("新建组件…", [this] { emit newComponentRequested(); });
        m.addAction("新建屏幕…", [this] { emit newScreenRequested(); });
        m.addAction("添加已有文件…", [this] { emit addFilesRequested(); });
        if (isFile) m.addAction("从工程中删除", [this, path] { emit removeRequested(path); });
        m.exec(mapToGlobal(pos));
    }

    QString m_manifest;
};

} // namespace designer
