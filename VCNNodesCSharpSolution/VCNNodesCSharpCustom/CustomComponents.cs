using System;
using System.Collections.Generic;
using System.Text;
using VCNNodesCSharp;

namespace VCNNodesCSharpCustom
{

//     [HideInEditor]
//     public class LightComponent : BaseComponent
//     {
//         public override string ComponentName { get { return "Light Component"; } }
// 
//         public Vector4 Diffuse;
// 
//         public Vector4 Specular;
// 
//         [DefaultValue(true)]
//         public bool IsOn;
//     }
// 
//     public class PointLightComponent : LightComponent
//     {
//         public override string ComponentName { get { return "Point Light Component"; } }
// 
//     }


    public class SoundEmitterEventComponent : SoundEmitterComponent
    {
        public override string ComponentName { get { return "Sound Emitter Event Component"; } }

        [DefaultValue("")]
        public string EventName;

    }

    public class SoundEmitterStateComponent : SoundEmitterComponent
    {
        public override string ComponentName { get { return "Sound Emitter State Component"; } }

        [DefaultValue("")]
        public string StateName;

        [DefaultValue("")]
        public string StateValue;
    }

    public class SoundEmitterSwitchComponent : SoundEmitterComponent
    {
        public override string ComponentName { get { return "Sound Emitter Switch Component"; } }

        [DefaultValue("")]
        public string SwitchName;

        [DefaultValue("")]
        public string SwitchValue;

    }

    public class PhysicsTriggerComponent : BaseComponent
    {
        public override string ComponentName { get { return "Physic Trigger Component"; } }

        [DefaultValue("")]
        public string NameOfObjectToWatch;

        public LuaTrigger OnEnter;
        public LuaTrigger OnExit;


    }

    [HideInEditor]
    public class SoundEmitterComponent : BaseComponent
    {
        public override string ComponentName
        {
            get
            {
                return "Sound Emitter";
            }
        }

        [DefaultValue(false)]
        public bool IsEnabled;

        [DefaultValue(1.0f)]
        [VCN_Range(0.0f, 1.0f)]
        [Getter]
        public float Volume;

//         [DefaultValue(false)]
//         [Getter]
//         public bool Loop;

//         [DefaultValue(true)]
//         [Getter]
//         public bool Is3D;

//         [DefaultValue(false)]
//         [Getter]
//         public bool IsStream;

        [DefaultValue(0.01f)]
        [Getter]
        public float MinDistance;

        [DefaultValue(1000.0f)]
        [Getter]
        public float MaxDistance;

        [DefaultValue(false)]
        [Getter]
        public bool StartOnLoad;

    }

    [HideInEditor]
    public class TestLua : BaseComponent
    {
        public override string ComponentName
        {
            get
            {
                return "Test Lua";
            }
        }

        public LuaTrigger Trigger1;
        public LuaTrigger Trigger2;


        public TestLua()
        {
            Trigger1 = new LuaTrigger("", "trig1Type");
        }
    }

    public class AutoRotateComponent : BaseComponent
    {
        public override string ComponentName { get { return "Auto Rotate Component"; } }

        [DefaultValue(1.0f)]
        [VCN_Range(0.0f, 1.0f)]
        public double SpeedFactor;
        public Vector3 RotationAxis;

        [NoAccessors]
        [DefaultValue(0.0f)]
        public double RotationPosition;

        public AutoRotateComponent()
            : base()
        {
        }

    }

    [HideInEditor]
    public class TestBert : BaseComponent
    {
        public override string ComponentName { get { return "Auto Rotate Component"; } }

        [DefaultValue(1.0f)]
        [VCN_Range(0.0f, 1.0f)]
        public double SpeedFactor;
        public Vector3 RotationAxis;

        [NoAccessors]
        [DefaultValue(0.0f)]
        public double RotationPosition;

        public TestBert()
            : base()
        {
        }


    }

    public class PhysicsComponent : BaseComponent
    {
        public override string ComponentName { get { return "Physics Component"; } }

        [DefaultValue(true)]
        public bool Active; // True means that a physics actor will be active on the object


        [DefaultValue(false)]
        public bool IsDynamic;

        [DefaultValue(false)]
        public bool HasGravity;

        [DefaultValue(false)]
        public bool IsKinematic;

        [DefaultValue(false)]
        [Getter]
        public bool ForceHollow;
    }


    public class CircularWallRotatingComponent : BaseComponent
    {
        public override string ComponentName { get { return "Circular Wall Rotating Component"; } }

        [DefaultValue(0.01f)]
        public double Speed;
    }

    public class NodePropertiesComponent : BaseComponent
    {
        public override string ComponentName { get { return "Node Properties Component"; } }

        [DefaultValue(false)]
        public bool IsNodeSelectable;

        [DefaultValue(true)]
        public bool IsNodeActive;

        [DefaultValue("")]
        public string NodeTag;

        [DefaultValue("")]
        public string ReadableName;
    }



    public class PuzzleCubeComponent : BaseComponent
    {
        public override string ComponentName { get { return "Puzzle Cube Component"; } }

        [DefaultValue(1)]
        public int PuzzlePiece;

