# LVGL++ Designer - VSCode Extension

Visual UI designer for LVGL++ — drag & drop components, edit properties, generate C++ code.

## Features

- 🎨 Visual editor for .lui files
- 🖱️ Drag & drop 16+ LVGL++ components
- 📝 Property panel with real-time editing
- 💾 Save generates .lui JSON
- ⚡ One-click compile to C++ via luic
- 👁️ Live code preview

## Quick Start

```bash
cd vscode-lvgl-designer
npm install
npm run compile
```

Then press F5 in VSCode to launch Extension Development Host.

## Usage

1. Create a `.lui` file in `ui/` directory
2. VSCode opens the visual editor automatically
3. Drag components from left panel to canvas
4. Edit properties in right panel
5. Click Save → run `make` → C++ code generated

## Supported Components

LLabel, LButton, LSlider, LBar, LCheckbox, LSwitch,
LDropdown, LTextArea, LArc, LSpinner, LLed,
LVStack, LHStack, LTable, LChart, LImage

## Requirements

- Node.js 18+
- VSCode 1.80+
- TypeScript 5+
