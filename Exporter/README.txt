Pour debug automatiquement, il fait faire 2 �tapes manuellement:
1-  Aller modifier le fichier plugin.ini dans le dossier d'installation de 3ds Max. Le path devrait ressembler � "<INSTALL_PATH>/en-US/plugin.ini".
    Il faut ajouter la ligne suivante dans la section "[Directories]": Vicuna Temp Plugin Folder=C:\temp\3dsMaxPlugins
2-  Modifier les propri�t�s de projet de "VCN3dsExporter" dans la solution du m�me nom. Aller � "Propri�t�s" -> "Configuration Properties" -> "Debugging" -> "Command" et mettre le path du exe de 3ds Max. Par exemple: "D:\Autodesk\3ds Max 2013 x86\3dsmax.exe"

Il devrait maintenant �tre possible de debug en faisant "Start Debugging" dans Visual Studio.
Le message de warning au debut du debug peut �tre ignor�. Une instance de 3ds Max devrait d�marrer (soyez patient) et le debug sera enable pour le code du plugin.

*** Pour debug du code Managed: 
L'option suivante fonctionne aussi pour le code normal, mais ralentit le d�marrage du debugger alors je recommance de le laisser disable sauf si on doit debug du code managed (C# ou managed C++).
1- Aller � "Propri�t�s" -> "Configuration Properties" -> "Debugging" ->"Debugger Type" et mettre la valeur � "Mixed".