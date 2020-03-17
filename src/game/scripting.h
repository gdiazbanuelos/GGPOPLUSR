typedef unsigned char byte;
typedef unsigned char undefined;
typedef unsigned int dword;

typedef enum ScriptInstructionCode {
    COPYSCRIPTSTATE = 87,
    DRAWLAYERDEPTH = 8,
    FACING = 15,
    FORCEP1FACING = 16,
    FORCEP2FACING = 17,
    GOTO = 49,
    INCTENSION = 52,
    NOOP_5E = 94,
    PLAYSOUND = 27,
    SCALE = 7,
    SETATTACKPUSH = 84,
    SETATTACKSLIP = 85,
    SETCROUCHVAL = 76,
    SETGRAVITY = 67,
    SPAWNOBJ = 25,
    UNC_JUMPINSTALL = 92,
    XMOVE = 23,
    XTRANSFORM = 65,
    YMOVE = 24,
    YTRANSFORM = 66
} ScriptInstructionCode;

struct ScriptInstruction_0x00_SetPose {
    enum ScriptInstructionCode instructionCode;
    char nFramesToHold;
    unsigned short nPoseIndex;
};

typedef struct ScriptInstruction_0x07_Scale ScriptInstruction_0x07_Scale, * PScriptInstruction_0x07_Scale;

typedef enum ScriptInstruction_0x07_ScaleType {
    SCALE_SETASPECT = 0,
    SCALE_SETVERTICAL = 1,
    SCALE_ADDASPECT = 2,
    SCALE_ADDVERTICAL = 3,
    SCALE_ADDASPECT_RANDOM = 4,
    SCALE_ADDVERTICAL_RANDOM = 5,
    SCALE_MULTASPECT = 6,
    SCALE_MULTVERTICAL = 7,
} ScriptInstruction_0x07_ScaleType;

struct ScriptInstruction_0x07_Scale {
    enum ScriptInstructionCode instructionCode;
    enum ScriptInstruction_0x07_ScaleType scaleType;
    short scaleMagnitude;
};

typedef struct ScriptInstruction_0x08_DrawLayerDepth ScriptInstruction_0x08_DrawLayerDepth, * PScriptInstruction_0x08_DrawLayerDepth;

typedef enum ScriptInstruction_0x08_DrawLayerDepth_OperatorType {
    DEPTH_SET0x1a = 3,
    DEPTH_SETRANDOM = 1,
    DEPTH_ADDRANDOM_MODULO = 5,
    DEPTH_ADDRANDOM_MASK = 4,
    DEPTH_SET = 0,
    DEPTH_ADD = 2
} ScriptInstruction_0x08_DrawLayerDepth_OperatorType;

struct ScriptInstruction_0x08_DrawLayerDepth {
    enum ScriptInstructionCode instructionCode;
    enum ScriptInstruction_0x08_DrawLayerDepth_OperatorType operatorType;
    short magnitude;
};

typedef struct ScriptInstruction_0x0f_Facing ScriptInstruction_0x0f_Facing, * PScriptInstruction_0x0f_Facing;

typedef enum ScriptInstruction_0x0f_Facing_OperatorType {
    FACING_UPDATE = 1,
    FACING_INVERT = 0
} ScriptInstruction_0x0f_Facing_OperatorType;

struct ScriptInstruction_0x0f_Facing {
    enum ScriptInstructionCode instructionCode;
    enum ScriptInstruction_0x0f_Facing_OperatorType facingType;
    short unused;
};

typedef struct ScriptInstruction_0x10_ForceP1Facing ScriptInstruction_0x10_ForceP1Facing, * PScriptInstruction_0x10_ForceP1Facing;

struct ScriptInstruction_0x10_ForceP1Facing {
    enum ScriptInstructionCode instructionCode;
};

typedef struct ScriptInstruction_0x11_ForceP2Facing ScriptInstruction_0x11_ForceP2Facing, * PScriptInstruction_0x11_ForceP2Facing;

struct ScriptInstruction_0x11_ForceP2Facing {
    enum ScriptInstructionCode instructionCode;
};

typedef struct ScriptInstruction_0x12 ScriptInstruction_0x12, * PScriptInstruction_0x12;

typedef enum ScriptInstruction_0x12_Subtype {
    UNK_FRC_CAPABLE = 4,
    UNK_ABAFRC = 6,
    SI12_ZEROFLAG = 0
} ScriptInstruction_0x12_Subtype;

struct ScriptInstruction_0x12 {
    enum ScriptInstructionCode instructionCode;
    enum ScriptInstruction_0x12_Subtype subtype;
    short data;
};

