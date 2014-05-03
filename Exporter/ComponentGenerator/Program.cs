
using System;
using System.Collections.Generic;
using System.IO;
using System.Reflection;
using System.Text;
using System.Linq;
using System.Web;
using System.Xml.Linq;
using VCNNodesCSharp;
using VCNNodesCSharpCustom;
using System.Diagnostics;
using System.Threading;
using System.Globalization;

namespace ComponentGenerator
{

    class Program
    {
        // Need to add "//" before to make it a C++ comment
        static string CodeRegionDelimiterBegin = "**CodeRegion";
        static string CodeRegionDelimiterEnd = "**EndCodeRegion";

        // True if modified
        static bool UpdateVSProjectXml(ref XElement doc, string componentName, string descendantName, string descendantAttrName, string extension )
        {
            bool modified = false;
            XNamespace ns = doc.Name.Namespace;

            // Edit the Visual Studio projects to reference the new files
            var clCompileElements = doc.Descendants(ns + descendantName);
            // Look for previously added
            bool alreadyThere = false;
            foreach (XElement elem in clCompileElements)
            {
                foreach (XAttribute att in elem.Attributes(descendantAttrName))
                {
                    if (att.Value.Contains(componentName + "." + extension))
                    {
                        alreadyThere = true;
                        break;
                    }
                }
            }
            if (!alreadyThere)
            {
                // Not there, add it
                foreach (XElement elem in clCompileElements)
                {
                    foreach (XAttribute att in elem.Attributes(descendantAttrName))
                    {
                        if (att.Value.Contains("\\Node." + extension))
                        {
                            // Good parent
                            XElement newElem = new XElement(elem);
                            string componentFileVal = newElem.Attribute(descendantAttrName).Value;
                            componentFileVal = componentFileVal.Replace("\\Node." + extension, "\\" + componentName + "." + extension);
                            newElem.Attribute(descendantAttrName).Value = componentFileVal;
                            elem.Parent.Add(newElem);
                            modified = true;
                        }
                    }
                }
            }
            return modified;
        }

        static void Main(string[] args)
        {
            // Args: ("VCNNodes"FolderPath) 
            Thread.CurrentThread.CurrentCulture = new CultureInfo("en-US", false);

            if (args.Length < 2)
            {
                Console.WriteLine("You need to specify the path of the \"VCNNodes\" folder as first argument.");
                Console.ReadLine();
                return;
            }

            // Do not remove. This will force a load of the custom components in the current assembly
            EmptyComponent ec = new EmptyComponent();

            string vCNNodesFolderPath = args[0];
            string vCNVSProjectsPath = args[1];

            List<Type> componentTypes = VCNNodesCSharp.VCNNodesCSharpHelper.GetListOfComponents();
            List<string> exportedFiles = new List<string>();

            XElement vcsprojDoc = null;
            try
            {
                vcsprojDoc = XElement.Load(vCNVSProjectsPath + Path.DirectorySeparatorChar + "VCNNodes.vcxproj");
            }
            catch (System.Exception) {}

            XElement vcsprojFiltersDoc = null;
            try
            {
                vcsprojFiltersDoc = XElement.Load(vCNVSProjectsPath + Path.DirectorySeparatorChar + "VCNNodes.vcxproj.filters");
            }
            catch (System.Exception) {}

            if (vcsprojDoc == null || vcsprojFiltersDoc == null)
            {
                Console.WriteLine("Could not open vcxproj or the vcxproj.filters file for edit. You may have to add the new components manually to the VCNNodes project.");
            }

            foreach (Type componentType in componentTypes)
            {
                string componentName = componentType.Name;


                if (BuildHFile(componentType, vCNNodesFolderPath))
                {
                    exportedFiles.Add(componentName + ".h");
                    Console.WriteLine(".hp file created/updated for component " + componentName);
                }
                else
                {
                    Console.WriteLine("Could not create .h file for class " + componentName);
                }

                if (BuildCPPFile(componentType, vCNNodesFolderPath))
                {
                    exportedFiles.Add(componentName + ".cpp");
                    Console.WriteLine(".cpp file created/updated for component " + componentName);
                }
                else
                {
                    Console.WriteLine("Could not create .cpp file for class " + componentName);
                }

                if (vcsprojDoc != null && vcsprojFiltersDoc != null)
                {
                    bool vcxprojNeedsSave = false;
                    bool vcxprojFilterNeedsSave = false;
                    vcxprojNeedsSave |= UpdateVSProjectXml(ref vcsprojDoc, componentName, "ClCompile", "Include", "cpp");
                    vcxprojNeedsSave |= UpdateVSProjectXml(ref vcsprojDoc, componentName, "ClInclude", "Include", "h");

                    vcxprojFilterNeedsSave |= UpdateVSProjectXml(ref vcsprojFiltersDoc, componentName, "ClCompile", "Include", "cpp");
                    vcxprojFilterNeedsSave |= UpdateVSProjectXml(ref vcsprojFiltersDoc, componentName, "ClInclude", "Include", "h");

                    if (vcxprojNeedsSave)
                    {
                        vcsprojDoc.Save(vCNVSProjectsPath + Path.DirectorySeparatorChar + "VCNNodes.vcxproj");
                    }
                    if (vcxprojFilterNeedsSave)
                    {
                        vcsprojFiltersDoc.Save(vCNVSProjectsPath + Path.DirectorySeparatorChar + "VCNNodes.vcxproj.filters");
                    }
                }
                

                Console.WriteLine("");

            }

            if (GenerateComponentFactory(vCNNodesFolderPath, componentTypes))
            {
                Console.WriteLine("Component Factory created.");
            }
            else
            {
                Console.WriteLine("Could not create component factory");
            }

            Console.WriteLine("Adding files to svn");
            try
            {
                Process proc = new Process();
                proc.StartInfo.WorkingDirectory = "C:\\Program Files\\TortoiseSVN\\bin";
                proc.StartInfo.FileName = "C:\\Program Files\\TortoiseSVN\\bin\\TortoiseProc.exe";
                StringBuilder sb = new StringBuilder();
                sb.Append("/command:add /path:\"");
                for (int i = 0; i < exportedFiles.Count; i++ )
                {
                    string filename = exportedFiles[i];
                    sb.Append(vCNNodesFolderPath + Path.DirectorySeparatorChar + filename);
                    if (i != exportedFiles.Count- 1 )
                    {
                        sb.Append("*");
                    }
                }
                sb.Append("\" /closeonend:0");
                proc.StartInfo.Arguments = sb.ToString();
                proc.StartInfo.UseShellExecute = false;
                proc.StartInfo.RedirectStandardError = false;
                proc.Start();
                proc.WaitForExit();
                proc.Close();
            }
            catch (Exception)
            {
            }

            Console.WriteLine("\nOperation completed");
            Console.WriteLine("Press enter to exit");
            Console.ReadLine();
            
        }


