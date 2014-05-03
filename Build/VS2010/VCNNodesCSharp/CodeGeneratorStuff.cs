using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;
using System.Reflection;
using System.Runtime.Serialization;
using System.Collections.Specialized;

namespace VCNNodesCSharp
{
    [Serializable()]
    public class MyObservableCollection<T> : System.Collections.ObjectModel.ObservableCollection<System.ComponentModel.INotifyPropertyChanged>
    {
        public MyObservableCollection()
        {
            CollectionChanged += ContentCollectionChanged;
        }

        public void ContentCollectionChanged(object sender, NotifyCollectionChangedEventArgs e)
        {
            if (e.Action == NotifyCollectionChangedAction.Remove)
            {
                foreach (System.ComponentModel.INotifyPropertyChanged item in e.OldItems)
                {
                    //Removed items
                    item.PropertyChanged -= ItemPropertyChanged;
                }
            }
            else if (e.Action == NotifyCollectionChangedAction.Add)
            {
                foreach (System.ComponentModel.INotifyPropertyChanged item in e.NewItems)
                {
                    //Added items
                    item.PropertyChanged += ItemPropertyChanged;
                }
            }
        }

        public void ItemPropertyChanged(object sender, System.ComponentModel.PropertyChangedEventArgs e)
        {
            OnContentPropertyChanged("Content");
        }

        public event System.ComponentModel.PropertyChangedEventHandler ContentPropertyChanged;
        protected void OnContentPropertyChanged(string name)
        {
            System.ComponentModel.PropertyChangedEventHandler handler = ContentPropertyChanged;
            if (handler != null)
            {
                handler(this, new System.ComponentModel.PropertyChangedEventArgs(name));
            }
        }
    }


    [Serializable()]
    public class Vector2 : System.ComponentModel.INotifyPropertyChanged, ICloneable
    {
        public Vector2()
        {
            _x = 0.0f;
            _y = 0.0f;
        }
        public Vector2(float pX, float pY)
        {
            _x = pX;
            _y = pY;
        }
        public Vector2(string str)
        {
            string[] parts = str.Split(';');
            if (parts.Length == 2)
            {
                try
                {
                    _x = Single.Parse(parts[0]);
                    _y = Single.Parse(parts[1]);
                }
                catch (System.Exception)
                {
                    _x = 0;
                    _y = 0;
                }
            }
            else
            {
                _x = 0;
                _y = 0;
            }
        }
        public override string ToString()
        {
            return x + ";" + y;
        }


        private float _x;
        private float _y;

        public float x
        {
            get { return _x; }
            set
            {
                _x = value;
                OnPropertyChanged("x");
            }
        }
        public float y
        {
            get { return _y; }
            set
            {
                _y = value;
                OnPropertyChanged("y");
            }
        }


        public event System.ComponentModel.PropertyChangedEventHandler PropertyChanged;
        protected void OnPropertyChanged(string name)
        {
            System.ComponentModel.PropertyChangedEventHandler handler = PropertyChanged;
            if (handler != null)
            {
                handler(this, new System.ComponentModel.PropertyChangedEventArgs(name));
            }
        }

        public object Clone()
        {
            return new Vector2(_x, _y);
        }
    }


    [Serializable()]
    public class LuaTrigger : System.ComponentModel.INotifyPropertyChanged, ICloneable
    {
        public LuaTrigger()
        {
            _TriggerName = "";
            _TriggerType = "";
        }
        public LuaTrigger(string pTriggerName, string pTriggerType)
        {
            TriggerName = pTriggerName;
            TriggerType = pTriggerType;
        }
        public LuaTrigger(string str)
        {
            string[] parts = str.Split(';');
            if (parts.Length == 2)
            {
                try
                {
                    TriggerName = parts[1];
                    TriggerType = parts[2];
                }
                catch (System.Exception)
                {
                    TriggerName = "";
                    TriggerType = "";
                }
            }
            else
            {
                TriggerName = "";
                TriggerType = "";
            }
        }
        public override string ToString()
        {
            return ";" + TriggerName + ";" + TriggerType + ";";
        }


        private string _TriggerName;
        private string _TriggerType;

