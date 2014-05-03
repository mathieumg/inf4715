using System;
using System.Collections.Generic;
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
using System.ComponentModel;
using VCNNodesCSharp;

namespace VCN3dsExporterPropertyEditor
{


    public class ElementTemplateSelector : DataTemplateSelector
    {
        public DataTemplate StringElementTemplate { get; set; }
        public DataTemplate IntElementTemplate { get; set; }
        public DataTemplate DoubleElementTemplate { get; set; }
        public DataTemplate FloatElementTemplate { get; set; }
        public DataTemplate GenericElementTemplate { get; set; }
        public DataTemplate Vector2ElementTemplate { get; set; }
        public DataTemplate Vector3ElementTemplate { get; set; }
        public DataTemplate Vector4ElementTemplate { get; set; }
        public DataTemplate ColorElementTemplate { get; set; }
        public DataTemplate BoolElementTemplate { get; set; }
        public DataTemplate LuaTriggerElementTemplate { get; set; }

        //For elements without bounds for the slider
        public DataTemplate IntElementTemplateNoSlider { get; set; }
        public DataTemplate DoubleElementTemplateNoSlider { get; set; }
        public DataTemplate FloatElementTemplateNoSlider { get; set; }
        public DataTemplate Vector2ElementTemplateNoSlider { get; set; }
        public DataTemplate Vector3ElementTemplateNoSlider { get; set; }
        public DataTemplate Vector4ElementTemplateNoSlider { get; set; }

        public override DataTemplate SelectTemplate(object item,
          DependencyObject container)
        {
            UIAttribute uiAtt = item as UIAttribute;
            if (uiAtt is UIAttributeString)
                return StringElementTemplate;
            if (uiAtt is UIAttributeBool)
                return BoolElementTemplate;
            if (uiAtt is UIAttributeChar)
                return StringElementTemplate;
            if (uiAtt is UIAttributeInt)
                if (uiAtt.IsRanged)
                    return IntElementTemplate;
                else
                    return IntElementTemplateNoSlider;
            if (uiAtt is UIAttributeUInt)
                if (uiAtt.IsRanged)
                    return IntElementTemplate;
                else
                    return IntElementTemplateNoSlider;
            if (uiAtt is UIAttributeDouble)
                if (uiAtt.IsRanged)
                    return DoubleElementTemplate;
                else
                    return DoubleElementTemplateNoSlider;
            if (uiAtt is UIAttributeFloat)
                if (uiAtt.IsRanged)
                    return FloatElementTemplate;
                else
                    return FloatElementTemplateNoSlider;
            if (uiAtt is UIAttributeVector2)
                if (uiAtt.IsRanged)
                    return Vector2ElementTemplate;
                else
                    return Vector2ElementTemplateNoSlider;
            if (uiAtt is UIAttributeVector3)
                if (uiAtt.IsRanged)
                    return Vector3ElementTemplate;
                else
                    return Vector3ElementTemplateNoSlider;
            if (uiAtt is UIAttributeVector4)
                if (uiAtt.IsRanged)
                    return Vector4ElementTemplate;
                else
                    return Vector4ElementTemplateNoSlider;
            if (uiAtt is UIAttributeColor)
                return ColorElementTemplate;
            if (uiAtt is UIAttributeLuaTrigger)
                return LuaTriggerElementTemplate;
            return GenericElementTemplate;

        }
    }
}