typedef struct ScriptInstruction_0x17_XMove ScriptInstruction_0x17_XMove, * PScriptInstruction_0x17_XMove;

struct ScriptInstruction_0x17_XMove {
    enum ScriptInstructionCode instructionCode;
    undefined field_0x1;
    short magnitude;
};

typedef struct ScriptInstruction_0x18_YMove ScriptInstruction_0x18_YMove, * PScriptInstruction_0x18_YMove;

struct ScriptInstruction_0x18_YMove {
    enum ScriptInstructionCode instructionCode;
    undefined field_0x1;
    short magnitude;
};

typedef struct ScriptInstruction_0x19_SpawnGameObject ScriptInstruction_0x19_SpawnGameObject, * PScriptInstruction_0x19_SpawnGameObject;

typedef enum ScriptInstruction_0x19_SpawnGameObject_Flags {
    SGO_SKIPACTION = 128
} ScriptInstruction_0x19_SpawnGameObject_Flags;

struct ScriptInstruction_0x19_SpawnGameObject {
    byte instructionCode;
    undefined field_0x1;
    unsigned short startActionID;
    int gameObjectID;
    enum ScriptInstruction_0x19_SpawnGameObject_Flags flags;
    undefined field_0xc;
    undefined field_0xd;
    short field_0xe;
};

typedef struct ScriptInstruction_0x1a ScriptInstruction_0x1a, * PScriptInstruction_0x1a;

typedef enum ScriptInstruction_0x1a_OperatorType {
    REAP_EFFECT_BY_ID = 0,
    REAP_EFFECT_STAGEOBJECTS = 1,
    REAP_UNKGAME_STAGEOBJECTS = 4,
    REAP_UNKGAME_BY_ID_AND_ACT = 5,
    REAP_UNKGAME_BY_ID = 3,
    REAP_EFFECT_BY_ID_AND_ACT = 2
} ScriptInstruction_0x1a_OperatorType;

struct ScriptInstruction_0x1a {
    enum ScriptInstructionCode instructionCode;
    byte targetObjectID;
    enum ScriptInstruction_0x1a_OperatorType operatorType;
    byte targetAction;
};

typedef struct ScriptInstruction_0x1b_PlaySound ScriptInstruction_0x1b_PlaySound, * PScriptInstruction_0x1b_PlaySound;

struct ScriptInstruction_0x1b_PlaySound {
    enum ScriptInstructionCode instructionCode;
    byte data;
    byte flags;
    byte randomFactor;
};

typedef struct ScriptInstruction_0x1c ScriptInstruction_0x1c, * PScriptInstruction_0x1c;

struct ScriptInstruction_0x1c {
    enum ScriptInstructionCode instructionCode;
    byte data1;
    byte data2;
    undefined field_0x3;
};

typedef struct ScriptInstruction_0x1d ScriptInstruction_0x1d, * PScriptInstruction_0x1d;

struct ScriptInstruction_0x1d {
    enum ScriptInstructionCode instructionCode;
    byte data1;
    byte data2;
    byte field_0x3;
};

typedef struct ScriptInstruction_0x1e ScriptInstruction_0x1e, * PScriptInstruction_0x1e;

struct ScriptInstruction_0x1e {
    enum ScriptInstructionCode instructionCode;
    byte data1;
    byte field_0x2;
    byte data2;
};

typedef struct ScriptInstruction_0x1f ScriptInstruction_0x1f, * PScriptInstruction_0x1f;

struct ScriptInstruction_0x1f {
    enum ScriptInstructionCode instructionCode;
    byte data1;
    byte field_0x2;
    byte data2;
};

typedef struct ScriptInstruction_0x21 ScriptInstruction_0x21, * PScriptInstruction_0x21;

typedef enum ScriptInstruction_0x21_Operator {
    SO21_New_Name = 0,
    SO21_New_Name_1 = 1,
    SO21_New_Name_2 = 2
} ScriptInstruction_0x21_Operator;

struct ScriptInstruction_0x21 {
    enum ScriptInstructionCode instructionCode;
    enum ScriptInstruction_0x21_Operator operatorType;
    short data;
};

typedef struct ScriptInstruction_0x22 ScriptInstruction_0x22, * PScriptInstruction_0x22;

typedef enum ScriptInstruction_0x22_Operator {
    SO22_New_Name = 0,
    SO22_New_Name_1 = 1
} ScriptInstruction_0x22_Operator;