        static bool GenerateComponentFactory(string vcnNodesFolderPath, List<Type> componentTypes)
        {
            string fullComponentFactoryHFilePath = vcnNodesFolderPath + Path.DirectorySeparatorChar + "ComponentFactory.h";
            string fullComponentFactoryHFilePathBackup = vcnNodesFolderPath + Path.DirectorySeparatorChar + "ComponentFactory_backup.h";
            string fullComponentFactoryCPPFilePath = vcnNodesFolderPath + Path.DirectorySeparatorChar + "ComponentFactory.cpp";
            string fullComponentFactoryCPPFilePathBackup = vcnNodesFolderPath + Path.DirectorySeparatorChar + "ComponentFactory_backup.cpp";

            string previousHCodeRegion = "";
            string previousCPPCodeRegion = "";
            try
            {
                string hText = System.IO.File.ReadAllText(fullComponentFactoryHFilePath);
                string cppText = System.IO.File.ReadAllText(fullComponentFactoryCPPFilePath);
                File.Copy(fullComponentFactoryHFilePath, fullComponentFactoryHFilePathBackup, true);
                File.Copy(fullComponentFactoryCPPFilePath, fullComponentFactoryCPPFilePathBackup, true);
                previousHCodeRegion = ExtractCodeRegionFromFile(hText, false);
                previousCPPCodeRegion = ExtractCodeRegionFromFile(cppText, false);
            }
            catch (System.Exception)
            {
                // Normal if there is not already an existing file
            }


            // Factory generation (.h)
            try
            {
                FileStream oStream = File.Open(fullComponentFactoryHFilePath, FileMode.Create);
                StringBuilder sb = new StringBuilder();

                sb.Append("#pragma once                                                                                         \n");
                sb.Append("                                                                                                     \n");
                sb.Append("#include \"ComponentBase.h\"                                                                         \n");
                sb.Append("#include <string>                                                                                    \n");
                sb.Append("                                                                                                     \n");
                sb.Append("class ComponentFactory                                                                               \n");
                sb.Append("{                                                                                                    \n");
                sb.Append("                                                                                                     \n");
                sb.Append("public:                                                                                              \n");
                sb.Append("                                                                                                     \n");
	            sb.Append("    // Method to build a component from a component name.                                            \n");
                sb.Append("    // Can return NULL if class is not found. Otherwise, you are responsible to delete the memory    \n");
                sb.Append("    static VCNIComponent* CreateNewComponent(const VCNString& componentName);                        \n");
                sb.Append("                                                                                                     \n");
                sb.Append("//" + CodeRegionDelimiterBegin + "\n");
                sb.Append(previousHCodeRegion);
                sb.Append("//" + CodeRegionDelimiterEnd + "\n");
                sb.Append("                                                                                                     \n");
                sb.Append("};                                                                                                   \n");

                string buf = sb.ToString();
                byte[] info = new UTF8Encoding(true).GetBytes(buf);
                oStream.Write(info, 0, info.Length);
                oStream.Close();

            }
            catch (System.Exception)
            {
                Console.WriteLine("An error has occurred when creating the h file for the Component Factory");
                return false;
            }

            // Factory generation (.cpp)
            try
            {
                FileStream oStream = File.Open(fullComponentFactoryCPPFilePath, FileMode.Create);
                StringBuilder sb = new StringBuilder();

                sb.Append("#include \"Precompiled.h\"                                                                                           \n");
                sb.Append("#include \"ComponentFactory.h\"                                                                                      \n");
                sb.Append("#include <algorithm>                                                                                                 \n");
                sb.Append("                                                                                                                     \n");
                foreach (Type componentType in componentTypes)
                {
                    sb.Append("#include \"" + componentType.Name + ".h\"                                                                        \n");
                }
                sb.Append("                                                                                                                     \n");
                sb.Append("VCNIComponent* ComponentFactory::CreateNewComponent( const VCNString& componentName )                                \n");
                sb.Append("{                                                                                                                    \n");
                sb.Append("    VCNString lowerComponentName = componentName;                                                                    \n");
	            sb.Append("    std::transform(lowerComponentName.begin(), lowerComponentName.end(), lowerComponentName.begin(), ::tolower);     \n");
                sb.Append("                                                                                                                     \n");
                bool f = true;
                foreach (Type componentType in componentTypes)
                {
                    if (f)
                    {
                        sb.Append("    ");
                    }
                    else
                    {
                        sb.Append("    else ");
                    }
                    sb.Append("if (lowerComponentName == L\"" + componentType.Name.ToLower() + "\")                                              \n");
                    sb.Append("    {                                                                                                            \n");
                    sb.Append("        return new VCN" + componentType.Name + "();                                                              \n");
                    sb.Append("    }                                                                                                            \n");
                }
                sb.Append("                                                                                                                     \n");
	            sb.Append("    return NULL;                                                                                                     \n");
                sb.Append("}                                                                                                                    \n");
                sb.Append("                                                                                                                     \n");
                sb.Append("//" + CodeRegionDelimiterBegin + "\n");
                sb.Append(previousCPPCodeRegion);
                sb.Append("//" + CodeRegionDelimiterEnd + "\n");
                sb.Append("                                                                                                                     \n");

                string buf = sb.ToString();
                byte[] info = new UTF8Encoding(true).GetBytes(buf);
                oStream.Write(info, 0, info.Length);
                oStream.Close();

            }
            catch (System.Exception)
            {
                Console.WriteLine("An error has occurred when creating the cpp file for the Component Factory.");
                return false;
            }

            return true;
        }