        public string TriggerName
        {
            get { return _TriggerName; }
            set
            {
                _TriggerName = value.Replace(";", "");
                OnPropertyChanged("TriggerName");
            }
        }
        public string TriggerType
        {
            get { return _TriggerType; }
            set
            {
                _TriggerType = value.Replace(";", "");
                OnPropertyChanged("TriggerType");
            }
        }


        public event System.ComponentModel.PropertyChangedEventHandler PropertyChanged;
        protected void OnPropertyChanged(string name)
        {
            System.ComponentModel.PropertyChangedEventHandler handler = PropertyChanged;
            if (handler != null)
            {
                handler(this, new System.ComponentModel.PropertyChangedEventArgs(name));
            }
        }

        public object Clone()
        {
            return new LuaTrigger(TriggerName, TriggerType);
        }
    }

    [Serializable()]
    public class Vector3 : System.ComponentModel.INotifyPropertyChanged, ICloneable
    {
        public Vector3()
        {
            _x = 0.0f;
            _y = 0.0f;
            _z = 0.0f;
        }
        public Vector3(float pX, float pY, float pZ)
        {
            _x = pX;
            _y = pY;
            _z = pZ;
        }
        public Vector3(string str)
        {
            string[] parts = str.Split(';');
            if (parts.Length == 3)
            {
                try
                {
                    _x = Single.Parse(parts[0]);
                    _y = Single.Parse(parts[1]);
                    _z = Single.Parse(parts[2]);
                }
                catch (System.Exception)
                {
                    _x = 0;
                    _y = 0;
                    _z = 0;
                }
            }
            else
            {
                _x = 0;
                _y = 0;
                _z = 0;
            }
        }
        public override string ToString()
        {
            return x + ";" + y + ";" + z;
        }


        private float _x;
        private float _y;
        private float _z;

        public float x
        {
            get { return _x; }
            set
            {
                _x = value;
                OnPropertyChanged("x");
            }
        }
        public float y
        {
            get { return _y; }
            set
            {
                _y = value;
                OnPropertyChanged("y");
            }
        }
        public float z
        {
            get { return _z; }
            set
            {
                _z = value;
                OnPropertyChanged("z");
            }
        }

        public event System.ComponentModel.PropertyChangedEventHandler PropertyChanged;
        protected void OnPropertyChanged(string name)
        {
            System.ComponentModel.PropertyChangedEventHandler handler = PropertyChanged;
            if (handler != null)
            {
                handler(this, new System.ComponentModel.PropertyChangedEventArgs(name));
            }
        }

        public object Clone()
        {
            return new Vector3(_x, _y, _z);
        }
    }

    [Serializable()]
    public class Vector4 : System.ComponentModel.INotifyPropertyChanged, ICloneable
    {
        public Vector4()
        {
            _x = 0.0f;
            _y = 0.0f;
            _z = 0.0f;
            _w = 0.0f;
        }
        public Vector4(float pX, float pY, float pZ, float pW)
        {
            _x = pX;
            _y = pY;
            _z = pZ;
            _w = pW;
        }
        public Vector4(string str)
        {
            string[] parts = str.Split(';');
            if (parts.Length == 4)
            {
                try
                {
                    _x = Single.Parse(parts[0]);
                    _y = Single.Parse(parts[1]);
                    _z = Single.Parse(parts[2]);
                    _w = Single.Parse(parts[3]);
                }
                catch (System.Exception)
                {
                    _x = 0;
                    _y = 0;
                    _z = 0;
                    _w = 0;
                }
            }
            else
            {
                _x = 0;
                _y = 0;
                _z = 0;
                _w = 0;
            }
        }
        public override string ToString()
        {
            return x + ";" + y + ";" + z + ";" + w;
        }

        private float _x;
        private float _y;
        private float _z;
        private float _w;

        public float x
        {
            get { return _x; }
            set
            {
                _x = value;
                OnPropertyChanged("x");
            }
        }
        public float y
        {
            get { return _y; }
            set
            {
                _y = value;
                OnPropertyChanged("y");
            }
        }
        public float z
        {
            get { return _z; }
            set
            {
                _z = value;
                OnPropertyChanged("z");
            }
        }
        public float w
        {
            get { return _w; }
            set
            {
                _w = value;
                OnPropertyChanged("w");
            }
        }

