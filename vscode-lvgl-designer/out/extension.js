"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.activate = activate;
exports.deactivate = deactivate;
const vscode = require("vscode");
const LuiEditorProvider_1 = require("./LuiEditorProvider");
function activate(context) {
    context.subscriptions.push(LuiEditorProvider_1.LuiEditorProvider.register(context));
    context.subscriptions.push(vscode.commands.registerCommand('lvglDesigner.openPreview', async (uri) => {
        var _a;
        const target = uri !== null && uri !== void 0 ? uri : (_a = vscode.window.activeTextEditor) === null || _a === void 0 ? void 0 : _a.document.uri;
        if (!target || target.fsPath.toLowerCase().endsWith('.lui') === false) {
            vscode.window.showWarningMessage('Please open or select a .lui file first.');
            return;
        }
        await vscode.commands.executeCommand('vscode.openWith', target, 'lvglDesigner.luiEditor');
    }));
    context.subscriptions.push(vscode.commands.registerCommand('lvglDesigner.newScreen', async () => {
        const name = await vscode.window.showInputBox({ prompt: 'Screen name (e.g. MainScreen)' });
        if (!name)
            return;
        const template = JSON.stringify({
            version: "1.0",
            name: name,
            size: [800, 480],
            children: []
        }, null, 2);
        const folders = vscode.workspace.workspaceFolders;
        if (!folders)
            return;
        const uiDir = vscode.Uri.joinPath(folders[0].uri, 'ui');
        await vscode.workspace.fs.createDirectory(uiDir);
        const uri = vscode.Uri.joinPath(uiDir, `${name.toLowerCase()}.lui`);
        await vscode.workspace.fs.writeFile(uri, Buffer.from(template, 'utf8'));
        await vscode.commands.executeCommand('vscode.openWith', uri, 'lvglDesigner.luiEditor');
    }));
    context.subscriptions.push(vscode.commands.registerCommand('lvglDesigner.compile', async () => {
        var _a;
        const workspaceFolder = (_a = vscode.workspace.workspaceFolders) === null || _a === void 0 ? void 0 : _a[0];
        const terminal = vscode.window.createTerminal({
            name: 'luic',
            cwd: workspaceFolder === null || workspaceFolder === void 0 ? void 0 : workspaceFolder.uri.fsPath
        });
        terminal.show();
        terminal.sendText('make -C build ui_compile', true);
    }));
}
function deactivate() { }
//# sourceMappingURL=extension.js.map
