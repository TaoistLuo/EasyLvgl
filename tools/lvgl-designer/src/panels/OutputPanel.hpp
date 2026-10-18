/**************************************************************************************************
 * @file        OutputPanel.hpp
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
#include <QWidget>
#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QFont>

namespace designer {

/** 内嵌输出面板：构建 / 运行的日志与程序输出（只读，可清空） */
class OutputPanel : public QWidget {
    Q_OBJECT
public:
    explicit OutputPanel(QWidget* parent = nullptr) : QWidget(parent) {
        auto* v = new QVBoxLayout(this);
        v->setContentsMargins(4, 4, 4, 4);
        v->setSpacing(4);
        auto* bar = new QHBoxLayout;
        auto* clear = new QPushButton("清空");
        bar->addStretch();
        bar->addWidget(clear);
        v->addLayout(bar);
        m_text = new QPlainTextEdit;
        m_text->setReadOnly(true);
        m_text->setMaximumBlockCount(20000);
        QFont f("Monospace");
        f.setStyleHint(QFont::TypeWriter);
        m_text->setFont(f);
        v->addWidget(m_text, 1);
        connect(clear, &QPushButton::clicked, m_text, &QPlainTextEdit::clear);
    }

    void append(const QString& s) {
        m_text->moveCursor(QTextCursor::End);
        m_text->insertPlainText(s);
        m_text->moveCursor(QTextCursor::End);
    }
    void clear() { m_text->clear(); }

private:
    QPlainTextEdit* m_text;
};

} // namespace designer