        public event System.ComponentModel.PropertyChangedEventHandler PropertyChanged;
        protected void OnPropertyChanged(string name)
        {
            System.ComponentModel.PropertyChangedEventHandler handler = PropertyChanged;
            if (handler != null)
            {
                handler(this, new System.ComponentModel.PropertyChangedEventArgs(name));
            }
        }

        public object Clone()
        {
            return new Vector4(_x, _y, _z, _w);
        }
    }


    [Serializable()]
    public class VCN_Color : System.ComponentModel.INotifyPropertyChanged, ICloneable
    {
        public VCN_Color()
        {
            _r = 0;
            _g = 0;
            _b = 0;
            _a = 255;
        }
        public VCN_Color(byte pX, byte pY, byte pZ, byte pW)
        {
            _r = pX;
            _g = pY;
            _b = pZ;
            _a = pW;
        }
        public VCN_Color(string str)
        {
            string[] parts = str.Split(';');
            if (parts.Length == 4)
            {
                try
                {
                    _r = Byte.Parse(parts[0]);
                    _g = Byte.Parse(parts[1]);
                    _b = Byte.Parse(parts[2]);
                    _a = Byte.Parse(parts[3]);
                }
                catch (System.Exception)
                {
                    _r = 0;
                    _g = 0;
                    _b = 0;
                    _a = 255;
                }
            }
            else
            {
                _r = 0;
                _g = 0;
                _b = 0;
                _a = 255;
            }
        }
        public override string ToString()
        {
            return r + ";" + g + ";" + b + ";" + a;
        }

        private byte _r;
        private byte _g;
        private byte _b;
        private byte _a;

        public byte r
        {
            get { return _r; }
            set
            {
                _r = value;
                OnPropertyChanged("r");
            }
        }
        public byte g
        {
            get { return _g; }
            set
            {
                _g = value;
                OnPropertyChanged("g");
            }
        }
        public byte b
        {
            get { return _b; }
            set
            {
                _b = value;
                OnPropertyChanged("b");
            }
        }
        public byte a
        {
            get { return _a; }
            set
            {
                _a = value;
                OnPropertyChanged("a");
            }
        }

        public event System.ComponentModel.PropertyChangedEventHandler PropertyChanged;
        protected void OnPropertyChanged(string name)
        {
            System.ComponentModel.PropertyChangedEventHandler handler = PropertyChanged;
            if (handler != null)
            {
                handler(this, new System.ComponentModel.PropertyChangedEventArgs(name));
            }
        }

        public object Clone()
        {
            return new VCN_Color(_r, _g, _b, _a);
        }
    }



    public class Getter : Attribute
    {
        public Getter()
        { }
    }

    public class Setter : Attribute
    {
        public Setter()
        { }
    }

    public class NoAccessors : Attribute
    {
        public NoAccessors()
        { }
    }

    [DefaultValue(AttributeTargets.Class)]
    public class DefaultValue : System.Attribute
    {
        private object _Value;
        public DefaultValue(object val)
        {
            this._Value = val;
        }
        public object Value
        {
            get { return _Value; }
        }
    }

    public class VCN_Range : Attribute
    {
        private object _Min;
        private object _Max;

        public VCN_Range(object min, object max)
        {
            _Min = min;
            _Max = max;
        }

        public object Min
        {
            get { return _Min; }
        }

        public object Max
        {
            get { return _Max; }
        }
    }

    public class HideInEditor : Attribute
    {
        public HideInEditor()
        { }
    }


    [Serializable()]
    public class UIComponentSerializable
    {
        public UIComponentSerializable()
        {
            _Name = "";
            _Attributes = new List<UIAttributeSerializable>();
        }

        public UIComponentSerializable(string name)
        {
            _Name = name;
            _Attributes = new List<UIAttributeSerializable>();
        }

        private string _Name;
        public string Name
        {
            get { return _Name; }
            set { _Name = value; }
        }

        private List<UIAttributeSerializable> _Attributes;
        public List<UIAttributeSerializable> Attributes
        {
            get { return _Attributes; }
            set { _Attributes = value; }
        }


    }

