# EditorTools for Unreal Engine 5

EditorTools is an utility plugin for Unreal Engine 5, developed entirely in C++. This toolset is designed to significantly enhance the workflow and boost the productivity of developers and technical artists by providing a comprehensive suite of quick actions, custom editor menus, asset management utilities, and flexible interface customization options that integrate seamlessly into the Unreal Engine 5 editor environment.

## Implemented Features

- 🛠️ **Quick Asset Actions**
  - **Smart Duplicate**: Duplicate assets multiple times with auto-incrementing numeric suffixes.
  - **Prefix Management**: Automatically assign or fix asset prefixes based on their specific class.
  - **Cleanup Name**: Strip unwanted substrings or pre-defined "mess" from selected asset names.
  - **Remove Unused Assets**: Identify and safely batch-delete assets that have no references in the project.
  - **Fix Up Redirectors**: Automatically find and fix up object redirectors across the game directory.

- 🎭 **Quick Actor Actions**
  - **Select Similar**: Automatically select all level actors sharing a similar name substring.
  - **Linear Duplication**: Duplicate selected actors multiple times with a customizable offset distance along specific axes (X, Y, or Z).
  - **Random Transform**: Apply random rotation, scaling, and location offsets to selected actors within defined minimum and maximum ranges.

- 🧩 **Advanced Deletion Widget (Slate)**
  - **Standalone UI**: Dedicated window for managing and deleting assets.
  - **Smart Filtering**: Filter displayed assets by categories such as All Assets, Unused Assets, and Duplicated Assets.
  - **Batch Deletion**: Select multiple assets using checkboxes for rapid bulk deletion.
  - **Browser Sync**: Synchronize the Content Browser seamlessly to clicked assets in the list.

- 🌍 **World Outliner Extensions**
  - **Actor Selection Column**: Adds a custom column with a lock/unlock toggle directly in the Scene Outliner to prevent accidental selection of specific actors.

- 🖥️ **UI Scale Customization**
  - **Interface Scaling**: Dedicated settings to dynamically adjust and scale the Unreal Engine editor interface controls for better visibility and comfort.

- 🧱 **Material Creation via Code**
  - **Asset Generation**: Generate simple materials and Material Instance (MI) assets directly from C++ logic, automating graphical resource setup.

- 🎨 **Custom Editor Icons**
  - **Visual Polish**: Display unique, custom visual icons for all custom tools, actions, and menu items to improve UX.

## Features in development

- 🧩 **Extended Level Editor Menu**
  - Add custom context entries and specialized commands directly to the actor viewport context menu.

- ⌨️ **Custom Editor Key Bindings**
  - Bind custom hotkeys and shortcuts to rapidly trigger specific EditorTools actions.
  
  
## Planned Features (TODO)

- 📈 **Outliner and Context Sorting**
  - Sort World Outliner and context menu items based on specified input parameters.

## Visual Overview

The following demonstration materials illustrate the operation of the key system components within the development environment.

![Asset Actions](https://github.com/Nepji/EditorTools/blob/main/.github/images/AssetAcion.png)

![Context Actions](https://github.com/Nepji/EditorTools/blob/main/.github/images/ContextAction.png)

![Slate Widget UI](https://github.com/Nepji/EditorTools/blob/main/.github/images/SlateWidget.png)

![Material Generation](https://github.com/Nepji/EditorTools/blob/main/.github/images/MaterialCreation.png)

![Quick Actor Actions](https://github.com/Nepji/EditorTools/blob/main/.github/images/QuickActorActions.png)

All necessary repository links and detailed author information can be found on the main project repository page.
