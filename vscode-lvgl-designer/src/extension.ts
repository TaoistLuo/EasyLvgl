import * as vscode from 'vscode';
import * as path from 'path';
import * as cp from 'child_process';
import { LuiEditorProvider } from './LuiEditorProvider';

function findDesignerBinary(workspaceFolder: vscode.WorkspaceFolder | undefined): string {
    if (workspaceFolder) {
        return path.join(workspaceFolder.uri.fsPath, 'bin', 'lvgl-designer');
    }
    return 'lvgl-designer';
}

export function activate(context: vscode.ExtensionContext) {
    context.subscriptions.push(LuiEditorProvider.register(context));

    context.subscriptions.push(
        vscode.commands.registerCommand('lvglDesigner.openDesigner', async (uri?: vscode.Uri) => {
            const target = uri ?? vscode.window.activeTextEditor?.document.uri;
            if (!target || !target.fsPath.toLowerCase().endsWith('.lui')) {
                vscode.window.showWarningMessage('Please open or select a .lui file first.');
                return;
            }

            const workspaceFolder = vscode.workspace.workspaceFolders?.[0];
            const designerPath = findDesignerBinary(workspaceFolder);

            try {
                const proc = cp.spawn(designerPath, [target.fsPath], {
                    detached: true,
                    stdio: 'ignore'
                });
                proc.unref();
                vscode.window.showInformationMessage('LVGL++ Designer launched.');
            } catch {
                vscode.window.showErrorMessage(
                    `Failed to launch designer at: ${designerPath}\n` +
                    'Build it first: cd tools/lvgl-designer && mkdir build && cd build && source x86 && cmake .. && make'
                );
            }
        })
    );

    context.subscriptions.push(
        vscode.commands.registerCommand('lvglDesigner.openPreview', async (uri?: vscode.Uri) => {
            const target = uri ?? vscode.window.activeTextEditor?.document.uri;
            if (!target || !target.fsPath.toLowerCase().endsWith('.lui')) {
                vscode.window.showWarningMessage('Please open or select a .lui file first.');
                return;
            }
            await vscode.commands.executeCommand('vscode.openWith', target, 'lvglDesigner.luiEditor');
        })
    );

    context.subscriptions.push(
        vscode.commands.registerCommand('lvglDesigner.newScreen', async () => {
            const name = await vscode.window.showInputBox({ prompt: 'Screen name (e.g. MainScreen)' });
            if (!name) { return; }

            const template = JSON.stringify({
                version: "1.0",
                name: name,
                size: [800, 480],
                children: []
            }, null, 2);

            const folders = vscode.workspace.workspaceFolders;
            if (!folders) { return; }

            const uiDir = vscode.Uri.joinPath(folders[0].uri, 'ui');
            await vscode.workspace.fs.createDirectory(uiDir);

            const uri = vscode.Uri.joinPath(uiDir, `${name.toLowerCase()}.lui`);
            await vscode.workspace.fs.writeFile(uri, Buffer.from(template, 'utf8'));

            const designerPath = findDesignerBinary(folders[0]);
            try {
                const proc = cp.spawn(designerPath, [uri.fsPath], {
                    detached: true,
                    stdio: 'ignore'
                });
                proc.unref();
            } catch {
                await vscode.commands.executeCommand('vscode.openWith', uri, 'lvglDesigner.luiEditor');
            }
        })
    );

    context.subscriptions.push(
        vscode.commands.registerCommand('lvglDesigner.compile', async () => {
            const workspaceFolder = vscode.workspace.workspaceFolders?.[0];
            const terminal = vscode.window.createTerminal({
                name: 'luic',
                cwd: workspaceFolder?.uri.fsPath
            });
            terminal.show();
            terminal.sendText('make -C build ui_compile', true);
        })
    );
}

export function deactivate() {}