    [Serializable()]
    public class UIComponent : ISerializable, System.ComponentModel.INotifyPropertyChanged
    {
        public void TriggerListModified(object sender, System.ComponentModel.PropertyChangedEventArgs e)
        {
            UpdateListCache();
            OnPropertyChanged("Attributes");
        }


        private string _Name;
        public string Name
        {
            get { return _Name; }
            set { _Name = value; OnPropertyChanged("Name"); }
        }


        private void UpdateListCache()
        {
        }


        public UIComponent()
        {
            _Name = "";
            _Attributes = new MyObservableCollection<UIAttribute>();
            _Attributes.ContentPropertyChanged += TriggerListModified;
            UpdateListCache();
        }

        public UIComponent(SerializationInfo info, StreamingContext context)
        {
            _Name = info.GetString("Name");
            _Attributes = new MyObservableCollection<UIAttribute>();
            // Reset the property value using the GetValue method.
            foreach (var val in info)
            {
                if (val.Name == "Name")
                {
                    continue;
                }
                _Attributes.Add(val.Value as UIAttribute);
            }
            _Attributes.ContentPropertyChanged += TriggerListModified;
            UpdateListCache();
        }

        public UIComponent(string n)
        {
            _Name = n;
            _Attributes = new MyObservableCollection<UIAttribute>();
            _Attributes.ContentPropertyChanged += TriggerListModified;
            UpdateListCache();
        }


        public void AddAttribute(UIAttribute attr)
        {
            _Attributes.Add(attr);
            UpdateListCache();
        }


        public override string ToString()
        {
            if (Attributes.Count == 0)
            {
                return "No attributes";
            }
            else
            {
                string rep = "";
                foreach (UIAttribute ui_att in Attributes)
                {
                    rep += ui_att.ToString() + "\n";
                }
                return rep;

            }
        }


        public void GetObjectData(SerializationInfo info, StreamingContext context)
        {
            info.AddValue("Name", Name);
            for (int i=0; i<Attributes.Count; i++)
            {
                info.AddValue(i.ToString(), Attributes[i]);
            }
        }

        [NonSerialized]
        private MyObservableCollection<UIAttribute> _Attributes;
        //[NonSerializedAttribute()]
        public MyObservableCollection<UIAttribute> Attributes
        {
            get { return _Attributes; }
            set
            {
                _Attributes = value;
                OnPropertyChanged("Attributes");
            }
        }


        [field: NonSerializedAttribute()]
        public event System.ComponentModel.PropertyChangedEventHandler PropertyChanged;
        protected void OnPropertyChanged(string name)
        {
            System.ComponentModel.PropertyChangedEventHandler handler = PropertyChanged;
            if (handler != null)
            {
                handler(this, new System.ComponentModel.PropertyChangedEventArgs(name));
            }
        }


        public static List<UIComponentSerializable> BuildSerializableCopy(List<UIComponent> list)
        {
            List<UIComponentSerializable> retList = new List<UIComponentSerializable>();
            foreach (UIComponent uic in list)
            {
                UIComponentSerializable uicS = new UIComponentSerializable();
                uicS.Name = String.Copy(uic.Name);
                foreach (UIAttribute uia in uic.Attributes)
                {
                    UIAttributeSerializable uiaS = new UIAttributeSerializable(uia);
                    uicS.Attributes.Add(uiaS);
                }
                retList.Add(uicS);
            }
            return retList;
        }


    }


    public interface StringSerializable
    {
        void BuildFromString(string content);
        string BuildString();
    }

    public interface UIAttributeWrapper
    {
        Type GetTargetType();
    }

    [Serializable()]
    public class UIAttributeSerializable
    {
        protected string _Name;
        protected object _Value;
        protected object _MinValue;
        protected object _MaxValue;

        public virtual object MinValue
        {
            get { return _MinValue; }
            set { _MinValue = value; }
        }

        public virtual object MaxValue
        {
            get { return _MaxValue; }
            set { _MaxValue = value; }
        }
        
        public virtual string Name
        {
            get { return _Name; }
            set { _Name = value; }
        }

