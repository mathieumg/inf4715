using System;
using System.Collections.Generic;
using System.ComponentModel;
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

namespace VCN3dsExporterLog
{

    public enum LogType
    {
        Message = 0, 
        Warning = 1, 
        Error = 2,
        Fatal = 3
    }


	/// <summary>
	/// Interaction logic for VCN3dsExporterLogWindow.xaml
	/// </summary>
	public partial class VCN3dsExporterLogWindow : Window
	{
		public VCN3dsExporterLogWindow()
		{
			this.InitializeComponent();
		}

        private SolidColorBrush GetColorForLogType(LogType type)
        {
            switch (type)
            {
            case LogType.Message:
                return Brushes.DarkGray;
            case LogType.Warning:
                return Brushes.Orange;
            case LogType.Error:
                return Brushes.Red;
            case LogType.Fatal:
                return Brushes.DarkRed;
            default:
                return Brushes.DarkGray;
            }
        }

        void OnClosing(object sender, CancelEventArgs e)
        {
            e.Cancel = true;
        }

        private static Action EmptyDelegate = delegate() { };

        public void AddLogEntry(string message, LogType logType = LogType.Message)
        {
            // 1 run and 1 parag per log message
            Paragraph p = new Paragraph();
            p.Inlines.Add(new Run(message)
            {
                Foreground = GetColorForLogType(logType)
            });
            myFlowDocument.Blocks.Add(p);

            myRichTextBox.ScrollToEnd();
            Dispatcher.Invoke(DispatcherPriority.Render, EmptyDelegate);
        }

        public void ClearContent()
        {
            myFlowDocument.Blocks.Clear();
        }

        private void ClearContentCallback(object sender, RoutedEventArgs e)
        {
            ClearContent();
        }
	}
}