        static string ExtractCodeRegionFromFile(string fileContent, bool fromEnd, string startKey = "", string endKey = "")
        {
            int iStartKey = 0;
            int iEndKey = fileContent.Length;
            if (startKey.Length > 0)
            {
                iStartKey = Math.Max(iStartKey, fileContent.IndexOf(startKey));
                if (endKey.Length > 0)
                {
                    int t = fileContent.IndexOf(endKey, iStartKey);
                    if (t != -1)
                    {
                        iEndKey = t;
                    }
                }
            }

            int iBegin = 0;
            int iEnd = 0;
            if (fromEnd)
            {
                iBegin = fileContent.LastIndexOf(CodeRegionDelimiterBegin);
                iEnd = fileContent.LastIndexOf(CodeRegionDelimiterEnd);
            }
            else
            {
                iBegin = fileContent.IndexOf(CodeRegionDelimiterBegin, iStartKey);
                iEnd = fileContent.IndexOf(CodeRegionDelimiterEnd, iStartKey);
            }

            if (iBegin > -1 && iEnd > iBegin && iEnd < iEndKey)
            {
                int iBeginNextLine = fileContent.IndexOf("\n", iBegin);
                int s = iBeginNextLine + 1;
                string ret = fileContent.Substring(s, iEnd - s - 2);
                return ret;
            }
            else
            {
                return "";
            }
        }