        public virtual object Value
        {
            get { return _Value; }
            set { _Value = value; }
        }

        public bool IsRanged
        {
            get { return MinValue != MaxValue; }
        }

        public UIAttributeSerializable()
        {
            _Name = "";
            _Value = null;
            _MinValue = null;
            _MaxValue = null;
        }

        public UIAttributeSerializable(string name)
        {
            _Name = name;
            _Value = null;
            _MinValue = null;
            _MaxValue = null;
        }

        public UIAttributeSerializable(UIAttributeSerializable toCopy)
        {
            _Name = String.Copy(toCopy.Name);
            ICloneable otherObject = toCopy.Value as ICloneable;
            if (otherObject == null)
            {
                _Value = toCopy.Value;
            }
            else
            {
                _Value = otherObject.Clone();
            }
            _MinValue = null;
            _MaxValue = null;
        }

    }

    [Serializable()]
    public class UIAttribute : UIAttributeSerializable, StringSerializable, UIAttributeWrapper, System.ComponentModel.INotifyPropertyChanged
    {
        public UIAttribute()
            :base()
        {
        }

        public UIAttribute(string name)
            :base(name)
        {
        }


        public UIAttribute(string name, string content)
            :base(name)
        {
            BuildFromString(content);
        }

        
        public override string Name
        {
            get { return _Name; }
            set
            {
                _Name = value;
                OnPropertyChanged("Name");
            }
        }

        protected void BindPropertyChangedEvents(object oldValue, object newValue)
        {
            if (oldValue != null)
            {
                System.ComponentModel.INotifyPropertyChanged oldV = oldValue as System.ComponentModel.INotifyPropertyChanged;
                if (oldV != null)
                {
                    oldV.PropertyChanged -= TriggerValueModified;
                }
            }
            System.ComponentModel.INotifyPropertyChanged newV = newValue as System.ComponentModel.INotifyPropertyChanged;
            if (newV != null)
            {
                newV.PropertyChanged += TriggerValueModified;
            }
        }

        public override object Value
        {
            get { return _Value; }
            set
            {
                BindPropertyChangedEvents(_Value, value);
                _Value = value;
                OnPropertyChanged("Value");
            }
        }

        public virtual void BuildFromString(string content)
        {
            _Value = content;
        }

        public virtual string BuildString()
        {
            return Value.ToString();
        }

        public virtual Type GetTargetType()
        {
            return typeof(object);
        }

        public void TriggerValueModified(object sender, System.ComponentModel.PropertyChangedEventArgs e)
        {
            OnPropertyChanged("Value");
        }

        public static UIAttribute BuildUIAttributeFromTargetType(Type targetType, string name, object value, object min, object max)
        {
            foreach (Assembly a in AppDomain.CurrentDomain.GetAssemblies())
            {
                foreach (Type t in a.GetTypes())
                {
                    if (t.IsSubclassOf(typeof(UIAttribute)))
                    {
                        // TODO: Optimize this to not have to create unnecessary objects
                        UIAttribute attr = (UIAttribute)Activator.CreateInstance(t, new object[] { });
                        if (attr.GetTargetType() == targetType)
                        {
                            attr.Name = name;
                            attr.Value = value;
                            return attr;
                        }
                    }
                }
            }
            return null;
        }

        [field: NonSerializedAttribute()]
        public event System.ComponentModel.PropertyChangedEventHandler PropertyChanged;

        protected void OnPropertyChanged(string name)
        {
            System.ComponentModel.PropertyChangedEventHandler handler = PropertyChanged;
            if (handler != null)
            {
                handler(this, new System.ComponentModel.PropertyChangedEventArgs(name));
            }
        }
    }


    #region Custom attributes

    [Serializable()]
    public class UIAttributeString : UIAttribute
    {
        public UIAttributeString()
            :base()
        {
        }

        public UIAttributeString(string name)
            :base(name)
        {
        }

        public UIAttributeString(string name, string content)
            :base(name, content)
        {
        }


        public override void BuildFromString(string content)
        {
            _Value = content;
        }

        public override Type GetTargetType()
        {
            return typeof(string);
        }