        [DefaultValue(1.0f)]
        [Getter]
        public float PushForce;

        [DefaultValue(true)]
        public bool LockOnAxis;

    }

    public class AiRobotComponent : BaseComponent
    {
        public override string ComponentName { get { return "Ai Robot Component"; } }

        [NoAccessors]
        [DefaultValue(0)]
        public int GridPosX;

        [NoAccessors]
        [DefaultValue(0)]
        public int GridPosY;

        [NoAccessors]
        [DefaultValue(3.0f)]
        [VCN_Range(0.0f, 20.0f)]
        public float MoveSpeed;

        [NoAccessors]
        [DefaultValue(10.0f)]
        [VCN_Range(0.0f, 90.0f)]
        public float FieldViewAngle;

        [NoAccessors]
        [DefaultValue(22.5f)]
        public float DeltaAngle;

        [NoAccessors]
        [DefaultValue(5.0f)]
        public float DistMargin;

        [NoAccessors]
        [DefaultValue("w")]
        public string CharacterNodeName;

        public LuaTrigger TriggerAwarenessChange;

        public AiRobotComponent()
        {
            TriggerAwarenessChange = new LuaTrigger("", "TriggerAwarenessChange");
        }
    }

    public class CharacterStartingPointComponent : BaseComponent
    {
        public override string ComponentName { get { return "Character Starting Point Component"; } }

        [DefaultValue(true)]
        public bool IsEnabled;
    }

    [HideInEditor]
     public class UsableComponent : BaseComponent
     {
         public override string ComponentName { get { return "Usable Component"; } }

         [DefaultValue(true)]
         public bool IsSelectable;

         [DefaultValue(5.0f)]
         public float UseDistance;
     }

    public class SlidingBridgeComponent : UsableComponent
    {
        public override string ComponentName { get { return "Sliding Bridge Component"; } }

        public Vector3 TravelDistance;

        [DefaultValue(1.0f)]
        public float slidingSpeed;

        [DefaultValue(0)]
        [NoAccessors]
        public int StartingMovingState; // See .h for definitions

        public LuaTrigger slideBridge; // Script called to extend the bridge

    }


     public class SlidingDoorComponent : DoorComponent
     {
         public override string ComponentName { get { return "Sliding Door Component"; } }


         public Vector3 TravelDistance;


         [DefaultValue(true)]
         public bool IsLeftPart;

         [DefaultValue("")]
         public string OtherPartName;
     }

     public class RotatingDoorComponent : DoorComponent
     {
         public override string ComponentName { get { return "Rotating Door Component"; } }

         [DefaultValue(true)]
         [Getter]
         public bool RotateClockwise;

         [DefaultValue(90.0f)]
         [Getter]
         public float MaxRotationAngle;

     }

     public class RemoteUsableComponent : UsableComponent
     {
         public override string ComponentName { get { return "Remote Usable Component"; } }

         [DefaultValue("")]
         [Getter]
         public string RemoteNodeName;
     }

     public class MazeSwitchComponent : UsableComponent
     {
         public override string ComponentName { get { return "Remote Usable Component"; } }

         [DefaultValue("")]
         [Getter]
         public string RemoteNodeName;
         public LuaTrigger ToggleSwitch;
     }

    [HideInEditor]
    public class DoorComponent : UsableComponent
    {
        public override string ComponentName { get { return "Door Component"; } }


        [DefaultValue(1.0f)]
        public float OpeningSpeed;

        [DefaultValue(1.0f)]
        public float ClosingSpeed;

        [DefaultValue(0)]
        [NoAccessors]
        public int StartingMovingState; // See .h for definitions

        
        public LuaTrigger OpenValidationScript; // Script called to validate before the door opens
        public LuaTrigger OnOpenTrigger; // Call this when the door is opened
        public LuaTrigger OnCloseTrigger; // Call this when the door is closed

    }

    // Need to inherit from BaseComponent to be scanned and generated
    [HideInEditor] // This will hide the component in the editor. Use this if you want an "anstract class just for inheritance"
    public class TestComponent : BaseComponent
    {
        // This name will be used in the UI of the Editor
        public override string ComponentName { get { return "Test Component"; } }

        [Setter]
        [DefaultValue(2)]
        public int Param1;

        [NoAccessors]
        // If Default Value not defined, the default value based on the type will be used
        public float Param2;

        // By default, a getter and a setter will be generated
        //[DefaultValue(new Vector2(1.0f, 2.0f))] // ** This is invalid. Only basic types allowed. Use the constructor for this one.
        public Vector2 AlloVect2;

        [Getter]
        [DefaultValue("this is a page content")]
        public string PageContent;

        public TestComponent()
            : base()
        {
            AlloVect2 = new Vector2(1.0f, 2.0f);
        }

        // You can override the default method to set custom properties or set them directly on the attribute
        //         protected override void InitObject()
        //         {
        //             Param1 = 10;
        //             Param2 = 5.0f;
        //             AlloVect2 = new Vector2(0.0f, 1.0f);
        //             PageContent = "testString";
        //         }
    }


    // Do not remove
    public class EmptyComponent : BaseComponent
    {
    }

}
