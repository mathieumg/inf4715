using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using VCNNodesCSharp;

namespace VCN3dsExporterPropertyEditor
{

    public delegate void EmptyDelegate();

    public interface PropertyEditorManager
    {
        void CallbackFromUIClosed();
        void CallbackFromUIActiveState(bool activeState);
        void CallbackFromUIModification();

        //List<UIComponent> GetCurrentUIComponents();
        //bool GetCurrentEditorState();
    }

    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class VCN3dsExporterPropertyEditorWindow : Window
    {
        private PropertyEditorManager _PropertyEditorManager;

        private bool _IsActiveState;

        private List<Type> _AvailableComponents;
        private List<BaseComponent> _CurrentComponents;

        private List<UIComponent> _CurrentComponentsUI;

        public List<UIComponent> CurrentComponentsTest
        {
            get { return _CurrentComponentsUI; }
        }

        public bool IsActiveState
        {
            get { return _IsActiveState; }
        }

        public List<BaseComponent> CurrentComponents
        {
            get { return _CurrentComponents; }
        }

        public VCN3dsExporterPropertyEditorWindow(PropertyEditorManager editorManager, 
            List<BaseComponent> data, 
            List<Type> availableComponents
            )
        {
            _PropertyEditorManager = editorManager;
            _CurrentComponents = new List<BaseComponent>();
            _AvailableComponents = availableComponents;
            InitializeComponent();

            Activated += App_Activated;
            Deactivated += App_Deactivated;

            _CurrentComponentsUI = new List<UIComponent>();

            ShowComponents();
        }
        
        void App_Activated(object sender, EventArgs e)
        {
            if (_PropertyEditorManager != null)
            {
                _PropertyEditorManager.CallbackFromUIActiveState(true);
            }
        }

        void App_Deactivated(object sender, EventArgs e)
        {
            if (_PropertyEditorManager != null)
            {
                _PropertyEditorManager.CallbackFromUIActiveState(false);
            }
        }
        
        public void UpdateComponents(List<BaseComponent> components)
        {
            _CurrentComponents = components;
            ShowComponents();
        }
        
        private void SendDataUpdate()
        {
            if (_PropertyEditorManager == null)
            {
                return;
            }
            _PropertyEditorManager.CallbackFromUIModification();
        }

        protected override void OnClosing(CancelEventArgs e)
        {
            if (_PropertyEditorManager != null)
            {
                _PropertyEditorManager.CallbackFromUIClosed();
            }
            _PropertyEditorManager = null;
            Activated -= App_Activated;
            Deactivated -= App_Deactivated;
            _AvailableComponents = null;
            _CurrentComponents = null;
            base.OnClosing(e);
        }



        private void ShowComponents()
        {
             ScrollViewer sv = new ScrollViewer();
             StackPanel stack = new StackPanel();
 
             StackPanel hStack = new StackPanel();
             hStack.Orientation = Orientation.Horizontal;
 
             ComboBox cb = new ComboBox();
             cb.ItemsSource = _AvailableComponents;
             hStack.Children.Add(cb);
 
             Button add = new Button();
             add.Content = "Add Component";
 
 
             add.Click += (object sender, RoutedEventArgs e) =>
             {
                 Type selected = (Type)cb.SelectedItem;
                 if (selected == null)
                 {
                     return;
                 }
                 if (_CurrentComponents.Any(t => { return t.GetType() == selected; }))
                 {
                     MessageBox.Show("You cannot have the same Component multiple times on the same Node");
                     return;
                 }
                 BaseComponent c = BaseComponent.BuildNewComponentOfType(selected);
                 CreateAndAddComponentBox(this, _CurrentComponents, stack, c); 
                 _CurrentComponents.Add(c);
                 SendDataUpdate();
             };
             hStack.Children.Add(add);
 
             stack.Children.Add(hStack);
 
             foreach (var component in _CurrentComponents)
             {
                 CreateAndAddComponentBox(this, _CurrentComponents, stack, component);
             }
 
             sv.Content = stack;
             Content = sv;
        }

        private static void CreateAndAddComponentBox(VCN3dsExporterPropertyEditorWindow referenceWindow, List<BaseComponent> components, StackPanel stack, BaseComponent component)
        {
             GroupBox grpBox = new GroupBox { Header = component.Name };
             grpBox.BorderThickness = new Thickness(3);
             grpBox.BorderBrush = new SolidColorBrush(Color.FromRgb(0, 0, 0));
             Dictionary<string, object> dict = component.GetAttributes();
             StackPanel sp = new StackPanel();
             grpBox.Content = sp;
             foreach (var item in dict)
             {
                 StackPanel sp2 = new StackPanel();
                 sp2.Orientation = Orientation.Horizontal;
                 Label lbl = new Label();
                 lbl.Content = item.Key;
                 sp2.Children.Add(lbl);
                 TextBox txt = new TextBox();
                 txt.Text = item.Value.ToString();
                 txt.TextChanged += (object sender, TextChangedEventArgs args) =>
                 {
                     component.SetAttribute(item.Key, txt.Text);
                     referenceWindow.SendDataUpdate();
                 };
                 sp2.Children.Add(txt);
                 sp.Children.Add(sp2);
             }
             Button remove = new Button();
             remove.Content = "Remove Component";
             remove.Click += (object sender, RoutedEventArgs e) =>
             {
                 stack.Children.Remove(grpBox);
                 components.Remove(component);
                 referenceWindow.SendDataUpdate();
             };
             sp.Children.Add(remove);
             stack.Children.Add(grpBox);
        }
    }
}