        public string ValueAsString
        {
            get { return Value as string; }
            set { Value = value; OnPropertyChanged("ValueAsString"); }
        }

    }

    [Serializable()]
    public class UIAttributeInt : UIAttribute
    {
        public UIAttributeInt()
            :base()
        {
        }

        public UIAttributeInt(string name)
            :base(name)
        {
        }

        public UIAttributeInt(string name, string content)
            :base(name, content)
        {
        }

        public override void BuildFromString(string content)
        {
            _Value = Convert.ToInt32(content);
        }

        public override Type GetTargetType()
        {
            return typeof(int);
        }

        public int ValueAsInt
        {
            get { return (int)Value; }
            set { Value = value; OnPropertyChanged("ValueAsInt"); }
        }
    }

    [Serializable()]
    public class UIAttributeUInt : UIAttribute
    {
        public UIAttributeUInt()
            : base()
        {
        }

        public UIAttributeUInt(string name)
            : base(name)
        {
        }

        public UIAttributeUInt(string name, string content)
            : base(name, content)
        {
        }

        public override void BuildFromString(string content)
        {
            _Value = Convert.ToUInt32(content);
        }

        public override Type GetTargetType()
        {
            return typeof(uint);
        }

        public uint ValueAsUInt
        {
            get { return (uint)Value; }
            set { Value = value; OnPropertyChanged("ValueAsUInt"); }
        }
    }

    [Serializable()]
    public class UIAttributeFloat : UIAttribute
    {
        public UIAttributeFloat()
            : base()
        {
        }

        public UIAttributeFloat(string name)
            : base(name)
        {
        }

        public UIAttributeFloat(string name, string content)
            : base(name, content)
        {
        }

        public override void BuildFromString(string content)
        {
            _Value = Convert.ToSingle(content);
        }

        public override Type GetTargetType()
        {
            return typeof(float);
        }

        public float ValueAsFloat
        {
            get { return (float)Value; }
            set { Value = value; OnPropertyChanged("ValueAsFloat"); }
        }
    }

    [Serializable()]
    public class UIAttributeDouble : UIAttribute
    {
        public UIAttributeDouble()
            : base()
        {
        }

        public UIAttributeDouble(string name)
            : base(name)
        {
        }

        public UIAttributeDouble(string name, string content)
            : base(name, content)
        {
        }

        public override void BuildFromString(string content)
        {
            _Value = Convert.ToDouble(content);
        }

        public override Type GetTargetType()
        {
            return typeof(double);
        }

        public double ValueAsDouble
        {
            get { return (double)Value; }
            set { Value = value; OnPropertyChanged("ValueAsDouble"); }
        }
    }

    [Serializable()]
    public class UIAttributeBool : UIAttribute
    {
        public UIAttributeBool()
            : base()
        {
        }

        public UIAttributeBool(string name)
            : base(name)
        {
        }

        public UIAttributeBool(string name, string content)
            : base(name, content)
        {
        }

        public override void BuildFromString(string content)
        {
            _Value = Convert.ToBoolean(content);
        }

        public override Type GetTargetType()
        {
            return typeof(bool);
        }

        public bool ValueAsBool
        {
            get { return (bool)Value; }
            set { Value = value; OnPropertyChanged("ValueAsBool"); }
        }
    }

    [Serializable()]
    public class UIAttributeChar : UIAttribute
    {
        public UIAttributeChar()
            : base()
        {
        }

        public UIAttributeChar(string name)
            : base(name)
        {
        }

        public UIAttributeChar(string name, string content)
            : base(name, content)
        {
        }

        public override void BuildFromString(string content)
        {
            _Value = Convert.ToChar(content);
        }

        public override Type GetTargetType()
        {
            return typeof(char);
        }

        public char ValueAsChar
        {
            get { return (char)Value; }
            set { Value = value; OnPropertyChanged("ValueAsChar"); }
        }
    }

    [Serializable()]
    public class UIAttributeVector2 : UIAttribute
    {
        public UIAttributeVector2()
            : base()
        {
        }

        public UIAttributeVector2(string name)
            : base(name)
        {
        }

        public UIAttributeVector2(string name, string content)
            : base(name, content)
        {
        }