        static string GetCPPStringRepresentationOfType(Type type)
        {
            if (type == typeof(int))
            {
                return "int";
            }
            else if (type == typeof(uint))
            {
                return "unsigned int";
            }
            else if (type == typeof(float))
            {
                return "float";
            }
            else if (type == typeof(double))
            {
                return "double";
            }
            else if (type == typeof(bool))
            {
                return "bool";
            }
            else if (type == typeof(string))
            {
                return "VCNString";
            }
            else if (type == typeof(char))
            {
                return "char";
            }
            else if (type == typeof(Vector2))
            {
                return "Vector2";
            }
            else if (type == typeof(Vector3))
            {
                return "Vector3";
            }
            else if (type == typeof(Vector4))
            {
                return "Vector4";
            }
            else if (type == typeof(LuaTrigger))
            {
                return "LuaTrigger";
            }
            return "void*";
        }

        static bool BuildHFile(Type componentType, string vcnNodesFolderPath)
        {
            string className = componentType.Name;

            string fullHFilePath = vcnNodesFolderPath + Path.DirectorySeparatorChar + className + ".h";
            string fullHFilePathCopy = FindNextBackupFileNameForFile(vcnNodesFolderPath, className, ".h");
            string previousCodeRegionContentIncludes = "";
            string previousCodeRegionContent = "";

            try
            {
                string hText = System.IO.File.ReadAllText(fullHFilePath);
                File.Copy(fullHFilePath, fullHFilePathCopy, true);
                previousCodeRegionContentIncludes = ExtractCodeRegionFromFile(hText, false); // Will get the first one
                previousCodeRegionContent = ExtractCodeRegionFromFile(hText, false, "public:"); // Will get the second one
            }
            catch (System.Exception)
            {
                // Normal if there is not already an existing file
            }

            FieldInfo[] fields;
            try
            {
                bool extendsBaseComponent = componentType.BaseType == typeof(BaseComponent);
                if (extendsBaseComponent)
                {
                    fields = componentType.GetFields();
                }
                else
                {
                    fields = componentType.GetFields(BindingFlags.DeclaredOnly | BindingFlags.Public | BindingFlags.Instance);
                }

                FileStream oStream = File.Open(fullHFilePath, FileMode.Create);
                StringBuilder sb = new StringBuilder();

                sb.Append("#pragma once                                                                                         \n");
                sb.Append("                                                                                                     \n");
                sb.Append("                                                                                                     \n");
                sb.Append("// DO NOT DECLARE MORE CODE REGIONS. USE THE ONES ALREADY IN THE FILE                                \n");
                sb.Append("                                                                                                     \n");
                sb.Append("#include \"VCNLua\\LuaTrigger.h\"                                                                    \n");
                if (extendsBaseComponent)
                {
                    sb.Append("#include \"ComponentBase.h\"                                                                     \n");
                }
                else
                {
                    sb.Append("#include \"" + componentType.BaseType.Name + ".h\"                                               \n");
                }
                sb.Append("//" + CodeRegionDelimiterBegin + "\n");
                sb.Append(previousCodeRegionContentIncludes);
                sb.Append("//" + CodeRegionDelimiterEnd + "\n");
                sb.Append("                                                                                                     \n");
                if (extendsBaseComponent)
                {
                    sb.Append("class VCN" + className + " :   public VCNComponentBase<VCN" + className + ">,                    \n");
                    sb.Append("                            public VCNIUpdatable                                                 \n");
                }
                else
                {
                    sb.Append("class VCN" + className + " :   public VCN" + componentType.BaseType.Name + "                        \n");
                }
                sb.Append("{                                                                                                    \n");
                sb.Append("public:                                                                                              \n");
                sb.Append("    VCN" + className + "();                                                                          \n");
                sb.Append("                                                                                                     \n");
                sb.Append("    virtual void Initialise(const Parameters& params) override;                                      \n");
                sb.Append("    virtual void Update(VCNFloat dt) override;                                                       \n");
                sb.Append("                                                                                                     \n");
                sb.Append("    virtual bool SetAttribute( const VCNString& attributeName, const VCNString& attributeValue );    \n");
                sb.Append("                                                                                                     \n");
                sb.Append("    virtual VCNIComponent::Ptr Copy() const;                                                         \n");
                sb.Append("                                                                                                     \n");

                foreach (FieldInfo fi in fields)
                {
                    string typeRepresentation = GetCPPStringRepresentationOfType(fi.FieldType);
                    object[] cAttrs = fi.GetCustomAttributes(false);

                    bool generateGetter = false;
                    bool generateSetter = false;
                    bool foundAccessorInfo = false;
                    foreach (object o in cAttrs)
                    {
                        Type t = o.GetType();
                        if (t == typeof(NoAccessors))
                        {
                            generateGetter = false;
                            generateSetter = false;
                            foundAccessorInfo = true;
                            break;
                        }
                        else if (t == typeof(Getter))
                        {
                            generateGetter = true;
                            foundAccessorInfo = true;
                        }
                        else if (t == typeof(Setter))
                        {
                            generateSetter = true;
                            foundAccessorInfo = true;
                        }
                    }
                    if (!foundAccessorInfo)
                    {
                        generateGetter = true;
                        generateSetter = true;
                    }
                    
                    // Getter
                    if (generateGetter)
                    {
                        sb.Append("    inline const " + typeRepresentation + "& Get" + fi.Name + "() const { return m" + fi.Name + "; } \n");
                    }
                    // Setter
                    if (generateSetter)
                    {
                        sb.Append("    inline void Set" + fi.Name + "(const " + typeRepresentation + "& val) { m" + fi.Name + " = val; } \n");
                    }
                    sb.Append("                                                                                               \n");
                }
                sb.Append("protected:                                                                                         \n");
                foreach (FieldInfo fi in fields)
                {
                    string typeRepresentation = GetCPPStringRepresentationOfType(fi.FieldType);
                    sb.Append("    " + typeRepresentation + " m" + fi.Name + ";                                               \n");
                }
                sb.Append("                                                                                                   \n");
                sb.Append("//" + CodeRegionDelimiterBegin + "\n");
                sb.Append(previousCodeRegionContent);
                sb.Append("//" + CodeRegionDelimiterEnd + "\n");
                sb.Append("};                                                                                                 \n");
                sb.Append("                                                                                                   \n");
                sb.Append("template class VCNComponentBase<VCN" + className + ">;                                             \n");

                string buf = sb.ToString();
                byte[] info = new UTF8Encoding(true).GetBytes(buf);
                oStream.Write(info, 0, info.Length);
                oStream.Close();

            }
            catch (System.Exception)
            {
                Console.WriteLine("Could not write new .h file. An error has occurred.");
                return false;
            }

            return true;
        }


