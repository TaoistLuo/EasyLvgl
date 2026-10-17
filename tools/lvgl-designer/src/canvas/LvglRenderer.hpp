#pragma once

#include <lvgl.h>
#include <string>
#include <vector>
#include <map>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include "model/LuiDocument.hpp"

namespace designer {

struct PendingEvent {
    enum Type { Click, Moved } type;
    std::string id;
    int x = 0, y = 0, w = 0, h = 0;
};

class LvglRenderer {
    std::map<std::string, lv_obj_t*> m_objects;
    std::map<lv_obj_t*, std::string> m_idMap;
    std::map<std::string, std::string> m_types;
    std::function<void(const std::string&)> m_onClick;
    std::function<void(const std::string& id, int x, int y, int w, int h)> m_onMoved;
    lv_obj_t* m_selectedObj = nullptr;
    std::vector<lv_obj_t*> m_multiSelectedObjs;

    static constexpr int HANDLE_SIZE = 12;
    static constexpr int MIN_WIDGET_SIZE = 20;

    lv_obj_t* m_handles[8] = {};
    bool m_handleDragging = false;
    int m_handleIndex = -1;
    lv_coord_t m_widgetOrigX = 0;
    lv_coord_t m_widgetOrigY = 0;
    lv_coord_t m_widgetOrigW = 0;
    lv_coord_t m_widgetOrigH = 0;
    lv_coord_t m_mouseStartX = 0;
    lv_coord_t m_mouseStartY = 0;

    bool m_widgetMoving = false;
    bool m_widgetMoved = false;
    lv_coord_t m_widgetOrigX2 = 0;
    lv_coord_t m_widgetOrigY2 = 0;
    lv_coord_t m_mouseStartX2 = 0;
    lv_coord_t m_mouseStartY2 = 0;

    std::vector<PendingEvent> m_pendingEvents;
    std::mutex m_pendingMutex;

    // StackedLayout navigation arrows
    struct StackedNav {
        lv_obj_t* navBar = nullptr;
        lv_obj_t* leftBtn = nullptr;
        lv_obj_t* rightBtn = nullptr;
        lv_obj_t* indexLabel = nullptr;
        int currentIndex = 0;
        int childCount = 0;
    };
    std::map<std::string, StackedNav> m_stackedNavs;

    std::deque<std::string> m_strStore;
    std::deque<std::vector<const char*>> m_mapStore;
    std::deque<std::vector<lv_point_precise_t>> m_ptsStore;

public:
    static constexpr const char* kRootId = "@root";

    const char* intern(const std::string& s) {
        m_strStore.push_back(s);
        return m_strStore.back().c_str();
    }

    std::vector<const char*>& storeMap() {
        m_mapStore.emplace_back();
        return m_mapStore.back();
    }

    std::vector<lv_point_precise_t>& storePoints() {
        m_ptsStore.emplace_back();
        return m_ptsStore.back();
    }

    void buildTree(const LuiDocument& doc);
    void clear();
    void pollEvents();

    lv_obj_t* findObject(const std::string& id) const;
    /** 读取对象当前实际生效的样式值（用于属性面板显示真值） */
    QVariant styleValue(const std::string& id, const std::string& prop) const;
    std::string findId(lv_obj_t* obj) const;
    std::string findContainerAt(int x, int y) const;
    std::string objectType(const std::string& id) const;
    void screenToRelative(const std::string& parentId, int& x, int& y) const;

    void setOnClick(std::function<void(const std::string&)> cb) { m_onClick = std::move(cb); }
    void setOnMoved(std::function<void(const std::string& id, int x, int y, int w, int h)> cb) { m_onMoved = std::move(cb); }
    void selectObject(const std::string& id);
    void selectMultiple(const std::vector<std::string>& ids);
    void addToSelection(const std::string& id);
    void removeFromSelection(const std::string& id);
    bool isSelected(const std::string& id) const;
    void deselectAll();
    void clearMultiSelection();
    std::vector<std::vector<lv_coord_t>> m_gridDescArrays;

private:
    lv_obj_t* createWidget(const WidgetNode& node, lv_obj_t* parent);
    void applyProps(lv_obj_t* obj, const WidgetNode& node);
    void registerObject(const std::string& id, const std::string& type, lv_obj_t* obj);
    void createHandles(lv_obj_t* widget);
    void destroyHandles();
    void updateHandlePositions();
    bool isLayoutChild(lv_obj_t* obj) const;
    void applyStackedVisibility(lv_obj_t* stackedObj, int currentIndex);
    void createStackedNav(lv_obj_t* stackedObj, const std::string& id, int currentIndex);
    void updateStackedNav(const std::string& id);

};

}