        public override void BuildFromString(string content)
        {
            _Value = new Vector2(content);
        }

        public override Type GetTargetType()
        {
            return typeof(Vector2);
        }

        public Vector2 ValueAsVecto2
        {
            get { return (Vector2)Value; }
            set { Value = value; OnPropertyChanged("ValueAsVector2"); }
        }
    }

    [Serializable()]
    public class UIAttributeVector3 : UIAttribute
    {
        public UIAttributeVector3()
            : base()
        {
        }

        public UIAttributeVector3(string name)
            : base(name)
        {
        }

        public UIAttributeVector3(string name, string content)
            : base(name, content)
        {
        }

        public override void BuildFromString(string content)
        {
            _Value = new Vector3(content);
        }

        public override Type GetTargetType()
        {
            return typeof(Vector3);
        }

        public Vector3 ValueAsVector3
        {
            get { return (Vector3)Value; }
            set { Value = value; OnPropertyChanged("ValueAsVector3"); }
        }
    }

    [Serializable()]
    public class UIAttributeLuaTrigger : UIAttribute
    {
        public UIAttributeLuaTrigger()
            : base()
        {
        }

        public UIAttributeLuaTrigger(string name)
            : base(name)
        {
        }

        public UIAttributeLuaTrigger(string name, string content)
            : base(name, content)
        {
        }

        public override void BuildFromString(string content)
        {
            _Value = new LuaTrigger(content);
        }

        public override Type GetTargetType()
        {
            return typeof(LuaTrigger);
        }

        public LuaTrigger ValueAsLuaTrigger
        {
            get { return (LuaTrigger)Value; }
            set { Value = value; OnPropertyChanged("ValueAsLuaTrigger"); }
        }
    }

    [Serializable()]
    public class UIAttributeVector4 : UIAttribute
    {
        public UIAttributeVector4()
            : base()
        {
        }

        public UIAttributeVector4(string name)
            : base(name)
        {
        }

        public UIAttributeVector4(string name, string content)
            : base(name, content)
        {
        }

        public override void BuildFromString(string content)
        {
            _Value = new Vector4(content);
        }

        public override Type GetTargetType()
        {
            return typeof(Vector4);
        }

        public Vector4 ValueAsVector4
        {
            get { return (Vector4)Value; }
            set { Value = value; OnPropertyChanged("ValueAsVector4"); }
        }
    }

    [Serializable()]
    public class UIAttributeColor : UIAttribute
    {
        public UIAttributeColor()
            : base()
        {
        }

        public UIAttributeColor(string name)
            : base(name)
        {
        }

        public UIAttributeColor(string name, string content)
            : base(name, content)
        {
        }

        public override void BuildFromString(string content)
        {
            _Value = new VCN_Color(content);
        }

        public override Type GetTargetType()
        {
            return typeof(VCN_Color);
        }

        public VCN_Color ValueAsVCN_Color
        {
            get { return (VCN_Color)Value; }
            set { Value = value; OnPropertyChanged("ValueAsVCN_Color"); }
        }
    }

    #endregion



    // Base class definition
    [Serializable()]
    public class BaseComponent : ISerializable
    {
        // Derived class should redefine this attribute
        public virtual string ComponentName { get { return GetType().Name; } }

        public string AssemblyName
        {
            get { return GetType().Name; }
        }
        

        // The value parameter must have the good type. For example, a Vector3 must be a Vector3 and not a string that represent a Vector3
        public void SetAttribute(string attr, object value)
        {
            if (value != null)
            {
                Type type = GetType();
                PropertyInfo propertyInfo = type.GetProperty(attr);
                if (propertyInfo != null)
                {
                    if (propertyInfo.PropertyType == value.GetType())
                    {
                        propertyInfo.SetValue(this, value, null);
                    }
                }
                else
                {
                    FieldInfo fi = type.GetField(attr);
                    if (fi != null)
                    {
                        if (fi.FieldType == value.GetType())
                        {
                            fi.SetValue(this, value);
                        }
                    }
                }
            }
        }

