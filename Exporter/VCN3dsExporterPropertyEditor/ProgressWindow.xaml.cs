using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.Windows.Threading;

namespace VCN3dsExporterPropertyEditor
{
    public class ProgressWindowViewModel : INotifyPropertyChanged, IDisposable
    {
        public event PropertyChangedEventHandler PropertyChanged;

        protected void OnPropertyChanged(string name) 
        {
            PropertyChangedEventHandler handler = PropertyChanged;
            if (handler != null)
            { 
                handler(this, new PropertyChangedEventArgs(name)); 
            }
        }

        private int _Progress;
        private string _ProgressName;

        public int Progress
        {
            get { return _Progress; }
            set
            {
                _Progress = value;
                OnPropertyChanged("Progress"); 
            }
        }

        public string ProgressName 
        {
            get { return _ProgressName; }
            set
            {
                _ProgressName = value;
                OnPropertyChanged("ProgressName");
            }
        }

        public void Dispose()
        {
            foreach (Delegate d in PropertyChanged.GetInvocationList())
            {
                PropertyChanged -= (PropertyChangedEventHandler)d;
            }
        }
    }



    /// <summary>
    /// Interaction logic for ProgressWindow.xaml
    /// </summary>
    public partial class ProgressWindow : Window, IDisposable
    {
        private ProgressWindowViewModel _ViewModel;

        public ProgressWindow()
        {
            _ViewModel = new ProgressWindowViewModel();
            _ViewModel.ProgressName = "";
            _ViewModel.Progress = 0;
            InitializeComponent();
            DataContext = _ViewModel;
        }


        private static Action EmptyDelegate = delegate() { };

        public void SetProgress(int progress)
        {
            _ViewModel.Progress = progress;
            Dispatcher.Invoke(DispatcherPriority.Render, EmptyDelegate);
        }

        public void SetName(string name)
        {
            _ViewModel.ProgressName = name;
            Dispatcher.Invoke(DispatcherPriority.Render, EmptyDelegate); 
        }


        public void CleanBeforeUnload()
        {
        }

        public void Dispose()
        {
            if (_ViewModel != null)
            {
                _ViewModel.Dispose();
            }
            _ViewModel = null;
            DataContext = null;
        }
    }
}




// 
// using System;
// using System.Collections.Generic;
// using System.Linq;
// using System.Text;
// using System.IO;
// using System.Globalization;
// using System.Security.Policy;
// using System.Reflection;
// using System.Diagnostics.CodeAnalysis;

