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