        public Dictionary<string, object> GetAttributes()
        {
            Dictionary<string, object> retDict = new Dictionary<string, object>();
            Type type = GetType();
            FieldInfo[] properties = type.GetFields();
            foreach (FieldInfo pi in properties)
            {
                object val = pi.GetValue(this);
                retDict[pi.Name] = val;
            }
            return retDict;
        }


        protected virtual void InitObject(Type t)
        {
            FieldInfo[] finfo = t.GetFields();
            foreach (FieldInfo fi in finfo)
            {
                bool found = false;
                object[] cAttrs = fi.GetCustomAttributes(false);
                foreach (object att in cAttrs)
                {
                    DefaultValue dv = att as DefaultValue;
                    if (dv != null)
                    {
                        SetAttribute(fi.Name, dv.Value);
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    if (fi.GetValue(this) != null)
                    {
                        SetAttribute(fi.Name, fi.GetValue(this));
                    }
                    else
                    {
                        Type fieldType = fi.FieldType;
                        object defaultObject = Activator.CreateInstance(fieldType);
                        SetAttribute(fi.Name, defaultObject);
                    }
                }
            }
        }

        public BaseComponent()
        {
        }


        public BaseComponent(SerializationInfo info, StreamingContext context)
        {
            // Reset the property value using the GetValue method.
            foreach (var val in info)
            {
                SetAttribute(val.Name, val.Value);
            }
        }

        public void GetObjectData(SerializationInfo info, StreamingContext context)
        {
            Type type = GetType();
            FieldInfo[] properties = type.GetFields();
            foreach (FieldInfo pi in properties)
            {
                object valToSave = pi.GetValue(this);
                info.AddValue(pi.Name, valToSave, valToSave.GetType());
            }
        }

        public static BaseComponent BuildNewComponentOfType(Type t)
        {
            BaseComponent c = (BaseComponent)Activator.CreateInstance(t, new object[] {});
            c.InitObject(t);
            return c;
        }

        public UIComponent BuildUIComponent()
        {
            UIComponent uic = new UIComponent();
            uic.Name = (string)AssemblyName.Clone();
            Type type = GetType();
            FieldInfo[] properties = type.GetFields();
            foreach (FieldInfo pi in properties)
            {
                object val = pi.GetValue(this);
                object min = null;
                object max = null;

                object[] ranges = pi.GetCustomAttributes(typeof(VCN_Range), false);
                if (ranges.Length > 0)
                {
                    VCN_Range range = (VCN_Range)ranges[0];
                    min = range.Min;
                    max = range.Max;
                }

                uic.AddAttribute(UIAttribute.BuildUIAttributeFromTargetType(pi.FieldType, pi.Name, val, min, max));
            }
            return uic;
        }

        public static List<UIComponent> BuildUIComponentList(List<BaseComponent> compList)
        {
            List<UIComponent> uiComponents = new List<UIComponent>();
            foreach (BaseComponent bc in compList)
			{
				UIComponent uic = bc.BuildUIComponent();
				if (uic != null)
				{
					uiComponents.Add(uic);
				}
			}
            return uiComponents;
        }
        
    }

    public class VCNNodesCSharpHelper
    {
        public static List<Type> GetListOfComponents(bool hideInEditor = false)
        {
            List<Type> componentTypes = new List<Type>();
            foreach (Assembly a in AppDomain.CurrentDomain.GetAssemblies())
            {
                foreach (Type t in a.GetTypes())
                {
                    if (t.IsSubclassOf(typeof(BaseComponent)))
                    {
                        // Check if it has Hide in Editor attribute
                        object[] listThatMustBeEmpty = t.GetCustomAttributes(typeof(HideInEditor), false);
                        if (!hideInEditor || (hideInEditor && listThatMustBeEmpty.Length == 0))
                        {
                            componentTypes.Add(t);
                        }
                    }
                }
            }
            return componentTypes;
        }

        public static BaseComponent BuildComponentFromName(string name)
        {
            try
            {
                Type t = Type.GetType("VCNNodesCSharp." + name);
                if (t == null)
                {
                    t = Type.GetType("VCNNodesCSharpCustom." + name + ", VCNNodesCSharpCustom");
                }
                return Activator.CreateInstance(t) as BaseComponent;
            }
            catch (System.Exception)
            {
                return null;
            }
        }
    }
}