        static string BuildAttributeDefaultValue(FieldInfo fi)
        {
            StringBuilder sb = new StringBuilder();
            object[] cAttrs = fi.GetCustomAttributes(false);
            string defaultValue = null;
            foreach (object o in cAttrs)
            {
                Type t = o.GetType();
                if (t == typeof(DefaultValue))
                {
                    defaultValue = (o as DefaultValue).Value.ToString().ToLower();
                    if (fi.FieldType == typeof(string))
                    {
                        defaultValue = "L\"" + defaultValue + "\"";
                    }
                    else if (fi.FieldType == typeof(char))
                    {
                        defaultValue = "\'" + defaultValue + "\'";
                    }
                    break;
                }
            }


            if (defaultValue == null)
            {
                if (fi.FieldType == typeof(int) || fi.FieldType == typeof(uint))
                {
                    defaultValue = "0";
                }
                else if (fi.FieldType == typeof(float))
                {
                    defaultValue = "0.0f";
                }
                else if (fi.FieldType == typeof(double))
                {
                    defaultValue = "0.0";
                }
                else if (fi.FieldType == typeof(bool))
                {
                    defaultValue = "false";
                }
                else if (fi.FieldType == typeof(char))
                {
                    defaultValue = "\' \'";
                }
                else if (fi.FieldType == typeof(string))
                {
                    defaultValue = "L\"\"";
                }
                else if (fi.FieldType == typeof(Vector2))
                {
                    defaultValue = "Vector2(0.0f, 0.0f)";
                }
                else if (fi.FieldType == typeof(Vector3))
                {
                    defaultValue = "Vector3(0.0f, 0.0f, 0.0f)";
                }
                else if (fi.FieldType == typeof(Vector4))
                {
                    defaultValue = "Vector4(0.0f, 0.0f, 0.0f, 0.0f)";
                }
                else if (fi.FieldType == typeof(LuaTrigger))
                {
                    defaultValue = "LuaTrigger(L\"\", L\"\")";
                }
            }

            sb.Append("    m"+ fi.Name + " = " + defaultValue + ";                   \n");


            return sb.ToString();
        }