struct ScriptInstruction_0x22 {
    enum ScriptInstructionCode instructionCode;
    enum ScriptInstruction_0x22_Operator operatorType;
    short data;
};

typedef struct ScriptInstruction_0x23 ScriptInstruction_0x23, * PScriptInstruction_0x23;

typedef enum ScriptInstruction_0x23_Operator {
    SO23_New_Name = 0,
    SO23_New_Name_1 = 1
} ScriptInstruction_0x23_Operator;

struct ScriptInstruction_0x23 {
    enum ScriptInstructionCode instructionCode;
    enum ScriptInstruction_0x23_Operator operatorType;
    short data;
};

typedef struct ScriptInstruction_0x24 ScriptInstruction_0x24, * PScriptInstruction_0x24;

typedef enum ScriptInstruction_0x24_Operator {
    SO24_New_Name = 0,
    SO24_New_Name_1 = 1
} ScriptInstruction_0x24_Operator;

struct ScriptInstruction_0x24 {
    enum ScriptInstructionCode instructionCode;
    enum ScriptInstruction_0x24_Operator operatorType;
    short data;
};

typedef struct ScriptInstruction_0x28 ScriptInstruction_0x28, * PScriptInstruction_0x28;

typedef enum ScriptInstruction_0x28_Operator {
    SO28_New_Name_5 = 5,
    SO28_New_Name_6 = 6,
    SO28_New_Name_7 = 7,
    SO28_New_Name = 0,
    SO28_FADEOUT = 1,
    SO28_New_Name_2 = 2,
    SO28_New_Name_3 = 3,
    SO28_New_Name_4 = 4
} ScriptInstruction_0x28_Operator;

struct ScriptInstruction_0x28 {
    enum ScriptInstructionCode instructionCode;
    enum ScriptInstruction_0x28_Operator operatorType;
    short data;
};

typedef struct ScriptInstruction_0x2D ScriptInstruction_0x2D, * PScriptInstruction_0x2D;

struct ScriptInstruction_0x2D {
    enum ScriptInstructionCode instructionCode;
    byte subtype;
    short data;
};

typedef struct ScriptInstruction_0x2f ScriptInstruction_0x2f, * PScriptInstruction_0x2f;

struct ScriptInstruction_0x2f {
    enum ScriptInstructionCode instructionCode;
    byte field_0x1;
    unsigned short data;
};

typedef struct ScriptInstruction_0x31_Jump ScriptInstruction_0x31_Jump, * PScriptInstruction_0x31_Jump;

typedef enum ScriptInstruction_0x31_Jump_OperatorType {
    JUMP_NORMAL = 0,
    JUMP_RELATIVE = 1
} ScriptInstruction_0x31_Jump_OperatorType;

struct ScriptInstruction_0x31_Jump {
    enum ScriptInstructionCode instructionCode;
    enum ScriptInstruction_0x31_Jump_OperatorType operatorType;
    undefined field_0x2;
    undefined field_0x3;
    dword jumpTarget;
};

typedef struct ScriptInstruction_0x32 ScriptInstruction_0x32, * PScriptInstruction_0x32;

typedef enum ScriptInstruction_0x32_Operator {
    SO32_New_Name = 0,
    SO32_New_Name_1 = 1
} ScriptInstruction_0x32_Operator;

struct ScriptInstruction_0x32 {
    enum ScriptInstructionCode instructionCode;
    enum ScriptInstruction_0x32_Operator operatorType;
    short data;
};

typedef struct ScriptInstruction_0x34_IncreaseTension ScriptInstruction_0x34_IncreaseTension, * PScriptInstruction_0x34_IncreaseTension;

struct ScriptInstruction_0x34_IncreaseTension {
    enum ScriptInstructionCode instructionCode;
    byte tensionVal;
    short unkData;
};

typedef struct ScriptInstruction_0x36 ScriptInstruction_0x36, * PScriptInstruction_0x36;

typedef enum ScriptInstruction_0x36_Flags {
    FLAG_SET = 1,
    FLAG_OTHERPLAYER = 2
} ScriptInstruction_0x36_Flags;

struct ScriptInstruction_0x36 {
    enum ScriptInstructionCode instructionCode;
    enum ScriptInstruction_0x36_Flags flags;
    byte data;
    byte field_0x3;
};

typedef struct ScriptInstruction_0x37 ScriptInstruction_0x37, * PScriptInstruction_0x37;

typedef enum ScriptInstruction_0x37_Flags {
    FLAG_OPPONENT = 2
} ScriptInstruction_0x37_Flags;

