using System;
using System.Collections.Generic;
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
using System.ComponentModel;
using System.Reflection;
using System.Windows.Markup;
using VCNNodesCSharp;
using System.Windows.Threading;
using System.Collections.ObjectModel;
using Xceed.Wpf.Toolkit;
using System.Collections.Specialized;
using System.Threading;
using System.Globalization;

namespace VCN3dsExporterPropertyEditor
{

    [ValueConversion(typeof(bool), typeof(bool))]
    public class InvertBooleanInverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            bool original = (bool)value;
            return !original;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            bool original = (bool)value;
            return !original;
        }
    }


    public class EditorWindowViewModel : INotifyPropertyChanged, IDisposable
    {
        public EditorWindowViewModel()
        {
            _AvailableComponents = new List<Type>();
            _Components = new MyObservableCollection<UIComponent>();
            _Components.CollectionChanged += TriggerListModified;
            _Components.ContentPropertyChanged += TriggerListContentModified;
            _HasInvalidSelection = false;
        }

        // Do not modify
        public event PropertyChangedEventHandler PropertyChanged;
        protected void OnPropertyChanged(string name)
        {
            PropertyChangedEventHandler handler = PropertyChanged;
            if (handler != null)
            {
                handler(this, new PropertyChangedEventArgs(name));
            }
        }


        // Add stuff here
        private MyObservableCollection<UIComponent> _Components;
        public MyObservableCollection<UIComponent> Components
        {
            get { return _Components; }
            set
            {
                _Components = value;
                OnPropertyChanged("Components");
            }
        }

        private bool _HasInvalidSelection;
        public bool HasInvalidSelection
        {
            get { return _HasInvalidSelection; }
            set
            {
                _HasInvalidSelection = value;
                OnPropertyChanged("HasInvalidSelection");
            }
        }

        public bool ContainsComponent(string name)
        {
            foreach (UIComponent component in Components)
            {
                if (component.Name == name)
                {
                    return true;
                }
            }
            return false;
        }

        private List<Type> _AvailableComponents;
        public List<Type> AvailableComponents
        {
            get { return _AvailableComponents; }
            set { _AvailableComponents = value; OnPropertyChanged("AvailableComponents"); }
        }

        public List<UIComponent> GetComponentsAsList()
        {
            return Components.Cast<UIComponent>().ToList();
        }

        public void TriggerListModified(object sender, NotifyCollectionChangedEventArgs e)
        {
            OnPropertyChanged("Components");
        }

        public void TriggerListContentModified(object sender, PropertyChangedEventArgs e)
        {
            OnPropertyChanged("Components");
        }

        public void RemoveComponent(string componentName)
        {
            foreach (UIComponent uic in _Components)
            {
                if (uic.Name == componentName)
                {
                    try
                    {
                        _Components.Remove(uic);
                    }
                    catch (System.Exception ex)
                    {
                        int i = 0;
                    }
                    break;
                }
            }
        }

        public void Dispose()
        {
            foreach (Delegate d in PropertyChanged.GetInvocationList())
            {
                PropertyChanged -= (PropertyChangedEventHandler)d;
            }
            _Components = null;
            _AvailableComponents = null;
        }
    }

    /// <summary>
    /// Interaction logic for NewPropertyEditor.xaml
    /// </summary>
    public partial class NewPropertyEditor : Window, IDisposable
    {
        private EditorWindowViewModel _ComponentsViewModel;
        private PropertyEditorManager _PropertyEditorManager;
        private bool _IsActiveState;
        private bool _IsWaitingForUpdateState;

        private static Action EmptyDelegate = delegate() { };

        public List<Type> AvailableComponents
        {
            get { return _ComponentsViewModel.AvailableComponents; }
            set { _ComponentsViewModel.AvailableComponents = value; }
        }

        public void ViewModelModified(Object sender, PropertyChangedEventArgs e)
        {
            if (!_IsWaitingForUpdateState)
            {
                return;
            }
            if (_PropertyEditorManager == null)
            {
                return;
            }
            _PropertyEditorManager.CallbackFromUIModification();
        }

        public bool IsActiveState
        {
            get { return _IsActiveState; }
        }

        

        public NewPropertyEditor(PropertyEditorManager editorManager, 
            List<UIComponent> data,
            List<Type> availableComponents)
        {
            _IsWaitingForUpdateState = true;
            Activated += App_Activated;
            Deactivated += App_Deactivated;
            _PropertyEditorManager = editorManager;
            _ComponentsViewModel = new EditorWindowViewModel();
            _ComponentsViewModel.AvailableComponents = availableComponents;
            _ComponentsViewModel.PropertyChanged += ViewModelModified;
            InitializeComponent();
            DataContext = _ComponentsViewModel;
        }

        public static void OpenTestEditorWindow()
        {
            List<Type> availableTypes = VCNNodesCSharpHelper.GetListOfComponents(true);

            // Setup test data
            List<UIComponent> components = new List<UIComponent>();

            // Test pour les types de vecteurs
            UIComponent c1 = new UIComponent("Vectors");
            {
                UIAttributeVector2 a1 = new UIAttributeVector2("Vector 2");
                a1.Value = new Vector2(5.5f, 20.7f);
                c1.AddAttribute(a1);

                UIAttributeVector3 a2 = new UIAttributeVector3("Vector 3");
                a2.Value = new Vector3(5.5f, 20.7f, 28.0f);
                c1.AddAttribute(a2);

                UIAttributeVector4 a3 = new UIAttributeVector4("Vector 4");
                a3.Value = new Vector4(5.5f, 20.7f, 28.0f, 25.9f);
                c1.AddAttribute(a3);

                UIAttributeLuaTrigger tr1 = new UIAttributeLuaTrigger("lua stuff");
                tr1.Value = new LuaTrigger("a", "b");
                c1.AddAttribute(tr1);

                UIAttributeFloat a4 = new UIAttributeFloat("float");
                a4.Value = 0.5f;
                c1.AddAttribute(a4);

                UIAttributeInt a5 = new UIAttributeInt("Integer");
                a5.Value = 2;
                c1.AddAttribute(a5);
            }
            components.Add(c1);

            // Je te laisse mettre ce que tu veux ici.
            // Pour les objets qui représentent des nombres 
            // Si tu veux metttre des min et max tu peux utiliser le field "MinValue" et "MaxValue" de UIAttribute.
            // Tu dois absolument utiliser le bon type de UIAttribute. Par exemple, un Int serait un UIAttributeInt et son MinValue et MaxValue doivent être des Int (même type que l'objet)
            // C'est normal si la liste dans le Combobox reste vide. L'assembly pour les components customs n'est pas liée à ce projet.

            NewPropertyEditor window = new NewPropertyEditor(
                new EmptyPropertyEditorManager(),
                components,
                availableTypes
            );
            window.UpdateComponents(components);
            window.Show();
        }

        public void SetValidSelection(bool validSelection)
        {
            _ComponentsViewModel.HasInvalidSelection = !validSelection;
        }


        public void UpdateComponents(List<UIComponent> components)
        {
            _IsWaitingForUpdateState = false; // Will disable the update notifications for the time that the data is loaded in the UI
            _ComponentsViewModel.Components.Clear();
            components.ForEach(x => _ComponentsViewModel.Components.Add(x));
            // Force a render to make sure that the user sees the change
            Dispatcher.Invoke(DispatcherPriority.Render, EmptyDelegate);
            _IsWaitingForUpdateState = true;
        }

        public List<UIComponent> GetCurrentUIComponents()
        {
            return _ComponentsViewModel.GetComponentsAsList();
        }

        private void AddSelectedComponent(object sender, RoutedEventArgs e)
        {
            Dispatcher.BeginInvoke(DispatcherPriority.Background, new ParameterizedThreadStart(WorkerAddCurrentComponent), NewComponentType.SelectedItem);
        }

        private void RemoveComponent(object sender, RoutedEventArgs e)
        {
            string componentNameToRemove = ((Button)sender).Tag as string;
            if (componentNameToRemove != null)
            {
                MessageBoxResult messageBoxResult = System.Windows.MessageBox.Show("Are you sure you want to remove component \"" + componentNameToRemove + "\" ?", "Remove Component Confirmation", System.Windows.MessageBoxButton.YesNo);
                if (messageBoxResult == MessageBoxResult.Yes)
                {
                    Dispatcher.BeginInvoke(DispatcherPriority.Background, new ParameterizedThreadStart(WorkerRemoveComponent), componentNameToRemove);
                    
                }    
            }
        }

        void WorkerRemoveComponent(object e) // e is the component name to remove
        {
            _ComponentsViewModel.RemoveComponent(e as string);
        }

        void WorkerAddCurrentComponent(object e) // e is the Type object of the component to add
        {
            if (e != null)
            {
                Type typeComp = e as Type;
                BaseComponent bc = BaseComponent.BuildNewComponentOfType(typeComp);
                if (bc != null)
                {
                    UIComponent newComp = bc.BuildUIComponent();
                    if (newComp != null)
                    {
                        if (_ComponentsViewModel.ContainsComponent(newComp.Name))
                        {
                            LblAddComponent.Content = "Component '" + typeComp.Name + "' already exists";
                        }
                        else
                        {
                            _ComponentsViewModel.Components.Add(newComp);
                            LblAddComponent.Content = "Component '" + typeComp.Name + "' Added";
                        }
                    }
                    else
                    {
                        LblAddComponent.Content = "Could not add component of type: " + typeComp.Name;
                    }
                }
            }
            else
            {
                LblAddComponent.Content = "Select the type of component you want to add";
            }
        }

        void App_Activated(object sender, EventArgs e)
        {
            _IsActiveState = true;
            if (_PropertyEditorManager != null)
            {
                _PropertyEditorManager.CallbackFromUIActiveState(true);
            }
        }

        void App_Deactivated(object sender, EventArgs e)
        {
            _IsActiveState = false;
            if (_PropertyEditorManager != null)
            {
                _PropertyEditorManager.CallbackFromUIActiveState(false);
            }
        }

        protected override void OnClosing(CancelEventArgs e)
        {
            if (_PropertyEditorManager != null)
            {
                _PropertyEditorManager.CallbackFromUIClosed();
            }
            base.OnClosing(e);
        }


        public void Dispose()
        {
            _ComponentsViewModel = null;
            _PropertyEditorManager = null;
            _IsActiveState = false;
            _IsWaitingForUpdateState = false;
            Activated -= App_Activated;
            Deactivated -= App_Deactivated;
            if (_ComponentsViewModel != null)
            {
                _ComponentsViewModel.Dispose();
            }
        }

        private Point StartDragPosition;
        private bool IsDragging = false;
        private double StartValue = 0.0;


        private void IntegerUpDown_MouseDown(object sender, MouseButtonEventArgs e)
        {
            if (e.LeftButton == MouseButtonState.Pressed)
            {
                if (IsDragging)
                {
                    UpDown_MouseUp(sender, e);
                }
                else
                {
                    var i = sender as Border;
                    if (i != null)
                    {
                        var inputField = i.Child as Xceed.Wpf.Toolkit.IntegerUpDown;
                        if (inputField != null)
                        {
                            Mouse.Capture(inputField, CaptureMode.Element);
                            StartDragPosition = e.GetPosition(inputField);
                            IsDragging = true;
                            StartValue = inputField.Value == null ? 0.0 : (double)inputField.Value;
                        }
                    }
                }
            }
        }

        private void IntegerUpDown_MouseMove(object sender, MouseEventArgs e)
        {
            // Update position
            if (IsDragging)
            {
                var i = sender as Border;
                if (i != null)
                {
                    var inputField = i.Child as Xceed.Wpf.Toolkit.IntegerUpDown;
                    if (inputField != null)
                    {
                        Point currentPosition = Mouse.GetPosition(inputField);
                        double diffY = StartDragPosition.Y - currentPosition.Y;
                        inputField.Value = (int)((StartValue + diffY)/3);
                    }
                }
            }

        }


        // Double section
        private void DoubleUpDown_MouseDown(object sender, MouseButtonEventArgs e)
        {
            if (e.LeftButton == MouseButtonState.Pressed)
            {
                if (IsDragging)
                {
                    UpDown_MouseUp(sender, e);
                }
                else
                {
                    var i = sender as Border;
                    if (i != null)
                    {
                        var inputField = i.Child as Xceed.Wpf.Toolkit.DoubleUpDown;
                        if (inputField != null)
                        {
                            Mouse.Capture(inputField, CaptureMode.Element);
                            StartDragPosition = e.GetPosition(inputField);
                            IsDragging = true;
                            StartValue = inputField.Value==null ? 0.0 : (double)inputField.Value;
                        }
                    }
                }
            }
        }

        private void DoubleUpDown_MouseMove(object sender, MouseEventArgs e)
        {
            // Update position
            if (IsDragging)
            {
                var i = sender as Border;
                if (i != null)
                {
                    var inputField = i.Child as Xceed.Wpf.Toolkit.DoubleUpDown;
                    if (inputField != null)
                    {
                        Point currentPosition = Mouse.GetPosition(inputField);
                        double diffY = StartDragPosition.Y - currentPosition.Y;
                        inputField.Value = (StartValue + diffY)/3.0;
                    }
                }
            }

        }

        private void UpDown_MouseUp(object sender, MouseButtonEventArgs e)
        {
            if (e.LeftButton == MouseButtonState.Released)
            {
                var inputField = sender as Border;
                if (inputField != null)
                {
                    IsDragging = false;
                    Mouse.Capture(null);
                }
            }
        }
    }
}