        static string BuildAttributeSetterPortion(string attributeName, bool isFirst, Type attributeType)
        {
            StringBuilder sb = new StringBuilder();

            if (!isFirst)
            {
                sb.Append("    else ");
            }

            if (attributeType == typeof(uint))
            {
                if (isFirst)
                {
                    sb.Append("    ");
                }
                sb.Append(    "if (attributeName == L\"" + attributeName + "\")                       \n");
                sb.Append("    {                                                                      \n");
                sb.Append("        m" + attributeName + " = abs((int)_wtoi(attributeValue.c_str()));  \n");
                sb.Append("        return true;                                                       \n");
                sb.Append("    }                                                                      \n");
            }
            else if (attributeType == typeof(float))
            {
                if (isFirst)
                {
                    sb.Append("    ");
                }
                sb.Append(    "if (attributeName == L\"" + attributeName + "\")                       \n");
                sb.Append("    {                                                                      \n");
                sb.Append("        m" + attributeName + " = (float)_wtof(attributeValue.c_str());     \n");
                sb.Append("        return true;                                                       \n");
                sb.Append("    }                                                                      \n");
            }
            else if (attributeType == typeof(double))
            {
                if (isFirst)
                {
                    sb.Append("    ");
                }
                sb.Append(    "if (attributeName == L\"" + attributeName + "\")                       \n");
                sb.Append("    {                                                                      \n");
                sb.Append("        m" + attributeName + " = _wtof(attributeValue.c_str());            \n");
                sb.Append("        return true;                                                       \n");
                sb.Append("    }                                                                      \n");
            }
            else if (attributeType == typeof(bool))
            {
                if (isFirst)
                {
                    sb.Append("    ");
                }
                sb.Append(    "if (attributeName == L\"" + attributeName + "\")                       \n");
                sb.Append("    {                                                                      \n");
                sb.Append("        m" + attributeName + " = (attributeValue != L\"0\");                \n");
                sb.Append("        return true;                                                       \n");
                sb.Append("    }                                                                      \n");
            }
            else if (attributeType == typeof(int))
            {
                if (isFirst)
                {
                    sb.Append("    ");
                }
                sb.Append(    "if (attributeName == L\"" + attributeName + "\")                       \n");
                sb.Append("    {                                                                      \n");
                sb.Append("        m" + attributeName + " = (int)_wtoi(attributeValue.c_str());       \n");
                sb.Append("        return true;                                                       \n");
                sb.Append("    }                                                                      \n");
            }
            else if (attributeType == typeof(string))
            {
                if (isFirst)
                {
                    sb.Append("    ");
                }
                sb.Append(    "if (attributeName == L\"" + attributeName + "\")                       \n");
                sb.Append("    {                                                                      \n");
                sb.Append("        m" + attributeName + " = attributeValue;                           \n");
                sb.Append("        return true;                                                       \n");
                sb.Append("    }                                                                      \n");
            }
            else if (attributeType == typeof(char))
            {
                if (isFirst)
                {
                    sb.Append("    ");
                }
                sb.Append(    "if (attributeName == L\"" + attributeName + "\")                       \n");
                sb.Append("    {                                                                      \n");
                sb.Append("        if (attributeValue.length() > 0)                                   \n");
                sb.Append("        {                                                                  \n");
                sb.Append("            m" + attributeName + " = attributeValue[0];                    \n");
                sb.Append("            return true;                                                   \n");
                sb.Append("        }                                                                  \n");
                sb.Append("        else                                                               \n");
                sb.Append("        {                                                                  \n");
                sb.Append("            m" + attributeName + " = '_';                                  \n");
                sb.Append("            return false;                                                  \n");
                sb.Append("        }                                                                  \n");
                sb.Append("    }                                                                      \n");
            }
            else if (attributeType == typeof(Vector2) || attributeType == typeof(Vector3) || attributeType == typeof(Vector4))
            {
                if (isFirst)
                {
                    sb.Append("    ");
                }
                sb.Append(    "if (attributeName == L\"" + attributeName + "\")                       \n");
                sb.Append("    {                                                                      \n");
                sb.Append("        std::stringstream ss;                                              \n");
                sb.Append("        std::string str( attributeValue.begin(), attributeValue.end() );   \n");
                sb.Append("        ss << str;                                                         \n");
                sb.Append("        ss >> m" + attributeName + ";                                      \n");
                sb.Append("        return true;                                                       \n");
                sb.Append("    }                                                                      \n");
            }
            else if (attributeType == typeof(LuaTrigger))
            {
                if (isFirst)
                {
                    sb.Append("    ");
                }
                sb.Append("if (attributeName == L\"" + attributeName + "\")                       \n");
                sb.Append("    {                                                                      \n");
                sb.Append("        std::stringstream ss;                                              \n");
                sb.Append("        std::string str( attributeValue.begin(), attributeValue.end() );   \n");
                sb.Append("        ss << str;                                                         \n");
                sb.Append("        ss >> m" + attributeName + ";                                      \n");
                sb.Append("        return true;                                                       \n");
                sb.Append("    }                                                                      \n");
            }
            else
            {
                return "";
            }

            return sb.ToString();
        }


