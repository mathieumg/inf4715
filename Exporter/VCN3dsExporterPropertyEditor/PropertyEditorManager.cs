using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace VCN3dsExporterPropertyEditor
{

    public delegate void EmptyDelegate();


    public interface PropertyEditorManager
    {
        void CallbackFromUIClosed();
        void CallbackFromUIActiveState(bool activeState);
        void CallbackFromUIModification();
    }

    public class EmptyPropertyEditorManager : PropertyEditorManager
    {
        public void CallbackFromUIClosed()
        {
        }

        public void CallbackFromUIActiveState(bool activeState)
        {
        }

        public void CallbackFromUIModification()
        {
        }
    }

}
