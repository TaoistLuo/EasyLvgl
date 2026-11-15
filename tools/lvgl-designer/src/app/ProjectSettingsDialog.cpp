#include "app/ProjectSettingsDialog.hpp"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRegularExpression>
#include <QSaveFile>
#include <QTableWidget>
#include <QVBoxLayout>

ProjectSettingsDialog::ProjectSettingsDialog(const QString& manifest,
                                             const QStringList& screens,
                                             QWidget* parent)
    : QDialog(parent), m_manifest(manifest) {
    setWindowTitle("工程设置");
    resize(680, 520);

    QJsonObject root;
    QFile file(manifest);
    if (file.open(QIODevice::ReadOnly)) {
        root = QJsonDocument::fromJson(file.readAll()).object();
        file.close();
    }

    m_name = new QLineEdit(root["name"].toString("Project"), this);
    m_target = new QLineEdit(
        root["target"].toString(root["name"].toString("Project")), this);
    m_entry = new QComboBox(this);
    m_entry->setEditable(true);
    for (const QString& screen : screens) m_entry->addItem(screen);
    m_entry->setCurrentText(root["entry"].toString());
    m_font = new QLineEdit(root["font"].toString(), this);
    m_buildDir = new QLineEdit(root["buildDir"].toString("build_lvgl"), this);
    m_outDir = new QLineEdit(root["outDir"].toString("ui_generated"), this);

    auto* form = new QFormLayout();
    form->addRow("工程名称", m_name);
    form->addRow("可执行目标", m_target);
    form->addRow("入口屏幕", m_entry);

    auto* buildRow = new QHBoxLayout();
    buildRow->addWidget(m_buildDir, 1);
    auto* buildBrowse = new QPushButton("浏览...", this);
    buildRow->addWidget(buildBrowse);
    form->addRow("编译输出目录", buildRow);
    connect(buildBrowse, &QPushButton::clicked, this,
            &ProjectSettingsDialog::browseBuildDir);

    auto* fontRow = new QHBoxLayout();
    fontRow->addWidget(m_font, 1);
    auto* browse = new QPushButton("浏览...", this);
    fontRow->addWidget(browse);
    form->addRow("项目字体", fontRow);

    form->addRow("UI 输出目录", m_outDir);

    m_external = new QTableWidget(0, 3, this);
    m_external->setHorizontalHeaderLabels({"名称", "include", "class"});
    m_external->horizontalHeader()->setStretchLastSection(true);
    m_external->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_external->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_external->verticalHeader()->setVisible(false);

    const QJsonObject external = root["externalComponents"].toObject();
    for (auto it = external.begin(); it != external.end(); ++it) {
        const QJsonObject item = it.value().toObject();
        addExternalRow(it.key(), item["include"].toString(), item["class"].toString());
    }

    auto* extButtons = new QHBoxLayout();
    auto* add = new QPushButton("添加", this);
    auto* remove = new QPushButton("删除", this);
    extButtons->addWidget(add);
    extButtons->addWidget(remove);
    extButtons->addStretch();

    auto* extBox = new QGroupBox("外部 C++ 组件", this);
    auto* extLayout = new QVBoxLayout(extBox);
    extLayout->addWidget(m_external);
    extLayout->addLayout(extButtons);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    auto* layout = new QVBoxLayout(this);
    layout->addLayout(form);
    layout->addWidget(extBox, 1);
    layout->addWidget(buttons);

    connect(browse, &QPushButton::clicked, this, &ProjectSettingsDialog::browseFont);
    connect(add, &QPushButton::clicked, this, [this] {
        addExternalRow();
        m_external->editItem(m_external->item(m_external->rowCount() - 1, 0));
    });
    connect(remove, &QPushButton::clicked, this, [this] {
        const int row = m_external->currentRow();
        if (row >= 0) m_external->removeRow(row);
    });
    connect(buttons, &QDialogButtonBox::accepted, this, [this] {
        if (apply()) accept();
    });
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void ProjectSettingsDialog::addExternalRow(const QString& name,
                                           const QString& include,
                                           const QString& cls) {
    const int row = m_external->rowCount();
    m_external->insertRow(row);
    m_external->setItem(row, 0, new QTableWidgetItem(name));
    m_external->setItem(row, 1, new QTableWidgetItem(include));
    m_external->setItem(row, 2, new QTableWidgetItem(cls));
}

void ProjectSettingsDialog::browseFont() {
    const QString projectDir = QFileInfo(m_manifest).absolutePath();
    const QString path = QFileDialog::getOpenFileName(
        this, "选择项目字体", projectDir, "字体 (*.ttf);;All Files (*)");
    if (path.isEmpty()) return;

    QString target = path;
    if (!QFileInfo(path).absoluteFilePath().startsWith(projectDir + "/")) {
        const QString fontDir = projectDir + "/fonts";
        QDir().mkpath(fontDir);
        target = fontDir + "/" + QFileInfo(path).fileName();
        QFile::remove(target);
        if (!QFile::copy(path, target)) {
            QMessageBox::critical(this, "项目字体", "复制字体失败：\n" + target);
            return;
        }
    }
    m_font->setText(QDir(projectDir).relativeFilePath(target));
}

void ProjectSettingsDialog::browseBuildDir() {
    const QString projectDir = QFileInfo(m_manifest).absolutePath();
    QString start = m_buildDir->text().trimmed();
    if (start.isEmpty() || QDir::isRelativePath(start))
        start = QDir(projectDir).absoluteFilePath(start);
    if (!QFileInfo(start).isDir()) start = projectDir;
    const QString path =
        QFileDialog::getExistingDirectory(this, "选择编译输出目录", start);
    if (path.isEmpty()) return;
    const QString abs = QFileInfo(path).absoluteFilePath();
    if (abs == projectDir || abs.startsWith(projectDir + "/"))
        m_buildDir->setText(QDir(projectDir).relativeFilePath(abs));
    else
        m_buildDir->setText(abs);
}

bool ProjectSettingsDialog::apply() {
    static const QRegularExpression ident("^[A-Za-z_][A-Za-z0-9_]*$");

    QJsonObject root;
    QFile file(m_manifest);
    if (file.open(QIODevice::ReadOnly)) {
        root = QJsonDocument::fromJson(file.readAll()).object();
        file.close();
    }

    const QString name = m_name->text().trimmed();
    if (name.isEmpty()) {
        QMessageBox::warning(this, "工程设置", "工程名称不能为空。");
        return false;
    }
    root["name"] = name;

    const QString target =
        m_target->text().trimmed().isEmpty() ? name : m_target->text().trimmed();
    if (!ident.match(target).hasMatch()) {
        QMessageBox::warning(
            this, "工程设置",
            "可执行目标只能包含字母、数字、下划线，且不能以数字开头。");
        return false;
    }
    root["target"] = target;

    const QString buildDir = m_buildDir->text().trimmed();
    root["buildDir"] = buildDir.isEmpty() ? "build_lvgl" : buildDir;

    const QString entry = m_entry->currentText().trimmed();
    if (!entry.isEmpty()) root["entry"] = entry;

    const QString font = m_font->text().trimmed();
    if (font.isEmpty()) root.remove("font");
    else root["font"] = font;

    const QString outDir = m_outDir->text().trimmed();
    if (outDir.isEmpty()) root.remove("outDir");
    else root["outDir"] = outDir;

    QJsonObject external;
    for (int row = 0; row < m_external->rowCount(); ++row) {
        auto text = [this, row](int column) {
            auto* item = m_external->item(row, column);
            return item ? item->text().trimmed() : QString();
        };
        const QString key = text(0);
        if (key.isEmpty()) continue;
        if (!ident.match(key).hasMatch()) {
            QMessageBox::warning(this, "工程设置",
                                 QString("外部组件名称不合法：%1").arg(key));
            return false;
        }
        const QString include = text(1);
        const QString cls = text(2);
        if (include.isEmpty() || cls.isEmpty()) {
            QMessageBox::warning(this, "工程设置",
                                 QString("外部组件 %1 需要 include 和 class。").arg(key));
            return false;
        }
        external[key] = QJsonObject{{"include", include}, {"class", cls}};
    }
    if (external.isEmpty()) root.remove("externalComponents");
    else root["externalComponents"] = external;

    QSaveFile out(m_manifest);
    if (!out.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, "工程设置", "无法写入工程清单。");
        return false;
    }
    out.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    if (!out.commit()) {
        QMessageBox::critical(this, "工程设置", "保存工程清单失败。");
        return false;
    }
    return true;
}
