#pragma once
#include <QTreeWidget>
#include <QDrag>
#include <QMimeData>
#include <QHeaderView>
#include <string>
#include <vector>
#include <utility>

namespace designer {

class ComponentPanel : public QTreeWidget {
    Q_OBJECT
    QTreeWidgetItem* m_projectGroup = nullptr;
public:
    explicit ComponentPanel(QWidget* parent = nullptr) : QTreeWidget(parent) {
        setDragEnabled(true);
        setHeaderHidden(true);
        setIndentation(16);
        setIconSize(QSize(16, 16));

        struct CompDef { const char* type; const char* label; };

        auto addGroup = [this](const char* name, std::initializer_list<CompDef> items) {
            auto* group = new QTreeWidgetItem(this);
            group->setText(0, name);
            group->setFlags(Qt::ItemIsEnabled);
            QFont f = group->font(0);
            f.setBold(true);
            group->setFont(0, f);
            for (auto& c : items) {
                auto* item = new QTreeWidgetItem(group);
                item->setText(0, QString("%1(%2)")
                                     .arg(QString::fromUtf8(c.label),
                                          QString::fromUtf8(c.type)));
                item->setData(0, Qt::UserRole, c.type);
                item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);
            }
            group->setExpanded(true);
        };

        addGroup("布局", {
            {"LVBoxLayout",    "垂直布局"},
            {"LHBoxLayout",    "水平布局"},
            {"LGridLayout",    "网格布局"},
            {"LStackedLayout", "堆叠布局"},
        });

        addGroup("按钮", {
            {"LButton",        "按钮"},
            {"LImageButton",   "图片按钮"},
            {"LCheckbox",      "复选框"},
            {"LRadioButton",   "单选按钮"},
            {"LSwitch",        "开关"},
            {"LButtonMatrix",  "按钮矩阵"},
        });

        addGroup("容器", {
            {"LGroupBox",      "分组框"},
            {"LList",          "列表"},
            {"LTabView",       "标签页"},
            {"LMenu",          "菜单"},
            {"LMenuPage",      "菜单页"},
            {"LMenuCont",      "菜单容器"},
            {"LMenuSection",   "菜单分区"},
            {"LTileView",      "平铺视图"},
            {"LTile",          "平铺块"},
            {"LWin",           "窗口"},
            {"LCanvas",        "画布"},
        });

        addGroup("输入", {
            {"LLineEdit",      "单行输入"},
            {"LTextEdit",      "多行输入"},
            {"LComboBox",      "下拉框"},
            {"LSlider",        "滑块"},
            {"LSpinbox",       "数字输入框"},
            {"LRoller",        "滚轮选择"},
            {"LKeyboard",      "虚拟键盘"},
        });

        addGroup("显示", {
            {"LLabel",         "标签"},
            {"LImage",         "图片"},
            {"LBar",           "进度条"},
            {"LArc",           "圆弧"},
            {"LSpinner",       "加载动画"},
            {"LLed",           "指示灯"},
            {"LLine",          "线条"},
            {"LScale",         "刻度尺"},
            {"LSpan",          "富文本片段"},
        });

        addGroup("视图 / 数据", {
            {"LTable",         "表格"},
            {"LChart",         "图表"},
            {"LCalendar",      "日历"},
        });

        addGroup("对话框", {
            {"LMsgBox",        "消息框"},
        });

        addGroup("动画", {
            {"LAnimImage",     "动画图片"},
            {"LLottie",        "Lottie 动画"},
        });
    }

public:
    /* 工程里的自定义组件（"项目组件"分组，拖动即插入 "@Name" 实例） */
    void setProjectComponents(const std::vector<std::string>& comps) {
        if (m_projectGroup) {
            delete m_projectGroup;
            m_projectGroup = nullptr;
        }
        if (comps.empty()) return;
        m_projectGroup = new QTreeWidgetItem();
        insertTopLevelItem(0, m_projectGroup);
        m_projectGroup->setText(0, "项目组件");
        m_projectGroup->setFlags(Qt::ItemIsEnabled);
        QFont f = m_projectGroup->font(0);
        f.setBold(true);
        m_projectGroup->setFont(0, f);
        for (auto& c : comps) {
            auto* item = new QTreeWidgetItem(m_projectGroup);
            item->setText(0, QString("%1(@%2)")
                                 .arg(QString::fromStdString(c), QString::fromStdString(c)));
            item->setData(0, Qt::UserRole, QString::fromStdString("@" + c));
            item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);
        }
        m_projectGroup->setExpanded(true);
    }

protected:
    void startDrag(Qt::DropActions) override {
        auto* item = currentItem();
        if (!item || item->data(0, Qt::UserRole).isNull()) return;

        auto* drag = new QDrag(this);
        auto* mime = new QMimeData();
        mime->setData("application/x-lvgl-widget",
                      item->data(0, Qt::UserRole).toString().toUtf8());
        drag->setMimeData(mime);
        drag->exec(Qt::CopyAction);
    }
};

}
