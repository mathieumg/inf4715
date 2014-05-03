
Directories:
+- Assets   : Raw assets (.max, .psd, etc.)
+- Build    : Solution files and makefiles for various platforms
+- Docs     : Engine and game documentation
+- Exporter : Autodesk 3ds Max 2012 Exporter
+- Game     : Game content and executables
+- Output   : Build outputs (can be cleaned up or deleted)
+- Patches  : In-development patches to be shared
+- Scripts  : Various script to build the game, get statistics, etc.
+- Setup    : Setup project to generate a packaged version of the game
+- Sources  : Engine and game source code
+- Tools    : Various tools and SDKs


  /         /\           /\         \    
 /    F    /  \    A    /  \    Q    \
/         /    \       /    \         \

========================================================================
Q: What do I need to do to compile Vicuna?
A: Install DXSDK June 2010 (make sure DXDIR is defined)
   Extract 3rd party archives.
      - ".\Sources\3rdparty\PhysX-3.2.2_PC_SDK_Core.zip"
      - ".\Sources\3rdparty\Wwise v2012.2.2 build 4430.rar"
      - ".\Sources\3rdparty\include\boost.zip"
   Open the VS solution under ".\Vicuna\Build\VS2010\Vicuna.sln"
   Set GameApp as the starting project
   Compile entire solution

========================================================================
Q: Comment supprimer les pivots interne dans 3ds max.
A: The way to fix export problems : 
   1. Select all sub objects to be exported
   2. Select hierarchy tab (3rd tab) in 3ds
   3. Click "Reset Pivot"
   4. Click "Reset Scale and Translate"
   5. Group your sub objects
   
Q: How to reset transform in 3dsmax
A: Goto the tool section and select "Reset Xform" tool
