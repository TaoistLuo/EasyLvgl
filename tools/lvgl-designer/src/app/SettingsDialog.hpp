#pragma once
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QFileDialog>
#include <QSettings>
#include <QHBoxLayout>
#include <QCoreApplication>
#include <QFileInfo>

namespace designer {

/** 构建配置：luic 工具、CMake、构建目录、生成器、额外参数（存 QSettings） */
class SettingsDialog : public QDialog {
public:
    explicit SettingsDialog(QWidget* parent = nullptr) : QDialog(parent) {
        setWindowTitle("设置");
        setMinimumWidth(560);

        m_luic = new QLineEdit;      m_cmake = new QLineEdit;
        m_build = new QLineEdit;     m_generator = new QComboBox;
        m_extra = new QLineEdit;

        m_generator->addItems({"Ninja", "Unix Makefiles", "Ninja Multi-Config"});
        m_generator->setEditable(true);

        auto* form = new QFormLayout(this);
        form->addRow("luic 路径", withBrowse(m_luic, true));
        form->addRow("CMake 路径", withBrowse(m_cmake, true));
        form->addRow("构建目录", withBrowse(m_build, false));
        form->addRow("生成器", m_generator);
        form->addRow("额外 CMake 参数", m_extra);

        auto* box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        form->addRow(box);
        connect(box, &QDialogButtonBox::accepted, this, &QDialog::accept);
        connect(box, &QDialogButtonBox::rejected, this, &QDialog::reject);
        load();
    }

    /* 读/写配置（QSettings 已按应用名+组织名落到用户目录） */
    static QString luicPath();
    static QString cmakePath();
    static QString buildDir();
    static QString generator();
    static QString extraArgs();

    void load() {
        m_luic->setText(luicPath());
        m_cmake->setText(cmakePath());
        m_build->setText(buildDir());
        m_generator->setCurrentText(generator());
        m_extra->setText(extraArgs());
    }

    void save() const {
        QSettings s;
        s.setValue("build/luic", m_luic->text());
        s.setValue("build/cmake", m_cmake->text());
        s.setValue("build/buildDir", m_build->text());
        s.setValue("build/generator", m_generator->currentText());
        s.setValue("build/extraArgs", m_extra->text());
    }

    void accept() override { save(); QDialog::accept(); }

private:
    QWidget* withBrowse(QLineEdit* edit, bool file) {
        auto* w = new QWidget;
        auto* h = new QHBoxLayout(w);
        h->setContentsMargins(0, 0, 0, 0);
        h->addWidget(edit, 1);
        auto* btn = new QPushButton("…");
        btn->setFixedWidth(28);
        h->addWidget(btn);
        connect(btn, &QPushButton::clicked, this, [this, edit, file] {
            QString p = file ? QFileDialog::getOpenFileName(this, "选择", edit->text())
                             : QFileDialog::getExistingDirectory(this, "选择", edit->text());
            if (!p.isEmpty()) edit->setText(p);
        });
        return w;
    }

    QLineEdit* m_luic; QLineEdit* m_cmake; QLineEdit* m_build;
    QComboBox* m_generator; QLineEdit* m_extra;
};

inline QString SettingsDialog::luicPath() {
    QString def = QCoreApplication::applicationDirPath() + "/luic";
    if (!QFileInfo::exists(def)) def = QCoreApplication::applicationDirPath() + "/../bin/luic";
    return QSettings().value("build/luic", def).toString();
}
inline QString SettingsDialog::cmakePath() { return QSettings().value("build/cmake", "cmake").toString(); }
inline QString SettingsDialog::buildDir() { return QSettings().value("build/buildDir", QString()).toString(); }
inline QString SettingsDialog::generator() { return QSettings().value("build/generator", "Unix Makefiles").toString(); }
inline QString SettingsDialog::extraArgs() { return QSettings().value("build/extraArgs", QString()).toString(); }

} // namespace designer
