#pragma once

#include <QCheckBox>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRegularExpression>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QWidget>

namespace designer {

class SearchPanel : public QWidget {
    Q_OBJECT
  public:
    explicit SearchPanel(QWidget * parent = nullptr) : QWidget(parent)
    {
        m_input = new QLineEdit(this);
        m_input->setPlaceholderText("在工程中搜索（Ctrl+Shift+F）");
        auto * searchButton = new QPushButton("搜索", this);

        auto * top = new QHBoxLayout();
        top->addWidget(m_input, 1);
        top->addWidget(searchButton);

        m_case         = new QCheckBox("区分大小写", this);
        m_regex        = new QCheckBox("正则", this);
        m_word         = new QCheckBox("全词", this);
        auto * options = new QHBoxLayout();
        options->addWidget(m_case);
        options->addWidget(m_regex);
        options->addWidget(m_word);
        options->addStretch();

        m_results = new QTreeWidget(this);
        m_results->setHeaderLabels({"文件", "行", "内容"});
        m_results->setRootIsDecorated(false);
        m_results->setUniformRowHeights(true);
        m_results->setAlternatingRowColors(true);
        m_results->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
        m_results->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
        m_results->header()->setStretchLastSection(true);

        m_status = new QLabel("输入内容后搜索当前工程", this);

        auto * layout = new QVBoxLayout(this);
        layout->setContentsMargins(6, 4, 6, 4);
        layout->addLayout(top);
        layout->addLayout(options);
        layout->addWidget(m_results, 1);
        layout->addWidget(m_status);

        connect(searchButton, &QPushButton::clicked, this, &SearchPanel::runSearch);
        connect(m_input, &QLineEdit::returnPressed, this, &SearchPanel::runSearch);
        connect(m_results, &QTreeWidget::itemDoubleClicked, this, [this](QTreeWidgetItem * item, int) {
            if(!item) return;
            emit openFileRequested(item->data(0, Qt::UserRole).toString(), item->data(1, Qt::UserRole).toInt());
        });
    }

    void setRootPath(const QString & path)
    {
        m_root = QFileInfo(path).absoluteFilePath();
    }

    void focusSearch()
    {
        m_input->setFocus();
        m_input->selectAll();
    }

  signals:
    void openFileRequested(const QString & path, int line);

  private:
    bool shouldSkip(const QString & path) const
    {
        const QString rel       = QDir(m_root).relativeFilePath(path);
        const QStringList parts = rel.split('/', QString::SkipEmptyParts);
        for(const QString & part : parts) {
            if(part == ".git" || part == "build" || part == "build_lvgl" || part == "build-win" ||
               part == "toolchain" || part == "ui_generated" || part == "dist")
                return true;
        }
        return false;
    }

    void runSearch()
    {
        m_results->clear();
        const QString query = m_input->text();
        if(query.isEmpty()) {
            m_status->setText("请输入搜索内容");
            return;
        }
        if(m_root.isEmpty()) {
            m_status->setText("没有打开的工程");
            return;
        }

        QRegularExpression expression;
        const bool useRegex = m_regex->isChecked() || m_word->isChecked();
        if(useRegex) {
            QString pattern = m_regex->isChecked() ? query : QRegularExpression::escape(query);
            if(m_word->isChecked()) pattern = "\\b(?:" + pattern + ")\\b";
            expression.setPattern(pattern);
            expression.setPatternOptions(m_case->isChecked() ? QRegularExpression::NoPatternOption
                                                             : QRegularExpression::CaseInsensitiveOption);
            if(!expression.isValid()) {
                m_status->setText("正则表达式无效");
                return;
            }
        }

        const QStringList filters{"*.h",    "*.hpp",  "*.hh",  "*.hxx", "*.c",     "*.cpp",
                                  "*.cc",   "*.cxx",  "*.inl", "*.ipp", "*.py",    "*.lui",
                                  "*.json", "*.lpro", "*.md",  "*.txt", "*.cmake", "CMakeLists.txt"};
        QDirIterator it(m_root, filters, QDir::Files, QDirIterator::Subdirectories);
        int matches = 0;
        m_status->setText("搜索中...");
        while(it.hasNext() && matches < 2000) {
            const QString path = it.next();
            if(shouldSkip(path)) continue;
            QFile file(path);
            if(!file.open(QIODevice::ReadOnly)) continue;
            if(file.size() > 4 * 1024 * 1024) continue;
            const QByteArray bytes = file.readAll();
            if(bytes.contains('\0')) continue;
            const QStringList lines = QString::fromUtf8(bytes).split('\n');
            for(int line = 0; line < lines.size() && matches < 2000; ++line) {
                const QString & text = lines.at(line);
                bool hit             = false;
                if(useRegex) {
                    hit = expression.match(text).hasMatch();
                } else {
                    hit = text.contains(query, m_case->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive);
                }
                if(!hit) continue;
                QString display = text.trimmed();
                if(display.size() > 240) display = display.left(237) + "...";
                auto * item = new QTreeWidgetItem(m_results);
                item->setText(0, QDir(m_root).relativeFilePath(path));
                item->setText(1, QString::number(line + 1));
                item->setText(2, display);
                item->setData(0, Qt::UserRole, path);
                item->setData(1, Qt::UserRole, line + 1);
                ++matches;
            }
        }
        m_status->setText(QString("找到 %1 处%2").arg(matches).arg(matches >= 2000 ? "（已截断）" : ""));
        if(matches > 0) m_results->setCurrentItem(m_results->topLevelItem(0));
    }

    QString m_root;
    QLineEdit * m_input     = nullptr;
    QCheckBox * m_case      = nullptr;
    QCheckBox * m_regex     = nullptr;
    QCheckBox * m_word      = nullptr;
    QTreeWidget * m_results = nullptr;
    QLabel * m_status       = nullptr;
};

} // namespace designer
