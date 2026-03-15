import * as vscode from 'vscode';

export class LuiEditorProvider implements vscode.CustomTextEditorProvider {

    public static register(context: vscode.ExtensionContext): vscode.Disposable {
        return vscode.window.registerCustomEditorProvider(
            'lvglDesigner.luiEditor',
            new LuiEditorProvider(context),
            { webviewOptions: { retainContextWhenHidden: true } }
        );
    }

    constructor(private readonly context: vscode.ExtensionContext) {}

    public async resolveCustomTextEditor(
        document: vscode.TextDocument,
        webviewPanel: vscode.WebviewPanel,
        _token: vscode.CancellationToken
    ): Promise<void> {
        webviewPanel.webview.options = { enableScripts: true };
        webviewPanel.webview.html = this.getHtml(webviewPanel.webview);

        const updateWebview = () => {
            try {
                const json = JSON.parse(document.getText());
                webviewPanel.webview.postMessage({ type: 'update', data: json });
            } catch {}
        };

        const changeDocSub = vscode.workspace.onDidChangeTextDocument(e => {
            if (e.document.uri.toString() === document.uri.toString()) {
                updateWebview();
            }
        });

        webviewPanel.onDidDispose(() => changeDocSub.dispose());

        webviewPanel.webview.onDidReceiveMessage(async msg => {
            if (msg.type === 'save' || msg.type === 'compile') {
                const edit = new vscode.WorkspaceEdit();
                edit.replace(
                    document.uri,
                    new vscode.Range(0, 0, document.lineCount, 0),
                    JSON.stringify(msg.data, null, 2)
                );
                await vscode.workspace.applyEdit(edit);
                await document.save();

                if (msg.type === 'compile') {
                    await vscode.commands.executeCommand('lvglDesigner.compile');
                }
                return;
            }
        });

        updateWebview();
    }