// namespace ConsoleApplication1
// {
// 
//     /// <span class="code-SummaryComment"><summary></span>
//     /// Loads an assembly into a new AppDomain and obtains all the
//     /// namespaces in the loaded Assembly, which are returned as a 
//     /// List. The new AppDomain is then Unloaded.
//     /// 
//     /// This class creates a new instance of a 
//     /// <span class="code-SummaryComment"><c>AssemblyLoader</c> class</span>
//     /// which does the actual ReflectionOnly loading 
//     /// of the Assembly into
//     /// the new AppDomain.
//     /// <span class="code-SummaryComment"></summary></span>
//     public class SeperateAppDomainAssemblyLoader
//     {
//         #region Public Methods
//         /// <span class="code-SummaryComment"><summary></span>
//         /// Loads an assembly into a new AppDomain and obtains all the
//         /// namespaces in the loaded Assembly, which are returned as a 
//         /// List. The new AppDomain is then Unloaded
//         /// <span class="code-SummaryComment"></summary></span>
//         /// <span class="code-SummaryComment"><param name="assemblyLocation">The Assembly file </span>
//         /// location<span class="code-SummaryComment"></param></span>
//         /// <span class="code-SummaryComment"><returns>A list of found namespaces</returns></span>
//         public List<String> LoadAssembly(FileInfo assemblyLocation)
//         {
//             List<String> namespaces = new List<String>();
// 
//             if (string.IsNullOrEmpty(assemblyLocation.Directory.FullName))
//             {
//                 throw new InvalidOperationException("Directory can't be null or empty.");
//             }
// 
//             if (!Directory.Exists(assemblyLocation.Directory.FullName))
//             {
//                 throw new InvalidOperationException(string.Format(CultureInfo.CurrentCulture,"Directory not found {0}", assemblyLocation.Directory.FullName));
//             }
// 
//             AppDomain childDomain = BuildChildDomain(AppDomain.CurrentDomain);
// 
//             try
//             {
//                 Type loaderType = typeof(AssemblyLoader);
//                 if (loaderType.Assembly != null)
//                 {
//                     var loader = (AssemblyLoader)childDomain.CreateInstanceFrom(loaderType.Assembly.Location, loaderType.FullName).Unwrap();
// 
//                     loader.LoadAssembly(assemblyLocation.FullName);
//                     namespaces = loader.GetNamespaces(assemblyLocation.Directory.FullName);
//                 }
//                 return namespaces;
//             }
// 
//             finally
//             {
// 
//                 AppDomain.Unload(childDomain);
//             }
//         }
//         #endregion
// 
//         #region Private Methods
//         /// <span class="code-SummaryComment"><summary></span>
//         /// Creates a new AppDomain based on the parent AppDomains 
//         /// Evidence and AppDomainSetup
//         /// <span class="code-SummaryComment"></summary></span>
//         /// <span class="code-SummaryComment"><param name="parentDomain">The parent AppDomain</param></span>
//         /// <span class="code-SummaryComment"><returns>A newly created AppDomain</returns></span>
//         private AppDomain BuildChildDomain(AppDomain parentDomain)
//         {
//             Evidence evidence = new Evidence(parentDomain.Evidence);
//             AppDomainSetup setup = parentDomain.SetupInformation;
//             return AppDomain.CreateDomain("DiscoveryRegion", evidence, setup);
//         }
//         #endregion
// 
// 
//         /// <span class="code-SummaryComment"><summary></span>
//         /// Remotable AssemblyLoader, this class 
//         /// inherits from <span class="code-SummaryComment"><c>MarshalByRefObject</c> </span>
//         /// to allow the CLR to marshall
//         /// this object by reference across 
//         /// AppDomain boundaries
//         /// <span class="code-SummaryComment"></summary></span>
//         class AssemblyLoader : MarshalByRefObject
//         {
//             #region Private/Internal Methods
//             /// <span class="code-SummaryComment"><summary></span>
//             /// Gets namespaces for ReflectionOnly Loaded Assemblies
//             /// <span class="code-SummaryComment"></summary></span>
//             /// <span class="code-SummaryComment"><param name="path">The path to the Assembly</param></span>
//             /// <span class="code-SummaryComment"><returns>A List of namespace strings</returns></span>
//             [SuppressMessage("Microsoft.Performance", "CA1822:MarkMembersAsStatic")]
//             internal List<String> GetNamespaces(string path)
//             {
//                 List<String> namespaces = new List<String>();
// 
//                 DirectoryInfo directory = new DirectoryInfo(path);
//                 ResolveEventHandler resolveEventHandler = 
//                     (s,e)=> { 
//                                 return OnReflectionOnlyResolve(
//                                     e, directory); 
//                             };
// 
//                 AppDomain.CurrentDomain.ReflectionOnlyAssemblyResolve += resolveEventHandler;
// 
//                 Assembly reflectionOnlyAssembly = AppDomain.CurrentDomain.ReflectionOnlyGetAssemblies().First();
//                 
//                 foreach (Type type in reflectionOnlyAssembly.GetTypes())
//                 {
//                     if (!namespaces.Contains(type.Namespace))
//                     {
//                         namespaces.Add(type.Namespace);
//                     }
//                 }
// 
//                 AppDomain.CurrentDomain.ReflectionOnlyAssemblyResolve -= resolveEventHandler;
//                 return namespaces;
// 
//             }
// 
//             /// <span class="code-SummaryComment"><summary></span>
//             /// Attempts ReflectionOnlyLoad of current 
//             /// Assemblies dependants
//             /// <span class="code-SummaryComment"></summary></span>
//             /// <span class="code-SummaryComment"><param name="args">ReflectionOnlyAssemblyResolve </span>
//             /// event args<span class="code-SummaryComment"></param></span>
//             /// <span class="code-SummaryComment"><param name="directory">The current Assemblies </span>
//             /// Directory<span class="code-SummaryComment"></param></span>
//             /// <span class="code-SummaryComment"><returns>ReflectionOnlyLoadFrom loaded</span>
//             /// dependant Assembly<span class="code-SummaryComment"></returns></span>
//             private Assembly OnReflectionOnlyResolve(ResolveEventArgs args, DirectoryInfo directory)
//             {
// 
//                 Assembly loadedAssembly = 
//                     AppDomain.CurrentDomain.ReflectionOnlyGetAssemblies()
//                         .FirstOrDefault(
//                           asm => string.Equals(asm.FullName, args.Name, 
//                               StringComparison.OrdinalIgnoreCase));
// 
//                 if (loadedAssembly != null)
//                 {
//                     return loadedAssembly;
//                 }
// 
//                 AssemblyName assemblyName = new AssemblyName(args.Name);
//                 string dependentAssemblyFilename = Path.Combine(directory.FullName, assemblyName.Name + ".dll");
// 
//                 if (File.Exists(dependentAssemblyFilename))
//                 {
//                     return Assembly.ReflectionOnlyLoadFrom(dependentAssemblyFilename);
//                 }
//                 return Assembly.ReflectionOnlyLoad(args.Name);
//             }
// 
// 
//             /// <span class="code-SummaryComment"><summary></span>
//             /// ReflectionOnlyLoad of single Assembly based on 
//             /// the assemblyPath parameter
//             /// <span class="code-SummaryComment"></summary></span>
//             /// <span class="code-SummaryComment"><param name="assemblyPath">The path to the Assembly</param></span>
//             [SuppressMessage("Microsoft.Performance", "CA1822:MarkMembersAsStatic")]
//             internal void LoadAssembly(String assemblyPath)
//             {
//                 try
//                 {
//                     Assembly.ReflectionOnlyLoadFrom(assemblyPath);
//                 }
//                 catch (FileNotFoundException)
//                 {
//                     /* Continue loading assemblies even if an assembly
//                      * can not be loaded in the new AppDomain. */
//                 }
//             }
//             #endregion
//         }
//     }
// }