struct ScriptInstruction_0x37 {
    enum ScriptInstructionCode instructionCode;
    enum ScriptInstruction_0x37_Flags flags;
    byte data;
    byte field_0x3;
};

typedef struct ScriptInstruction_0x39 ScriptInstruction_0x39, * PScriptInstruction_0x39;

struct ScriptInstruction_0x39 {
    enum ScriptInstructionCode instructionCode;
    byte data;
};

typedef struct ScriptInstruction_0x3a ScriptInstruction_0x3a, * PScriptInstruction_0x3a;

typedef enum ScriptInstruction_0x3a_Operator {
    SO3A_New_Name = 0,
    SO3A_New_Name_1 = 1,
    SO3A_New_Name_2 = 2,
    SO3A_New_Name_3 = 3
} ScriptInstruction_0x3a_Operator;

struct ScriptInstruction_0x3a {
    enum ScriptInstructionCode instructionCode;
    enum ScriptInstruction_0x3a_Operator operatorType;
};

typedef struct ScriptInstruction_0x3b ScriptInstruction_0x3b, * PScriptInstruction_0x3b;

struct ScriptInstruction_0x3b {
    enum ScriptInstructionCode instrCode;
    byte data1;
    byte data2;
    byte data3;
};

typedef struct ScriptInstruction_0x3c ScriptInstruction_0x3c, * PScriptInstruction_0x3c;

typedef enum ScriptInstruction_0x3c_Operator {
    SO3C_New_Name = 0,
    SO3C_New_Name_1 = 1
} ScriptInstruction_0x3c_Operator;

struct ScriptInstruction_0x3c {
    enum ScriptInstructionCode instrCode;
    enum ScriptInstruction_0x3c_Operator operatorType;
    byte data;
};

typedef struct ScriptInstruction_0x41_XTransform ScriptInstruction_0x41_XTransform, * PScriptInstruction_0x41_XTransform;

typedef enum ScriptInstruction_0x41_XTransformType {
    XTRANS_MULTIPLYVELX = 3,
    XTRANS_LOADVELOCITY = 11,
    XTRANS_RANDOMVELX = 8,
    XTRANS_SETLOCALVELX = 0,
    XTRANS_MODLOCALTRANSX = 2,
    XTRANS_MODLOCALVELX_INCRONLY = 4,
    XTRANS_MODLOCALVELX = 1,
    XTRANS_STOREVELOCITY = 10,
    XTRANS_SETGLOBALPOSX = 6,
    XTRANS_ROTATE = 7,
    XTRANS_MODLOCALVELX_DECRONLY = 5
} ScriptInstruction_0x41_XTransformType;

struct ScriptInstruction_0x41_XTransform {
    enum ScriptInstructionCode instructionCode;
    enum ScriptInstruction_0x41_XTransformType transformType;
    short transformMagnitude;
};

typedef struct ScriptInstruction_0x42_YTransform ScriptInstruction_0x42_YTransform, * PScriptInstruction_0x42_YTransform;

typedef enum ScriptInstruction_0x42_YTransformType {
    YTRANS_MODVEL = 1,
    YTRANS_SETVEL = 0,
    YTRANS_MODVEL_RANDOM = 8,
    YTRANS_MODPOS = 2,
    YTRANS_SETPOS = 6,
    YTRANS_MULTVEL = 3
} ScriptInstruction_0x42_YTransformType;

struct ScriptInstruction_0x42_YTransform {
    enum ScriptInstructionCode instructionCode;
    enum ScriptInstruction_0x42_YTransformType transformType;
    short transformMagnitude;
};

typedef struct ScriptInstruction_0x43_SetGravity ScriptInstruction_0x43_SetGravity, * PScriptInstruction_0x43_SetGravity;

struct ScriptInstruction_0x43_SetGravity {
    enum ScriptInstructionCode instructionCode;
    undefined field_0x1;
    short gravity;
};

typedef struct ScriptInstruction_0x4a ScriptInstruction_0x4a, * PScriptInstruction_0x4a;

struct ScriptInstruction_0x4a {
    enum ScriptInstructionCode instrCode;
    short stunValue;
};

typedef struct ScriptInstruction_0x4c_SetCrouchValue ScriptInstruction_0x4c_SetCrouchValue, * PScriptInstruction_0x4c_SetCrouchValue;

struct ScriptInstruction_0x4c_SetCrouchValue {
    enum ScriptInstructionCode instrCode;
    byte field_0x1;
    short bShouldCrouch;
};

