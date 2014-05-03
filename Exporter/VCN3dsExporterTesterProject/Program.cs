using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using VCN3dsExporterPropertyEditor;
using System.Reflection;

namespace VCN3dsExporterTesterProject
{
    class Program
    {
        static void Main(string[] args)
        {
            // Load assemblies
//             try
//             {
//                 Assembly.LoadFrom("VCNNodesCSharpCustom.dll");
//             }
//             catch (System.Exception)
//             {
//                 if (System.Diagnostics.Debugger.IsAttached)
//                     System.Diagnostics.Debugger.Break();
//                 // Si ce breakpoint est hit c'est parce que la DLL "VCNNodesCSharpCustom.dll" n'est pas dans le dossier "trunk\Vicuna\Exporter\VCN3dsExporterTesterProject\bin\Debug\"
//                 // Il faut alors compiler la solution "trunk\Vicuna\VCNNodesCSharpSolution\VCNNodesCSharpSolution.sln" et copier TOUT le contenu de "trunk\Vicuna\Output\Debug\VCNNodesCSharp\" dans "trunk\Vicuna\Exporter\VCN3dsExporterTesterProject\bin\Debug\"
// 
//             }
            

            var thread = new Thread(new ThreadStart(DisplayFormThread));
            thread.SetApartmentState(ApartmentState.STA);
            thread.Start();
            thread.Join();
        }

        private static void DisplayFormThread()
        {
            NewPropertyEditor.OpenTestEditorWindow();
            System.Windows.Threading.Dispatcher.Run();
        }
    }
}
