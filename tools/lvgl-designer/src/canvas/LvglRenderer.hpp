/**************************************************************************************************
 * @file        LvglRenderer.hpp
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
    /* "#别名" -> 预览用绝对路径（编译期嵌入由 luic 负责，设计器只做预览） */
    std::map<std::string, std::string> m_resAbs;
    /* 组件实例预览：名字 -> 组件文档（由工程提供） */
    std::function<const LuiDocument*(const std::string&)> m_componentResolver;
    /* 构建单个实例时，内部子节点的 id -> 对象（用于应用导出参数） */
    std::map<std::string, lv_obj_t*> m_instanceObjects;
    int m_instanceDepth = 0;   // 组件实例递归深度保护
    std::function<void(const std::string&)> m_onClick;
    std::function<void(const std::string& id, int x, int y, int w, int h)> m_onMoved;
    lv_obj_t* m_selectedObj = nullptr;
    std::vector<lv_obj_t*> m_multiSelectedObjs;
    /* 多选时给每个附加选中对象画 4 个角标记（主选中对象仍有 8 个手柄） */
    std::vector<lv_obj_t*> m_multiHandles;
    /* 多选拖动：记录每个选中对象的起始位置，按同一增量移动 */
    std::vector<lv_obj_t*> m_multiMoveObjs;
    std::vector<lv_point_t> m_multiMoveOrig;

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
    /** 坐标处最上层的"已注册控件" id（跳过手柄等辅助对象）；没有则返回空 */
    std::string findIdAt(int x, int y) const;
    std::string findContainerAt(int x, int y) const;
    std::string objectType(const std::string& id) const;
    void screenToRelative(const std::string& parentId, int& x, int& y) const;

    void setComponentResolver(std::function<const LuiDocument*(const std::string&)> r) {
        m_componentResolver = std::move(r);
    }
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
    lv_obj_t* createWidget(const WidgetNode& node, lv_obj_t* parent, bool selectable = true);
    void applyInstanceExports(lv_obj_t* instanceRoot, const WidgetNode& node,
                              const LuiDocument& componentDoc);
    void applyProps(lv_obj_t* obj, const WidgetNode& node);
    void registerObject(const std::string& id, const std::string& type, lv_obj_t* obj);
    void createHandles(lv_obj_t* widget);
    void drawMultiHandles(lv_obj_t* obj);
    void clearMultiHandles();
    void destroyHandles();
    void updateHandlePositions();
    bool isLayoutChild(lv_obj_t* obj) const;
    void applyStackedVisibility(lv_obj_t* stackedObj, int currentIndex);
    void createStackedNav(lv_obj_t* stackedObj, const std::string& id, int currentIndex);
    void updateStackedNav(const std::string& id);

};

}