typedef struct ScriptInstruction_0x4e ScriptInstruction_0x4e, * PScriptInstruction_0x4e;

struct ScriptInstruction_0x4e {
    enum ScriptInstructionCode instrCode;
    byte operatorType;
    short data;
};

typedef struct ScriptInstruction_0x54_SetAttackPushback ScriptInstruction_0x54_SetAttackPushback, * PScriptInstruction_0x54_SetAttackPushback;

struct ScriptInstruction_0x54_SetAttackPushback {
    enum ScriptInstructionCode instrCode;
    short magnitude;
};

typedef struct ScriptInstruction_0x55_SetSlipAmount ScriptInstruction_0x55_SetSlipAmount, * PScriptInstruction_0x55_SetSlipAmount;

struct ScriptInstruction_0x55_SetSlipAmount {
    enum ScriptInstructionCode code;
    short magnitude;
};

typedef struct ScriptInstruction_0x56 ScriptInstruction_0x56, * PScriptInstruction_0x56;

typedef enum ScriptInstruction_0x56_Operator {
    SO56_New_Name = 0,
    SO56_New_Name_1 = 1,
    SO56_New_Name_2 = 2,
    SO56_New_Name_3 = 3
} ScriptInstruction_0x56_Operator;

struct ScriptInstruction_0x56 {
    enum ScriptInstructionCode instrCode;
    enum ScriptInstruction_0x56_Operator operatorType;
    short gameObjectID;
    unsigned short bufferedAction;
    short actionID;
};

typedef struct ScriptInstruction_0x58 ScriptInstruction_0x58, * PScriptInstruction_0x58;

typedef enum ScriptInstruction_0x58_Operator {
    OWNER = 0,
    SELF = 1
} ScriptInstruction_0x58_Operator;

struct ScriptInstruction_0x58 {
    enum ScriptInstructionCode instrCode;
    enum ScriptInstruction_0x58_Operator operatorType;
    short field_0x2;
    short field_0x4;
    short field_0x6;
};

typedef struct ScriptInstruction_0x59 ScriptInstruction_0x59, * PScriptInstruction_0x59;

struct ScriptInstruction_0x59 {
    enum ScriptInstructionCode instrCode;
    byte field_0x1;
    short field_0x2;
};

typedef struct ScriptInstruction_0x5a ScriptInstruction_0x5a, * PScriptInstruction_0x5a;

struct ScriptInstruction_0x5a {
    enum ScriptInstructionCode instrCode;
    byte field_0x1;
    short field_0x2;
    short field_0x4;
    short field_0x6;
};

typedef struct ScriptInstruction_0x5b ScriptInstruction_0x5b, * PScriptInstruction_0x5b;

struct ScriptInstruction_0x5b {
    enum ScriptInstructionCode instrcode;
    byte field_0x1;
    short field_0x2;
    int field_0x4;
};

typedef struct ScriptInstruction_0x5c_Jumpinstall ScriptInstruction_0x5c_Jumpinstall, * PScriptInstruction_0x5c_Jumpinstall;

typedef enum ScriptInstruction_0x5c_Jumpinstall_Operator {
    GRANT = 0,
    REMOVE = 1
} ScriptInstruction_0x5c_Jumpinstall_Operator;

struct ScriptInstruction_0x5c_Jumpinstall {
    enum ScriptInstructionCode instructionCode;
    enum ScriptInstruction_0x5c_Jumpinstall_Operator operatorType;
};

typedef struct ScriptInstruction_0x5f ScriptInstruction_0x5f, * PScriptInstruction_0x5f;

struct ScriptInstruction_0x5f {
    enum ScriptInstructionCode instructionCode;
    undefined field_0x1;
    undefined field_0x2;
    undefined field_0x3;
    dword flags;
    short field_0x8;
    byte field_0xa;
    byte field_0xb;
};

typedef struct ScriptInstruction_0x65_SetAttackProperties ScriptInstruction_0x65_SetAttackProperties, * PScriptInstruction_0x65_SetAttackProperties;

typedef enum ScriptInstruction_0x65_SetAttackProperties_OperatorType {
    ATTACKPROP_CLEANHIT = 3
} ScriptInstruction_0x65_SetAttackProperties_OperatorType;

struct ScriptInstruction_0x65_SetAttackProperties {
    enum ScriptInstructionCode instrcode;
    enum ScriptInstruction_0x65_SetAttackProperties_OperatorType operatorType;
    short field_0x2;
    undefined field_0x4;
    undefined field_0x5;
    undefined field_0x6;
    undefined field_0x7;
};
