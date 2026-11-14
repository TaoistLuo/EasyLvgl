#pragma once

#include <QDialog>
#include <QString>
#include <QStringList>

class QComboBox;
class QLineEdit;
class QTableWidget;

class ProjectSettingsDialog : public QDialog {
    Q_OBJECT
public:
    ProjectSettingsDialog(const QString& manifest, const QStringList& screens,
                          QWidget* parent = nullptr);

    bool apply();

private:
    void browseFont();
    void addExternalRow(const QString& name = QString(),
                        const QString& include = QString(),
                        const QString& cls = QString());

    QString m_manifest;
    QLineEdit* m_name = nullptr;
    QComboBox* m_entry = nullptr;
    QLineEdit* m_font = nullptr;
    QLineEdit* m_outDir = nullptr;
    QTableWidget* m_external = nullptr;
};