        static string BuildAttributeCopyPortion(string attributeName, Type attributeType)
        {

            if (attributeType == typeof(uint))
            {
                return "    otherComponent->m" + attributeName + " = m" + attributeName + ";\n";
            }
            else if (attributeType == typeof(float))
            {
                return "    otherComponent->m" + attributeName + " = m" + attributeName + ";\n";
            }
            else if (attributeType == typeof(double))
            {
                return "    otherComponent->m" + attributeName + " = m" + attributeName + ";\n";
            }
            else if (attributeType == typeof(bool))
            {
                return "    otherComponent->m" + attributeName + " = m" + attributeName + ";\n";
            }
            else if (attributeType == typeof(int))
            {
                return "    otherComponent->m" + attributeName + " = m" + attributeName + ";\n";
            }
            else if (attributeType == typeof(string))
            {
                return "    otherComponent->m" + attributeName + " = VCNString(m" + attributeName + ");\n";
            }
            else if (attributeType == typeof(char))
            {
                return "    otherComponent->m" + attributeName + " = m" + attributeName + ";\n";
            }
            else if (attributeType == typeof(Vector2))
            {
                return "    otherComponent->m" + attributeName + " = Vector2(m" + attributeName + ");\n";
            }
            else if (attributeType == typeof(Vector3))
            {
                return "    otherComponent->m" + attributeName + " = Vector3(m" + attributeName + ");\n";
            }
            else if (attributeType == typeof(Vector4))
            {
                return "    otherComponent->m" + attributeName + " = Vector4(m" + attributeName + ");\n";
            }
            else if (attributeType == typeof(LuaTrigger))
            {
                return "    otherComponent->m" + attributeName + " = LuaTrigger(m" + attributeName + ");\n";
            }
            else
            {
                return "";
            }
        }

        static string FindNextBackupFileNameForFile(string vcnNodesFolderPath, string className, string extension)
        {
            int count = 1;
            string filePathPrefix = vcnNodesFolderPath + Path.DirectorySeparatorChar + className + "_backup";
            while (count < 10000)
            {
                string testFile = filePathPrefix + count.ToString() + extension;
                if (!File.Exists(testFile))
                {
                    return testFile;
                }
                count++;
            }
            return "";
        }