    private getHtml(webview: vscode.Webview): string {
        return `<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<style>
* { margin: 0; padding: 0; box-sizing: border-box; }
body { display: flex; height: 100vh; font-family: -apple-system, BlinkMacSystemFont, sans-serif; background: #1e1e1e; color: #ccc; }

#component-panel { width: 200px; background: #252526; border-right: 1px solid #3c3c3c; padding: 10px; overflow-y: auto; }
#component-panel h3 { color: #569cd6; margin-bottom: 10px; font-size: 13px; }
.comp-item { padding: 6px 10px; margin: 2px 0; background: #2d2d2d; border: 1px solid #3c3c3c; border-radius: 4px; cursor: grab; font-size: 12px; }
.comp-item:hover { background: #37373d; border-color: #569cd6; }
.comp-item:active { cursor: grabbing; }

#canvas-area { flex: 1; display: flex; flex-direction: column; }
#toolbar { height: 36px; background: #333; display: flex; align-items: center; padding: 0 10px; gap: 8px; border-bottom: 1px solid #3c3c3c; }
#toolbar button { background: #0e639c; color: white; border: none; padding: 4px 12px; border-radius: 3px; cursor: pointer; font-size: 12px; }
#toolbar button:hover { background: #1177bb; }
#canvas-wrapper { flex: 1; overflow: auto; display: flex; justify-content: center; align-items: center; background: #1a1a1a; }
#canvas { background: #2d2d2d; border: 2px solid #569cd6; position: relative; overflow: hidden; }
.widget { position: absolute; border: 1px solid #555; background: #3c3c3c; display: flex; align-items: center; justify-content: center; font-size: 11px; cursor: move; user-select: none; border-radius: 3px; }
.widget:hover { border-color: #569cd6; }
.widget.selected { border-color: #569cd6; border-width: 2px; box-shadow: 0 0 0 1px #569cd6; }
.widget-label { color: #9cdcfe; }
.widget-button { background: #0e639c; color: white; border-radius: 4px; }
.widget-slider { background: #444; border-radius: 5px; }
.widget-slider::after { content: ''; position: absolute; left: 0; top: 0; width: 60%; height: 100%; background: #0e639c; border-radius: 5px; }
.widget-checkbox { background: transparent; border: none; }
.widget-bar { background: #444; border-radius: 3px; }
.widget-bar::after { content: ''; position: absolute; left: 0; top: 0; width: 50%; height: 100%; background: #4caf50; border-radius: 3px; }

#props-panel { width: 260px; background: #252526; border-left: 1px solid #3c3c3c; padding: 10px; overflow-y: auto; }
#props-panel h3 { color: #569cd6; margin-bottom: 10px; font-size: 13px; }
.prop-row { margin-bottom: 8px; }
.prop-row label { display: block; font-size: 11px; color: #888; margin-bottom: 2px; }
.prop-row input, .prop-row select { width: 100%; background: #3c3c3c; border: 1px solid #555; color: #ccc; padding: 4px 6px; border-radius: 3px; font-size: 12px; }
.prop-row input:focus { border-color: #569cd6; outline: none; }

#code-preview { height: 200px; background: #1e1e1e; border-top: 1px solid #3c3c3c; padding: 10px; overflow: auto; font-family: 'Consolas', monospace; font-size: 11px; color: #d4d4d4; white-space: pre; }
#canvas-error { position: absolute; left: 12px; top: 12px; right: 12px; padding: 8px 10px; background: rgba(95, 32, 32, 0.92); border: 1px solid #c77474; color: #f3d6d6; font-size: 12px; border-radius: 4px; display: none; z-index: 20; }
</style>
</head>
<body>

<div id="component-panel">
    <h3>Components</h3>
    <div class="comp-item" draggable="true" data-type="LLabel">📝 LLabel</div>
    <div class="comp-item" draggable="true" data-type="LButton">🔘 LButton</div>
    <div class="comp-item" draggable="true" data-type="LSlider">🎚️ LSlider</div>
    <div class="comp-item" draggable="true" data-type="LBar">📊 LBar</div>
    <div class="comp-item" draggable="true" data-type="LCheckbox">☑️ LCheckbox</div>
    <div class="comp-item" draggable="true" data-type="LSwitch">🔀 LSwitch</div>
    <div class="comp-item" draggable="true" data-type="LDropdown">📋 LDropdown</div>
    <div class="comp-item" draggable="true" data-type="LTextArea">📄 LTextArea</div>
    <div class="comp-item" draggable="true" data-type="LArc">⭕ LArc</div>
    <div class="comp-item" draggable="true" data-type="LSpinner">🔄 LSpinner</div>
    <div class="comp-item" draggable="true" data-type="LLed">💡 LLed</div>
    <div class="comp-item" draggable="true" data-type="LVStack">📦 LVStack</div>
    <div class="comp-item" draggable="true" data-type="LHStack">📦 LHStack</div>
    <div class="comp-item" draggable="true" data-type="LTable">📋 LTable</div>
    <div class="comp-item" draggable="true" data-type="LChart">📈 LChart</div>
    <div class="comp-item" draggable="true" data-type="LImage">🖼️ LImage</div>
</div>

<div id="canvas-area">
    <div id="toolbar">
        <button onclick="saveDoc()">💾 Save</button>
        <button onclick="compileLui()">⚡ Compile</button>
        <button onclick="deleteSelected()">🗑️ Delete</button>
        <span style="color:#888;font-size:12px" id="status">Ready</span>
    </div>
    <div id="canvas-wrapper">
        <div id="canvas">
            <div id="canvas-error"></div>
        </div>
    </div>
    <div id="code-preview"></div>
</div>

<div id="props-panel">
    <h3>Properties</h3>
    <div id="props-content"><p style="color:#666;font-size:12px">Select a widget</p></div>
</div>

<script>
const vscode = acquireVsCodeApi();
let doc = { version: "1.0", name: "Unnamed", size: [800, 480], children: [] };
let selectedIdx = -1;
let dragType = null;
let autoIdCounter = 0;

function setStatus(message, isError = false) {
    const status = document.getElementById('status');
    status.textContent = message;
    status.style.color = isError ? '#f48771' : '#888';
}

function showCanvasError(message) {
    const banner = document.getElementById('canvas-error');
    if (!banner) return;
    banner.textContent = message || '';
    banner.style.display = message ? 'block' : 'none';
}

function getWidgetProps(w) {
    if (!w.props || typeof w.props !== 'object' || Array.isArray(w.props)) {
        w.props = {};
    }
    return w.props;
}

function ensureWidgetShape(widget, idx) {
    const safeWidget = widget && typeof widget === 'object' ? widget : {};
    const normalized = {
        ...safeWidget,
        type: typeof safeWidget.type === 'string' && safeWidget.type ? safeWidget.type : 'LLabel',
        id: typeof safeWidget.id === 'string' && safeWidget.id ? safeWidget.id : 'widget_' + idx,
        props: safeWidget.props && typeof safeWidget.props === 'object' && !Array.isArray(safeWidget.props) ? { ...safeWidget.props } : {},
        events: Array.isArray(safeWidget.events) ? [...safeWidget.events] : [],
        children: Array.isArray(safeWidget.children) ? safeWidget.children : [],
    };
    normalizeWidgetProps(normalized);
    return normalized;
}

function normalizeDocument(rawDoc) {
    const input = rawDoc && typeof rawDoc === 'object' ? rawDoc : {};
    const size = Array.isArray(input.size) ? input.size : [800, 480];
    return {
        version: typeof input.version === 'string' ? input.version : '1.0',
        name: typeof input.name === 'string' && input.name ? input.name : 'Unnamed',
        size: [Math.max(1, parseIntValue(size[0], 800)), Math.max(1, parseIntValue(size[1], 480))],
        children: Array.isArray(input.children) ? input.children.map((child, idx) => ensureWidgetShape(child, idx)) : [],
    };
}

const PROP_TYPES = {
    LButton:   { text: 'string', size: 'int_pair', pos: 'int_pair', theme: 'string' },
    LLabel:    { text: 'string', color: 'hex', font_size: 'int', pos: 'int_pair' },
    LSlider:   { range: 'int_pair', value: 'int', size: 'int_pair', pos: 'int_pair' },
    LBar:      { range: 'int_pair', value: 'int', size: 'int_pair', pos: 'int_pair' },
    LCheckbox: { text: 'string', checked: 'bool', pos: 'int_pair' },
    LSwitch:   { checked: 'bool', pos: 'int_pair' },
    LDropdown: { options: 'string', selected: 'int', size: 'int_pair', pos: 'int_pair' },
    LTextArea: { text: 'string', placeholder: 'string', one_line: 'bool', size: 'int_pair', pos: 'int_pair' },
    LArc:      { range: 'int_pair', value: 'int', rotation: 'int', size: 'int', pos: 'int_pair' },
    LSpinner:  { size: 'int', pos: 'int_pair' },
    LLed:      { color: 'hex', brightness: 'int', on: 'bool', size: 'int', pos: 'int_pair' },
    LVStack:   { spacing: 'int', padding: 'int', size: 'int_pair', pos: 'int_pair' },
    LHStack:   { spacing: 'int', padding: 'int', size: 'int_pair', pos: 'int_pair' },
    LTable:    { row_count: 'int', column_count: 'int', size: 'int_pair', pos: 'int_pair' },
    LChart:    { point_count: 'int', size: 'int_pair', pos: 'int_pair' },
    LImage:    { src: 'string', size: 'int_pair', pos: 'int_pair' },
};

function getPropType(widgetType, propName) {
    return (PROP_TYPES[widgetType] && PROP_TYPES[widgetType][propName]) || null;
}

function parseIntValue(val, fallback = 0) {
    const num = parseInt(String(val).trim(), 10);
    return Number.isNaN(num) ? fallback : num;
}

function parsePropValue(widgetType, key, val) {
    const type = getPropType(widgetType, key);
    if (type === 'int') return parseIntValue(val);
    if (type === 'int_pair') {
        const parts = String(val).split(',').map(v => parseIntValue(v.trim())).slice(0, 2);
        while (parts.length < 2) parts.push(0);
        return parts;
    }
    if (type === 'bool') return val === true || val === 'true';
    return val;
}

function getWidgetSizeValue(w) {
    const size = getWidgetProps(w).size;
    if (Array.isArray(size)) return size;
    if (typeof size === 'number') return [size, size];
    return [100, 40];
}

function normalizeWidgetProps(w) {
    if (!w) return;
    const props = getWidgetProps(w);
    Object.keys(props).forEach(key => {
        const type = getPropType(w.type, key);
        const value = props[key];
        if (type === 'int' && Array.isArray(value)) {
            props[key] = parseIntValue(value[0]);
        } else if (type === 'int_pair' && !Array.isArray(value) && value !== undefined) {
            const scalar = parseIntValue(value);
            props[key] = [scalar, scalar];
        }
    });
}

function formatCodeValue(widgetType, key, value) {
    const type = getPropType(widgetType, key);
    if (type === 'hex') return String(value);
    if (type === 'string') return JSON.stringify(String(value));
    if (type === 'int_pair' && Array.isArray(value)) return value.join(', ');
    return String(value);
}

const DEFAULTS = {
    LLabel:    { text: "Label", color: "0xFFFFFF", font_size: 14 },
    LButton:   { text: "Button", size: [120, 50], theme: "primary" },
    LSlider:   { range: [0, 100], value: 50, size: [200, 10] },
    LBar:      { range: [0, 100], value: 50, size: [200, 20] },
    LCheckbox: { text: "Checkbox", checked: false },
    LSwitch:   { checked: false },
    LDropdown: { options: "Option 1\\nOption 2\\nOption 3", size: [150, 40] },
    LTextArea: { placeholder: "Enter text...", size: [200, 80] },
    LArc:      { range: [0, 100], value: 75, size: 100 },
    LSpinner:  { size: 50 },
    LLed:      { color: "0x00FF00", size: 30 },
    LVStack:   { spacing: 10, padding: 15, size: [300, 400] },
    LHStack:   { spacing: 10, padding: 15, size: [400, 100] },
    LTable:    { row_count: 3, column_count: 3, size: [300, 200] },
    LChart:    { point_count: 10, size: [300, 200] },
    LImage:    { size: [100, 100] },
};

const EVENTS = {
    LButton: ["clicked"], LSlider: ["value_changed"], LCheckbox: ["value_changed"],
    LSwitch: ["value_changed"], LDropdown: ["value_changed"], LTextArea: ["value_changed"],
    LArc: ["value_changed"], LSpinbox: ["value_changed"],
};

function initCanvas() {
    const c = document.getElementById('canvas');
    c.style.width = doc.size[0] + 'px';
    c.style.height = doc.size[1] + 'px';
}

function render() {
    try {
        const c = document.getElementById('canvas');
        c.innerHTML = '<div id="canvas-error"></div>';
        showCanvasError('');
        doc.children.forEach((child, i) => renderWidget(c, child, i));
        updateCodePreview();
        setStatus('Ready');
    } catch (err) {
        const message = err && err.message ? err.message : String(err);
        showCanvasError('Render error: ' + message);
        setStatus('Render error', true);
    }
}

function renderWidget(parent, w, idx) {
    const el = document.createElement('div');
    el.className = 'widget' + (idx === selectedIdx ? ' selected' : '');
    normalizeWidgetProps(w);
    const props = getWidgetProps(w);
    const s = getWidgetSizeValue(w);
    const p = Array.isArray(props.pos) ? props.pos : [10 + idx * 20, 10 + idx * 30];
    el.style.left = p[0] + 'px';
    el.style.top = p[1] + 'px';
    el.style.width = s[0] + 'px';
    el.style.height = s[1] + 'px';

    if (w.type === 'LButton') el.className += ' widget-button';
    if (w.type === 'LSlider') el.className += ' widget-slider';
    if (w.type === 'LBar') el.className += ' widget-bar';
    if (w.type === 'LLabel') el.className += ' widget-label';

    el.textContent = props.text || w.type + ' (' + w.id + ')';
    el.onclick = (e) => { e.stopPropagation(); selectWidget(idx); };

    let dragging = false, startX, startY, origX, origY;
    el.onmousedown = (e) => {
        if (e.button !== 0) return;
        dragging = true; startX = e.clientX; startY = e.clientY;
        origX = p[0]; origY = p[1];
        selectWidget(idx);
        e.preventDefault();
    };
    document.onmousemove = (e) => {
        if (!dragging) return;
        const dx = e.clientX - startX, dy = e.clientY - startY;
        const nx = Math.max(0, origX + dx), ny = Math.max(0, origY + dy);
        const selectedWidget = doc.children[selectedIdx];
        if (!selectedWidget) return;
        getWidgetProps(selectedWidget).pos = [nx, ny];
        render();
    };
    document.onmouseup = () => { dragging = false; };

    parent.appendChild(el);
}

function selectWidget(idx) {
    selectedIdx = idx;
    render();
    showProps(doc.children[idx]);
}

function showProps(w) {
    const panel = document.getElementById('props-content');
    const props = getWidgetProps(w);
    let html = '<div class="prop-row"><label>Type</label><input value="' + w.type + '" disabled></div>';
    html += '<div class="prop-row"><label>ID</label><input value="' + w.id + '" onchange="updateProp(\'id\',this.value)"></div>';

    for (const [key, val] of Object.entries(props)) {
        if (getPropType(w.type, key) === 'int_pair' && Array.isArray(val)) {
            html += '<div class="prop-row"><label>' + key + '</label><input value="' + val.join(', ') + '" onchange="updateArrayProp(\'' + key + '\',this.value)"></div>';
        } else if (typeof val === 'boolean') {
            html += '<div class="prop-row"><label>' + key + '</label><select onchange="updateBoolProp(\'' + key + '\',this.value)"><option ' + (val?'selected':'') + '>true</option><option ' + (!val?'selected':'') + '>false</option></select></div>';
        } else {
            html += '<div class="prop-row"><label>' + key + '</label><input value="' + val + '" onchange="updateProp(\'' + key + '\',this.value)"></div>';
        }
    }

    const evts = EVENTS[w.type] || [];
    if (evts.length) {
        html += '<div class="prop-row"><label>Events</label></div>';
        evts.forEach(evt => {
            const checked = (w.events || []).includes(evt);
            html += '<div class="prop-row"><label><input type="checkbox" ' + (checked?'checked':'') + ' onchange="toggleEvent(\'' + evt + '\',this.checked)"> ' + evt + '</label></div>';
        });
    }
    panel.innerHTML = html;
}

function updateProp(key, val) {
    if (selectedIdx < 0) return;
    if (key === 'id') { doc.children[selectedIdx].id = val; }
    else getWidgetProps(doc.children[selectedIdx])[key] = parsePropValue(doc.children[selectedIdx].type, key, val);
    render();
}

function updateArrayProp(key, val) {
    if (selectedIdx < 0) return;
    getWidgetProps(doc.children[selectedIdx])[key] = parsePropValue(doc.children[selectedIdx].type, key, val);
    render();
}

function updateBoolProp(key, val) {
    if (selectedIdx < 0) return;
    getWidgetProps(doc.children[selectedIdx])[key] = val === 'true';
    render();
}

function toggleEvent(evt, checked) {
    if (selectedIdx < 0) return;
    const w = doc.children[selectedIdx];
    if (!w.events) w.events = [];
    if (checked && !w.events.includes(evt)) w.events.push(evt);
    if (!checked) w.events = w.events.filter(e => e !== evt);
    render();
}

function deleteSelected() {
    if (selectedIdx < 0) return;
    doc.children.splice(selectedIdx, 1);
    selectedIdx = -1;
    render();
    document.getElementById('props-content').innerHTML = '<p style="color:#666;font-size:12px">Select a widget</p>';
}

function updateCodePreview() {
    const pre = document.getElementById('code-preview');
    let code = '// Generated from ' + doc.name + '.lui\\n';
    code += 'class ' + doc.name + '_UI {\\n';
    code += 'protected:\\n';
    doc.children.forEach(w => { code += '    ' + w.type + '* m_' + w.id + ';\\n'; });
    code += '\\npublic:\\n    void setup(lv_obj_t* parent) {\\n';
    doc.children.forEach(w => {
        normalizeWidgetProps(w);
        const propsObject = getWidgetProps(w);
        code += '        m_' + w.id + ' = new ' + w.type + '(parent);\\n';
        const props = Object.entries(propsObject).filter(([k]) => k !== 'pos' && k !== 'size' && k !== 'theme');
        if (props.length || propsObject.size !== undefined || propsObject.pos || propsObject.theme) {
            code += '        m_' + w.id + '->';
            let chain = [];
            if (propsObject.size !== undefined) chain.push('size(' + formatCodeValue(w.type, 'size', propsObject.size) + ')');
            if (propsObject.pos) chain.push('pos(' + propsObject.pos.join(', ') + ')');
            props.forEach(([k, v]) => {
                chain.push(k + '(' + formatCodeValue(w.type, k, v) + ')');
            });
            if (propsObject.theme) chain.push(propsObject.theme + '()');
            code += chain.join('.') + ';\\n';
        }
    });
    code += '    }\\n};';
    pre.textContent = code;
}

function saveDoc() {
    vscode.postMessage({ type: 'save', data: doc });
    document.getElementById('status').textContent = 'Saved ✓';
    setTimeout(() => document.getElementById('status').textContent = 'Ready', 2000);
}

function compileLui() {
    vscode.postMessage({ type: 'compile', data: doc });
    document.getElementById('status').textContent = 'Compiling...';
}

// Drag & drop from component panel
document.querySelectorAll('.comp-item').forEach(item => {
    item.ondragstart = (e) => {
        dragType = item.dataset.type || null;
        if (e.dataTransfer && dragType) {
            e.dataTransfer.effectAllowed = 'copy';
            e.dataTransfer.setData('text/plain', dragType);
        }
    };
    item.ondragend = () => { dragType = null; };
});

document.getElementById('canvas').ondragover = (e) => {
    e.preventDefault();
    if (e.dataTransfer) e.dataTransfer.dropEffect = 'copy';
};
document.getElementById('canvas').ondrop = (e) => {
    e.preventDefault();
    const droppedType = dragType || (e.dataTransfer ? e.dataTransfer.getData('text/plain') : '');
    if (!droppedType) return;
    const rect = document.getElementById('canvas').getBoundingClientRect();
    const x = Math.max(0, Math.round(e.clientX - rect.left));
    const y = Math.max(0, Math.round(e.clientY - rect.top));

    const defaults = DEFAULTS[droppedType] || {};
    const id = droppedType.toLowerCase().replace(/^l/, '') + '_' + (autoIdCounter++);
    const newWidget = {
        type: droppedType,
        id: id,
        props: { ...defaults, pos: [x, y] },
        events: []
    };
    doc = normalizeDocument(doc);
    doc.children.push(newWidget);
    selectedIdx = doc.children.length - 1;
    showCanvasError('');
    setStatus('Component added');
    render();
    showProps(newWidget);
    dragType = null;
};

document.getElementById('canvas').onclick = () => {
    selectedIdx = -1;
    render();
    document.getElementById('props-content').innerHTML = '<p style="color:#666;font-size:12px">Select a widget</p>';
};

// Receive data from extension
window.addEventListener('message', e => {
    if (e.data.type === 'update') {
        doc = normalizeDocument(e.data.data);
        initCanvas();
        render();
    }
});

doc = normalizeDocument(doc);
initCanvas();
render();
</script>
</body>
</html>`;
    }
}