        static bool BuildCPPFile(Type componentType, string vcnNodesFolderPath)
        {
            string className = componentType.Name;
            bool extendsBaseComponent = componentType.BaseType == typeof(BaseComponent);
            FieldInfo[] fields;
            if (extendsBaseComponent)
            {
                fields = componentType.GetFields();
            }
            else
            {
                fields = componentType.GetFields(BindingFlags.DeclaredOnly | BindingFlags.Public | BindingFlags.Instance);
            }


            string fullCPPFilePath = vcnNodesFolderPath + Path.DirectorySeparatorChar + className + ".cpp";
            string fullCPPFilePathCopy = FindNextBackupFileNameForFile(vcnNodesFolderPath, className, ".cpp");
            string previousCodeRegionCtorContent = "";
            string previousCodeRegionInitContent = "";
            string previousCodeRegionUpdateContent = "";
            string previousGlobalCodeRegionContent = "";
            string previousIncludesContent = "";

            try
            {
                
                string cppText = System.IO.File.ReadAllText(fullCPPFilePath);
                File.Copy(fullCPPFilePath, fullCPPFilePathCopy, true);
                previousCodeRegionCtorContent = ExtractCodeRegionFromFile(cppText, false, "VCN"+className + "::VCN" + className + "()", "");
                previousCodeRegionInitContent = ExtractCodeRegionFromFile(cppText, false, "::Initialise", "");
                previousCodeRegionUpdateContent = ExtractCodeRegionFromFile(cppText, false, "::Update", "");
                previousGlobalCodeRegionContent = ExtractCodeRegionFromFile(cppText, true);
                previousIncludesContent = ExtractCodeRegionFromFile(cppText, false, "#include", "VCN" + className + "::VCN" + className + "()");
            }
            catch (System.Exception)
            {
                // Normal if there is not already an existing file
            }

            try
            {
                FileStream oStream = File.Open(fullCPPFilePath, FileMode.Create);
                StringBuilder sb = new StringBuilder();

                sb.Append("#include \"Precompiled.h\"                                     \n");
                sb.Append("#include \"" + className + ".h\"                               \n");
                sb.Append("                                                               \n");
                sb.Append("// DO NOT DECLARE MORE CODE REGIONS. USE THE ONES ALREADY IN THE FILE \n");
                sb.Append("                                                               \n");
                sb.Append("//" + CodeRegionDelimiterBegin + "\n");
                sb.Append(previousIncludesContent);
                sb.Append("//" + CodeRegionDelimiterEnd + "\n");
                sb.Append("                                                               \n");
                sb.Append("VCN" + className + "::VCN" + className + "()                    \n");
                sb.Append("{                                                              \n");
                sb.Append("    // Default values                                          \n");
                foreach (FieldInfo fi in fields)
                {
                    string attrRep = BuildAttributeDefaultValue(fi);
                    sb.Append(attrRep);
                }
                sb.Append("//" + CodeRegionDelimiterBegin + "\n");
                sb.Append(previousCodeRegionCtorContent);
                sb.Append("//" + CodeRegionDelimiterEnd + "\n");
                sb.Append("}                                                              \n");
                sb.Append("                                                               \n");
                sb.Append("void VCN" + className + "::Initialise(const Parameters& params)\n");
                sb.Append("{                                                              \n");
                if (!extendsBaseComponent)
                {
                    sb.Append("    VCN" + componentType.BaseType.Name + "::Initialise(params); \n");
                }
                sb.Append("//" + CodeRegionDelimiterBegin + "\n");
                sb.Append(previousCodeRegionInitContent);
                sb.Append("//" + CodeRegionDelimiterEnd + "\n");
                sb.Append("}                                                              \n");
                sb.Append("                                                               \n");
                sb.Append("void VCN" + className + "::Update(VCNFloat dt)                 \n");
                sb.Append("{                                                              \n");
                if (!extendsBaseComponent)
                {
                    sb.Append("    VCN" + componentType.BaseType.Name + "::Update(dt);    \n");
                }
                sb.Append("//" + CodeRegionDelimiterBegin + "\n");
                sb.Append(previousCodeRegionUpdateContent);
                sb.Append("//" + CodeRegionDelimiterEnd + "\n");
                sb.Append("}                                                              \n");
                sb.Append("                                                               \n");
                sb.Append("bool VCN" + className + "::SetAttribute( const VCNString& attributeName, const VCNString& attributeValue )\n");
                sb.Append("{                                                              \n");
                int c = 0;
                foreach (FieldInfo fi in fields)
                {
                    string attrRep = BuildAttributeSetterPortion(fi.Name, c == 0, fi.FieldType);
                    sb.Append(attrRep);
                    c++;
                }
                if (!extendsBaseComponent)
                {
                    sb.Append("    VCN" + componentType.BaseType.Name + "::SetAttribute(attributeName, attributeValue); \n");
                }
                sb.Append("    return false;                                              \n");
                sb.Append("}                                                              \n");
                sb.Append("                                                               \n");


                sb.Append("VCNIComponent::Ptr VCN" + className + "::Copy() const                          \n");
                sb.Append("{                                                                              \n");
                sb.Append("    VCN" + className + "* otherComponent = new VCN" + className + "();         \n");
                sb.Append("                                                                               \n");
                foreach (FieldInfo fi in componentType.GetFields())
                {
                    string attrRep = BuildAttributeCopyPortion(fi.Name, fi.FieldType);
                    sb.Append(attrRep);
                }
                sb.Append("                                                                               \n");
                sb.Append("    return std::shared_ptr<VCN" + className + ">(otherComponent);              \n");
                sb.Append("}                                                                              \n");
                sb.Append("                                                               \n");
                sb.Append("                                                               \n");
                sb.Append("//" + CodeRegionDelimiterBegin + "\n");
                sb.Append(previousGlobalCodeRegionContent);
                sb.Append("//" + CodeRegionDelimiterEnd + "\n");

                string buf = sb.ToString();
                byte[] info = new UTF8Encoding(true).GetBytes(buf);
                oStream.Write(info, 0, info.Length);
                oStream.Close();

                return true;
            }
            catch (System.Exception)
            {
                Console.WriteLine("An error has occurred when creating the cpp file for class " + className);
                return false;
            }
        }

        
    }
}
